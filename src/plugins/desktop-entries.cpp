#include "desktop-entries.hpp"
#include <XdgUtils/DesktopEntry/DesktopEntry.h>
#include <filesystem>
#include <format>
#include <fstream>
#include <glibmm/miscutils.h>
#include <glibmm/spawn.h>
#include <ranges>
#include <unistd.h>
#include <unordered_set>

std::vector<std::string> split(const std::string &string, char delim) {
    return string
           | std::views::split(delim)
           | std::views::transform([](auto &&str) { return std::string{str.begin(), str.end()}; })
           | std::ranges::to<std::vector>();
}

DesktopAction::DesktopAction(const XdgUtils::DesktopEntry::DesktopEntry &desktop_entry,
                             const std::string &action)
    : name{desktop_entry.get(std::format("Desktop Action {}/Name", action))},
      exec{desktop_entry.get(std::format("Desktop Action {}/Exec", action))} {}

std::string DesktopAction::get_icon() const {
    return "";
}

std::string DesktopAction::display() const {
    return name;
}

void DesktopAction::selected() {
    Glib::spawn_command_line_async(exec);
}

DesktopEntry::DesktopEntry(const std::filesystem::path &path) {
    std::ifstream ifs{path};

    XdgUtils::DesktopEntry::DesktopEntry desktop_entry{ifs};

    desktop_actions = split(desktop_entry.get("Desktop Entry/Actions"), ';')
                      | std::views::filter([](auto &&string) { return string.length() != 0; })
                      | std::views::transform([&desktop_entry](auto &&action) {
                            return DesktopAction{desktop_entry, action};
                        })
                      | std::ranges::to<std::vector>();

    keywords = split(desktop_entry.get("Desktop Entry/Actions"), ';')
               | std::views::filter([](auto &&string) { return string.length() != 0; })
               | std::ranges::to<std::vector>();

    name = desktop_entry.get("Desktop Entry/Name");
    exec = desktop_entry.get("Desktop Entry/Exec");
    icon = desktop_entry.get("Desktop Entry/Icon");
}

std::string DesktopEntry::get_icon() const {
    return icon;
}

std::string DesktopEntry::display() const {
    return name;
}

void DesktopEntry::selected() {
    Glib::spawn_command_line_async(exec);
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
                desktop_entries.emplace_back(entry.path());
            }
        }
    }
}

std::vector<Entry *> DesktopEntries::get_entries(const std::string &input) {
    return desktop_entries
           | std::views::filter([&input](const auto &entry) {
                 return input.length() != 0 && entry.display().rfind(input, 0) == 0;
             })
           | std::views::transform([](auto& entry) { return static_cast<Entry *>(&entry); })
           | std::ranges::to<std::vector>();
}

PluginInfo DesktopEntries::info() const {
    return {.name = "Applications"};
}
