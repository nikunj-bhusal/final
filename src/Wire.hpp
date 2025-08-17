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
    Wire(size_t srcGate, int srcPin, size_t dstGate, int dstPin);
    void draw(sf::RenderWindow &window) const;
    void setPositions(sf::Vector2f start, sf::Vector2f end);
    size_t getSrcGate() const { return srcGate; }
    int getSrcPin() const { return srcPin; }
    size_t getDstGate() const { return dstGate; }
    int getDstPin() const { return dstPin; }
    void setSrcGate(size_t g) { srcGate = g; }
    void setDstGate(size_t g) { dstGate = g; }
};