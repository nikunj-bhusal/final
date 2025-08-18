#include "App.hpp"

App::App() : window(sf::VideoMode::getDesktopMode(), "Digital Logic Suite"), simulator(), canvas(simulator), palette() {
    if (!font.openFromFile("src/assets/fonts/poppins.ttf")) {
        return;
    }
    simulator.setFont(font);
    palette.setFont(font);
}

void App::run() {
    while (window.isOpen()) {
        processEvents();
        update();
        render();
    }
}

void App::processEvents() {
    while (auto event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window.close();
        } else if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
            switch (key->scancode) {
                case sf::Keyboard::Scancode::C:
                    simulator.clearCircuit();
                    break;
                case sf::Keyboard::Scancode::Escape:
                    simulator.cancelSelection();
                    break;
                case sf::Keyboard::Scancode::Delete:
                    simulator.deleteSelectedGates();
                    break;
                case sf::Keyboard::Scancode::Q:
                    window.close();
                    break;
                default:
                    break;
            }
        }

        canvas.handleEvent(*event, window);
        palette.handleEvent(*event, window);
        simulator.handleEvent(*event, window, canvas.getView(), palette.getSelectedGateType());
    }
}

void App::update() {
    palette.update();
    simulator.update();
}

void App::render() {
    window.clear(sf::Color(30, 30, 30));
    canvas.draw(window);
    palette.draw(window);
    simulator.drawUI(window);
    window.display();
}
