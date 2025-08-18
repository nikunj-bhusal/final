#include "Simulator.hpp"

#include <chrono>

int g_selectedGate = -1;
int g_selectedPin = -100;

Simulator::Simulator() {}

void Simulator::handleEvent(const sf::Event &event, const sf::RenderWindow &window, const sf::View &view, GateType selectedGateType) {
    if (const auto *clicked = event.getIf<sf::Event::MouseButtonPressed>()) {
        if (clicked->button == sf::Mouse::Button::Left) {
            sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
            sf::Vector2i mousePixel = sf::Mouse::getPosition(window);
            sf::Vector2f mousePos{static_cast<float>(mousePixel.x), static_cast<float>(mousePixel.y)};
            if (ui.getShowExpression()) {
                sf::FloatRect closeBtn({1110.f, 55.f}, {60.f, 20.f});
                if (closeBtn.contains(mousePos)) {
                    ui.setShowExpression(false);
                    return;
                }
            }
            if (ui.getShowTruthTable()) {
                sf::FloatRect closeBtn({1110.f, 205.f}, {60.f, 20.f});
                if (closeBtn.contains(mousePos)) {
                    ui.setShowTruthTable(false);
                    return;
                }
            }
            if (ui.getShowInputField(1) || ui.getShowInputField(2)) {
                sf::FloatRect closeBtn({1110.f, 355.f}, {60.f, 20.f});
                if (closeBtn.contains(mousePos)) {
                    ui.setShowInputField(false, 1);
                    ui.setShowInputField(false, 2);
                    ui.setInputExpression("", 1);
                    ui.setInputExpression("", 2);
                    return;
                }
            }
            if (mousePos.x <= desktop.size.x * 0.18f) return;
            float windowWidth = window.getSize().x;
            float rightPanelStart = windowWidth * (0.75f);
            if (mousePos.x >= rightPanelStart) return;
            sf::Vector2f worldPos = window.mapPixelToCoords(mousePixel, view);
            bool hitGate = false;
            for (size_t i = 0; i < circuit.getGates().size(); ++i) {
                if (circuit.getGates()[i].getType() != GateType::OUTPUT) {
                    sf::Vector2f outPin = circuit.getGates()[i].getOutputPinPosition();
                    if (sf::FloatRect(outPin - sf::Vector2f{8.f, 8.f}, {16.f, 16.f}).contains(worldPos)) {
                        if (selection.isSelectingSource() && selection.getSelectedGate() == std::numeric_limits<size_t>::max()) {
                            selection.setSelectedGate(i);
                            selection.setSelectedPin(-1);
                            selection.setSelectingSource(false);
                        }
                        hitGate = true;
                        break;
                    }
                }
                if (circuit.getGates()[i].getType() != GateType::INPUT) {
                    int inputCount =
                        (circuit.getGates()[i].getType() == GateType::NOT || circuit.getGates()[i].getType() == GateType::OUTPUT) ? 1 : 2;
                    for (int j = 0; j < inputCount; ++j) {
                        sf::Vector2f inPin = circuit.getGates()[i].getInputPinPosition(j);
                        if (sf::FloatRect(inPin - sf::Vector2f{8.f, 8.f}, {16.f, 16.f}).contains(worldPos)) {
                            if (!selection.isSelectingSource() && selection.getSelectedGate() != std::numeric_limits<size_t>::max() &&
                                selection.getSelectedGate() < circuit.getGates().size()) {
                                circuit.addWire(selection.getSelectedGate(), selection.getSelectedPin(), i, j);
                                selection.setSelectedGate(std::numeric_limits<size_t>::max());
                                selection.setSelectedPin(-1);
                                selection.setSelectingSource(true);
                            }
                            hitGate = true;
                            break;
                        }
                    }
                    if (hitGate) break;
                }
                if (circuit.getGates()[i].getBounds().contains(worldPos)) {
                    if (circuit.getGates()[i].getType() == GateType::INPUT) {
                        circuit.getGates()[i].setState(!circuit.getGates()[i].getState());
                    }
                    selection.selectGateAt(worldPos, circuit);
                    hitGate = true;
                    break;
                }
            }
            if (!hitGate && selection.getSelectedGate() == std::numeric_limits<size_t>::max()) {
                circuit.addGate(selectedGateType, worldPos);
            }
        } else if (clicked->button == sf::Mouse::Button::Right) {
            selection.cancelSelection(circuit);
        }
    }
    if (ui.getShowInputField(1) || ui.getShowInputField(2)) {
        if (const auto *textEntered = event.getIf<sf::Event::TextEntered>()) {
            if (textEntered->unicode < 128) {
                char c = static_cast<char>(textEntered->unicode);
                if (std::tolower(c) == 'o') return;

                int activeField = ui.getActiveExpressionField();
                std::string inp = ui.getInputExpression(activeField);

                if (c == '\b' && !inp.empty()) {
                    inp.pop_back();
                } else if (c == '\r') {
                    if (!inp.empty()) {
                        std::vector<std::string> singleExpression = {inp};
                        ui.processMultipleOutputs(singleExpression);
                    }
                } else if ((std::isalpha(c) && std::tolower(c) != 'o') || c == '.' || c == '+' || c == '~' || c == '^' || c == '(' || c == ')') {
                    inp += c;
                }
                ui.setInputExpression(inp, activeField);
                ui.initializeUITexts();
            }
        }
    }
}

void Simulator::update() {
    circuit.updateWirePositions();
    circuit.evaluateCircuit();

    ui.updateFromCircuit(circuit);
}

void Simulator::draw(sf::RenderWindow &window) const {
    g_selectedGate = (int)selection.getSelectedGate();
    g_selectedPin = selection.getSelectedPin();
    circuit.drawAllGates(window);
    g_selectedGate = -1;
    g_selectedPin = -100;
    for (const auto &wire : circuit.getWires()) {
        wire.draw(window);
    }
}

void Simulator::drawUI(sf::RenderWindow &window) const { ui.drawUI(window); }

void Simulator::generateTruthTable() { ui.updateFromCircuit(circuit); }

void Simulator::generateLogicalExpression() { UIManager ui.updateFromCircuit(circuit); }

void Simulator::clearCircuit() {
    circuit.clearCircuit();
    selection.cancelSelection(circuit);
}

void Simulator::deleteSelectedGates() { selection.deleteSelectedGates(circuit); }

void Simulator::cancelSelection() { selection.cancelSelection(circuit); }

void Simulator::setFont(const sf::Font &font) {
    ui.setFont(font);
    circuit.setFont(font);
}

void Simulator::generateExpressionTruthTable() { ui.updateFromCircuit(circuit); }