#include "window.hpp"
#include "plugins/desktop-entries.hpp"
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
    m_button_data.clear();

    for (const auto &plugin : m_plugins) {
        auto entries =
            plugin->get_entries(text)
            | std::views::filter([](const auto *entry) { return entry->confidence() > 75; })
            | std::ranges::to<std::vector>();

        std::ranges::sort(entries, [](const auto *a, const auto *b) {
            // Sort entries by confidence and then by name
            return a->confidence() != b->confidence() ? a->confidence() > b->confidence()
                                                      : a->label() < b->label();
        });

        bool first_entry = true;

        for (auto *const entry : entries | std::views::take(10)) {
            auto &button = m_entry_buttons.emplace_back(std::make_unique<Gtk::Button>());
            m_button_data.push_back(entry);

            button->signal_clicked().connect(
                sigc::bind(sigc::mem_fun(*this, &MainWindow::on_button_clicked), entry));

            auto motion_controller = Gtk::EventControllerMotion::create();
            motion_controller->signal_enter().connect(
                sigc::bind(sigc::mem_fun(*this, &MainWindow::on_button_hovered), button.get()));
            button->add_controller(motion_controller);

            m_button_box.append(*button);

            button->set_can_focus(false);

            auto *box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 40);
            button->set_child(*box);

            auto *plugin_box = Gtk::make_managed<Gtk::Box>();
            box->append(*plugin_box);
            plugin_box->set_size_request(get_width() / 3);
            plugin_box->set_homogeneous();

            if (first_entry) {
                auto *plugin_label = Gtk::make_managed<Gtk::Label>(plugin->info().name);
                plugin_box->append(*plugin_label);
                plugin_label->set_halign(Gtk::Align::END);
                plugin_label->add_css_class("plugin-label");

                first_entry = false;
            }

            auto *info_box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 10);
            box->append(*info_box);

            if (entry->sub_entries().size() > 0) {
                auto *open_arrow = Gtk::make_managed<Gtk::Image>();
                open_arrow->set_from_icon_name("arrow-right-symbolic");
                open_arrow->add_css_class("open-arrow");
                info_box->append(*open_arrow);
            }

            auto *icon = Gtk::make_managed<Gtk::Image>(entry->icon());
            info_box->append(*icon);

            auto *label = Gtk::make_managed<Gtk::Label>(entry->label());
            info_box->append(*label);
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

void MainWindow::on_button_hovered(double /*x*/, double /*y*/, Gtk::Button *button) {
    set_selected_button(button);
}

void MainWindow::set_selected_button(Gtk::Button *new_button) {
    const auto &buttons = m_button_box.get_children();
    const auto selected_it = std::ranges::find(buttons, new_button);

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
        dynamic_cast<Gtk::Button *>(m_button_box.get_children()[m_selected_entry_index]);
    const auto selected_it =
        std::ranges::find_if(m_entry_buttons, [selected_button](const auto &button) {
            return button.get() == selected_button;
        });

    if (selected_it == m_entry_buttons.end()) { return; }

    const auto real_index = std::distance(m_entry_buttons.begin(), selected_it);

    const auto &sub_entries = m_button_data[real_index]->sub_entries();
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

    for (auto *const sub_entry : sub_entries) {
        auto &button = m_toggled_sub_buttons.emplace_back(std::make_unique<Gtk::Button>());

        button->signal_clicked().connect(
            sigc::bind(sigc::mem_fun(*this, &MainWindow::on_button_clicked), sub_entry));

        auto motion_controller = Gtk::EventControllerMotion::create();
        motion_controller->signal_enter().connect(
            sigc::bind(sigc::mem_fun(*this, &MainWindow::on_button_hovered), button.get()));
        button->add_controller(motion_controller);

        m_button_box.insert_child_after(*button, *selected_button);

        button->set_can_focus(false);

        auto *box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 40);
        button->set_child(*box);

        auto *plugin_box = Gtk::make_managed<Gtk::Box>();
        box->append(*plugin_box);
        plugin_box->set_size_request(get_width() / 3);
        plugin_box->set_homogeneous();

        auto *info_box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 10);
        box->append(*info_box);

        auto *icon = Gtk::make_managed<Gtk::Image>();
        info_box->append(*icon);

        auto *label = Gtk::make_managed<Gtk::Label>(sub_entry->label());
        info_box->append(*label);
    }
}
