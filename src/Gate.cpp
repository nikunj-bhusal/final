#include "Gate.hpp"

#include <stdexcept>

#include "Configuration.h"

using namespace UIConstants;

Gate::Gate(GateType type, sf::Vector2f position, int persistentLabel) : type(type), position(position), persistentLabel(persistentLabel) {
    using namespace UIConstants;
    shape.setSize({50.f, 50.f});
    shape.setPosition(position);
    shape.setOutlineThickness(OUTLINE_THICKNESS);
    shape.setOutlineColor(GATE_DEFAULT_OUTLINE);

    switch (type) {
        case GateType::INPUT:
        case GateType::OUTPUT:
            shape.setFillColor(GATE_IO_FILL);
            break;
        default:
            shape.setFillColor(GATE_DEFAULT_FILL);
            break;
    }
}

void Gate::setFont(const sf::Font &font) {
    if (font.getInfo().family.empty()) return;  // Basic validation
    currentFont = &font;
}

void Gate::draw(sf::RenderWindow &window, size_t gateIndex, const std::vector<Gate> &gates, int selectedPin) const {
    sf::RectangleShape gateShape = shape;

    if (selected) {
        gateShape.setOutlineThickness(OUTLINE_THICKNESS * 2);
        gateShape.setOutlineColor(GATE_SELECTED_OUTLINE);
    }

    window.draw(gateShape);

    // Label
    drawGateLabel(window, gateIndex, gates);

    // Draw pins
    sf::CircleShape pin(6.f);
    pin.setOutlineThickness(1.f);
    pin.setOutlineColor(PIN_OUTLINE_COLOR);

    // Output pin (red if HIGH, white if LOW)
    if (type != GateType::OUTPUT) {
        pin.setFillColor(state ? PIN_HIGH_COLOR : PIN_LOW_COLOR);
        sf::Vector2f outPinPos = getOutputPinPosition();
        pin.setPosition(outPinPos - sf::Vector2f{6.f, 6.f});
        window.draw(pin);
        // Highlight if selected
        if (selectedPin == -1) {
            sf::CircleShape highlight(8.f);
            highlight.setOrigin({8.f, 8.f});
            highlight.setPosition(outPinPos);
            highlight.setFillColor(sf::Color::Transparent);
            highlight.setOutlineColor(sf::Color(0, 120, 255));
            highlight.setOutlineThickness(4.f);
            window.draw(highlight);
        }
    }

    // Input pins
    if (type != GateType::INPUT) {
        int inputCount = (type == GateType::NOT || type == GateType::OUTPUT) ? 1 : 2;
        pin.setFillColor(PIN_LOW_COLOR);

        for (int i = 0; i < inputCount; ++i) {
            sf::Vector2f inPinPos = getInputPinPosition(i);
            pin.setPosition(inPinPos - sf::Vector2f{6.f, 6.f});
            window.draw(pin);
            // Highlight if selected
            if (selectedPin == i) {
                sf::CircleShape highlight(8.f);
                highlight.setOrigin({8.f, 8.f});
                highlight.setPosition(inPinPos);
                highlight.setFillColor(sf::Color::Transparent);
                highlight.setOutlineColor(sf::Color(0, 120, 255));
                highlight.setOutlineThickness(4.f);
                window.draw(highlight);
            }
        }
    }
}

bool Gate::evaluate(const std::vector<bool> &inputs) const {
    try {
        switch (type) {
            case GateType::AND:
                return inputs.size() >= 2 && (inputs.at(0) && inputs.at(1));
            case GateType::OR:
                return inputs.size() >= 2 && (inputs.at(0) || inputs.at(1));
            case GateType::NOT:
                return inputs.size() >= 1 && !inputs.at(0);
            case GateType::NAND:
                return inputs.size() >= 2 && !(inputs.at(0) && inputs.at(1));
            case GateType::NOR:
                return inputs.size() >= 2 && !(inputs.at(0) || inputs.at(1));
            case GateType::XOR:
                return inputs.size() >= 2 && (inputs.at(0) != inputs.at(1));
            case GateType::INPUT:
                return state;
            case GateType::OUTPUT:
                return !inputs.empty() ? inputs.at(0) : false;
            default:
                return false;
        }
    } catch (const std::out_of_range &) {
        // Handle invalid input access safely
        return false;
    }
}

sf::FloatRect Gate::getBounds() const { return shape.getGlobalBounds(); }

sf::Vector2f Gate::getInputPinPosition(int pinIndex) const {
    if (type == GateType::NOT || type == GateType::OUTPUT) {
        return position + sf::Vector2f{0.f, 25.f};  // single input (center left)
    }
    return position + sf::Vector2f{0.f, (pinIndex == 0) ? 15.f : 35.f};  // two inputs
}

sf::Vector2f Gate::getOutputPinPosition() const {
    return position + sf::Vector2f{50.f, 25.f};  // right center
}

void Gate::setState(bool state) {
    this->state = state;
    if (type == GateType::INPUT || type == GateType::OUTPUT) {
        shape.setFillColor(state ? PIN_HIGH_COLOR : GATE_IO_FILL);
    }
}

bool Gate::getState() const { return state; }

GateType Gate::getType() const { return type; }

std::string Gate::getGateTypeString(size_t gateIndex, const std::vector<Gate> &gates) const {
    switch (type) {
        case GateType::INPUT:
            return persistentLabel >= 0 ? std::string(1, 'A' + persistentLabel) : "IN";
        case GateType::OUTPUT:
            return persistentLabel >= 0 ? "Y" + std::to_string(persistentLabel + 1) : "OUT";
        case GateType::AND:
            return "AND";
        case GateType::OR:
            return "OR";
        case GateType::NOT:
            return "NOT";
        case GateType::NAND:
            return "NAND";
        case GateType::NOR:
            return "NOR";
        case GateType::XOR:
            return "XOR";
        default:
            return "?";
    }
}

void Gate::drawGateLabel(sf::RenderWindow &window, size_t gateIndex, const std::vector<Gate> &gates) const {
    if (!currentFont) return;

    try {
        sf::Text text(*currentFont, getGateTypeString(gateIndex, gates), FONT_CONTENT);
        text.setFillColor(COLOR_CONTENT_TEXT);

        sf::FloatRect textBounds = text.getLocalBounds();
        text.setOrigin({textBounds.size.x / 2.f, textBounds.size.y / 2.f});
        text.setPosition(position + sf::Vector2f{25.f, 25.f});  // center of gate

        window.draw(text);
    } catch (const std::exception &) {
        // Safely handle any SFML exceptions
        return;
    }
}

void Gate::setSelected(bool isSelected) { selected = isSelected; }
