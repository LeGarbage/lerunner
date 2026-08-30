#include "window.hpp"
#include "plugins/desktop-entries.hpp"
#include <gdk/gdkkeysyms.h>
#include <gdkmm/monitor.h>
#include <gdkmm/rectangle.h>
#include <gtkmm/cssprovider.h>
#include <gtkmm/enums.h>
#include <gtkmm/eventcontrollerfocus.h>
#include <gtkmm/eventcontrollerkey.h>
#include <gtkmm/eventcontrollermotion.h>
#include <gtkmm/image.h>
#include <gtkmm/label.h>
#include <gtkmm/object.h>
#include <iterator>
#include <print>
#include <ranges>

MainWindow::MainWindow() {
    auto css = Gtk::CssProvider::create();
    css->load_from_resource("/style.css");
    Gtk::CssProvider::add_provider_for_display(
        get_display(), css, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    set_default_size(640, -1);

    m_box.set_orientation(Gtk::Orientation::VERTICAL);
    set_child(m_box);

    m_entry.signal_changed().connect(sigc::mem_fun(*this, &MainWindow::on_search_changed));
    m_box.append(m_entry);
    m_entry.set_placeholder_text("Search");
    m_entry.set_icon_from_icon_name("search-symbolic");

    // TODO: Add automatic plugin loading
    m_plugins.push_back(std::make_unique<DesktopEntries>());

    auto key_controller = Gtk::EventControllerKey::create();
    key_controller->signal_key_pressed().connect(sigc::mem_fun(*this, &MainWindow::on_key_pressed),
                                                 false);
    add_controller(key_controller);

    auto focus_controller = Gtk::EventControllerFocus::create();
    focus_controller->signal_leave().connect([this] { close(); });
    add_controller(focus_controller);
}

void MainWindow::on_button_clicked(Entry *entry) {
    entry->selected();
    unset_focus();
}

void MainWindow::on_search_changed() {
    auto text = m_entry.get_text();

    for (auto &button : m_entry_buttons) {
        m_box.remove(*button);
    }

    m_entry_buttons.clear();

    auto entries = m_plugins[0]->get_entries(text)
                   | std::views::filter([](const auto *entry) { return entry->confidence() > 75; })
                   | std::ranges::to<std::vector>();

    std::ranges::sort(entries, [](const auto *a, const auto *b) {
        // Sort entries by confidence and then by name
        return a->confidence() != b->confidence() ? a->confidence() > b->confidence()
                                                  : a->label() < b->label();
    });

    for (auto *const entry : entries | std::views::take(10)) {
        auto &button = m_entry_buttons.emplace_back(std::make_unique<Gtk::Button>());

        button->signal_clicked().connect(
            sigc::bind(sigc::mem_fun(*this, &MainWindow::on_button_clicked), entry));

        auto motion_controller = Gtk::EventControllerMotion::create();
        motion_controller->signal_enter().connect(
            sigc::bind(sigc::mem_fun(*this, &MainWindow::on_button_hovered), button.get()));
        button->add_controller(motion_controller);

        m_box.append(*button);

        button->set_can_focus(false);

        auto *box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 40);
        auto *plugin_box = Gtk::make_managed<Gtk::Box>();
        auto *info_box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 10);
        auto *icon = Gtk::make_managed<Gtk::Image>(entry->icon());
        auto *label = Gtk::make_managed<Gtk::Label>(entry->label());
        auto *plugin_label = Gtk::make_managed<Gtk::Label>(m_plugins[0]->info().name);
        button->set_child(*box);
        box->append(*plugin_box);
        box->append(*info_box);
        plugin_box->append(*plugin_label);
        info_box->append(*icon);
        info_box->append(*label);
    }

    reset_selected_button();
}

bool MainWindow::on_key_pressed(guint keyval, guint /*keycode*/, Gdk::ModifierType /*state*/) {
    if (keyval == GDK_KEY_Escape) {
        // The focus handler already closes the window, so this prevents double closing
        unset_focus();
        return true;
    }
    if (keyval == GDK_KEY_Up) {
        set_selected_button(-1);
        return true;
    }
    if (keyval == GDK_KEY_Down) {
        set_selected_button(1);
        return true;
    }

    return false;
}

void MainWindow::on_button_hovered(double /*x*/, double /*y*/, Gtk::Button *button) {
    set_selected_button(button);
}

void MainWindow::set_selected_button(Gtk::Button *new_button) {
    const auto selected_it =
        std::ranges::find_if(m_entry_buttons, [this, &new_button](const auto &button) {
            return button.get() == new_button;
        });

    m_selected_entry_index = std::distance(m_entry_buttons.begin(), selected_it);

    reset_selected_button();
}

void MainWindow::set_selected_button(int offset) {
    m_selected_entry_index += offset;
    m_selected_entry_index %= m_entry_buttons.size();

    reset_selected_button();
}

void MainWindow::reset_selected_button() {
    // Cap the index just in case the list shrunk
    m_selected_entry_index = std::min(m_selected_entry_index, m_entry_buttons.size() - 1);

    std::println("{}", m_selected_entry_index);

    if (m_entry_buttons.size() == 0) {
        // std::min does nothing if the arguments are negative, so set it here
        m_selected_entry_index = 0;
        return;
    }

    for (auto &button : m_entry_buttons) {
        button->remove_css_class("selected");
    }

    m_entry_buttons[m_selected_entry_index]->add_css_class("selected");
    set_default_widget(*m_entry_buttons[m_selected_entry_index]);
    std::println("{}", m_entry_buttons[m_selected_entry_index]->get_label().c_str());
}
