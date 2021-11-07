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

#include "SampleImages.hpp"
#include <ImgMergeBuffer.hpp>
#include <cstdint>

namespace {
RGBQUAD makeRgbQuad(std::uint8_t r, std::uint8_t g, std::uint8_t b)
{
	RGBQUAD result {};
	result.rgbRed = r;
	result.rgbGreen = g;
	result.rgbBlue = b;
	return result;
};

RGBQUAD black = makeRgbQuad(0x00, 0x00, 0x00);
RGBQUAD yellow = makeRgbQuad(0x22, 0x77, 0x77);
RGBQUAD blue = makeRgbQuad(0x77, 0x22, 0x22);
RGBQUAD red = makeRgbQuad(0xff, 0x00, 0x00);
RGBQUAD cyan = makeRgbQuad(0x00, 0xff, 0xff);
RGBQUAD magenta = makeRgbQuad(0xff, 0x00, 0xff);
RGBQUAD grey = makeRgbQuad(0x80, 0x80, 0x80);
RGBQUAD lightGrey = makeRgbQuad(0xd0, 0xd0, 0xd0);

struct Circle
{
	POINT centre;
	POINT radius;
	RGBQUAD color;
};

fipImage makeCircles(const std::vector<Circle>& circles)
{
	const int width = 457;
	const int height = 561;
	fipImage result(FIT_BITMAP, width, height, 24);

	auto isInCircle = [](const POINT& point, const Circle& circle) {
		auto square = [](int x) { return x * x; };
		return square((point.x - circle.centre.x) * circle.radius.y)
			+ square((point.y - circle.centre.y) * circle.radius.x)
			<= square(circle.radius.x * circle.radius.y);
	};

	for (int iy = 0; iy < height; ++iy)
	{
		for (int ix = 0; ix < width; ++ix)
		{
			result.setPixelColor(ix, iy, &lightGrey);
			for (auto iter = circles.rbegin(); iter < circles.rend(); ++iter)
			{
				auto& circle = *iter;
				if (isInCircle({ix, iy}, circle))
				{
					RGBQUAD color = circle.color;
					result.setPixelColor(ix, iy, &color);
				}
			}
		}
	}

	return result;
}
}

fipImage makeButterFly()
{
	std::vector<Circle> circles = {
		{{83, 230}, {13, 15}, red},
		{{156, 230}, {12, 15}, red},
		{{91, 171}, {13, 13}, magenta},
		{{150, 176}, {13, 11}, magenta},
		{{80, 215}, {31, 42}, cyan},
		{{156, 215}, {31, 42}, cyan},
		{{87, 170}, {26, 38}, cyan},
		{{153, 181}, {28, 35}, cyan},
		{{118, 180}, {7, 52}, black},
		{{117, 244}, {9, 15}, black},
		{{113, 266}, {3, 10}, grey},
		{{119, 267}, {2, 8}, grey}};
	return makeCircles(circles);
}

fipImage makeButterFly2()
{
	std::vector<Circle> circles = {
		{{83, 220}, {13, 13}, red},
		{{156, 220}, {12, 13}, red},
		{{91, 171}, {13, 13}, magenta},
		{{150, 176}, {13, 11}, magenta},
		{{80, 215}, {31, 42}, cyan},
		{{156, 215}, {31, 42}, cyan},
		{{87, 170}, {26, 38}, cyan},
		{{153, 181}, {28, 35}, cyan},
		{{118, 180}, {7, 52}, black},
		{{117, 244}, {9, 15}, black},
		{{113, 266}, {4, 10}, grey},
		{{119, 267}, {4, 8}, grey}};
	return makeCircles(circles);
}
