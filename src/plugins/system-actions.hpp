#pragma once

#include "../plugin.hpp"

class SubSystemAction : public SubEntry {
    public:
    SubSystemAction(Glib::ustring name, Glib::ustring command);

    [[nodiscard]] Glib::ustring label() const override;
    void selected() override;

    private:
    Glib::ustring m_name;
    Glib::ustring m_command;
};

class SystemAction : public Entry {
    public:
    SystemAction(Glib::ustring name,
                 Glib::ustring command,
                 const Glib::ustring &icon,
                 std::vector<std::shared_ptr<SubSystemAction>> sub_actions = {});

    [[nodiscard]] Glib::RefPtr<Gio::Icon> icon() const override;
    [[nodiscard]] Glib::ustring label() const override;
    [[nodiscard]] int confidence() const override;
    [[nodiscard]] std::vector<std::shared_ptr<SubEntry>> sub_entries() override;
    void selected() override;

    void set_confidence(int new_confidence);

    private:
    Glib::ustring m_name;
    Glib::ustring m_command;
    Glib::RefPtr<Gio::Icon> m_icon;
    std::vector<std::shared_ptr<SubSystemAction>> m_sub_actions;
    int m_confidence{0};
};

class SystemActions : public Plugin {
    public:
    SystemActions();

    [[nodiscard]] std::vector<std::shared_ptr<Entry>>
    get_entries(const Glib::ustring &input) override;
    [[nodiscard]] PluginInfo info() const override;

    private:
    std::vector<std::shared_ptr<SystemAction>> m_system_actions;
};
