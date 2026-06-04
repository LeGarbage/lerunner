#include "window.hpp"
#include <giomm/application.h>
#include <gtkmm/application.h>
#include <gtk4-layer-shell.h>

int main(int argc, char* argv[]) {
    auto app = Gtk::Application::create("org.gtkmm.example");

    MainWindow window{};

    app->signal_activate().connect([&window, &app] {
        gtk_layer_init_for_window(window.gobj());
        gtk_layer_set_layer(window.gobj(), GTK_LAYER_SHELL_LAYER_OVERLAY);

        app->add_window(window);
        window.present();
    });

    return app->run(argc, argv);
}
