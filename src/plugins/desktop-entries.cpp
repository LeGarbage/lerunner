#include "desktop-entries.hpp"
#include "../matcher/matcher.hpp"
#include <format>
#include <giomm/appinfo.h>
#include <giomm/desktopappinfo.h>
#include <glibmm/spawn.h>
#include <glibmm/ustring.h>
#include <ranges>
#include <string>
#include <utility>

DesktopAction::DesktopAction(Glib::RefPtr<Gio::DesktopAppInfo> desktop_entry,
                             Glib::ustring action_name)
    : m_desktop_entry{std::move(desktop_entry)},
      m_action_name{std::move(action_name)} {}

Glib::ustring DesktopAction::label() const {
    return m_desktop_entry->get_action_name(m_action_name);
}

void DesktopAction::selected() {
    // TODO: Add a configuration and conditionally call launch or a user-defined command

    // WARN: Launch does not silence stdout/stderr. This means that the launched application will
    // output to the terminal. If the terminal is then closed, the application may have issues if it
    // tries to print to a nonexistent terminal

    // m_desktop_entry->launch_action(m_action_name);
    Glib::spawn_command_line_async(std::format("uwsm-app -s app.slice -- {}:{}",
                                               m_desktop_entry->get_id(),
                                               static_cast<std::string>(m_action_name)));
}

DesktopEntry::DesktopEntry(Glib::RefPtr<Gio::DesktopAppInfo> desktop_entry)
    : m_desktop_entry{std::move(desktop_entry)} {
    for (const auto &action : m_desktop_entry->list_actions()) {
        m_desktop_actions.emplace_back(m_desktop_entry, action);
    }
}

Glib::RefPtr<Gio::Icon> DesktopEntry::icon() const {
    return m_desktop_entry->get_icon();
}

Glib::ustring DesktopEntry::label() const {
    return m_desktop_entry->get_display_name();
}

int DesktopEntry::confidence() const {
    return m_confidence;
}

void DesktopEntry::selected() {
    // TODO: Add a configuration and conditionally call launch or a user-defined command

    // WARN: Launch does not silence stdout/stderr. This means that the launched application will
    // output to the terminal. If the terminal is then closed, the application may have issues if it
    // tries to print to a nonexistent terminal

    // m_desktop_entry->launch(nullptr);
    Glib::spawn_command_line_async(
        std::format("uwsm-app -s app.slice -- {}", m_desktop_entry->get_id()));
}

std::vector<SubEntry *> DesktopEntry::sub_entries() {
    return m_desktop_actions
           | std::views::transform([](auto &entry) { return static_cast<SubEntry *>(&entry); })
           | std::ranges::to<std::vector>();
}

void DesktopEntry::set_confidence(int new_confidence) {
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
    Matcher matcher(input);
    return m_desktop_entries
           | std::views::transform([&matcher](auto &entry) {
                 entry.set_confidence(matcher.score(entry.label().lowercase()));
                 return static_cast<Entry *>(&entry);
             })
           | std::ranges::to<std::vector>();
}

PluginInfo DesktopEntries::info() const {
    return {.name = "Applications"};
}
