#pragma once
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <limits>
#include <vector>

#include "Circuit.hpp"

class Selection {
   private:
    size_t selectedGate = std::numeric_limits<size_t>::max();
    int selectedPin = -1;
    bool selectingSource = true;
    std::vector<size_t> selectedGates;

   public:
    size_t getSelectedGate() const { return selectedGate; }
    void setSelectedGate(size_t g) { selectedGate = g; }
    int getSelectedPin() const { return selectedPin; }
    void setSelectedPin(int p) { selectedPin = p; }
    bool isSelectingSource() const { return selectingSource; }
    void setSelectingSource(bool s) { selectingSource = s; }
    const std::vector<size_t>& getSelectedGates() const { return selectedGates; }

    void cancelSelection(Circuit& circuit) {
        selectedGate = std::numeric_limits<size_t>::max();
        selectedPin = -1;
        selectingSource = true;
        selectedGates.clear();
        circuit.deselectAllGates();
    }

    void selectGateAt(sf::Vector2f worldPos, Circuit& circuit) {
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

    void deleteSelectedGates(Circuit& circuit) {
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
};