#pragma once

#include "plugins.hpp"
#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/entry.h>
#include <gtkmm/window.h>
#include <memory>
#include <vector>

class MainWindow : public Gtk::Window {
    public:
    MainWindow();

    private:
    void on_button_clicked(const std::string &data);
    void on_search_changed();
    bool on_key_pressed(guint keyval, guint keycode, Gdk::ModifierType state);
    Gtk::Box box;
    std::vector<Gtk::Button> desktop_app_buttons;
    Gtk::Entry entry;
    std::vector<std::unique_ptr<Plugin>> plugins;
};
