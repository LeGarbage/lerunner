#include "window.hpp"
#include <giomm/application.h>
#include <gtk4-layer-shell.h>
#include <gtkmm/application.h>

#define FLOATING

int main(int argc, char *argv[]) {
    auto app = Gtk::Application::create("org.gtkmm.example");
    MainWindow window;

    app->signal_activate().connect([&window, &app] {
#ifdef FLOATING
        gtk_layer_init_for_window(window.gobj());
        gtk_layer_set_layer(window.gobj(), GTK_LAYER_SHELL_LAYER_OVERLAY);
        gtk_layer_set_keyboard_mode(window.gobj(), GTK_LAYER_SHELL_KEYBOARD_MODE_EXCLUSIVE);
#endif // FLOATING

        app->add_window(window);
        window.present();
    });

    return app->run(argc, argv);
}
