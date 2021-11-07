#include "SampleImages.hpp"
#include <ImgWindow.hpp>
#include <gtkmm/application.h>
#include <gtkmm/box.h>
#include <gtkmm/spinbutton.h>
#include <gtkmm/window.h>

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
