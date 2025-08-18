#pragma once
#include <SFML/Graphics.hpp>

#include "Simulator.hpp"

class Canvas {
   private:
    sf::View view;
    sf::Vector2f initialPos{};
    bool dragging = false;
    Simulator &simulator;

    void drawGrid(sf::RenderWindow &window);

   public:
    Canvas(Simulator &simulator);
    void handleEvent(const sf::Event &event, const sf::RenderWindow &window);
    void draw(sf::RenderWindow &window);
    const sf::View &getView() const { return view; }
};
