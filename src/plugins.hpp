#pragma once

#include <string>
#include <vector>

class Entry {
    public:
    Entry() = default;
    virtual ~Entry() = default;

    Entry(const Entry &) = default;
    Entry(Entry &&) = delete;
    Entry &operator=(const Entry &) = default;
    Entry &operator=(Entry &&) = delete;

    [[nodiscard]] virtual std::string icon() const = 0;
    [[nodiscard]] virtual std::string display() const = 0;
    [[nodiscard]] virtual double confidence() const = 0;
    virtual void selected() = 0;
};

struct PluginInfo {
    std::string name;
};

class Plugin {
    public:
    Plugin() = default;
    virtual ~Plugin() = default;

    Plugin(const Plugin &) = delete;
    Plugin(Plugin &&) = delete;
    Plugin &operator=(const Plugin &) = delete;
    Plugin &operator=(Plugin &&) = delete;

    [[nodiscard]] virtual std::vector<Entry *> get_entries(const std::string &input) = 0;
    [[nodiscard]] virtual PluginInfo info() const = 0;
};
