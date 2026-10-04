#include "window.hpp"
#include "plugins/desktop-entries.hpp"
#include "plugins/system-actions.hpp"
#include <gdk/gdkkeysyms.h>
#include <gdkmm/monitor.h>
#include <gdkmm/rectangle.h>
#include <giomm/icon.h>
#include <gtkmm/cssprovider.h>
#include <gtkmm/enums.h>
#include <gtkmm/eventcontrollerfocus.h>
#include <gtkmm/eventcontrollerkey.h>
#include <gtkmm/eventcontrollermotion.h>
#include <gtkmm/expander.h>
#include <gtkmm/icontheme.h>
#include <gtkmm/image.h>
#include <gtkmm/label.h>
#include <gtkmm/object.h>
#include <iterator>
#include <print>
#include <ranges>

#define FLOATING

MainWindow::MainWindow() {
    auto css = Gtk::CssProvider::create();
    css->load_from_resource("/style.css");
    Gtk::CssProvider::add_provider_for_display(
        get_display(), css, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    set_default_size(640, -1);

    auto *main_box = Gtk::make_managed<Gtk::Box>();
    main_box->set_orientation(Gtk::Orientation::VERTICAL);
    set_child(*main_box);

    m_entry.signal_changed().connect(sigc::mem_fun(*this, &MainWindow::on_search_changed));
    main_box->append(m_entry);
    m_entry.set_placeholder_text("Search");
    m_entry.set_icon_from_icon_name("search-symbolic");
    m_entry.signal_activate().connect(sigc::mem_fun(*this, &MainWindow::on_search_activated));

    m_button_box.set_orientation(Gtk::Orientation::VERTICAL);
    main_box->append(m_button_box);

    // TODO: Add automatic plugin loading
    m_plugins.push_back(std::make_unique<DesktopEntries>());
    m_plugins.push_back(std::make_unique<SystemActions>());

    auto key_controller = Gtk::EventControllerKey::create();
    key_controller->signal_key_pressed().connect(sigc::mem_fun(*this, &MainWindow::on_key_pressed),
                                                 false);
    add_controller(key_controller);

    auto focus_controller = Gtk::EventControllerFocus::create();
    focus_controller->signal_leave().connect([this] { close(); });
#ifdef FLOATING
    add_controller(focus_controller);
#endif // FLOATING
}

void MainWindow::on_button_clicked(Selectable *entry) {
    entry->selected();
    unset_focus();
}

void MainWindow::on_search_changed() {
    auto text = m_entry.get_text();

    for (auto &button : m_entry_buttons) {
        m_button_box.remove(*button);
    }

    m_entry_buttons.clear();

    // TODO: Sort the plugins based on the confidence of their first entry and only take 10 total
    std::vector<std::pair<Plugin *, std::vector<std::shared_ptr<Entry>>>> all_entries;

    for (const auto &plugin : m_plugins) {
        auto entries = plugin->get_entries(text);

        std::ranges::sort(entries, [](const auto &a, const auto &b) {
            // Sort entries by confidence, then length, and then by name
            if (a->confidence() != b->confidence()) { return a->confidence() > b->confidence(); }
            if (a->label().size() != b->label().size()) {
                return a->label().size() < b->label().size();
            }

            return a->label() > b->label();
        });

        all_entries.emplace_back(plugin.get(), entries);
    }

    std::ranges::sort(all_entries, [](const auto &a, const auto &b) {
        return a.second[0]->confidence() > b.second[0]->confidence();
    });

    auto top_entry_confidence = all_entries[0].second[0]->confidence();

    std::println();
    for (auto [plugin, entries] : all_entries) {
        bool first_entry = true;

        for (const auto &entry : entries | std::views::filter([&](const auto &a) {
                                     return a->confidence() > top_entry_confidence / 4;
                                 })) {
            std::println("{} confidence: {}", entry->label().c_str(), entry->confidence());
            m_entry_buttons.push_back(std::make_unique<EntryButton>(entry, get_width()));
            auto &button = m_entry_buttons.back();
            button->signal_hovered().connect(sigc::mem_fun(*this, &MainWindow::on_button_hovered));

            m_button_box.append(*button);

            if (first_entry) {
                button->add_plugin_label(*plugin);
                first_entry = false;
            }
        }
    }

    reset_selected_button();
}

void MainWindow::on_search_activated() {
    dynamic_cast<Gtk::Button *>(get_default_widget())->activate();
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
    if (keyval == GDK_KEY_Tab) {
        toggle_selected_button();
        return true;
    }

    return false;
}

void MainWindow::on_button_hovered(EntryButtonBase &button) {
    set_selected_button(button);
}

void MainWindow::set_selected_button(EntryButtonBase &new_button) {
    const auto &buttons = m_button_box.get_children();
    const auto selected_it = std::ranges::find(buttons, &new_button);

    m_selected_entry_index = std::distance(buttons.begin(), selected_it);

    reset_selected_button();
}

void MainWindow::set_selected_button(int offset) {
    while (offset < 0) {
        offset += static_cast<int>(m_button_box.get_children().size());
    }
    m_selected_entry_index += offset;
    m_selected_entry_index %= m_button_box.get_children().size();

    reset_selected_button();
}

void MainWindow::reset_selected_button() {
    // Cap the index just in case the list shrunk
    m_selected_entry_index = std::min(m_selected_entry_index, m_button_box.get_children().size());

    std::println("Selected index: {}", m_selected_entry_index);

    if (m_entry_buttons.size() == 0) {
        unset_default_widget();
        // The index underflows if the size is 0, so manually set it
        m_selected_entry_index = 0;
        return;
    }

    for (auto *button : m_button_box.get_children()) {
        button->remove_css_class("selected");
    }

    m_button_box.get_children()[m_selected_entry_index]->add_css_class("selected");
    set_default_widget(*m_button_box.get_children()[m_selected_entry_index]);
}

void MainWindow::clean_up_toggled(std::size_t toggled_sub_count) {
    for (auto &button : m_toggled_sub_buttons) {
        m_button_box.remove(*button);
    }
    m_toggled_sub_buttons.clear();

    if (m_toggled_button != nullptr) {
        const auto buttons = m_button_box.get_children();
        const auto toggled_it = std::ranges::find(buttons, m_toggled_button);
        const auto toggled_index = std::distance(buttons.begin(), toggled_it);

        // Reset the index to where it should be when the button is collapsed
        if (m_selected_entry_index > toggled_index + toggled_sub_count) {
            m_selected_entry_index = toggled_index;
        } else if (m_selected_entry_index > toggled_index) {
            m_selected_entry_index--;
        }
        reset_selected_button();

        m_toggled_button->get_child()->get_last_child()->get_first_child()->remove_css_class(
            "rotated");
        m_toggled_button = nullptr;
    }
}

void MainWindow::toggle_selected_button() {
    auto *const selected_button =
        dynamic_cast<EntryButton *>(m_button_box.get_children()[m_selected_entry_index]);
    const auto selected_it =
        std::ranges::find_if(m_entry_buttons, [selected_button](const auto &button) {
            return button.get() == selected_button;
        });

    if (selected_it == m_entry_buttons.end()) { return; }

    const auto real_index = std::distance(m_entry_buttons.begin(), selected_it);

    const auto &sub_entries = m_entry_buttons[real_index]->get_sub_entries();
    if (sub_entries.size() == 0) { return; }

    if (m_entry_buttons[real_index].get() == m_toggled_button) {
        // The function call needs to be both inside and after the if statement because it messes
        // with the indices
        clean_up_toggled(sub_entries.size());
        return;
    }

    clean_up_toggled(sub_entries.size());

    selected_button->get_child()->get_last_child()->get_first_child()->add_css_class("rotated");
    m_toggled_button = selected_button;

    for (const auto &sub_entry : sub_entries) {
        m_toggled_sub_buttons.push_back(std::make_unique<SubEntryButton>(sub_entry, get_width()));
        auto &button = m_toggled_sub_buttons.back();

        m_button_box.insert_child_after(*button, *selected_button);

        button->signal_hovered().connect(sigc::mem_fun(*this, &MainWindow::on_button_hovered));
    }
}
