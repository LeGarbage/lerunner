#pragma once

#include "plugins.hpp"
#include <cstddef>
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
    void on_search_activated();
    bool on_key_pressed(guint keyval, guint keycode, Gdk::ModifierType state);
    void on_button_hovered(double x, double y, Gtk::Button *button);
    void set_selected_button(Gtk::Button *button);
    void set_selected_button(int offset);
    void reset_selected_button();
    Gtk::Box m_box;
    std::vector<std::unique_ptr<Gtk::Button>> m_entry_buttons;
    std::size_t m_selected_entry_index = 0;
    Gtk::Entry m_entry;
    std::vector<std::unique_ptr<Plugin>> m_plugins;
};
