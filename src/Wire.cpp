// engine/Wire.cpp

// engine/Wire.cpp
#include "Wire.hpp"

#include "Configuration.h"
using namespace UIConstants;

Wire::Wire(size_t srcGate, int srcPin, size_t dstGate, int dstPin) : srcGate(srcGate), srcPin(srcPin), dstGate(dstGate), dstPin(dstPin) {
    line = sf::VertexArray(sf::PrimitiveType::Lines, 2);
    line[0].color = sf::Color::Black;
    line[1].color = sf::Color::Black;
}

void Wire::draw(sf::RenderWindow &window) const {
    if (!window.isOpen()) return;

    try {
        // Draw the main wire with thickness
        window.draw(line);

        // Create thicker appearance by drawing offset lines
        sf::VertexArray thickLine = line;

        // Draw upper line
        thickLine[0].position.y -= 1;
        thickLine[1].position.y -= 1;
        window.draw(thickLine);

        // Draw lower line
        thickLine[0].position.y += 2;
        thickLine[1].position.y += 2;
        window.draw(thickLine);
    } catch (const std::exception &) {
        // Safely handle any SFML drawing errors
        return;
    }
}

void Wire::setPositions(sf::Vector2f start, sf::Vector2f end) {
    try {
        line[0].position = start;
        line[1].position = end;
    } catch (const std::exception &) {
        // Handle any SFML position setting errors
        return;
    }
}
