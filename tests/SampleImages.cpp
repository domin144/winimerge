/* SPDX-FileCopyrightText: Copyright © 2024 Dominik Wójt <domin144@o2.pl>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include <SampleImages.hpp>
#include <ImgMergeBuffer.hpp>
#include <cstdint>

namespace {
RGBQUAD makeRgbQuad(std::uint8_t r, std::uint8_t g, std::uint8_t b)
{
	RGBQUAD result {};
	result.rgbRed = r;
	result.rgbGreen = g;
	result.rgbBlue = b;
	result.rgbReserved = 0xff;
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

Image makeCircles(const std::vector<Circle>& circles)
{
	const int width = 457;
	const int height = 561;
	Image result(width, height);

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
			result.setPixel(ix, height - iy - 1, lightGrey);
			for (auto iter = circles.rbegin(); iter < circles.rend(); ++iter)
			{
				auto& circle = *iter;
				if (isInCircle({ix, iy}, circle))
				{
					RGBQUAD color = circle.color;
					result.setPixel(ix, height - iy - 1, color);
				}
			}
		}
	}

	return result;
}
}

/* y coordinate counted from bottom to top */
Image makeButterFly()
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

Image makeButterFly2()
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
