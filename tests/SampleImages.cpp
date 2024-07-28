/* SPDX-FileCopyrightText: Copyright © 2024 Dominik Wójt <domin144@o2.pl>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include <SampleImages.hpp>
#include <ImgMergeBuffer.hpp>
#include <cstdint>

namespace {
Image::Color makeRgbQuad(const std::uint8_t r, const std::uint8_t g, const std::uint8_t b)
{
	return {
		static_cast<float>(r/255.0),
		static_cast<float>(g/255.0),
		static_cast<float>(b/255.0),
		1.0};
};

Image::Color black = makeRgbQuad(0x00, 0x00, 0x00);
Image::Color yellow = makeRgbQuad(0x22, 0x77, 0x77);
Image::Color blue = makeRgbQuad(0x77, 0x22, 0x22);
Image::Color red = makeRgbQuad(0xff, 0x00, 0x00);
Image::Color cyan = makeRgbQuad(0x00, 0xff, 0xff);
Image::Color magenta = makeRgbQuad(0xff, 0x00, 0xff);
Image::Color grey = makeRgbQuad(0x80, 0x80, 0x80);
Image::Color lightGrey = makeRgbQuad(0xd0, 0xd0, 0xd0);

struct Circle
{
	POINT centre;
	POINT radius;
	Image::Color color;
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
					result.setPixel(ix, height - iy - 1, circle.color);
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
