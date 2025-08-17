#include "Selection.hpp"

#include <algorithm>

#include "Configuration.h"

using namespace UIConstants;

void Selection::cancelSelection(Circuit& circuit) {
    selectedGate = std::numeric_limits<size_t>::max();
    selectedPin = -1;
    selectingSource = true;
    selectedGates.clear();
    circuit.deselectAllGates();
}

void Selection::selectGateAt(sf::Vector2f worldPos, Circuit& circuit) {
    for (size_t i = 0; i < circuit.getGates().size(); ++i) {
        if (circuit.getGates()[i].getBounds().contains(worldPos)) {
            circuit.getGates()[i].setSelected(true);
            if (std::find(selectedGates.begin(), selectedGates.end(), i) == selectedGates.end()) {
                selectedGates.push_back(i);
            }
            break;
        }
    }
}

void Selection::deleteSelectedGates(Circuit& circuit) {
    if (selectedGates.empty()) return;

    std::sort(selectedGates.begin(), selectedGates.end());
    selectedGates.erase(std::unique(selectedGates.begin(), selectedGates.end()), selectedGates.end());
    std::sort(selectedGates.begin(), selectedGates.end(), std::greater<size_t>());

    std::vector<size_t> validIndices;
    for (size_t gateIndex : selectedGates) {
        if (gateIndex < circuit.getGates().size()) {
            validIndices.push_back(gateIndex);
        }
    }

    for (size_t gateIndex : validIndices) {
        circuit.removeWiresConnectedToGate(gateIndex);
        circuit.removeGate(gateIndex);
    }

    selectedGates.clear();
}