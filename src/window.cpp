#include "window.hpp"
#include "plugins/desktop-entries.hpp"
#include <gdkmm/monitor.h>
#include <gdkmm/rectangle.h>
#include <gtkmm/eventcontrollerfocus.h>
#include <gtkmm/eventcontrollerkey.h>
#include <iostream>

MainWindow::MainWindow() {
    set_default_size(640, 0);

    box.set_orientation(Gtk::Orientation::VERTICAL);
    set_child(box);

    entry.signal_changed().connect(sigc::mem_fun(*this, &MainWindow::on_search_changed));
    box.append(entry);
    entry.set_placeholder_text("Search");

    plugins.push_back(std::make_unique<DesktopEntries>());

    for (const auto& entry : plugins[0]->get_entries("S")) {
        auto &button = desktop_app_buttons.emplace_back(entry->display());

        button.signal_clicked().connect([this, entry] {
            entry->selected();
            unset_focus();
        });
        box.append(button);
        // button.set_visible(false);
        button.set_expand();
    }

    auto key_controller = Gtk::EventControllerKey::create();
    key_controller->signal_key_pressed().connect(sigc::mem_fun(*this, &MainWindow::on_key_pressed),
                                                 false);
    add_controller(key_controller);

    auto focus_controller = Gtk::EventControllerFocus::create();
    focus_controller->signal_leave().connect([this] { close(); });
    add_controller(focus_controller);
}

void MainWindow::on_button_clicked(const std::string &data) {
    std::cout << "Button \"" << data << "\" pressed\n";
    unset_focus();
}

void MainWindow::on_search_changed() {
    // FIX: WTF??
    //
    // auto text = entry.get_text().lowercase();
    // for (auto &button : desktop_app_buttons) {
    //     box.remove(button);
    // }
    //
    // desktop_app_buttons.clear();
    //
    // for (auto &button : desktop_app_buttons) {
    //     if (text.length() != 0 && button.get_label().lowercase().rfind(text, 0) == 0) {
    //         button.set_visible();
    //     } else {
    //         button.set_visible(false);
    //     }
    // }
    // for (auto *const entry : plugins[0]->get_entries(text)) {
    //     // if (entry.get("Desktop Entry/NoDisplay") == "true") { continue; }
    //
    //     auto &button = desktop_app_buttons.emplace_back(entry->display());
    //
    //     button.signal_clicked().connect([&entry]() { entry->selected(); });
    //     box.append(button);
    //     button.set_expand();
    // }
}

bool MainWindow::on_key_pressed(guint keyval, guint keycode, Gdk::ModifierType state) {
    if (keyval == GDK_KEY_Escape) {
        // The focus handler already closes the window, so this prevents double closing
        unset_focus();
        return true;
    }

    return false;
}
