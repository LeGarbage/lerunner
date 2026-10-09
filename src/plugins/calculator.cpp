#include "calculator.hpp"
#include <gdkmm/clipboard.h>
#include <gdkmm/display.h>
#include <libqalculate/Calculator.h>
#include <libqalculate/includes.h>

namespace {
bool is_math_symbol(unsigned char symbol) {
    return symbol == '1'
           || symbol == '2'
           || symbol == '3'
           || symbol == '4'
           || symbol == '5'
           || symbol == '6'
           || symbol == '7'
           || symbol == '8'
           || symbol == '9'
           || symbol == '0'
           || symbol == '+'
           || symbol == '-'
           || symbol == '*'
           || symbol == '/'
           || symbol == '='
           || symbol == '('
           || symbol == ')';
}
} // namespace

namespace plugins {

CalculatorResult::CalculatorResult(std::string label)
    : m_label(std::move(label)) {}

Glib::RefPtr<Gio::Icon> CalculatorResult::icon() const {
    return Gio::Icon::create("accessories-calculator");
}

Glib::ustring CalculatorResult::label() const {
    return m_label;
}

int CalculatorResult::confidence() const {
    return m_confidence;
}

std::vector<std::shared_ptr<SubEntry>> CalculatorResult::sub_entries() {
    return {};
}

void CalculatorResult::selected() {
    Gdk::Display::get_default()->get_clipboard()->set_text(m_label);
}

void CalculatorResult::set_confidence(int new_confidence) {
    m_confidence = new_confidence;
}

Calculator::Calculator() {
    m_calculator.loadExchangeRates();
    m_calculator.loadGlobalDefinitions();
    m_calculator.loadLocalDefinitions();
};

std::vector<std::shared_ptr<Entry>> Calculator::get_entries(const Glib::ustring &input) {
    EvaluationOptions eo; // NOLINT(readability-identifier-length)
    eo.auto_post_conversion = POST_CONVERSION_BEST;
    eo.parse_options.angle_unit = ANGLE_UNIT_RADIANS;
    eo.structuring = STRUCTURING_SIMPLIFY;
    eo.approximation = APPROXIMATION_APPROXIMATE;

    PrintOptions po; // NOLINT(readability-identifier-length)
    po.number_fraction_format = FRACTION_FRACTIONAL;
    po.interval_display = INTERVAL_DISPLAY_SIGNIFICANT_DIGITS;

    Glib::ustring calc_input = input;

    int confidence = 0;
    bool starts_with_equals = !input.empty() && input[0] == '=';
    if (starts_with_equals) {
        confidence = 999;
        calc_input = calc_input.substr(1);
    } else {
        for (auto letter : input) {
            if (is_math_symbol(letter)) { confidence++; }
        }
    }

    auto output = m_calculator.calculateAndPrint(
        m_calculator.unlocalizeExpression(calc_input), 10000, eo, po);

    auto result = std::make_shared<CalculatorResult>(output);
    result->set_confidence(confidence);

    return {result};
}

PluginInfo Calculator::info() const {
    return {.name = "Calculator"};
}
} // namespace plugins
