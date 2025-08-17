#include "Canvas.hpp"

#include <cmath>

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
        float factor = (wheel->delta > 0) ? 0.95f : 1.05f;
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

    for (float x = std::floor(left / 50.f) * 50.f; x <= right; x += 50.f) {
        lines.append({{x, top}, sf::Color(60, 60, 60)});
        lines.append({{x, bottom}, sf::Color(60, 60, 60)});
    }

    for (float y = std::floor(top / 50.f) * 50.f; y <= bottom; y += 50.f) {
        lines.append({{left, y}, sf::Color(60, 60, 60)});
        lines.append({{right, y}, sf::Color(60, 60, 60)});
    }

    window.draw(lines);
}

void Canvas::draw(sf::RenderWindow &window) {
    window.setView(view);
    drawGrid(window);
    simulator.draw(window);
}
