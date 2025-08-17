#include "ComponentPalette.hpp"

namespace {
constexpr float LEFT_MARGIN = 50.f;
constexpr float TOP_MARGIN = 40.f;
constexpr float SPACING = 25.f;

constexpr float BOX_WIDTH = 180.f;
constexpr float BOX_HEIGHT = 60.f;
constexpr float OUTLINE_THICKNESS = 2.f;

constexpr float BOX_Y_SPACING = BOX_HEIGHT + SPACING;
constexpr float BOX_Y_START = BOX_Y_SPACING + TOP_MARGIN;

}  // namespace

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

    // Gate labels
    for (size_t i = 0; i < type.size(); ++i) {
        sf::Text label(*currentFont);
        label.setString(getGateTypeName(type[i]));
        label.setCharacterSize(18);
        label.setFillColor(sf::Color::Black);

        sf::Vector2f pos = buttons[i].getPosition();
        sf::Vector2f size = buttons[i].getSize();
        sf::FloatRect bounds = label.getLocalBounds();

        label.setPosition({pos.x + (size.x - bounds.size.x) / 2.f - bounds.position.x, pos.y + (size.y - bounds.size.y) / 2.f - bounds.position.y});

        buttonLabels.push_back(label);
    }

    // Title
    titleText = sf::Text(*currentFont);
    titleText->setString("Components");
    titleText->setCharacterSize(20);
    titleText->setFillColor(sf::Color::White);

    sf::FloatRect tBounds = titleText->getLocalBounds();
    titleText->setPosition({LEFT_MARGIN + (BOX_WIDTH - tBounds.size.x) / 2.f - tBounds.position.x,
                            TOP_MARGIN + (BOX_HEIGHT - tBounds.size.y) / 2.f - tBounds.position.y});

    // Instructions
    float instrStartY = BOX_Y_START + type.size() * BOX_Y_SPACING + SPACING;
    std::vector<std::string> instructions = {
        "CONTROLS:",           "O             Output (Circuit)", "I             Input Expression", "Enter         Output (Expression)",
        "C             Clear", "Esc           Cancel Selection", "Del           Delete",           "Q             Quit"};

    for (size_t i = 0; i < instructions.size(); ++i) {
        sf::Text instr(*currentFont);
        instr.setString(instructions[i]);
        instr.setCharacterSize(18);
        instr.setFillColor(sf::Color::Black);
        instr.setPosition({LEFT_MARGIN, instrStartY + i * SPACING});
        instructionTexts.push_back(instr);
    }
}
