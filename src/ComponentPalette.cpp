// ui/ComponentPalette.cpp

#include "ComponentPalette.hpp"

#include <iostream>
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

// const sf::Color COLOR_SELECTED = sf::Color::Yellow;
const sf::Color COLOR_HOVERED = sf::Color(220, 220, 220);
const sf::Color COLOR_DEFAULT = sf::Color::White;
const sf::Color COLOR_OUTLINE = sf::Color::Black;
const sf::Color COLOR_BG = sf::Color(200, 200, 200, 200);
const sf::Color COLOR_TITLE_BG = sf::Color::Black;
const sf::Color COLOR_TITLE_TEXT = sf::Color::White;
const sf::Color COLOR_LABEL_TEXT = sf::Color::Black;
const sf::Color COLOR_INSTR_TEXT = sf::Color::Black;
const sf::Color COLOR_PANEL_BG = sf::Color(220, 220, 220, 200);
const sf::Color COLOR_CONTENT_TEXT = sf::Color::Black;
const sf::Color COLOR_INPUT_BG = sf::Color(255, 255, 200);
const sf::Color COLOR_EXPRESSION_BG = sf::Color(200, 255, 200);
const sf::Color COLOR_TRUTH_BG = sf::Color(200, 200, 255);
const sf::Color GRID_COLOR = sf::Color(60, 60, 60);
const sf::Color PIN_OUTLINE_COLOR = sf::Color::Black;
const sf::Color PIN_HIGH_COLOR = sf::Color::Red;
const sf::Color PIN_LOW_COLOR = sf::Color::White;
const sf::Color GATE_SELECTED_OUTLINE = sf::Color::Yellow;
const sf::Color GATE_DEFAULT_OUTLINE = sf::Color::Black;
const sf::Color GATE_DEFAULT_FILL = sf::Color(200, 200, 200);
const sf::Color GATE_IO_FILL = sf::Color(128, 128, 128);
}  // namespace UIConstants

// sf::VideoMode desktop = sf::VideoMode::getDesktopMode();

using namespace UIConstants;
const float BOX_Y_START = BOX_Y_SPACING + TOP_MARGIN;

ComponentPalette::ComponentPalette() {
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    sf::Vector2f paletteSize = {desktop.size.x * 0.18f, desktop.size.y * 1.f};
    uiView.setSize(paletteSize);
    uiView.setCenter(paletteSize / 2.f);
    uiView.setViewport(sf::FloatRect({0, 0}, {0.18f, 1}));

    type = {GateType::INPUT, GateType::AND, GateType::OR, GateType::NOT, GateType::NAND, GateType::NOR, GateType::XOR, GateType::OUTPUT};
    setupButtons();
}

void ComponentPalette::setupButtons() {
    buttons.clear();

    for (size_t i = 0; i < type.size(); ++i) {
        sf::RectangleShape button({BOX_WIDTH, BOX_HEIGHT});
        button.setPosition({LEFT_MARGIN, BOX_Y_START + i * BOX_Y_SPACING});

        if (i == selectedIndex)
            button.setFillColor(sf::Color::Yellow);
        else if (i == hoveredIndex)
            button.setFillColor(sf::Color(220, 220, 220));
        else
            button.setFillColor(sf::Color::White);

        button.setOutlineThickness(OUTLINE_THICKNESS);
        button.setOutlineColor(sf::Color::Black);

        buttons.push_back(button);
    }

    if (currentFont != nullptr) setupTexts();
}

void ComponentPalette::setFont(const sf::Font &font) {
    currentFont = &font;
    setupTexts();
}

void ComponentPalette::setupTexts() {
    if (!currentFont) return;

    buttonLabels.clear();
    instructionTexts.clear();

    // Button labels
    for (size_t i = 0; i < type.size(); ++i) {
        sf::Text label(*currentFont);
        label.setString(getGateTypeName(type[i]));
        label.setCharacterSize(FONT_LABEL);
        label.setFillColor(COLOR_LABEL_TEXT);

        sf::Vector2f pos = buttons[i].getPosition();
        sf::Vector2f size = buttons[i].getSize();
        sf::FloatRect bounds = label.getLocalBounds();

        label.setPosition({pos.x + (size.x - bounds.size.x) / 2.f - bounds.position.x, pos.y + (size.y - bounds.size.y) / 2.f - bounds.position.y});

        buttonLabels.push_back(label);
    }

    // Title text
    titleText = sf::Text(*currentFont);
    titleText->setString("Components");
    titleText->setCharacterSize(FONT_TITLE);
    titleText->setFillColor(COLOR_TITLE_TEXT);

    sf::FloatRect tBounds = titleText->getLocalBounds();
    titleText->setPosition({LEFT_MARGIN + (BOX_WIDTH - tBounds.size.x) / 2.f - tBounds.position.x,
                            TOP_MARGIN + (BOX_HEIGHT - tBounds.size.y) / 2.f - tBounds.position.y});

    // Instructions
    float instrStartY = BOX_Y_START + type.size() * BOX_Y_SPACING + SPACING;
    std::vector<std::string> instructions = {"CONTROLS:",
                                             "O             Output (Circuit)",
                                             "I                Input Expression",
                                             "Enter     Output (Expression)",
                                             "C             Clear",
                                             "Esc         Cancel Selection",
                                             "Del         Delete",
                                             "Q            Quit"};

    for (size_t i = 0; i < instructions.size(); ++i) {
        sf::Text instr(*currentFont);
        instr.setString(instructions[i]);
        instr.setCharacterSize(FONT_INSTRUCTION);
        instr.setFillColor(COLOR_INSTR_TEXT);
        instr.setPosition({LEFT_MARGIN, instrStartY + i * SPACING});
        instructionTexts.push_back(instr);
    }
}

void ComponentPalette::handleEvent(const sf::Event &event, const sf::RenderWindow &window) {
    if (event.is<sf::Event::MouseMoved>()) {
        sf::Vector2i mousePixel = sf::Mouse::getPosition(window);
        sf::Vector2f mousePos = window.mapPixelToCoords(mousePixel, uiView);
        int newHovered = -1;

        if (mousePos.x >= LEFT_MARGIN && mousePos.x <= LEFT_MARGIN + BOX_WIDTH) {
            for (size_t i = 0; i < buttons.size(); ++i) {
                if (buttons[i].getGlobalBounds().contains(mousePos)) {
                    newHovered = static_cast<int>(i);
                    break;
                }
            }
        }

        if (newHovered != hoveredIndex) {
            hoveredIndex = newHovered;
            setupButtons();
        }
    }

    if (auto clicked = event.getIf<sf::Event::MouseButtonPressed>()) {
        if (clicked->button == sf::Mouse::Button::Left) {
            sf::Vector2i mp = sf::Mouse::getPosition(window);
            sf::Vector2f pos = window.mapPixelToCoords(mp, uiView);
            if (pos.x >= LEFT_MARGIN && pos.x <= LEFT_MARGIN + BOX_WIDTH) {
                for (size_t i = 0; i < buttons.size(); ++i) {
                    if (buttons[i].getGlobalBounds().contains(pos)) {
                        selectedIndex = static_cast<int>(i);
                        setupButtons();
                        break;
                    }
                }
            }
        }
    }
}

void ComponentPalette::update() { setupButtons(); }

void ComponentPalette::draw(sf::RenderWindow &window) {
    window.setView(uiView);

    // Background
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    sf::RectangleShape bg({desktop.size.x * 1.f, desktop.size.y * 1.f});
    bg.setFillColor(COLOR_BG);
    bg.setPosition({0.f, 0.f});
    window.draw(bg);

    // Title background
    sf::RectangleShape titleBg({BOX_WIDTH, BOX_HEIGHT});
    titleBg.setPosition({LEFT_MARGIN, BOX_Y_START - BOX_HEIGHT - SPACING});
    titleBg.setFillColor(COLOR_TITLE_BG);
    titleBg.setOutlineThickness(OUTLINE_THICKNESS);
    titleBg.setOutlineColor(COLOR_OUTLINE);
    window.draw(titleBg);

    if (currentFont && titleText.has_value()) window.draw(titleText.value());

    // Buttons
    for (size_t i = 0; i < buttons.size(); ++i) {
        window.draw(buttons[i]);
        if (currentFont && i < buttonLabels.size()) window.draw(buttonLabels[i]);
    }

    // Instructions
    if (currentFont) {
        for (auto &instr : instructionTexts) window.draw(instr);
    }
}

std::string ComponentPalette::getGateTypeName(GateType type) const {
    switch (type) {
        case GateType::INPUT:
            return "INPUT";
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
        case GateType::OUTPUT:
            return "OUTPUT";
        default:
            return "UNKNOWN";
    }
}

GateType ComponentPalette::getSelectedGateType() const { return type[selectedIndex]; }
