#include "desktop-entries.hpp"
#include <XdgUtils/DesktopEntry/DesktopEntry.h>
#include <cctype>
#include <filesystem>
#include <format>
#include <fstream>
#include <glibmm/miscutils.h>
#include <glibmm/spawn.h>
#include <ranges>
#include <rapidfuzz/fuzz.hpp>
#include <unordered_set>

std::vector<std::string> split(const std::string &string, char delim) {
    return string
           | std::views::split(delim)
           | std::views::transform([](auto &&str) { return std::string{str.begin(), str.end()}; })
           | std::ranges::to<std::vector>();
}

std::string lower(const std::string &string) {
    return string
           | std::views::transform([](unsigned char chr) { return std::tolower(chr); })
           | std::ranges::to<std::string>();
}

DesktopAction::DesktopAction(const XdgUtils::DesktopEntry::DesktopEntry &desktop_entry,
                             const std::string &action)
    : m_name{desktop_entry.get(std::format("Desktop Action {}/Name", action))},
      m_exec{desktop_entry.get(std::format("Desktop Action {}/Exec", action))} {}

std::string DesktopAction::icon() const {
    return "";
}

std::string DesktopAction::display() const {
    return m_name;
}

double DesktopAction::confidence() const {
    return m_confidence;
}

void DesktopAction::selected() {
    Glib::spawn_command_line_async(m_exec);
}

void DesktopAction::set_confidence(double new_confidence) {
    m_confidence = new_confidence;
}

DesktopEntry::DesktopEntry(const XdgUtils::DesktopEntry::DesktopEntry &desktop_entry)
    : m_name{desktop_entry.get("Desktop Entry/Name")},
      m_exec{desktop_entry.get("Desktop Entry/Exec")},
      m_icon{desktop_entry.get("Desktop Entry/Icon")},

      m_desktop_actions{split(desktop_entry.get("Desktop Entry/Actions"), ';')
                        | std::views::filter([](auto &&string) { return string.length() != 0; })
                        | std::views::transform([&desktop_entry](auto &&action) {
                              return DesktopAction{desktop_entry, action};
                          })
                        | std::ranges::to<std::vector>()},

      m_keywords{split(desktop_entry.get("Desktop Entry/Keywords"), ';')
                 | std::views::filter([](auto &&string) { return string.length() != 0; })
                 | std::ranges::to<std::vector>()} {}

std::string DesktopEntry::icon() const {
    return m_icon;
}

std::string DesktopEntry::display() const {
    return m_name;
}

double DesktopEntry::confidence() const {
    return m_confidence;
}

void DesktopEntry::selected() {
    Glib::spawn_command_line_async(m_exec);
}

void DesktopEntry::set_confidence(double new_confidence) {
    m_confidence = new_confidence;
}

DesktopEntries::DesktopEntries() {
    const auto xdg_data_home = Glib::get_user_data_dir();
    const auto xdg_data_dirs =
        Glib::get_system_data_dirs() | std::views::join_with(':') | std::ranges::to<std::string>();

    const auto app_search_dirs = std::format("{}:{}", xdg_data_home, xdg_data_dirs);

    std::unordered_set<std::filesystem::path> relative_paths;
    for (const auto &dir : split(app_search_dirs, ':')) {
        auto application_path = std::filesystem::path{dir} / "applications";
        if (!std::filesystem::is_directory(application_path)) { continue; }

        for (const auto &entry : std::filesystem::recursive_directory_iterator{application_path}) {
            if (entry.path().extension() != ".desktop") { continue; }

            const auto &relative_path = entry.path().lexically_relative(application_path);

            // .second is true if the insertion actually took place (prevents duplicate entries)
            if (relative_paths.insert(relative_path).second) {
                std::ifstream ifs{entry.path()};
                XdgUtils::DesktopEntry::DesktopEntry desktop_entry{ifs};

                if (desktop_entry.get("Desktop Entry/NoDisplay") == "true") { continue; }

                m_desktop_entries.emplace_back(desktop_entry);
            }
        }
    }
}

std::vector<Entry *> DesktopEntries::get_entries(const std::string &input) {
    return m_desktop_entries
           | std::views::transform([&input](auto &entry) {
                 entry.set_confidence(
                     // rapidfuzz::fuzz::partial_ratio(input, lower(entry.display()))
                     rapidfuzz::fuzz::WRatio(input, lower(entry.display())));
                 return static_cast<Entry *>(&entry);
             })
           | std::ranges::to<std::vector>();
}

PluginInfo DesktopEntries::info() const {
    return {.name = "Applications"};
}
