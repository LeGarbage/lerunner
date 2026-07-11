#pragma once

#include <giomm/icon.h>
#include <glibmm/ustring.h>
#include <vector>

class Entry {
    public:
    Entry() = default;
    virtual ~Entry() = default;

    Entry(const Entry &) = default;
    Entry(Entry &&) = delete;
    Entry &operator=(const Entry &) = default;
    Entry &operator=(Entry &&) = delete;

    [[nodiscard]] virtual Glib::RefPtr<Gio::Icon> icon() const = 0;
    [[nodiscard]] virtual Glib::ustring display() const = 0;
    [[nodiscard]] virtual double confidence() const = 0;
    virtual void selected() = 0;
};

struct PluginInfo {
    Glib::ustring name;
};

class Plugin {
    public:
    Plugin() = default;
    virtual ~Plugin() = default;

    Plugin(const Plugin &) = delete;
    Plugin(Plugin &&) = delete;
    Plugin &operator=(const Plugin &) = delete;
    Plugin &operator=(Plugin &&) = delete;

    [[nodiscard]] virtual std::vector<Entry *> get_entries(const Glib::ustring &input) = 0;
    [[nodiscard]] virtual PluginInfo info() const = 0;
};
