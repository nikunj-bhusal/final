#include "ComponentPalette.hpp"

const float LEFT_MARGIN = 50.f;
const float FONT_INSTRUCTION = 18;
const float TOP_MARGIN = 40.f;
const float SPACING = 25.f;
const float OUTLINE_THICKNESS = 2.f;
const float BOX_WIDTH = 180.f;
const float BOX_HEIGHT = 60.f;
const float BOX_Y_SPACING = BOX_HEIGHT + SPACING;
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

    titleText = sf::Text(*currentFont);
    titleText->setString("Components");
    titleText->setCharacterSize(20);
    titleText->setFillColor(sf::Color::White);

    sf::FloatRect tBounds = titleText->getLocalBounds();
    titleText->setPosition({LEFT_MARGIN + (BOX_WIDTH - tBounds.size.x) / 2.f - tBounds.position.x,
                            TOP_MARGIN + (BOX_HEIGHT - tBounds.size.y) / 2.f - tBounds.position.y});

    float instrStartY = BOX_Y_START + type.size() * BOX_Y_SPACING + SPACING;
    std::vector<std::string> instructions = {"CONTROLS:", "C             Clear", "Esc         Cancel Selection", "Del         Delete",
                                             "Q            Quit"};

    for (size_t i = 0; i < instructions.size(); ++i) {
        sf::Text instr(*currentFont);
        instr.setString(instructions[i]);
        instr.setCharacterSize(FONT_INSTRUCTION);
        instr.setFillColor(sf::Color::Black);
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

    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    sf::RectangleShape bg({desktop.size.x * 1.f, desktop.size.y * 1.f});
    bg.setFillColor(sf::Color(200, 200, 200, 200));
    bg.setPosition({0.f, 0.f});
    window.draw(bg);

    sf::RectangleShape titleBg({BOX_WIDTH, BOX_HEIGHT});
    titleBg.setPosition({LEFT_MARGIN, BOX_Y_START - BOX_HEIGHT - SPACING});
    titleBg.setFillColor(sf::Color::Black);
    titleBg.setOutlineThickness(OUTLINE_THICKNESS);
    titleBg.setOutlineColor(sf::Color::Black);
    window.draw(titleBg);

    if (currentFont && titleText.has_value()) window.draw(titleText.value());

    for (size_t i = 0; i < buttons.size(); ++i) {
        window.draw(buttons[i]);
        if (currentFont && i < buttonLabels.size()) window.draw(buttonLabels[i]);
    }

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