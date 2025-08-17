#include "Gate.hpp"

#include <stdexcept>

namespace UIConstants {
constexpr float LEFT_MARGIN = 50.f;
constexpr float TOP_MARGIN = 40.f;
constexpr float SPACING = 25.f;
constexpr float OUTLINE_THICKNESS = 2.f;
constexpr float TITLE_HEIGHT = 40.f;
constexpr float SECTION_SPACING = 30.f;
constexpr float LINE_HEIGHT = 20.f;
constexpr float SECTION_WIDTH = 280.f;
constexpr float BOX_WIDTH = 180.f;
constexpr float BOX_HEIGHT = 60.f;
constexpr float BOX_Y_SPACING = BOX_HEIGHT + SPACING;
constexpr float FONT_TITLE = 20;
constexpr float FONT_LABEL = 18;
constexpr float FONT_INSTRUCTION = 18;
constexpr float FONT_CONTENT = 18;
constexpr float CELL_PADDING = 10.f;
constexpr float GRID_LINE_THICKNESS = 2.f;
constexpr float GRID_SIZE = 50.f;
constexpr float ZOOM_IN_FACTOR = 0.95f;
constexpr float ZOOM_OUT_FACTOR = 1.05f;

// const sf::Color COLOR_SELECTED = sf::Color::Yellow;
// const sf::Color COLOR_HOVERED = sf::Color(220, 220, 220);
// const sf::Color COLOR_DEFAULT = sf::Color::White;
// const sf::Color COLOR_OUTLINE = sf::Color::Black;
// const sf::Color COLOR_BG = sf::Color(200, 200, 200, 200);
// const sf::Color COLOR_TITLE_BG = sf::Color::Black;
// const sf::Color COLOR_TITLE_TEXT = sf::Color::White;
// const sf::Color COLOR_LABEL_TEXT = sf::Color::Black;
// const sf::Color COLOR_INSTR_TEXT = sf::Color::Black;
// const sf::Color COLOR_PANEL_BG = sf::Color(220, 220, 220, 200);
// const sf::Color COLOR_CONTENT_TEXT = sf::Color::Black;
// const sf::Color COLOR_INPUT_BG = sf::Color(255, 255, 200);
// const sf::Color COLOR_EXPRESSION_BG = sf::Color(200, 255, 200);
// const sf::Color COLOR_TRUTH_BG = sf::Color(200, 200, 255);
// const sf::Color GRID_COLOR = sf::Color(60, 60, 60);
// const sf::Color PIN_OUTLINE_COLOR = sf::Color::Black;
// const sf::Color PIN_HIGH_COLOR = sf::Color::Red;
// const sf::Color PIN_LOW_COLOR = sf::Color::White;
// const sf::Color GATE_SELECTED_OUTLINE = sf::Color::Yellow;
// const sf::Color GATE_DEFAULT_OUTLINE = sf::Color::Black;
// const sf::Color GATE_DEFAULT_FILL = sf::Color(200, 200, 200);
// const sf::Color sf::Color(128, 128, 128) = sf::Color(128, 128, 128);
}  // namespace UIConstants

using namespace UIConstants;

Gate::Gate(GateType type, sf::Vector2f position, int persistentLabel) : type(type), position(position), persistentLabel(persistentLabel) {
    using namespace UIConstants;
    shape.setSize({50.f, 50.f});
    shape.setPosition(position);
    shape.setOutlineThickness(OUTLINE_THICKNESS);
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
        gateShape.setOutlineThickness(OUTLINE_THICKNESS * 2);
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
        pin.setFillColor(sf::Color::White);

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
        sf::Text text(*currentFont, getGateTypeString(gateIndex, gates), FONT_CONTENT);
        text.setFillColor(sf::Color::Black);

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
