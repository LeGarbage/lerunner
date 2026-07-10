#pragma once

#include "../plugins.hpp"
#include <XdgUtils/DesktopEntry/DesktopEntry.h>
#include <string>
#include <vector>

class DesktopAction : public Entry {
    public:
    DesktopAction(const XdgUtils::DesktopEntry::DesktopEntry &desktop_entry,
                  const std::string &action);

    [[nodiscard]] std::string icon() const override;
    [[nodiscard]] std::string display() const override;
    [[nodiscard]] double confidence() const override;
    void selected() override;

    void set_confidence(double new_confidence);

    private:
    std::string m_name;
    std::string m_exec;
    double m_confidence{0};
};

class DesktopEntry : public Entry {
    public:
    DesktopEntry(const XdgUtils::DesktopEntry::DesktopEntry &desktop_entry);

    [[nodiscard]] std::string icon() const override;
    [[nodiscard]] std::string display() const override;
    [[nodiscard]] double confidence() const override;
    void selected() override;

    void set_confidence(double new_confidence);

    private:
    std::vector<DesktopAction> m_desktop_actions;
    std::vector<std::string> m_keywords;
    std::string m_name;
    std::string m_exec;
    std::string m_icon;
    double m_confidence{0};
};

class DesktopEntries : public Plugin {
    public:
    DesktopEntries();

    [[nodiscard]] std::vector<Entry *> get_entries(const std::string &input) override;
    [[nodiscard]] PluginInfo info() const override;

    private:
    std::vector<DesktopEntry> m_desktop_entries;
};
