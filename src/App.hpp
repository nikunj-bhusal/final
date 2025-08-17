// App.hpp
#pragma once
#include <SFML/Graphics.hpp>

#include "Canvas.hpp"
#include "ComponentPalette.hpp"
#include "Simulator.hpp"

class App {
   private:
    void processEvents();
    void update();
    void render();

    sf::RenderWindow window;
    sf::Font font;
    Simulator simulator;
    Canvas canvas;
    ComponentPalette palette;

   public:
    App();
    void run();
};
