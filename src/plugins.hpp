#pragma once

#include <giomm/icon.h>
#include <glibmm/ustring.h>
#include <vector>

class Selectable {
    public:
    Selectable() = default;
    virtual ~Selectable() = default;

    Selectable(const Selectable &) = default;
    Selectable(Selectable &&) = delete;
    Selectable &operator=(const Selectable &) = default;
    Selectable &operator=(Selectable &&) = delete;

    virtual void selected() = 0;
};

class SubEntry : public Selectable {
    public:
    SubEntry() = default;
    ~SubEntry() override = default;

    SubEntry(const SubEntry &) = default;
    SubEntry(SubEntry &&) = delete;
    SubEntry &operator=(const SubEntry &) = default;
    SubEntry &operator=(SubEntry &&) = delete;

    [[nodiscard]] virtual Glib::ustring label() const = 0;
};

class Entry : public Selectable {
    public:
    Entry() = default;
    ~Entry() override = default;

    Entry(const Entry &) = default;
    Entry(Entry &&) = delete;
    Entry &operator=(const Entry &) = default;
    Entry &operator=(Entry &&) = delete;

    [[nodiscard]] virtual Glib::RefPtr<Gio::Icon> icon() const = 0;
    [[nodiscard]] virtual Glib::ustring label() const = 0;
    [[nodiscard]] virtual double confidence() const = 0;
    [[nodiscard]] virtual std::vector<SubEntry *> sub_entries() = 0;
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
