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

#include <ImgWindow.hpp>

Glib::RefPtr<Gdk::Pixbuf> fipToGdkPixbuf(const fipImage &image)
{
	/* TODO: format conversion, vertical flip */
	const int bitsPerSample = 8;
	const int bytesPerSample = bitsPerSample / 8;
	auto result = Gdk::Pixbuf::create(
				Gdk::COLORSPACE_RGB, true, bitsPerSample, image.getWidth(), image.getHeight());
	std::uint8_t *const resultPixels = result->get_pixels();
	for (int y = 0; y < image.getHeight(); ++y)
	{
		for (int x = 0; x < image.getWidth(); ++x)
		{
			std::uint8_t* const resultPixel =
					&resultPixels
					[y * result->get_rowstride()
					+ x * bytesPerSample * result->get_n_channels()];
			RGBQUAD color{};
			image.getPixelColor(x, image.getHeight() - y - 1, &color);
			resultPixel[0] = color.rgbRed;
			resultPixel[1] = color.rgbGreen;
			resultPixel[2] = color.rgbBlue;
			resultPixel[3] = 0xff;
		}
	}
	return result;
}

int CImgWindow::getHScrollPos() const
{
	return static_cast<int>(m_scrolledWindow.get_hadjustment()->get_value());
}

int CImgWindow::getVScrollPos() const
{
	return static_cast<int>(m_scrolledWindow.get_vadjustment()->get_value());
}
