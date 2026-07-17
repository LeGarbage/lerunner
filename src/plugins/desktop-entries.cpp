#include "desktop-entries.hpp"
#include <giomm/appinfo.h>
#include <giomm/desktopappinfo.h>
#include <glibmm/ustring.h>
#include <ranges>
#include <rapidfuzz/fuzz.hpp>
#include <string>

DesktopEntry::DesktopEntry(Glib::RefPtr<Gio::DesktopAppInfo> desktop_entry)
    : m_desktop_entry{std::move(desktop_entry)} {}

Glib::RefPtr<Gio::Icon> DesktopEntry::icon() const {
    return m_desktop_entry->get_icon();
}

Glib::ustring DesktopEntry::display() const {
    return m_desktop_entry->get_display_name();
}

double DesktopEntry::confidence() const {
    return m_confidence;
}

void DesktopEntry::selected() {
    // WARN: Launch does not silence stdout/stderr. This means that the launched application will
    // output to the terminal. If the terminal is then closed, the application may have issues if it
    // tries to print to a nonexistent terminal
    m_desktop_entry->launch(std::vector<Glib::RefPtr<Gio::File>>{});
}

void DesktopEntry::set_confidence(double new_confidence) {
    m_confidence = new_confidence;
}

DesktopEntries::DesktopEntries() {
    const auto apps = Gio::AppInfo::get_all();

    for (const auto &app : apps) {
        if (!app->should_show()) { continue; }

        m_desktop_entries.emplace_back(Gio::DesktopAppInfo::create(app->get_id()));
    }
}

std::vector<Entry *> DesktopEntries::get_entries(const Glib::ustring &input) {
    return m_desktop_entries
           | std::views::transform([&input](auto &entry) {
                 entry.set_confidence(
                     // rapidfuzz::fuzz::partial_ratio(input, lower(entry.display()))
                     rapidfuzz::fuzz::WRatio(
                         static_cast<std::string>(input),
                         static_cast<std::string>(entry.display().lowercase())));
                 return static_cast<Entry *>(&entry);
             })
           | std::ranges::to<std::vector>();
}

PluginInfo DesktopEntries::info() const {
    return {.name = "Applications"};
}
