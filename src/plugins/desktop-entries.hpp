#pragma once

#include "../plugins.hpp"
#include <giomm/desktopappinfo.h>
#include <glibmm/ustring.h>
#include <vector>

class DesktopEntry : public Entry {
    public:
    DesktopEntry(Glib::RefPtr<Gio::DesktopAppInfo> desktop_entry);

    [[nodiscard]] Glib::RefPtr<Gio::Icon> icon() const override;
    [[nodiscard]] Glib::ustring display() const override;
    [[nodiscard]] double confidence() const override;
    void selected() override;

    void set_confidence(double new_confidence);

    private:
    Glib::RefPtr<Gio::DesktopAppInfo> m_desktop_entry;
    double m_confidence{0};
};

class DesktopEntries : public Plugin {
    public:
    DesktopEntries();

    [[nodiscard]] std::vector<Entry *> get_entries(const Glib::ustring &input) override;
    [[nodiscard]] PluginInfo info() const override;

    private:
    std::vector<DesktopEntry> m_desktop_entries;
};
