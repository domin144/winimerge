#include <ImgWindow.hpp>
#include <gtkmm/application.h>
#include <gtkmm/box.h>
#include <gtkmm/spinbutton.h>
#include <gtkmm/window.h>

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

fipImage makeButterFly()
{
	const int width = 457;
	const int height = 561;
	fipImage result(FIT_BITMAP, width, height, 24);

	struct Circle {
		POINT centre;
		POINT radius;
		RGBQUAD color;
	};

	auto isInCircle = [](const POINT &point, const Circle& circle) {
		auto square = [](int x) { return x * x; };
		return square((point.x - circle.centre.x) * circle.radius.y)
			+ square((point.y - circle.centre.y) * circle.radius.x)
			<= square(circle.radius.x * circle.radius.y);
	};

	std::vector<Circle> circles = {{{83, 230}, {13, 15}, red},
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
								   {{119, 267}, {2, 8}, grey}
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
					result.setPixelColor(ix, iy, &circle.color);
			}
		}
	}

	return result;
}
}

int main(const int argc, char* argv[])
{
	auto app = Gtk::Application::create("winimerge.example.imgwindow");
	Gtk::Window window;
	window.set_default_size(200, 100);

    Gtk::Box box(Gtk::ORIENTATION_VERTICAL);
    window.add(box);

    CImgWindow imgWindow;
    box.pack_start(imgWindow, Gtk::PACK_EXPAND_WIDGET);

    Gtk::SpinButton zoom {Gtk::Adjustment::create(1, 0.001, 1000, 0.1, 1, 1)};
    zoom.set_digits(3);
    zoom.signal_value_changed().connect(sigc::track_obj(
        [&zoom, &imgWindow]() { imgWindow.SetZoom(zoom.get_value()); },
        imgWindow));
    box.pack_start(zoom, Gtk::PACK_SHRINK);

	fipImage image = makeButterFly();
	image.save("test.png");

	auto imageGdk = fipToGdkPixbuf(image);
	imageGdk->save("test_gdk.png", "png");

    imgWindow.SetImage(&image);

    window.show_all();

	return app->run(window, argc, argv);
}
