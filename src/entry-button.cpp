#include "entry-button.hpp"
#include <gtkmm/eventcontrollermotion.h>
#include <gtkmm/image.h>
#include <gtkmm/label.h>
#include <gtkmm/root.h>

EntryButtonBase::type_signal_hovered EntryButtonBase::signal_hovered() {
    return m_signal_hovered;
}

EntryButton::EntryButton(std::shared_ptr<Entry> entry)
    : m_entry(std::move(entry)),
      m_plugin_box(Gtk::make_managed<Gtk::Box>()) {
    property_parent().signal_changed().connect([this]() {
        auto motion_controller = Gtk::EventControllerMotion::create();
        motion_controller->signal_enter().connect(
            [this](double /*x*/, double /*y*/) { signal_hovered().emit(*this); });
        add_controller(motion_controller);

        set_can_focus(false);

        auto *box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 40);
        set_child(*box);

        box->append(*m_plugin_box);
        m_plugin_box->set_size_request(get_width() / 3);
        m_plugin_box->set_homogeneous();

        auto *info_box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 10);
        box->append(*info_box);

        if (m_entry->sub_entries().size() > 0) {
            auto *open_arrow = Gtk::make_managed<Gtk::Image>();
            open_arrow->set_from_icon_name("arrow-right-symbolic");
            open_arrow->add_css_class("open-arrow");
            info_box->append(*open_arrow);
        }

        auto *icon = Gtk::make_managed<Gtk::Image>(m_entry->icon());
        info_box->append(*icon);

        auto *label = Gtk::make_managed<Gtk::Label>(m_entry->label());
        info_box->append(*label);
    });
}

std::vector<std::shared_ptr<SubEntry>> EntryButton::get_sub_entries() const {
    return m_entry->sub_entries();
}

void EntryButton::on_clicked() {
    m_entry->selected();
    get_root()->unset_focus();
}

void EntryButton::add_plugin_label(Plugin &plugin) {
    auto *plugin_label = Gtk::make_managed<Gtk::Label>(plugin.info().name);
    m_plugin_box->append(*plugin_label);
    plugin_label->set_halign(Gtk::Align::END);
    plugin_label->add_css_class("plugin-label");
}

SubEntryButton::SubEntryButton(std::shared_ptr<SubEntry> sub_entry)
    : m_sub_entry(std::move(sub_entry)) {
    auto motion_controller = Gtk::EventControllerMotion::create();
    motion_controller->signal_enter().connect(
        [this](double /*x*/, double /*y*/) { signal_hovered().emit(*this); });
    add_controller(motion_controller);

    set_can_focus(false);

    auto *box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 40);
    set_child(*box);

    auto *plugin_box_padding = Gtk::make_managed<Gtk::Box>();
    box->append(*plugin_box_padding);
    plugin_box_padding->set_size_request(get_width() / 3);
    plugin_box_padding->set_homogeneous();

    auto *info_box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 10);
    box->append(*info_box);

    auto *open_arrow_padding = Gtk::make_managed<Gtk::Image>();
    info_box->append(*open_arrow_padding);

    auto *icon_padding = Gtk::make_managed<Gtk::Image>();
    info_box->append(*icon_padding);

    auto *label = Gtk::make_managed<Gtk::Label>(m_sub_entry->label());
    info_box->append(*label);
}

void SubEntryButton::on_clicked() {
    m_sub_entry->selected();
    get_root()->unset_focus();
}
