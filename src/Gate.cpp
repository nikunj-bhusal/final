#include "Gate.hpp"

#include <stdexcept>

Gate::Gate(GateType type, sf::Vector2f position, int persistentLabel) : type(type), position(position), persistentLabel(persistentLabel) {
    shape.setSize({100.f, 70.f});
    shape.setPosition(position);
    shape.setOutlineThickness(2.f);
    shape.setOutlineColor(sf::Color::Black);

    switch (type) {
        case GateType::INPUT:
        case GateType::OUTPUT:
            shape.setFillColor(sf::Color(128, 128, 128));
            break;
        default:
            shape.setFillColor(sf::Color(200, 200, 200));
            break;
    }
}

void Gate::setFont(const sf::Font &font) {
    if (font.getInfo().family.empty()) return;
    currentFont = &font;
}

void Gate::draw(sf::RenderWindow &window, size_t gateIndex, const std::vector<Gate> &gates, int selectedPin) const {
    sf::RectangleShape gateShape = shape;

    if (selected) {
        gateShape.setOutlineThickness(2.f * 2);
        gateShape.setOutlineColor(sf::Color::Yellow);
    }

    window.draw(gateShape);

    // Label
    drawGateLabel(window, gateIndex, gates);

    // Draw pins
    sf::CircleShape pin(6.f);
    pin.setOutlineThickness(1.f);
    pin.setOutlineColor(sf::Color::Black);

    // Output pin (red if HIGH, white if LOW)
    if (type != GateType::OUTPUT) {
        pin.setFillColor(state ? sf::Color::Red : sf::Color::White);
        sf::Vector2f outPinPos = getOutputPinPosition();
        pin.setPosition(outPinPos - sf::Vector2f{6.f, 6.f});
        window.draw(pin);
        // Highlight if selected
        if (selectedPin == -1) {
            drawPinHighlight(window, outPinPos);
        }
    }

    // Input pins
    if (type != GateType::INPUT) {
        int inputCount = (type == GateType::NOT || type == GateType::OUTPUT) ? 1 : 2;
        pin.setFillColor(sf::Color::White);

        for (int i = 0; i < inputCount; ++i) {
            sf::Vector2f inPinPos = getInputPinPosition(i);
            pin.setPosition(inPinPos - sf::Vector2f{6.f, 6.f});
            window.draw(pin);
            // Highlight if selected
            if (selectedPin == i) {
                drawPinHighlight(window, inPinPos);
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
                return !(inputs.size() >= 2 && inputs.at(0) && inputs.at(1));
            case GateType::NOR:
                return !(inputs.size() >= 2 && (inputs.at(0) || inputs.at(1)));
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
        return position + sf::Vector2f{0.f, 35.f};  // single input (center left)
    }
    return position + sf::Vector2f{0.f, (pinIndex == 0) ? 20.f : 50.f};  // two inputs
}

sf::Vector2f Gate::getOutputPinPosition() const {
    return position + sf::Vector2f{100.f, 35.f};  // right center
}

void Gate::setState(bool state) {
    this->state = state;
    if (type == GateType::INPUT || type == GateType::OUTPUT) {
        shape.setFillColor(state ? sf::Color::Red : sf::Color(128, 128, 128));
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
        sf::Text text(*currentFont, getGateTypeString(gateIndex, gates), 16);
        text.setFillColor(sf::Color::Black);

        sf::FloatRect textBounds = text.getLocalBounds();
        text.setOrigin({textBounds.size.x / 2.f, textBounds.size.y / 2.f});
        text.setPosition(position + sf::Vector2f{50.f, 35.f});  // center of gate

        window.draw(text);
    } catch (const std::exception &) {
        // Safely handle any SFML exceptions
        return;
    }
}

void Gate::drawPinHighlight(sf::RenderWindow &window, sf::Vector2f pinPos) const {
    sf::CircleShape highlight(8.f);
    highlight.setOrigin({8.f, 8.f});
    highlight.setPosition(pinPos);
    highlight.setFillColor(sf::Color::Transparent);
    highlight.setOutlineColor(sf::Color(0, 120, 255));
    highlight.setOutlineThickness(4.f);
    window.draw(highlight);
}

void Gate::setSelected(bool isSelected) { selected = isSelected; }
