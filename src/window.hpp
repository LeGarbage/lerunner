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
    void on_button_clicked(Entry *entry);
    void on_search_changed();
    bool on_key_pressed(guint keyval, guint keycode, Gdk::ModifierType state);
    Gtk::Box m_box;
    std::vector<Gtk::Button> m_desktop_app_buttons;
    Gtk::Entry m_entry;
    std::vector<std::unique_ptr<Plugin>> m_plugins;
};
