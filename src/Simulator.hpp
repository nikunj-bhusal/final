#pragma once
#include <SFML/Graphics.hpp>

#include "Circuit.hpp"
#include "Selection.hpp"
#include "UIManager.hpp"

class Simulator {
   private:
    Circuit circuit;
    Selection selection;
    UIManager ui;

   public:
    Simulator();
    void handleEvent(const sf::Event &event, const sf::RenderWindow &window, const sf::View &view, GateType selectedGateType);
    void update();
    void draw(sf::RenderWindow &window) const;
    void drawUI(sf::RenderWindow &window) const;
    void generateTruthTable();
    void generateLogicalExpression();
    void clearCircuit();
    void deleteSelectedGates();
    void cancelSelection();
    void setFont(const sf::Font &font);
    void toggleInputField(int expressionNumber = 1);
    void generateExpressionTruthTable();
};