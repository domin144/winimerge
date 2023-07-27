#include "SampleImages.hpp"
#include <ImgMergeWindow.hpp>
#include <ImgToolWindow.hpp>
#include <gtkmm/application.h>
#include <gtkmm/box.h>
#include <gtkmm/spinbutton.h>
#include <gtkmm/window.h>

int main(const int argc, char* argv[])
{
	auto app = Gtk::Application::create("winimerge.example.imgmergewindow");
	Gtk::Window window;
	window.set_default_size(200, 100);

    Gtk::Box box(Gtk::ORIENTATION_VERTICAL);
    window.add(box);

    CImgMergeWindow imgMergeWindow;
    box.pack_start(imgMergeWindow, Gtk::PACK_EXPAND_WIDGET);



    window.show_all();

    const Image butterFly0{makeButterFly()};
    const Image butterFly1{makeButterFly2()};

    imgMergeWindow.NewImages(2, 1, 557, 611);
    imgMergeWindow.GetImage(0)->pasteSubImage(butterFly0, 12, 13);
    imgMergeWindow.GetImage(1)->pasteSubImage(butterFly1, 12, 13);

    return app->run(window, argc, argv);
}
