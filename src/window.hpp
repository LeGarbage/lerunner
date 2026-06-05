#pragma once

#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/entry.h>
#include <gtkmm/window.h>

class MainWindow : public Gtk::Window {
    public:
    MainWindow();

    private:
    void on_button_clicked(const Glib::ustring &data);
    void on_search_changed();
    bool on_key_pressed(guint keyval, guint keycode, Gdk::ModifierType state);
    Gtk::Box box;
    Gtk::Button button1, button2, close_button;
    Gtk::Entry entry;
};
