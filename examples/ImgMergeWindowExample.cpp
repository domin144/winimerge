#include <ImgMergeWindow.hpp>
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

    CImgMergeWindow imgMergeWindow;
    box.pack_start(imgMergeWindow, Gtk::PACK_EXPAND_WIDGET);

    window.show_all();

    return app->run(window, argc, argv);
}
