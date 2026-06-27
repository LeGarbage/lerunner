#pragma once

#include <XdgUtils/DesktopEntry/DesktopEntry.h>
#include <filesystem>
#include <string>
#include <vector>

class DesktopEntry {
    public:
    DesktopEntry(const std::filesystem::path &path);
    [[nodiscard]] std::string get(const std::string &path, const std::string &fallback = "") const;

    private:
    XdgUtils::DesktopEntry::DesktopEntry desktop_entry;
};

class DesktopEntryParser {
    public:
    // std::vector<DesktopEntry> get_desktop_entries();
    static std::vector<std::filesystem::path> get_desktop_entries();
};
