#include <ImgWindow.hpp>
#include <gtkmm/application.h>
#include <gtkmm/window.h>

int main(const int argc, char* argv[])
{
	auto app = Gtk::Application::create("winimerge.example.imgwindow");
	Gtk::Window window;
	window.set_default_size(200, 100);
	CImgWindow img_window;
	window.add(img_window);

	const int input_width = 10;
	const int input_height = 8;
	const char* bitmap = "00******00" //
						 "0********0" //
						 "**==**==**" //
						 "**==**==**" //
						 "**********" //
						 "*===**===*" //
						 "0**====**0" //
						 "00******00";
	constexpr int width = 200;
	constexpr int height = 120;
	fipImage image(FIT_BITMAP, width, height, 24);
	RGBQUAD black = {0x00, 0x00, 0x00, 0x00};
	RGBQUAD yellow = {0x22, 0x77, 0x77, 0x00};
	RGBQUAD blue = {0x77, 0x22, 0x22, 0x00};
	for (int iy = 0; iy < height; ++iy)
	{
		for (int ix = 0; ix < width; ++ix)
		{
			switch (bitmap
						[(ix * input_width / width)
						 + (iy * input_height / height) * input_width])
			{
			case '0':
				image.setPixelColor(ix, height - iy - 1, &black);
				break;
			case '*':
				image.setPixelColor(ix, height - iy - 1, &yellow);
				break;
			case '=':
				image.setPixelColor(ix, height - iy - 1, &blue);
				break;
			default:
				image.setPixelColor(ix, height - iy - 1, &black);
				break;
			}
		}
	}
	image.save("test.png");

	img_window.SetImage(&image);

	return app->run(window, argc, argv);
}
