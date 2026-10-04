#include "system-actions.hpp"
#include "../matcher/matcher.hpp"
#include <glibmm/spawn.h>
#include <ranges>
#include <utility>

SubSystemAction::SubSystemAction(Glib::ustring name, Glib::ustring command)
    : m_name(std::move(name)),
      m_command(std::move(command)) {}

Glib::ustring SubSystemAction::label() const {
    return m_name;
}

void SubSystemAction::selected() {
    Glib::spawn_command_line_async(m_command);
}

SystemAction::SystemAction(Glib::ustring name,
                           Glib::ustring command,
                           const Glib::ustring &icon,
                           std::vector<SubSystemAction> sub_actions)
    : m_name(std::move(name)),
      m_command(std::move(command)),
      m_sub_actions(std::move(sub_actions)) {
    m_icon = Gio::Icon::create(icon);
}

Glib::RefPtr<Gio::Icon> SystemAction::icon() const {
    return m_icon;
}

Glib::ustring SystemAction::label() const {
    return m_name;
}

int SystemAction::confidence() const {
    return m_confidence;
}

void SystemAction::selected() {
    Glib::spawn_command_line_async(m_command);
}

std::vector<SubEntry *> SystemAction::sub_entries() {
    return m_sub_actions
           | std::views::transform(
               [](auto &sub_action) { return static_cast<SubEntry *>(&sub_action); })
           | std::ranges::to<std::vector>();
}

void SystemAction::set_confidence(int new_confidence) {
    m_confidence = new_confidence;
}

SystemActions::SystemActions() {
    m_system_actions = {
        SystemAction("Suspend", "systemctl suspend", "xfsm-suspend"),
        SystemAction("Lock", "loginctl lock-session", "xfsm-lock"),
        SystemAction("Log Out", "loginctl terminate-session $XDG_SESSION_ID", "xfsm-logout"),
        SystemAction("Restart",
                     "systemctl reboot",
                     "xfsm-reboot",
                     {SubSystemAction("Restart to BIOS", "systemctl reboot --firmware-setup")}),
        SystemAction("Shut Down", "systemctl poweroff", "xfsm-shutdown")};
}

std::vector<Entry *> SystemActions::get_entries(const Glib::ustring &input) {
    Matcher matcher(input);
    return m_system_actions
           | std::views::transform([&matcher](auto &action) {
                 action.set_confidence(matcher.score(action.label().lowercase()));
                 return static_cast<Entry *>(&action);
             })
           | std::ranges::to<std::vector>();
}

PluginInfo SystemActions::info() const {
    return {.name = "System"};
}
