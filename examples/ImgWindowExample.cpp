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

	const char* bitmap = "00****00" //
						 "0******0" //
						 "*==**==*" //
						 "*==**==*" //
						 "********" //
						 "*==**==*" //
						 "0*====*0" //
						 "00****00";
	constexpr int width = 8;
	constexpr int height = 8;
	std::vector<std::uint8_t> pixels;
	pixels.reserve(width * height * 3);
	for (int i = 0; i < width * height; ++i)
	{
		switch (bitmap[i])
		{
		case '0':
			pixels.push_back(0x00);
			pixels.push_back(0x00);
			pixels.push_back(0x00);
			break;
		case '*':
			pixels.push_back(0x77);
			pixels.push_back(0x77);
			pixels.push_back(0x22);
			break;
		case '=':
			pixels.push_back(0x22);
			pixels.push_back(0x22);
			pixels.push_back(0x77);
			break;
		default:
			pixels.push_back(0x00);
			pixels.push_back(0x00);
			pixels.push_back(0x00);
			break;
		}
	}

	static_assert (width % 8 == 0, "need to consider FreeImage scan line");
	fipImage image(FIT_BITMAP, width, height, 24);
	std::copy(pixels.begin(), pixels.end(), image.accessPixels());

	img_window.SetImage(&image);

	return app->run(window, argc, argv);
}
