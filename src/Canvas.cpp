// ui/Canvas.cpp

#include "Canvas.hpp"

#include <cmath>
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

const sf::Color COLOR_SELECTED = sf::Color::Yellow;
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

using namespace UIConstants;

const sf::Mouse::Button DRAG_BUTTON = sf::Mouse::Button::Middle;
sf::VideoMode desktop = sf::VideoMode::getDesktopMode();

Canvas::Canvas(Simulator &simulator) : simulator(simulator) {
    sf::Vector2f canvasSize = {desktop.size.x * 1.f, desktop.size.y * 1.f};
    sf::Vector2f paletteSize = {desktop.size.x * 0.18f, desktop.size.y * 1.f};

    view.setSize(canvasSize);
    view.setCenter({paletteSize.x + canvasSize.x / 2.f, canvasSize.y / 2.f});
}

void Canvas::handleEvent(const sf::Event &event, const sf::RenderWindow &window) {
    if (const auto *wheel = event.getIf<sf::Event::MouseWheelScrolled>()) {
        float factor = (wheel->delta > 0) ? ZOOM_IN_FACTOR : ZOOM_OUT_FACTOR;
        view.setSize(view.getSize() * factor);
        return;
    }

    if (const auto *pressed = event.getIf<sf::Event::MouseButtonPressed>()) {
        if (pressed->button == DRAG_BUTTON) {
            dragging = true;
            initialPos = static_cast<sf::Vector2f>(sf::Mouse::getPosition(window));
        }
        return;
    }

    if (const auto *released = event.getIf<sf::Event::MouseButtonReleased>()) {
        if (released->button == DRAG_BUTTON) {
            dragging = false;
        }
        return;
    }

    if (dragging && event.is<sf::Event::MouseMoved>()) {
        sf::Vector2f finalPos = static_cast<sf::Vector2f>(sf::Mouse::getPosition(window));
        sf::Vector2f diff = initialPos - finalPos;

        float zoomFactor = view.getSize().x / static_cast<float>(window.getSize().x);
        view.move(diff * zoomFactor);

        initialPos = finalPos;
    }
}

void Canvas::drawGrid(sf::RenderWindow &window) {
    sf::Vector2f size = view.getSize();
    sf::Vector2f center = view.getCenter();

    float left = center.x - size.x / 2.f;
    float right = center.x + size.x / 2.f;
    float top = center.y - size.y / 2.f;
    float bottom = center.y + size.y / 2.f;

    sf::VertexArray lines(sf::PrimitiveType::Lines);

    for (float x = std::floor(left / GRID_SIZE) * GRID_SIZE; x <= right; x += GRID_SIZE) {
        lines.append({{x, top}, GRID_COLOR});
        lines.append({{x, bottom}, GRID_COLOR});
    }

    for (float y = std::floor(top / GRID_SIZE) * GRID_SIZE; y <= bottom; y += GRID_SIZE) {
        lines.append({{left, y}, GRID_COLOR});
        lines.append({{right, y}, GRID_COLOR});
    }

    window.draw(lines);
}

void Canvas::draw(sf::RenderWindow &window) {
    window.setView(view);
    drawGrid(window);
    simulator.draw(window);
}
