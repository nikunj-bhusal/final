#pragma once
#include <SFML/Graphics.hpp>
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
    void cancelSelection(Circuit& circuit);
    void selectGateAt(sf::Vector2f worldPos, Circuit& circuit);
    void deleteSelectedGates(Circuit& circuit);
};