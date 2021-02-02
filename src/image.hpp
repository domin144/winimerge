/////////////////////////////////////////////////////////////////////////////
//    License (GPLv2+):
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful, but
//    WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//    General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
/////////////////////////////////////////////////////////////////////////////

#ifndef IMAGE_HPP
#define IMAGE_HPP

#include <FreeImage.h>
#include <FreeImagePlus.h>
#include <algorithm>
#include <cstdio>
#include <fstream>
#include <ios>
#include <string>
#include <map>
#include <cstring>
#include <filesystem>

inline FREE_IMAGE_FORMAT identifyFIF(const std::filesystem::path& path)
{
    return [](const auto* str) {
        if constexpr (std::
                          is_same_v<std::filesystem::path::value_type, wchar_t>)
        {
            return fipImage::identifyFIFU(str);
        }
        else
        {
            return fipImage::identifyFIF(str);
        }
    }(path.c_str());
}

inline bool load(fipImage& image, const std::filesystem::path& path)
{
    return [&](const auto* str) {
        if constexpr (std::
                          is_same_v<std::filesystem::path::value_type, wchar_t>)
        {
            return image.loadU(str) == TRUE;
        }
        else
        {
            return image.load(str) == TRUE;
        }
    }(path.c_str());
}

inline bool save(fipImage& image, const std::filesystem::path& path)
{
    return [&](const auto* str) {
        if constexpr (std::
                          is_same_v<std::filesystem::path::value_type, wchar_t>)
        {
            return image.saveU(str) == TRUE;
        }
        else
        {
            return image.save(str) == TRUE;
        }
    }(path.c_str());
}

class fipImageEx : public fipImage
{
public:
	fipImageEx(FREE_IMAGE_TYPE image_type = FIT_BITMAP, unsigned width = 0, unsigned height = 0, unsigned bpp = 0)
		: fipImage(image_type, width, height, bpp) {}
	fipImageEx(const fipImageEx& Image) { *this = Image; }
	explicit fipImageEx(const fipImage& Image) { *this = Image; }
	explicit fipImageEx(FIBITMAP *bitmap) { *this = bitmap; }
	virtual ~fipImageEx() {}

	fipImageEx& operator=(const fipImageEx& Image)
	{
		if (this != &Image)
		{
			FIBITMAP *clone = FreeImage_Clone(static_cast<FIBITMAP*>(Image._dib));
			replace(clone);
			_fif = Image._fif;
		}
		return *this;
	}

	fipImageEx& operator=(const fipImage& Image)
	{
		if (this != &Image)
		{
			FIBITMAP *clone = FreeImage_Clone(static_cast<FIBITMAP*>(const_cast<fipImage&>(Image)));
			replace(clone);
			_fif = Image.getFIF();
		}
		return *this;
	}

	fipImageEx& operator=(FIBITMAP *dib)
	{
		if (_dib != dib)
			replace(dib);
		return *this;
	}

	void swap(fipImageEx& other)
	{
		std::swap(_dib, other._dib);
		std::swap(this->_fif, other._fif);
		std::swap(this->_bHasChanged, other._bHasChanged);
	}

	FIBITMAP *detach()
	{
		FIBITMAP *dib = _dib;
        _dib = nullptr;
		clear();
		return dib;
	}

    BOOL colorQuantizeEx(FREE_IMAGE_QUANTIZE quantize = FIQ_WUQUANT, int PaletteSize = 256, int ReserveSize = 0, RGBQUAD *ReservePalette = nullptr)
	{
		if(_dib) {
			FIBITMAP *dib8 = FreeImage_ColorQuantizeEx(_dib, quantize, PaletteSize, ReserveSize, ReservePalette);
			return !!replace(dib8);
		}
		return false;
	}

    bool convertColorDepth(unsigned bpp, RGBQUAD *pPalette = nullptr)
	{
		switch (bpp)
		{
		case 1:
			return !!threshold(128);
		case 4:
		{
			fipImageEx tmp = *this;
			tmp.convertTo24Bits();
			if (pPalette)
				tmp.colorQuantizeEx(FIQ_NNQUANT, 16, 16, pPalette);
			else
				tmp.colorQuantizeEx(FIQ_WUQUANT, 16);
			setSize(tmp.getImageType(), tmp.getWidth(), tmp.getHeight(), 4);
			for (unsigned y = 0; y < tmp.getHeight(); ++y)
			{
				const BYTE *line_src = tmp.getScanLine(y);
				BYTE *line_dst = getScanLine(y);
				for (unsigned x = 0; x < tmp.getWidth(); ++x)
					line_dst[x / 2] |= ((x % 2) == 0) ? (line_src[x] << 4) : line_src[x];
			}

			RGBQUAD *rgbq_dst = getPalette();
			RGBQUAD *rgbq_src = pPalette ? pPalette : tmp.getPalette();
			std::memcpy(rgbq_dst, rgbq_src, sizeof(RGBQUAD) * 16);
			return true;
		}
		case 8:
			convertTo24Bits();
			if (pPalette)
				return !!colorQuantizeEx(FIQ_NNQUANT, 256, 256, pPalette);
			else
				return !!colorQuantizeEx(FIQ_WUQUANT, 256);
		case 15:
			return !!convertTo16Bits555();
		case 16:
			return !!convertTo16Bits565();
		case 24:
			return !!convertTo24Bits();
		default:
		case 32:
			return !!convertTo32Bits();
		}
	}

	void copyAnimationMetadata(fipImage& src)
	{
		fipTag tag;
		fipMetadataFind finder;
		if (finder.findFirstMetadata(FIMD_ANIMATION, src, tag))
		{
			do
			{
				setMetadata(FIMD_ANIMATION, tag.getKey(), tag);
			} while (finder.findNextMetadata(tag));
		}
	}
};

class fipMultiPageEx : public fipMultiPage
{
public:
	explicit fipMultiPageEx(BOOL keep_cache_in_memory = FALSE)
		: fipMultiPage(keep_cache_in_memory)
	{
    }
   
	bool openU(const std::filesystem::path &pathName, int flags = 0)
    {
        /* Load multipage files from a temporary copy to avoid a lock on the
         * original files, so they can be saved back if desired (#14) */
        const std::filesystem::path tmpName = std::tmpnam(nullptr);
        std::filesystem::copy_file(pathName, tmpName);
        m_temporaryFile.emplace(tmpName);
        m_sourceFile.open(tmpName, std::ios::binary);
		if (!m_sourceFile)
		{
            m_temporaryFile.reset();
            return false;
		}

		FreeImageIO m_io;
		m_io.read_proc  = myReadProc;
		m_io.write_proc = nullptr;
		m_io.seek_proc  = mySeekReadProc;
		m_io.tell_proc  = myTellReadProc;
        const BOOL result = open(&m_io, static_cast<fi_handle>(&m_sourceFile), flags);
        if (result == FALSE)
        {
            m_temporaryFile.reset();
        }
        return result == TRUE;
	}

	bool saveU(const std::filesystem::path &pathName, int flag = 0) const
	{
		std::fstream outputFile(pathName, std::ios::out | std::ios::binary);
		if (!outputFile)
		{
			return false;
        }
        const FREE_IMAGE_FORMAT fif = identifyFIF(pathName);

        FreeImageIO m_io;
        m_io.read_proc = nullptr;
        m_io.write_proc = myWriteProc;
        m_io.seek_proc = mySeekWriteProc;
        m_io.tell_proc = myTellWriteProc;
        bool result = !!saveToHandle(fif, &m_io, static_cast<fi_handle>(&outputFile), flag);
        return result;
	}

private:
        struct TemporaryFile
        {
        private:
            std::filesystem::path m_path;
        public:
            TemporaryFile(const std::filesystem::path& path) : m_path(path) {}
            TemporaryFile(const TemporaryFile&) = delete;

            ~TemporaryFile()
            {
                std::filesystem::remove(m_path);
            }
        };
    std::optional<TemporaryFile> m_temporaryFile;

	static unsigned DLL_CALLCONV myReadProc(
			void *buffer,
			unsigned size,
			unsigned count,
			fi_handle handle)
	{
		auto &file = *static_cast<std::ifstream *>(handle);
		auto *byteBuffer = reinterpret_cast<std::fstream::char_type *>(buffer);
		file.read(byteBuffer, size * count);
		return file.gcount() / size;
	}

	static unsigned DLL_CALLCONV myWriteProc(void *buffer, unsigned size, unsigned count, fi_handle handle)
	{
		auto &file = *static_cast<std::ofstream *>(handle);
		auto *byteBuffer = reinterpret_cast<std::fstream::char_type *>(buffer);
		file.write(byteBuffer, size * count);
		if (!file)
		{
			return 0;
		}
		return count;
	}

	static int DLL_CALLCONV mySeekReadProc(fi_handle handle, long offset, int origin) {
		auto &file = *static_cast<std::ifstream *>(handle);
		const auto dir = [&](){
			switch (origin) {
			case SEEK_SET:
				return std::ios::beg;
			case SEEK_CUR:
				return std::ios::cur;
			case SEEK_END:
				return std::ios::end;
			}
			return std::ios::beg;
		}();
		return file.seekg(offset, dir) ? 0 : 1;
	}

	static long DLL_CALLCONV myTellReadProc(fi_handle handle) {
		auto &file = *static_cast<std::ifstream *>(handle);
		return file.tellg();
	}

	static int DLL_CALLCONV mySeekWriteProc(fi_handle handle, long offset, int origin) {
		auto &file = *static_cast<std::ofstream *>(handle);
		const auto dir = [&](){
			switch (origin) {
			case SEEK_SET:
				return std::ios::beg;
			case SEEK_CUR:
				return std::ios::cur;
			case SEEK_END:
				return std::ios::end;
			}
			return std::ios::beg;
		}();
		return file.seekp(offset, dir) ? 0 : 1;
	}

	static long DLL_CALLCONV myTellWriteProc(fi_handle handle) {
		auto &file = *static_cast<std::ofstream *>(handle);
		return file.tellp();
	}

	std::ifstream m_sourceFile;

};

class MultiPageImages;

class Image
{
	friend MultiPageImages;
public:
	typedef RGBQUAD Color;
	Image() {}
	Image(int w, int h) : image_(FIT_BITMAP, w, h, 32) {}
	Image(const Image& other) : image_(other.image_) {}
	explicit Image(FIBITMAP *bitmap) : image_(bitmap) {}
	explicit Image(const fipImage& image) : image_(image) {}
	BYTE *scanLine(int y) { return image_.getScanLine(image_.getHeight() - y - 1); }
	const BYTE *scanLine(int y) const { return image_.getScanLine(image_.getHeight() - y - 1); }
	bool convertTo32Bits() {
		if (image_.convertTo32Bits())
			return true;
		return image_.convertTo8Bits() && image_.convertTo32Bits();
	}
    bool load(const std::filesystem::path& filename)
    {
        return ::load(image_, filename);
    }
    bool save(const std::filesystem::path& filename)
    {
        return ::save(image_, filename);
    }
    int depth() const { return image_.getBitsPerPixel(); }
	unsigned width() const  { return image_.getWidth(); }
	unsigned height() const { return image_.getHeight(); }
	void clear() { image_.clear(); }
	void setSize(int w, int h) { image_.setSize(FIT_BITMAP, w, h, 32); }
	const fipImageEx *getImage() const { return &image_; }
	fipImageEx *getFipImage() { return &image_; }
	Color pixel(int x, int y) const
	{
		RGBQUAD color = {0};
		color.rgbReserved = 0xFF;
		image_.getPixelColor(x, image_.getHeight() - y - 1, &color);
		return color;
	}
	bool copySubImage(Image& image, int x, int y, int x2, int y2) const
	{
		return !!image_.copySubImage(image.image_, x, y, x2, y2);
	}
	bool pasteSubImage(const Image& image, int x, int y)
	{
		return !!image_.pasteSubImage(const_cast<fipImageEx&>(image.image_), x, y);
	}
	bool rotate(double angle)
	{
		return !!image_.rotate(angle);
	}
	bool pullImageKeepingBPP(const Image& other)
	{
		unsigned bpp =  image_.getBitsPerPixel();
		RGBQUAD palette[256];
		if (image_.getPaletteSize() > 0)
			memcpy(palette, image_.getPalette(), image_.getPaletteSize());
		image_ = other.image_;
		return image_.convertColorDepth(bpp, palette);
	}
	std::map<std::string, std::string> getMetadata() const
	{
		std::map<std::string, std::string> metadata;
		fipTag tag;
		fipMetadataFind finder;
		static const struct {
			FREE_IMAGE_MDMODEL model;
			const char *name;
		} models[] = {
			{ FIMD_COMMENTS, "COMMENTS" },
			{ FIMD_EXIF_MAIN, "EXIF_MAIN" },
			{ FIMD_EXIF_EXIF, "EXIF_EXIF" },
			{ FIMD_EXIF_GPS, "EXIF_GPS" },
			{ FIMD_EXIF_MAKERNOTE, "EXIF_MAKERNOTE" },
			{ FIMD_EXIF_INTEROP, "EXIF_INTEROP" },
			{ FIMD_IPTC, "IPTC" },
			{ FIMD_XMP, "XMP" },
			{ FIMD_GEOTIFF, "GEOTIFF" },
			{ FIMD_ANIMATION, "ANIMATION" },
			{ FIMD_CUSTOM, "CUSTOM" },
			{ FIMD_EXIF_RAW, "EXIF_RAW" },
		};
		for (auto m: models)
		{
			if (finder.findFirstMetadata(m.model, const_cast<fipImageEx &>(image_), tag)) {
				do
				{
					metadata.insert_or_assign(std::string(m.name) + "/" + tag.getKey(), tag.toString(m.model));
				} while (finder.findNextMetadata(tag));
			}
		}
		return metadata;
	}

	static int valueR(Color color) { return color.rgbRed; }
	static int valueG(Color color) { return color.rgbGreen; }
	static int valueB(Color color) { return color.rgbBlue; }
	static int valueA(Color color) { return color.rgbReserved; }
	static Color Rgb(int r, int g, int b)
	{
		Color color;
		color.rgbRed = r;
		color.rgbGreen = g;
		color.rgbBlue = b;
		return color;
	}
private:
	fipImageEx image_;
};

class MultiPageImages
{
public:
	MultiPageImages() {}
	~MultiPageImages() { multi_.close(); }
	bool close() { return !!multi_.close(); }
	bool isValid() const { return !!multi_.isValid(); }
	int getPageCount() const { return multi_.getPageCount(); }
	bool load(const std::filesystem::path& filename) { return !!multi_.openU(filename); }
	bool save(const std::filesystem::path& filename) { return !!multi_.saveU(filename); }
	Image getImage(int page)
	{
		FIBITMAP *bitmaptmp, *bitmap;
		bitmaptmp = FreeImage_LockPage(multi_, page);
		bitmap = FreeImage_Clone(bitmaptmp);
		FreeImage_UnlockPage(multi_, bitmaptmp, false);
		return Image(bitmap);
	}
	void insertPage(int page, const Image& image)
	{
		fipImageEx imgAdd = image.image_;
		multi_.insertPage(page, imgAdd);
	}
	void replacePage(int page, const Image& image)
	{
		fipImageEx imgOrg, imgAdd;
		imgAdd = image.image_;
		imgOrg = multi_.lockPage(page);
		imgAdd.copyAnimationMetadata(imgOrg);
		multi_.unlockPage(imgOrg, false);
		multi_.insertPage(page, imgAdd);
		imgAdd.detach();
		multi_.deletePage(page + 1);
	}

	fipMultiPageEx multi_;
};

#endif /* IMAGE_HPP */
