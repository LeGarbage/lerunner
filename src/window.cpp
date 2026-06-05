#include "window.hpp"
#include <gtkmm/eventcontrollerkey.h>
#include <gtkmm/eventcontrollerfocus.h>
#include <iostream>

MainWindow::MainWindow()
    : button1("Button 1"),
      button2("Button 2"),
      close_button("Close") {
    box.set_margin(10);
    box.set_orientation(Gtk::Orientation::VERTICAL);

    set_child(box);

    entry.signal_changed().connect(sigc::mem_fun(*this, &MainWindow::on_search_changed));
    box.append(entry);
    entry.set_placeholder_text("Search");

    // button1.signal_clicked().connect(
    //     sigc::bind(sigc::mem_fun(*this, &MainWindow::on_button_clicked), "button 1"));
    // box.append(button1);
    // button1.set_expand();

    // button2.signal_clicked().connect(
    //     sigc::bind(sigc::mem_fun(*this, &MainWindow::on_button_clicked), "button 2"));
    // box.append(button2);
    // button2.set_expand();

    close_button.signal_clicked().connect(
        sigc::bind(sigc::mem_fun(*this, &MainWindow::on_button_clicked), "close"));
    box.append(close_button);
    close_button.set_visible(false);
    close_button.set_expand();

    auto key_controller = Gtk::EventControllerKey::create();
    key_controller->signal_key_pressed().connect(sigc::mem_fun(*this, &MainWindow::on_key_pressed), false);
    add_controller(key_controller);

    auto focus_controller = Gtk::EventControllerFocus::create();
    focus_controller->signal_leave().connect([this] { close(); });
    add_controller(focus_controller);
}

void MainWindow::on_button_clicked(const Glib::ustring &data) {
    std::cout << "Button \"" << data << "\" pressed\n";

    if (data == "close") { close(); }
}

void MainWindow::on_search_changed() {
    auto text = entry.get_text();
    if (text.length() != 0 && Glib::ustring{"close"}.rfind(text, 0) == 0) {
        close_button.set_visible();
    } else {
        close_button.set_visible(false);
    }
}

bool MainWindow::on_key_pressed(guint keyval, guint keycode, Gdk::ModifierType state) {
    if (keyval == GDK_KEY_Escape) {
        close();
        return true;
    }

    return false;
}
