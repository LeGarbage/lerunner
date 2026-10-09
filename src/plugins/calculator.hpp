#pragma once

#include "../plugin.hpp"
#include <libqalculate/Calculator.h>
#include <libqalculate/MathStructure.h>

namespace plugins {

class CalculatorResult : public Entry {
    public:
    CalculatorResult(std::string label);

    [[nodiscard]] Glib::RefPtr<Gio::Icon> icon() const override;
    [[nodiscard]] Glib::ustring label() const override;
    [[nodiscard]] int confidence() const override;
    [[nodiscard]] std::vector<std::shared_ptr<SubEntry>> sub_entries() override;
    void selected() override;

    void set_confidence(int new_confidence);

    private:
    std::string m_label;
    int m_confidence{0};
};

class Calculator : public Plugin {
    public:
    Calculator();

    [[nodiscard]] std::vector<std::shared_ptr<Entry>>
    get_entries(const Glib::ustring &input) override;
    [[nodiscard]] PluginInfo info() const override;

    private:
    ::Calculator m_calculator;
};

} // namespace plugins
