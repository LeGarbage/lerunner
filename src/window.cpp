#include "window.hpp"
#include "plugins/desktop-entries.hpp"
#include <gdk/gdkkeysyms.h>
#include <gdkmm/monitor.h>
#include <gdkmm/rectangle.h>
#include <gtkmm/cssprovider.h>
#include <gtkmm/eventcontrollerfocus.h>
#include <gtkmm/eventcontrollerkey.h>
#include <ranges>

MainWindow::MainWindow() {
    auto css = Gtk::CssProvider::create();
    css->load_from_resource("/style.css");
    Gtk::CssProvider::add_provider_for_display(get_display(), css, 0);
    add_css_class("test");

    set_default_size(640, 0);

    m_box.set_orientation(Gtk::Orientation::VERTICAL);
    set_child(m_box);

    m_entry.signal_changed().connect(sigc::mem_fun(*this, &MainWindow::on_search_changed));
    m_box.append(m_entry);
    m_entry.set_placeholder_text("Search");

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

    for (auto &button : m_desktop_app_buttons) {
        m_box.remove(button);
    }

    m_desktop_app_buttons.clear();

    auto entries = m_plugins[0]->get_entries(text)
                   | std::views::filter([](const auto *entry) { return entry->confidence() > 75; })
                   | std::ranges::to<std::vector>();

    std::ranges::sort(entries, [](const auto *a, const auto *b) {
        return a->confidence() != b->confidence() ? a->confidence() > b->confidence()
                                                  : a->display() < b->display();
    });

    for (auto *const entry : entries | std::views::take(10)) {
        auto &button = m_desktop_app_buttons.emplace_back(entry->display());

        button.signal_clicked().connect(
            sigc::bind(sigc::mem_fun(*this, &MainWindow::on_button_clicked), entry));

        m_box.append(button);
        button.set_expand();
    }
}

bool MainWindow::on_key_pressed(guint keyval, guint /*keycode*/, Gdk::ModifierType /*state*/) {
    if (keyval == GDK_KEY_Escape) {
        // The focus handler already closes the window, so this prevents double closing
        unset_focus();
        return true;
    }

    return false;
}
