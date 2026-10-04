#pragma once

#include "../plugin.hpp"
#include <giomm/desktopappinfo.h>
#include <glibmm/ustring.h>
#include <vector>

class DesktopAction : public SubEntry {
    public:
    DesktopAction(Glib::RefPtr<Gio::DesktopAppInfo> desktop_entry, Glib::ustring action_name);

    [[nodiscard]] Glib::ustring label() const override;
    void selected() override;

    private:
    Glib::RefPtr<Gio::DesktopAppInfo> m_desktop_entry;
    Glib::ustring m_action_name;
};

class DesktopEntry : public Entry {
    public:
    DesktopEntry(Glib::RefPtr<Gio::DesktopAppInfo> desktop_entry);

    [[nodiscard]] Glib::RefPtr<Gio::Icon> icon() const override;
    [[nodiscard]] Glib::ustring label() const override;
    [[nodiscard]] int confidence() const override;
    [[nodiscard]] std::vector<SubEntry *> sub_entries() override;
    void selected() override;

    void set_confidence(int new_confidence);

    private:
    Glib::RefPtr<Gio::DesktopAppInfo> m_desktop_entry;
    std::vector<DesktopAction> m_desktop_actions;
    int m_confidence{0};
};

class DesktopEntries : public Plugin {
    public:
    DesktopEntries();

    [[nodiscard]] std::vector<Entry *> get_entries(const Glib::ustring &input) override;
    [[nodiscard]] PluginInfo info() const override;

    private:
    std::vector<DesktopEntry> m_desktop_entries;
};
