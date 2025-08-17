// ui/Canvas.cpp

#include "Canvas.hpp"

#include <cmath>

#include "Configuration.h"

using namespace UIConstants;

const sf::Mouse::Button DRAG_BUTTON = sf::Mouse::Button::Middle;

Canvas::Canvas(Simulator &simulator) : simulator(simulator) {
    sf::Vector2f canvasSize = WindowConfig::getCanvasSize();
    sf::Vector2f paletteSize = WindowConfig::getPaletteSize();

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

void Canvas::update() { simulator.update(); }

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
