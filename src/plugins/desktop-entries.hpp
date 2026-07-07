#pragma once

#include "../plugins.hpp"
#include <XdgUtils/DesktopEntry/DesktopEntry.h>
#include <filesystem>
#include <string>
#include <vector>

class DesktopAction : public Entry {
    public:
    DesktopAction(const XdgUtils::DesktopEntry::DesktopEntry &desktop_entry,
                  const std::string &action);

    [[nodiscard]] std::string get_icon() const override;
    [[nodiscard]] std::string display() const override;
    void selected() override;

    private:
    std::string name;
    std::string exec;
};

class DesktopEntry : public Entry {
    public:
    DesktopEntry(const std::filesystem::path &path);

    [[nodiscard]] std::string get_icon() const override;
    [[nodiscard]] std::string display() const override;
    void selected() override;

    private:
    std::vector<DesktopAction> desktop_actions;
    std::vector<std::string> keywords;
    std::string name;
    std::string exec;
    std::string icon;
};

class DesktopEntries : public Plugin {
    public:
    DesktopEntries();

    [[nodiscard]] std::vector<Entry *> get_entries(const std::string &input) override;
    [[nodiscard]] PluginInfo info() const override;

    private:
    std::vector<DesktopEntry> desktop_entries;
};
