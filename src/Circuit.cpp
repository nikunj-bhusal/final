#include "Circuit.hpp"

#include <algorithm>

#include "Gate.hpp"

void Circuit::setFont(const sf::Font& font) {
    currentFont = &font;
    for (auto& gate : gates) {
        gate.setFont(font);
    }
}

void Circuit::deselectAllGates() {
    for (auto& gate : gates) {
        gate.setSelected(false);
    }
}
void Circuit::addGate(GateType type, sf::Vector2f position) {
    if (gates.size() >= 100) {
        return;
    }

    if (type == GateType::INPUT && inputCounter >= 4) {
        return;
    }

    if (type == GateType::OUTPUT && outputCounter >= 2) {
        return;
    }

    int label = -1;
    if (type == GateType::INPUT) {
        label = nextInputLabel++;
        ++inputCounter;
    } else if (type == GateType::OUTPUT) {
        label = nextOutputLabel++;
        ++outputCounter;
    }

    try {
        gates.emplace_back(type, position, label);
        if (currentFont) {
            gates.back().setFont(*currentFont);
        }
    } catch (const std::exception& e) {
        if (type == GateType::INPUT) {
            --inputCounter;
            --nextInputLabel;
        } else if (type == GateType::OUTPUT) {
            --outputCounter;
            --nextOutputLabel;
        }
    }
}

void Circuit::addWire(size_t srcGate, int srcPin, size_t dstGate, int dstPin) { wires.emplace_back(srcGate, srcPin, dstGate, dstPin); }

void Circuit::clearCircuit() {
    gates.clear();
    wires.clear();
    inputCounter = 0;
    outputCounter = 0;
    nextInputLabel = 0;
    nextOutputLabel = 0;
}

void Circuit::drawAllGates(sf::RenderWindow& window) const {
    if (!window.isOpen() || gates.empty()) return;

    int selectedGate = -1;
    int selectedPin = -100;
    extern int g_selectedGate;
    extern int g_selectedPin;

    try {
        selectedGate = (g_selectedGate >= 0 && g_selectedGate < static_cast<int>(gates.size())) ? g_selectedGate : -1;
        selectedPin = (selectedGate != -1 && g_selectedPin >= -1) ? g_selectedPin : -100;

        const size_t gateCount = gates.size();
        for (size_t i = 0; i < gateCount; ++i) {
            try {
                if (i >= gates.size()) break;

                const Gate& currentGate = gates[i];
                if ((int)i == selectedGate) {
                    currentGate.draw(window, i, gates, selectedPin);
                } else {
                    currentGate.draw(window, i, gates, -100);
                }
            } catch (const std::exception&) {
                continue;
            }
        }
    } catch (const std::exception&) {
        return;
    }
}

void Circuit::removeGate(size_t gateIndex) {
    if (gateIndex < gates.size()) {
        gates.erase(gates.begin() + gateIndex);

        int inputLabel = 0, outputLabel = 0;
        inputCounter = 0;
        outputCounter = 0;

        for (auto& gate : gates) {
            if (gate.getType() == GateType::INPUT) {
                gate.setPersistentLabel(inputLabel++);
                ++inputCounter;
            } else if (gate.getType() == GateType::OUTPUT) {
                gate.setPersistentLabel(outputLabel++);
                ++outputCounter;
            }
        }
        nextInputLabel = inputLabel;
        nextOutputLabel = outputLabel;
    }
}

void Circuit::removeWiresConnectedToGate(size_t gateIndex) {
    if (gateIndex >= gates.size()) return;

    wires.erase(
        std::remove_if(wires.begin(), wires.end(), [gateIndex](const Wire& w) { return w.getSrcGate() == gateIndex || w.getDstGate() == gateIndex; }),
        wires.end());

    for (auto& w : wires) {
        if (w.getSrcGate() > gateIndex) w.setSrcGate(w.getSrcGate() - 1);
        if (w.getDstGate() > gateIndex) w.setDstGate(w.getDstGate() - 1);
    }
}

void Circuit::updateWirePositions() {
    wires.erase(std::remove_if(wires.begin(), wires.end(),
                               [this](const Wire& w) { return w.getSrcGate() >= gates.size() || w.getDstGate() >= gates.size(); }),
                wires.end());

    for (auto& wire : wires) {
        try {
            sf::Vector2f start, end;
            const bool isFeedbackLoop = wire.getDstGate() <= wire.getSrcGate();

            const Gate& srcGate = gates.at(wire.getSrcGate());
            start = (wire.getSrcPin() == -1) ? srcGate.getOutputPinPosition() : srcGate.getInputPinPosition(wire.getSrcPin());

            const Gate& dstGate = gates.at(wire.getDstGate());
            end = (wire.getDstPin() == -1) ? dstGate.getOutputPinPosition() : dstGate.getInputPinPosition(wire.getDstPin());

            if (isFeedbackLoop) {
                const float vertOffset = 40.0f;
                const float horizOffset = 30.0f;

                sf::Vector2f p1 = start + sf::Vector2f(horizOffset, 0);
                sf::Vector2f p2 = p1 + sf::Vector2f(0, -vertOffset);
                sf::Vector2f p3 = end + sf::Vector2f(-horizOffset, -vertOffset);
                sf::Vector2f p4 = end + sf::Vector2f(-horizOffset, 0);

                wire.setPositions(start, end);
            } else {
                wire.setPositions(start, end);
            }
        } catch (const std::exception&) {
            continue;
        }
    }
}

void Circuit::evaluateCircuit() {
    if (gates.empty()) return;

    const int MAX_ITERATIONS = 100;
    const size_t gateCount = gates.size();

    std::vector<bool> newStates(gateCount, false);
    std::vector<bool> oldStates(gateCount, false);
    std::vector<bool> gateEvaluated(gateCount, false);

    for (size_t i = 0; i < gateCount; ++i) {
        oldStates[i] = gates[i].getState();
        newStates[i] = oldStates[i];

        if (gates[i].getType() == GateType::INPUT) {
            gateEvaluated[i] = true;
        }
    }

    bool changed = true;
    int iterations = 0;

    while (changed && iterations < MAX_ITERATIONS) {
        changed = false;

        for (size_t i = 0; i < gateCount; ++i) {
            if (gates[i].getType() != GateType::INPUT) {
                gateEvaluated[i] = false;
            }
        }

        for (size_t i = 0; i < gateCount; ++i) {
            if (gateEvaluated[i]) continue;

            if (gates[i].getType() == GateType::OUTPUT) {
                std::vector<bool> inputs;
                for (const auto& w : wires) {
                    if (w.getDstGate() == i && w.getSrcGate() < gateCount && gateEvaluated[w.getSrcGate()]) {
                        inputs.push_back(oldStates[w.getSrcGate()]);
                    }
                }
                newStates[i] = inputs.empty() ? false : inputs[0];
                gateEvaluated[i] = true;
                if (newStates[i] != oldStates[i]) changed = true;
                continue;
            }

            std::vector<bool> inputs;
            bool allInputsEvaluated = true;

            for (const auto& w : wires) {
                if (w.getDstGate() == i && w.getSrcGate() < gateCount) {
                    if (!gateEvaluated[w.getSrcGate()]) {
                        allInputsEvaluated = false;
                        break;
                    }
                    inputs.push_back(oldStates[w.getSrcGate()]);
                }
            }

            if (allInputsEvaluated) {
                newStates[i] = inputs.empty() ? false : gates[i].evaluate(inputs);
                gateEvaluated[i] = true;
                if (newStates[i] != oldStates[i]) changed = true;
            }
        }

        oldStates = newStates;
        iterations++;
    }

    for (size_t i = 0; i < gates.size(); ++i) {
        gates[i].setState(newStates[i]);
    }
}
std::vector<size_t> Circuit::getInputGates() const {
    std::vector<size_t> inputs;
    for (size_t i = 0; i < gates.size(); ++i) {
        if (gates[i].getType() == GateType::INPUT) inputs.push_back(i);
    }
    return inputs;
}

std::vector<size_t> Circuit::getOutputGates() const {
    std::vector<size_t> outputs;
    if (gates.empty()) return outputs;

    for (size_t i = 0; i < gates.size(); ++i) {
        if (gates[i].getType() == GateType::OUTPUT) {
            outputs.push_back(i);
        }
    }
    return outputs;
}

std::string Circuit::generateExpressionForGate(size_t gateIndex, std::map<size_t, std::string>& expressions) const {
    if (gateIndex >= gates.size()) return "0";
    if (auto it = expressions.find(gateIndex); it != expressions.end()) {
        return it->second;
    }

    const Gate& gate = gates[gateIndex];
    std::string result;

    if (gate.getType() == GateType::INPUT) {
        result = (gate.getPersistentLabel() >= 0) ? std::string(1, static_cast<char>('A' + gate.getPersistentLabel())) : "0";
        expressions[gateIndex] = result;
        return result;
    }

    std::vector<std::string> inputExprs;
    std::vector<size_t> inputGates;

    for (const Wire& wire : wires) {
        if (wire.getDstGate() != gateIndex || wire.getSrcGate() >= gates.size()) continue;

        if (wire.getSrcGate() == gateIndex) continue;

        inputGates.push_back(wire.getSrcGate());

        std::string inExpr = generateExpressionForGate(wire.getSrcGate(), expressions);
        if (inExpr != "0") {
            inputExprs.push_back(inExpr);
        }
    }

    std::sort(inputExprs.begin(), inputExprs.end());

    if (inputExprs.empty()) {
        result = "0";
    } else if (gate.getType() == GateType::OUTPUT) {
        result = inputExprs[0];
    } else {
        bool validInputs = (gate.getType() == GateType::NOT && inputExprs.size() == 1) || (gate.getType() != GateType::NOT && inputExprs.size() == 2);

        if (!validInputs) {
            result = "0";
        } else {
            switch (gate.getType()) {
                case GateType::AND:
                    result = "(" + inputExprs[0] + "." + inputExprs[1] + ")";
                    break;
                case GateType::OR:
                    result = "(" + inputExprs[0] + "+" + inputExprs[1] + ")";
                    break;
                case GateType::NOT:
                    result = "~(" + inputExprs[0] + ")";
                    break;
                case GateType::NAND:
                    result = "~(" + inputExprs[0] + "." + inputExprs[1] + ")";
                    break;
                case GateType::NOR:
                    result = "~(" + inputExprs[0] + "+" + inputExprs[1] + ")";
                    break;
                case GateType::XOR:
                    result = "(" + inputExprs[0] + "^" + inputExprs[1] + ")";
                    break;
                default:
                    result = "0";
            }
        }
    }

    expressions[gateIndex] = result;
    return result;
}

std::string Circuit::getGateSymbol(GateType type) const {
    switch (type) {
        case GateType::AND:
            return ".";
        case GateType::OR:
            return "+";
        case GateType::NOT:
            return "~";
        case GateType::NAND:
            return "NAND";
        case GateType::NOR:
            return "NOR";
        case GateType::XOR:
            return "^";
        default:
            return "";
    }
}