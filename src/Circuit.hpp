#pragma once
#include <SFML/Graphics.hpp>
#include <map>
#include <string>
#include <vector>

#include "Gate.hpp"
#include "Wire.hpp"

class Circuit {
   private:
    std::vector<Gate> gates;
    std::vector<Wire> wires;
    int inputCounter = 0;
    int outputCounter = 0;
    int nextInputLabel = 0;
    int nextOutputLabel = 0;
    const sf::Font* currentFont = nullptr;

    // Helper function to detect cycles in expression evaluation
    bool hasCycle(size_t startGate, std::vector<bool>& visited, std::vector<bool>& inStack) const;

   public:
    void setFont(const sf::Font& font);
    void addGate(GateType type, sf::Vector2f position);
    void addWire(size_t srcGate, int srcPin, size_t dstGate, int dstPin);
    void clearCircuit();
    void deselectAllGates();
    void drawAllGates(sf::RenderWindow& window) const;
    void removeGate(size_t gateIndex);
    void removeWiresConnectedToGate(size_t gateIndex);
    void updateWirePositions();
    void evaluateCircuit();
    std::vector<size_t> getInputGates() const;
    std::vector<size_t> getOutputGates() const;
    std::string generateExpressionForGate(size_t gateIndex, std::map<size_t, std::string>& expressions) const;
    std::string getGateSymbol(GateType type) const;
    const std::vector<Gate>& getGates() const { return gates; }
    std::vector<Gate>& getGates() { return gates; }
    const std::vector<Wire>& getWires() const { return wires; }
    std::vector<Wire>& getWires() { return wires; }
};