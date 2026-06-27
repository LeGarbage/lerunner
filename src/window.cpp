#include "window.hpp"
#include "desktop-entries.hpp"
#include <gdkmm/monitor.h>
#include <gdkmm/rectangle.h>
#include <gtkmm/eventcontrollerfocus.h>
#include <gtkmm/eventcontrollerkey.h>

MainWindow::MainWindow()
    : close_button("Close") {
    signal_realize().connect([this] {
        auto monitor = get_display()->get_monitor_at_surface(get_surface());
        Gdk::Rectangle geometry;
        monitor->get_geometry(geometry);
        set_default_size(static_cast<int>(geometry.get_width() * 0.8), 0);
    });

    box.set_margin(10);
    box.set_orientation(Gtk::Orientation::VERTICAL);
    set_child(box);

    entry.signal_changed().connect(sigc::mem_fun(*this, &MainWindow::on_search_changed));
    box.append(entry);
    entry.set_placeholder_text("Search");

    for (const auto &path : DesktopEntryParser::get_desktop_entries()) {
        DesktopEntry entry{path};

        if (entry.get("Desktop Entry/NoDisplay") == "true") { continue; }

        auto &button = desktop_app_buttons.emplace_back(entry.get("Desktop Entry/Name"));

        button.signal_clicked().connect(sigc::bind(
            sigc::mem_fun(*this, &MainWindow::on_button_clicked), entry.get("Desktop Entry/Name")));
        box.append(button);
        button.set_visible(false);
        button.set_expand();
    }

    close_button.signal_clicked().connect(
        sigc::bind(sigc::mem_fun(*this, &MainWindow::on_button_clicked), "close"));
    box.append(close_button);
    close_button.set_visible(false);
    close_button.set_expand();

    auto key_controller = Gtk::EventControllerKey::create();
    key_controller->signal_key_pressed().connect(sigc::mem_fun(*this, &MainWindow::on_key_pressed),
                                                 false);
    add_controller(key_controller);

    auto focus_controller = Gtk::EventControllerFocus::create();
    focus_controller->signal_leave().connect([this] { close(); });
    add_controller(focus_controller);
}

void MainWindow::on_button_clicked(const Glib::ustring &data) {
    std::cout << "Button \"" << data << "\" pressed\n";

    if (data == "close") {
        // The focus handler already closes the window, so this prevents double closing
        unset_focus();
    }
}

void MainWindow::on_search_changed() {
    auto text = entry.get_text().lowercase();

    for (auto &button : desktop_app_buttons) {
        if (text.length() != 0 && button.get_label().lowercase().rfind(text, 0) == 0) {
            button.set_visible();
        } else {
            button.set_visible(false);
        }
    }

    if (text.length() != 0 && Glib::ustring{"close"}.rfind(text, 0) == 0) {
        close_button.set_visible();
    } else {
        close_button.set_visible(false);
    }
}

bool MainWindow::on_key_pressed(guint keyval, guint keycode, Gdk::ModifierType state) {
    if (keyval == GDK_KEY_Escape) {
        // The focus handler already closes the window, so this prevents double closing
        unset_focus();
        return true;
    }

    return false;
}
