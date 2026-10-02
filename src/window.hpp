#pragma once

#include "entry-button.hpp"
#include "plugin.hpp"
#include <cstddef>
#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/entry.h>
#include <gtkmm/listview.h>
#include <gtkmm/window.h>
#include <memory>
#include <vector>

class MainWindow : public Gtk::Window {
    public:
    MainWindow();

    private:
    void on_button_clicked(Selectable *entry);
    void on_search_changed();
    void on_search_activated();
    bool on_key_pressed(guint keyval, guint keycode, Gdk::ModifierType state);
    void on_button_hovered(EntryButtonBase &button);
    void set_selected_button(EntryButtonBase &new_button);
    void set_selected_button(int offset);
    void reset_selected_button();
    void clean_up_toggled(std::size_t toggled_sub_count);
    void toggle_selected_button();

    Gtk::Box m_button_box;
    std::vector<std::unique_ptr<EntryButton>> m_entry_buttons;
    std::size_t m_selected_entry_index = 0;
    EntryButton *m_toggled_button = nullptr;
    std::vector<std::unique_ptr<SubEntryButton>> m_toggled_sub_buttons;
    Gtk::Entry m_entry;
    std::vector<std::unique_ptr<Plugin>> m_plugins;
};
