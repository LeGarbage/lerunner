#pragma once

#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/entry.h>
#include <gtkmm/window.h>
#include <vector>

class MainWindow : public Gtk::Window {
    public:
    MainWindow();

    private:
    void on_button_clicked(const Glib::ustring &data);
    void on_search_changed();
    bool on_key_pressed(guint keyval, guint keycode, Gdk::ModifierType state);
    Gtk::Box box;
    std::vector<Gtk::Button> desktop_app_buttons;
    Gtk::Button close_button;
    Gtk::Entry entry;
};
