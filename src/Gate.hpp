#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

enum class GateType { AND, OR, NOT, NAND, NOR, XOR, INPUT, OUTPUT };

class Gate {
   private:
    GateType type;
    sf::Vector2f position;
    sf::RectangleShape shape;
    bool state = false;
    bool selected = false;
    int persistentLabel = -1;

    const sf::Font *currentFont = nullptr;

    void drawGateLabel(sf::RenderWindow &window, size_t gateIndex, const std::vector<Gate> &gates) const;
    void drawPinHighlight(sf::RenderWindow &window, sf::Vector2f pinPos) const;

   public:
    Gate(GateType type, sf::Vector2f position, int persistentLabel);
    int getPersistentLabel() const { return persistentLabel; }
    void setPersistentLabel(int label) { persistentLabel = label; }

    void setFont(const sf::Font &font);
    void draw(sf::RenderWindow &window, size_t gateIndex, const std::vector<Gate> &gates, int selectedPin = -100) const;
    bool evaluate(const std::vector<bool> &inputs) const;

    sf::Vector2f getInputPinPosition(int index) const;
    sf::Vector2f getOutputPinPosition() const;

    void setState(bool state);

    sf::FloatRect getBounds() const;
    bool getState() const;
    GateType getType() const;
    std::string getGateTypeString(size_t gateIndex, const std::vector<Gate> &gates) const;

    void setSelected(bool isSelected);
};
