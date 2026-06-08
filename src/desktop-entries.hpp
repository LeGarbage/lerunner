#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include <vector>

class DesktopEntry {
    public:
    static std::optional<DesktopEntry> from_path(const std::filesystem::path &path);
    std::string test;
    std::filesystem::path path;
};

class DesktopEntryParser {
    public:
    // std::vector<DesktopEntry> get_desktop_entries();
    static std::vector<std::filesystem::path> get_desktop_entries();
};
