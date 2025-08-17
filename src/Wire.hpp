#pragma once
#include <SFML/Graphics.hpp>

class Wire {
   private:
    sf::VertexArray line;
    size_t srcGate;
    int srcPin;
    size_t dstGate;
    int dstPin;

   public:
    Wire(size_t srcGate, int srcPin, size_t dstGate, int dstPin) : srcGate(srcGate), srcPin(srcPin), dstGate(dstGate), dstPin(dstPin) {
        line = sf::VertexArray(sf::PrimitiveType::Lines, 2);
        line[0].color = sf::Color::Yellow;
        line[1].color = sf::Color::Yellow;
    }
    void draw(sf::RenderWindow &window) const {
        if (!window.isOpen()) return;
        window.draw(line);
        sf::VertexArray thickLine = line;
        thickLine[0].position.y -= 1;
        thickLine[1].position.y -= 1;
        window.draw(thickLine);
        thickLine[0].position.y += 2;
        thickLine[1].position.y += 2;
        window.draw(thickLine);
    }
    void setPositions(sf::Vector2f start, sf::Vector2f end) {
        line[0].position = start;
        line[1].position = end;
    }

    size_t getSrcGate() const { return srcGate; }
    int getSrcPin() const { return srcPin; }
    size_t getDstGate() const { return dstGate; }
    int getDstPin() const { return dstPin; }
    void setSrcGate(size_t g) { srcGate = g; }
    void setDstGate(size_t g) { dstGate = g; }
};