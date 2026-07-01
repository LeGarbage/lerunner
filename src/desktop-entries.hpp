#pragma once

#include <XdgUtils/DesktopEntry/DesktopEntry.h>
#include <filesystem>
#include <string>
#include <vector>

class DesktopAction {
    public:
    DesktopAction(const XdgUtils::DesktopEntry::DesktopEntry &desktop_entry,
                  const std::string &action);
    [[nodiscard]] std::string get_name() const;
    [[nodiscard]] std::string get_exec() const;

    private:
    std::string name;
    std::string exec;
};

class DesktopEntry {
    public:
    DesktopEntry(const std::filesystem::path &path);
    [[nodiscard]] std::string get(const std::string &path, const std::string &fallback = "") const;
    [[nodiscard]] std::vector<DesktopAction> get_actions() const;

    private:
    XdgUtils::DesktopEntry::DesktopEntry desktop_entry;
    std::vector<DesktopAction> desktop_actions;
};

class DesktopEntryParser {
    public:
    // std::vector<DesktopEntry> get_desktop_entries();
    static std::vector<std::filesystem::path> get_desktop_entries();
};
