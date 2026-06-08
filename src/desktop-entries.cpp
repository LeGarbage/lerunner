#include "desktop-entries.hpp"
#include <cstdlib>
#include <filesystem>
#include <format>
#include <fstream>
#include <ranges>
#include <stdexcept>
#include <unordered_set>

std::optional<DesktopEntry> DesktopEntry::from_path(const std::filesystem::path &path) {
    std::ifstream desktop_file{path};

    if (!desktop_file.is_open()) { return std::nullopt; }

    std::string test{};

    std::getline(desktop_file, test);

    return DesktopEntry{.test = test, .path = path};
}

std::vector<std::string> split(const std::string &string, char delim) {
    return string
           | std::views::split(delim)
           | std::views::transform([](auto &&str) { return std::string{str.begin(), str.end()}; })
           | std::ranges::to<std::vector>();
}

std::vector<std::filesystem::path> DesktopEntryParser::get_desktop_entries() {
    auto *const data_dirs_env = std::getenv("XDG_DATA_DIRS");
    auto *const data_home_env = std::getenv("XDG_DATA_HOME");
    auto *const home_env = std::getenv("HOME");

    if (home_env == nullptr || *home_env == '\0') {
        throw std::runtime_error("$HOME is not set (you should probably fix this)");
    }

    // Set the default paths if the environment variable does not exist or is empty
    const std::string xdg_data_dirs{(data_dirs_env != nullptr && *data_dirs_env != '\0')
                                        ? data_dirs_env
                                        : "/usr/local/share:/usr/share"};

    const std::string xdg_data_home{(data_home_env != nullptr && *data_home_env != '\0')
                                        ? data_home_env
                                        : std::format("{}/.local/share", home_env)};

    const std::string app_search_dirs = std::format("{}:{}", xdg_data_home, xdg_data_dirs);

    std::vector<std::filesystem::path> desktop_files{};
    std::unordered_set<std::filesystem::path> relative_paths{};
    for (const auto &dir : split(app_search_dirs, ':')) {
        auto application_path = std::filesystem::path{dir} / "applications";
        if (!std::filesystem::is_directory(application_path)) { continue; }

        for (const auto &entry : std::filesystem::recursive_directory_iterator{application_path}) {
            if (entry.path().extension() != ".desktop") { continue; }

            const auto &relative_path = entry.path().lexically_relative(application_path);

            // .second is true if the insertion actually took place (prevents duplicate entries)
            if (relative_paths.insert(relative_path).second) {
                desktop_files.push_back(entry.path());
            }
        }
    }

    return desktop_files;
}
