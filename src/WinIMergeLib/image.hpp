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

#pragma once
#pragma warning(disable: 4819)

#include <boost/nowide/convert.hpp>
#include <OpenImageIO/imageio.h>
#include <OpenImageIO/typedesc.h>
#include <OpenImageIO/imagebuf.h>
#include <OpenImageIO/imagebufalgo.h>
// #include "FreeImagePlus.h"
#include <algorithm>
#include <filesystem>
#include <string>
#include <map>
#include <array>

// class fipImageEx
// {
// private:
// 	OIIO::ImageBuf image_;
// public:
// 	fipImageEx(OIIO::TypeDesc image_type = FIT_BITMAP, unsigned width = 0, unsigned height = 0, unsigned bpp = 0)
// 		: fipWinImage(image_type, width, height, bpp) {}
// 	fipImageEx(const fipImageEx& Image) { *this = Image; }
// 	explicit fipImageEx(const fipWinImage& Image) { *this = Image; }
// 	explicit fipImageEx(FIBITMAP *bitmap) { *this = bitmap; }
// 	virtual ~fipImageEx() {}

// 	fipImageEx& operator=(const fipImageEx& Image)
// 	{
// 		if (this != &Image)
// 		{
// 			FIBITMAP *clone = FreeImage_Clone(static_cast<FIBITMAP*>(Image._dib));
// 			replace(clone);
// 			_fif = Image._fif;
// 		}
// 		return *this;
// 	}

// 	fipImageEx& operator=(const fipWinImage& Image)
// 	{
// 		if (this != &Image)
// 		{
// 			FIBITMAP *clone = FreeImage_Clone(static_cast<FIBITMAP*>(const_cast<fipWinImage&>(Image)));
// 			replace(clone);
// 			_fif = Image.getFIF();
// 		}
// 		return *this;
// 	}

// 	fipImageEx& operator=(FIBITMAP *dib)
// 	{
// 		if (_dib != dib)
// 			replace(dib);
// 		return *this;
// 	}

// 	void swap(fipImageEx& other)
// 	{
// 		std::swap(_dib, other._dib);
// 		std::swap(this->_fif, other._fif);
// 		std::swap(this->_bHasChanged, other._bHasChanged);
// 	}

// 	FIBITMAP *detach()
// 	{
// 		FIBITMAP *dib = _dib;
// 		_dib = NULL;
// 		clear();
// 		return dib;
// 	}

// 	BOOL colorQuantizeEx(FREE_IMAGE_QUANTIZE quantize = FIQ_WUQUANT, int PaletteSize = 256, int ReserveSize = 0, RGBQUAD *ReservePalette = NULL)
// 	{
// 		if(_dib) {
// 			FIBITMAP *dib8 = FreeImage_ColorQuantizeEx(_dib, quantize, PaletteSize, ReserveSize, ReservePalette);
// 			return !!replace(dib8);
// 		}
// 		return false;
// 	}

// 	bool convertColorDepth(unsigned bpp, RGBQUAD *pPalette = NULL)
// 	{
// 		switch (bpp)
// 		{
// 		case 1:
// 			return !!threshold(128);
// 		case 4:
// 		{
// 			fipImageEx tmp = *this;
// 			tmp.convertTo24Bits();
// 			if (pPalette)
// 				tmp.colorQuantizeEx(FIQ_NNQUANT, 16, 16, pPalette);
// 			else
// 				tmp.colorQuantizeEx(FIQ_WUQUANT, 16);
// 			setSize(tmp.getImageType(), tmp.getWidth(), tmp.getHeight(), 4);
// 			for (unsigned y = 0; y < tmp.getHeight(); ++y)
// 			{
// 				const BYTE *line_src = tmp.getScanLine(y);
// 				BYTE *line_dst = getScanLine(y);
// 				for (unsigned x = 0; x < tmp.getWidth(); ++x)
// 					line_dst[x / 2] |= ((x % 2) == 0) ? (line_src[x] << 4) : line_src[x];
// 			}

// 			RGBQUAD *rgbq_dst = getPalette();
// 			RGBQUAD *rgbq_src = pPalette ? pPalette : tmp.getPalette();
// 			memcpy(rgbq_dst, rgbq_src, sizeof(RGBQUAD) * 16);
// 			return true;
// 		}
// 		case 8:
// 			convertTo24Bits();
// 			if (pPalette)
// 				return !!colorQuantizeEx(FIQ_NNQUANT, 256, 256, pPalette);
// 			else
// 				return !!colorQuantizeEx(FIQ_WUQUANT, 256);
// 		case 15:
// 			return !!convertTo16Bits555();
// 		case 16:
// 			return !!convertTo16Bits565();
// 		case 24:
// 			return !!convertTo24Bits();
// 		default:
// 		case 32:
// 			return !!convertTo32Bits();
// 		}
// 	}

// 	void copyAnimationMetadata(fipImage& src)
// 	{
// 		fipTag tag;
// 		fipMetadataFind finder;
// 		if (finder.findFirstMetadata(FIMD_ANIMATION, src, tag))
// 		{
// 			do
// 			{
// 				setMetadata(FIMD_ANIMATION, tag.getKey(), tag);
// 			} while (finder.findNextMetadata(tag));
// 		}
// 	}
// };

// class fipMultiPageEx : public fipMultiPage
// {
// public:
// 	explicit fipMultiPageEx(BOOL keep_cache_in_memory = FALSE)
// 		: fipMultiPage(keep_cache_in_memory)
// 		, m_handle(NULL)
// 	{
// 	}

// 	BOOL openU(const wchar_t* lpszPathName, BOOL create_new, BOOL read_only, int flags = 0)
// 	{
// 		FILE *fp = NULL;
// #ifdef _WIN32
// 		_wfopen_s(&fp, lpszPathName, L"r+b");
// 		if (fp != NULL && !read_only)
// 		{
// 			WCHAR szTempPathName[MAX_PATH];
// 			DWORD cchTempPath = GetTempPathW(_countof(szTempPathName), szTempPathName);
// 			if (cchTempPath <= MAX_PATH - 40)
// 			{
// 				GUID guid;
// 				CoCreateGuid(&guid);
// 				StringFromGUID2(guid, szTempPathName + cchTempPath, 40);
// 				FILE *fpTmp = NULL;
// 				// Create file with "D" option so it vanishes when closed
// 				_wfopen_s(&fpTmp, szTempPathName, L"w+bD");
// 				if (fpTmp != NULL)
// 				{
// 					char buffer[16384];
// 					while (size_t bytes = fread(buffer, 1, sizeof buffer, fp))
// 					{
// 						if (fwrite(buffer, 1, bytes, fpTmp) != bytes)
// 						{
// 							fclose(fpTmp);
// 							fpTmp = NULL;
// 							break;
// 						}
// 					}
// 					fclose(fp);
// 					fp = fpTmp;
// 				}
// 			}
// 		}
// #else
// 		char filename[260];
// 		snprintf(filename, sizeof(filename), "%ls", lpszPathName);
// 		fp = fopen(filename, "r+b");
// #endif
// 		if (fp != NULL)
// 		{
// 			FreeImageIO io;
// 			io.read_proc = myReadProc;
// 			io.write_proc = myWriteProc;
// 			io.seek_proc = mySeekProc;
// 			io.tell_proc = myTellProc;
// 			FREE_IMAGE_FORMAT fif = fipImage::identifyFIFU(lpszPathName);
// 			_mpage = FreeImage_OpenMultiBitmapFromHandle(fif, &io, fp, flags);
// 			if (_mpage != NULL)
// 			{
// 				m_handle = fp;
// 			}
// 			else
// 			{
// 				fclose(fp);
// 			}
// 		}
// 		return _mpage != NULL;
// 	}

// 	BOOL openU(const wchar_t* lpszPathName, BOOL create_new, BOOL read_only, int flags = 0)
// 	{
// 		FILE *fp = NULL;
// #ifdef _WIN32
// 		_wfopen_s(&fp, lpszPathName, L"rb");
// 		if (fp != NULL && !read_only)
// 		{
// 			WCHAR szTempPathName[MAX_PATH];
// 			DWORD cchTempPath = GetTempPathW(_countof(szTempPathName), szTempPathName);
// 			if (cchTempPath <= MAX_PATH - 40)
// 			{
// 				GUID guid;
// 				CoCreateGuid(&guid);
// 				StringFromGUID2(guid, szTempPathName + cchTempPath, 40);
// 				FILE *fpTmp = NULL;
// 				// Create file with "D" option so it vanishes when closed
// 				_wfopen_s(&fpTmp, szTempPathName, L"w+bD");
// 				if (fpTmp != NULL)
// 				{
// 					char buffer[16384];
// 					while (size_t bytes = fread(buffer, 1, sizeof buffer, fp))
// 					{
// 						if (fwrite(buffer, 1, bytes, fpTmp) != bytes)
// 						{
// 							fclose(fpTmp);
// 							fpTmp = NULL;
// 							break;
// 						}
// 					}
// 					fclose(fp);
// 					fp = fpTmp;
// 				}
// 			}
// 		}
// #else
// 		char filename[260];
// 		snprintf(filename, sizeof(filename), "%ls", lpszPathName);
// 		fp = fopen(filename, "rb");
// #endif
// 		if (fp != NULL)
// 		{
// 			FreeImageIO io;
// 			io.read_proc = myReadProc;
// 			io.write_proc = myWriteProc;
// 			io.seek_proc = mySeekProc;
// 			io.tell_proc = myTellProc;
// 			FREE_IMAGE_FORMAT fif = fipImage::identifyFIFU(lpszPathName);
// 			_mpage = FreeImage_OpenMultiBitmapFromHandle(fif, &io, fp, flags);
// 			if (_mpage != NULL)
// 			{
// 				m_handle = fp;
// 			}
// 			else
// 			{
// 				fclose(fp);
// 			}
// 		}
// 		return _mpage != NULL;
// 	}

// 	bool saveU(const wchar_t* lpszPathName, int flag = 0) const
// 	{
// 		FILE *fp = NULL;
// #ifdef _WIN32
// 		_wfopen_s(&fp, lpszPathName, L"w+b");
// #else
// 		char filename[260];
// 		snprintf(filename, sizeof(filename), "%ls", lpszPathName);
// 		fp = fopen(filename, "w+b");
// #endif
// 		if (!fp)
// 			return false;
// 		FreeImageIO io;
// 		io.read_proc  = myReadProc;
// 		io.write_proc = myWriteProc;
// 		io.seek_proc  = mySeekProc;
// 		io.tell_proc  = myTellProc;
// 		FREE_IMAGE_FORMAT fif = fipImage::identifyFIFU(lpszPathName);
// 		bool result = !!saveToHandle(fif, &io, (fi_handle)fp, flag);
// 		fclose(fp);
// 		return result;
// 	}

// private:
// 	FILE *m_handle; // Refers to temporary copy of original file

// 	static unsigned DLL_CALLCONV myReadProc(void *buffer, unsigned size, unsigned count, fi_handle handle) {
// 		return (unsigned)fread(buffer, size, count, (FILE *)handle);
// 	}

// 	static unsigned DLL_CALLCONV myWriteProc(void *buffer, unsigned size, unsigned count, fi_handle handle) {
// 		return (unsigned)fwrite(buffer, size, count, (FILE *)handle);
// 	}

// 	static int DLL_CALLCONV mySeekProc(fi_handle handle, long offset, int origin) {
// 		return fseek((FILE *)handle, offset, origin);
// 	}

// 	static long DLL_CALLCONV myTellProc(fi_handle handle) {
// 		return ftell((FILE *)handle);
// 	}
// };

class MultiPageImages;

class Image
{
	friend MultiPageImages;
public:
	static constexpr int channelsCount = 4;
	using Color = std::array<float, channelsCount>;
	Image() {}
	Image(int w, int h) : image_(OIIO::ImageSpec{w, h, channelsCount}) {}
	Image(const Image& other) = default;
	Image(Image&& other) = default;
	Image(const OIIO::ImageBuf& image) : image_(image) {}
	Image(OIIO::ImageBuf&& image) : image_(std::move(image)) {}
	bool load(const std::filesystem::path& filename)
	{
		image_.reset(filename.u8string());
		return true;
	}
	bool isSaveSupported() const { return true; }
	bool save(const std::filesystem::path& filename)
	{
		return image_.write(filename.u8string());
	}
	bool save(const std::string& filename)
	{
		return image_.write(filename);
	}
	int depth() const { return image_.spec().channel_bytes() * 8; }
	unsigned width() const  { return image_.spec().width; }
	unsigned height() const { return image_.spec().height; }
	void clear() { image_.reset(); }
	void setSize(int w, int h)
	{
		image_.reset(OIIO::ImageSpec{w, h, channelsCount});
	}
	const OIIO::ImageBuf *getImage() const { return &image_; }
	OIIO::ImageBuf *getFipImage() { return &image_; }
	Color pixel(int x, int y) const
	{
		Color color{0, 0, 0, 1};
		image_.getpixel(x, height() - y - 1, color.data(), channelsCount);
		return color;
	}
	void setPixel(int x, int y, Color color)
	{
		image_.setPixelColor(x, image_.getHeight() - y - 1, &color);
	}
	bool copySubImage(Image& image, int x, int y, int x2, int y2) const
	{
		image.image_ = OIIO::ImageBufAlgo::cut(image_, OIIO::ROI{x, x2, y, y2});
		return true;
	}
	bool pasteSubImage(const Image& image, int x, int y)
	{
		return OIIO::ImageBufAlgo::paste(image_, x, y, 0, 0, image.image_);
	}
	bool rotate(const double angle)
	{
		const double rem = std::fmod(angle, 360);
		const double normalizedAngle = rem < 0 ? rem + 360 : rem;
		if (normalizedAngle == 0)
		{
			return true;
		}
		else if (normalizedAngle == 90)
		{
			image_ = OIIO::ImageBufAlgo::rotate90(image_);
			return true;
		}
		else if (normalizedAngle == 180)
		{
			image_ = OIIO::ImageBufAlgo::rotate180(image_);
			return true;
		}
		else if (normalizedAngle == 270)
		{
			image_ = OIIO::ImageBufAlgo::rotate270(image_);
			return true;
		}
		return false;
	}
	bool flipHorizontal()
	{
		image_ = OIIO::ImageBufAlgo::flop(image_);
		return true;
	}
	bool flipVertical()
	{
		image_ = OIIO::ImageBufAlgo::flip(image_);
		return true;
	}
	bool pullImageKeepingBPP(const Image& other)
	{
		image_ = OIIO::ImageBufAlgo::copy(other.image_, image_.spec().format);
		return true;
	}
	std::map<std::string, std::string> getMetadata() const
	{
		std::map<std::string, std::string> metadata;
		for (const auto &attribute : image_.spec().extra_attribs)
		{
			metadata[attribute.name().string()] = attribute.get_string();
		}
		return metadata;
	}

	void copyFromBitmap(const HBITMAP bitmap)
	{
		image_.reset(OIIO::ImageSpec{});
	}

	static float valueR(Color color) { return color[0]; }
	static float valueG(Color color) { return color[1]; }
	static float valueB(Color color) { return color[2]; }
	static float valueA(Color color) { return color[3]; }
	static Color Rgb(const float r, const float g, const float b)
	{
		return {r, g, b, 0};
	}
private:
	OIIO::ImageBuf image_;
};

class MultiPageImages
{
public:
	MultiPageImages() {}
	~MultiPageImages() {}
	bool close() { multi_.clear(); return true; }
	bool isValid() const { return !multi_.empty(); }
	
	int getPageCount() const { return multi_.size(); }
	bool load(const std::filesystem::path& filename)
	{
		OIIO::ImageBuf image(filename);
		int subimageIndex = 0;
		while (image.read(subimageIndex))
		{
			multi_.emplace_back(OIIO::ImageBufAlgo::copy(image));
			++subimageIndex;
		}
		return subimageIndex;
	}
	bool save(const std::filesystem::path& filename)
	{
		if (multi_.empty())
			return false;

		std::vector<OIIO::ImageSpec> specs;
		std::transform(
			multi_.begin(),
			multi_.end(),
			std::back_inserter(specs),
			[](const auto &x) { return x.spec(); });

		auto output = OIIO::ImageOutput::create(filename);
		output->open(filename, specs.size(), specs.data());
		const bool result = multi_.front().write(output.get());
		if (!result)
			return false;
		for (auto iter = multi_.begin() + 1; iter != multi_.end(); ++iter)
		{
			const bool result =
				output->open(
					filename,
					iter->spec(), OIIO::ImageOutput::AppendSubimage)
				&& iter->write(output.get());
			if (!result)
				return false;
		}
		return true;
	}
	Image getImage(int page)
	{
		return Image{multi_[page]};
	}
	void insertPage(int page, const Image& image)
	{
		multi_.insert(multi_.begin() + page, *image.getImage());
	}
	void replacePage(int page, const Image& image)
	{
		multi_[page] = *image.getImage();
	}

	std::vector<OIIO::ImageBuf> multi_;
};
