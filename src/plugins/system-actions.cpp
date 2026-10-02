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
                           std::vector<std::shared_ptr<SubSystemAction>> sub_actions)
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

std::vector<std::shared_ptr<SubEntry>> SystemAction::sub_entries() {
    return m_sub_actions
           | std::views::transform([](const auto &sub_action) {
                 return std::static_pointer_cast<SubEntry>(sub_action);
             })
           | std::ranges::to<std::vector>();
}

void SystemAction::set_confidence(int new_confidence) {
    m_confidence = new_confidence;
}

SystemActions::SystemActions()
    : m_system_actions{
          std::make_shared<SystemAction>("Suspend", "systemctl suspend", "xfsm-suspend"),
          std::make_shared<SystemAction>("Lock", "loginctl lock-session", "xfsm-lock"),
          std::make_shared<SystemAction>(
              "Log Out", "loginctl terminate-session $XDG_SESSION_ID", "xfsm-logout"),
          std::make_shared<SystemAction>(
              "Restart",
              "systemctl reboot",
              "xfsm-reboot",
              std::vector{std::make_shared<SubSystemAction>("Restart to BIOS",
                                                            "systemctl reboot --firmware-setup")}),
          std::make_shared<SystemAction>("Shut Down", "systemctl poweroff", "xfsm-shutdown")} {}

std::vector<std::shared_ptr<Entry>> SystemActions::get_entries(const Glib::ustring &input) {
    Matcher matcher(input);
    return m_system_actions
           | std::views::transform([&matcher](auto &action) {
                 action->set_confidence(matcher.score(action->label().lowercase()));
                 return std::static_pointer_cast<Entry>(action);
             })
           | std::ranges::to<std::vector>();
}

PluginInfo SystemActions::info() const {
    return {.name = "System"};
}
