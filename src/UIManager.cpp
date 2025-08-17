// engine/UIManager.cpp
#include "UIManager.hpp"

#include <numeric>

class GridConfig {
   public:
    static constexpr int GRID_ROWS = 12;
    static constexpr int GRID_COLS = 4;
    static constexpr float GRID_PADDING = 10.f;
    static constexpr float GRID_MARGIN = 15.f;

    static sf::Vector2f getGridCellSize() {
        sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
        sf::Vector2f panelSize = {desktop.size.x * 0.25f, desktop.size.y * 1.f};
        float cellWidth = (panelSize.x - 2 * GRID_MARGIN - (GRID_COLS - 1) * GRID_PADDING) / GRID_COLS;
        float cellHeight = (panelSize.y - 2 * GRID_MARGIN - (GRID_ROWS - 1) * GRID_PADDING) / GRID_ROWS;
        return {cellWidth, cellHeight};
    }

    static sf::Vector2f getGridPosition(int row, int col) {
        sf::Vector2f cellSize = getGridCellSize();
        float x = GRID_MARGIN + col * (cellSize.x + GRID_PADDING);
        float y = GRID_MARGIN + row * (cellSize.y + GRID_PADDING);
        return {x, y};
    }

    static sf::Vector2f getGridAreaSize(int rows, int cols) {
        sf::Vector2f cellSize = getGridCellSize();
        float width = cols * cellSize.x + (cols - 1) * GRID_PADDING;
        float height = rows * cellSize.y + (rows - 1) * GRID_PADDING;
        return {width, height};
    }
};

UIManager::UIManager() { setupRightPanelView(); }

void UIManager::setupRightPanelView() {
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    sf::Vector2f windowSize = {desktop.size.x * 1.f, desktop.size.y * 1.f};
    sf::Vector2f panelSize = {desktop.size.x * 0.25f, desktop.size.y * 1.f};
    rightPanelView.setSize(panelSize);
    rightPanelView.setCenter(panelSize / 2.f);
    float leftOffset = 0.75f;
    rightPanelView.setViewport(sf::FloatRect({leftOffset, 0}, {0.25f, 1}));
}

void UIManager::setFont(const sf::Font& font) {
    currentFont = &font;
    initializeUITexts();
}

void UIManager::setupBackgrounds() const {
    if (!rightPanelBg) {
        sf::Vector2f panelSize = rightPanelView.getSize();
        rightPanelBg = std::make_unique<sf::RectangleShape>(panelSize);
        rightPanelBg->setPosition({0.f, 0.f});
        rightPanelBg->setFillColor(sf::Color(220, 220, 220, 200));
    }

    // Expression 1 backgrounds
    if (!inputFieldBg1) {
        sf::Vector2f position = GridConfig::getGridPosition(0, 0);
        sf::Vector2f size = GridConfig::getGridAreaSize(1, 4);
        inputFieldBg1 = std::make_unique<sf::RectangleShape>(size);
        inputFieldBg1->setPosition(position);
        inputFieldBg1->setFillColor(sf::Color(220, 220, 220, 200));
        inputFieldBg1->setOutlineThickness(2.f);
        inputFieldBg1->setOutlineColor(sf::Color::Black);
    }
    if (!expressionBg1) {
        sf::Vector2f position = GridConfig::getGridPosition(1, 0);
        sf::Vector2f size = GridConfig::getGridAreaSize(1, 4);
        expressionBg1 = std::make_unique<sf::RectangleShape>(size);
        expressionBg1->setPosition(position);
        expressionBg1->setFillColor(sf::Color(200, 255, 200));
        expressionBg1->setOutlineThickness(2.f);
        expressionBg1->setOutlineColor(sf::Color::Black);
    }

    // Expression 2 backgrounds
    if (!inputFieldBg2) {
        sf::Vector2f position = GridConfig::getGridPosition(2, 0);
        sf::Vector2f size = GridConfig::getGridAreaSize(1, 4);
        inputFieldBg2 = std::make_unique<sf::RectangleShape>(size);
        inputFieldBg2->setPosition(position);
        inputFieldBg2->setFillColor(sf::Color(255, 255, 200));
        inputFieldBg2->setOutlineThickness(2.f);
        inputFieldBg2->setOutlineColor(sf::Color::Black);
    }
    if (!expressionBg2) {
        sf::Vector2f position = GridConfig::getGridPosition(3, 0);
        sf::Vector2f size = GridConfig::getGridAreaSize(1, 4);
        expressionBg2 = std::make_unique<sf::RectangleShape>(size);
        expressionBg2->setPosition(position);
        expressionBg2->setFillColor(sf::Color(200, 255, 200));
        expressionBg2->setOutlineThickness(2.f);
        expressionBg2->setOutlineColor(sf::Color::Black);
    }

    // Truth table background
    if (!truthTableBg) {
        sf::Vector2f position = GridConfig::getGridPosition(4, 0);
        sf::Vector2f size = GridConfig::getGridAreaSize(8, 4);
        truthTableBg = std::make_unique<sf::RectangleShape>(size);
        truthTableBg->setPosition(position);
        truthTableBg->setFillColor(sf::Color(200, 200, 255));
        truthTableBg->setOutlineThickness(2.f);
        truthTableBg->setOutlineColor(sf::Color::Black);
    }
}

void UIManager::setupTitles() const {
    if (!currentFont) return;

    // Expression 1 titles
    if (!inputTitleText1) {
        sf::Vector2f position = GridConfig::getGridPosition(0, 0);
        inputTitleText1 = std::make_unique<sf::Text>(*currentFont);
        inputTitleText1->setString("Input Expression 1");
        inputTitleText1->setCharacterSize(20);
        inputTitleText1->setFillColor(sf::Color::Black);
        inputTitleText1->setPosition({position.x + 10.f, position.y + 5.f});
    }
    if (!expressionTitleText1) {
        sf::Vector2f position = GridConfig::getGridPosition(1, 0);
        expressionTitleText1 = std::make_unique<sf::Text>(*currentFont);
        expressionTitleText1->setString("Expression 1 Output:");
        expressionTitleText1->setCharacterSize(20);
        expressionTitleText1->setFillColor(sf::Color::Black);
        expressionTitleText1->setPosition({position.x + 10.f, position.y + 5.f});
    }

    // Expression 2 titles
    if (!inputTitleText2) {
        sf::Vector2f position = GridConfig::getGridPosition(2, 0);
        inputTitleText2 = std::make_unique<sf::Text>(*currentFont);
        inputTitleText2->setString("Input Expression 2");
        inputTitleText2->setCharacterSize(20);
        inputTitleText2->setFillColor(sf::Color::Black);
        inputTitleText2->setPosition({position.x + 10.f, position.y + 5.f});
    }
    if (!expressionTitleText2) {
        sf::Vector2f position = GridConfig::getGridPosition(3, 0);
        expressionTitleText2 = std::make_unique<sf::Text>(*currentFont);
        expressionTitleText2->setString("Expression 2 Output:");
        expressionTitleText2->setCharacterSize(20);
        expressionTitleText2->setFillColor(sf::Color::Black);
        expressionTitleText2->setPosition({position.x + 10.f, position.y + 5.f});
    }

    // Truth table title
    if (!truthTableTitleText) {
        sf::Vector2f position = GridConfig::getGridPosition(4, 0);
        truthTableTitleText = std::make_unique<sf::Text>(*currentFont);
        truthTableTitleText->setString("Combined Truth Table:");
        truthTableTitleText->setCharacterSize(20);
        truthTableTitleText->setFillColor(sf::Color::Black);
        truthTableTitleText->setPosition({position.x + 10.f, position.y + 5.f});
    }
}

void UIManager::initializeUITexts() {
    if (!currentFont) return;
    setupBackgrounds();
    setupTitles();

    // Initialize Expression 1 input field
    if (!inputFieldText1) {
        sf::Vector2f position = GridConfig::getGridPosition(0, 0);
        inputFieldText1 = std::make_unique<sf::Text>(*currentFont);
        inputFieldText1->setCharacterSize(20);
        inputFieldText1->setFillColor(sf::Color::Black);
        inputFieldText1->setPosition({position.x + 10.f, position.y + 50.f});
    }

    // Initialize Expression 1 output
    if (!expressionText1) {
        sf::Vector2f position = GridConfig::getGridPosition(1, 0);
        expressionText1 = std::make_unique<sf::Text>(*currentFont);
        expressionText1->setCharacterSize(20);
        expressionText1->setFillColor(sf::Color::Black);
        expressionText1->setPosition({position.x + 10.f, position.y + 50.f});
    }

    // Initialize Expression 2 input field
    if (!inputFieldText2) {
        sf::Vector2f position = GridConfig::getGridPosition(2, 0);
        inputFieldText2 = std::make_unique<sf::Text>(*currentFont);
        inputFieldText2->setCharacterSize(20);
        inputFieldText2->setFillColor(sf::Color::Black);
        inputFieldText2->setPosition({position.x + 10.f, position.y + 50.f});
    }

    // Initialize Expression 2 output
    if (!expressionText2) {
        sf::Vector2f position = GridConfig::getGridPosition(3, 0);
        expressionText2 = std::make_unique<sf::Text>(*currentFont);
        expressionText2->setCharacterSize(20);
        expressionText2->setFillColor(sf::Color::Black);
        expressionText2->setPosition({position.x + 10.f, position.y + 50.f});
    }

    setupUITexts();
}

struct TableLayout {
    std::vector<float> colWidths;
    std::vector<float> rowHeights;
    std::vector<std::vector<std::string>> cells;
    sf::Vector2f startPos;
};

TableLayout computeTableLayout(const std::vector<std::string>& truthTable, const sf::Font& font, float maxWidth, float maxHeight) {
    TableLayout layout;
    if (truthTable.size() < 3) return layout;  // Need at least header, separator, and one row

    // Parse table (assumes format from ExpressionProcessor)
    std::vector<std::string> headers;
    std::vector<std::vector<std::string>> rows;
    bool isHeader = true;
    for (const auto& line : truthTable) {
        if (line.find('+') != std::string::npos) continue;  // Skip borders
        std::vector<std::string> cells;
        std::string cell;
        for (size_t i = 1; i < line.size() - 1; ++i) {  // Skip '│' at start/end
            if (line[i] == '|') {
                while (!cell.empty() && cell.back() == ' ') cell.pop_back();
                while (!cell.empty() && cell.front() == ' ') cell.erase(0, 1);
                cells.push_back(cell);
                cell.clear();
            } else {
                cell += line[i];
            }
        }
        if (!cell.empty()) cells.push_back(cell);
        if (isHeader) {
            headers = cells;
            isHeader = false;
        } else {
            rows.push_back(cells);
        }
    }

    // Calculate column widths
    layout.cells.push_back(headers);
    layout.cells.insert(layout.cells.end(), rows.begin(), rows.end());
    layout.colWidths.resize(headers.size(), 0.f);
    sf::Text tempText(font);
    tempText.setString("");
    tempText.setCharacterSize(20 - 2);
    for (size_t col = 0; col < headers.size(); ++col) {
        tempText.setString(headers[col]);
        float width = tempText.getLocalBounds().size.x + 20.f;
        for (const auto& row : rows) {
            if (col < row.size()) {
                tempText.setString(row[col]);
                width = std::max(width, tempText.getLocalBounds().size.x + 20.f);
            }
        }
        layout.colWidths[col] = std::min(width, maxWidth / headers.size());  // Cap width
    }

    // Calculate row heights
    tempText.setString("0");
    float charHeight = tempText.getLocalBounds().size.y + 20.f;
    layout.rowHeights.resize(rows.size() + 1, charHeight);
    float totalWidth = std::accumulate(layout.colWidths.begin(), layout.colWidths.end(), 0.f);
    if (totalWidth > maxWidth) {
        float scale = maxWidth / totalWidth;
        for (auto& w : layout.colWidths) w *= scale;
    }

    // Set start position (below title)
    layout.startPos = GridConfig::getGridPosition(5, 0) + sf::Vector2f{10.f, 40.f};
    return layout;
}

void UIManager::setupUITexts() const {
    if (!currentFont) return;

    // Setup Input Field 1
    if (!inputFieldText1) {
        sf::Vector2f position = GridConfig::getGridPosition(0, 0);
        inputFieldText1 = std::make_unique<sf::Text>(*currentFont);
        inputFieldText1->setCharacterSize(20);
        inputFieldText1->setFillColor(sf::Color::Black);
        inputFieldText1->setPosition({position.x + 10.f, position.y + 50.f});
    }
    if (inputFieldText1) {
        std::string currentInput = getInputExpression(1);
        std::string displayText = currentInput.empty() ? "Enter expression 1..." : currentInput;
        inputFieldText1->setString(displayText);
    }

    // Setup Input Field 2
    if (!inputFieldText2) {
        sf::Vector2f position = GridConfig::getGridPosition(2, 0);
        inputFieldText2 = std::make_unique<sf::Text>(*currentFont);
        inputFieldText2->setCharacterSize(20);
        inputFieldText2->setFillColor(sf::Color::Black);
        inputFieldText2->setPosition({position.x + 10.f, position.y + 50.f});
    }
    if (inputFieldText2) {
        std::string currentInput = getInputExpression(2);
        std::string displayText = currentInput.empty() ? "Enter expression 2..." : currentInput;
        inputFieldText2->setString(displayText);
    }

    // Setup Expression Output 1
    if (!expressionText1) {
        sf::Vector2f position = GridConfig::getGridPosition(1, 0);
        expressionText1 = std::make_unique<sf::Text>(*currentFont);
        expressionText1->setCharacterSize(20);
        expressionText1->setFillColor(sf::Color::Black);
        expressionText1->setPosition({position.x + 10.f, position.y + 50.f});
    }
    if (expressionText1) {
        std::string display = currentExpression1.empty() ? "No expression 1 generated yet" : currentExpression1;
        expressionText1->setString(display);
    }

    // Setup Expression Output 2
    if (!expressionText2) {
        sf::Vector2f position = GridConfig::getGridPosition(3, 0);
        expressionText2 = std::make_unique<sf::Text>(*currentFont);
        expressionText2->setCharacterSize(20);
        expressionText2->setFillColor(sf::Color::Black);
        expressionText2->setPosition({position.x + 10.f, position.y + 50.f});
    }
    if (expressionText2) {
        std::string display = currentExpression2.empty() ? "No expression 2 generated yet" : currentExpression2;
        expressionText2->setString(display);
    }

    // Setup Truth Table
    truthTableTexts.clear();
    if (!truthTable.empty()) {
        sf::Vector2f tableSize = GridConfig::getGridAreaSize(8, 4) - sf::Vector2f{20.f, 50.f};  // Account for margins and title
        TableLayout layout = computeTableLayout(truthTable, *currentFont, tableSize.x, tableSize.y);
        float y = layout.startPos.y;
        for (size_t row = 0; row < layout.cells.size(); ++row) {
            float x = layout.startPos.x;
            for (size_t col = 0; col < layout.cells[row].size(); ++col) {
                sf::Text text(*currentFont);
                text.setString(layout.cells[row][col]);
                text.setCharacterSize(20 - 2);
                text.setFillColor(sf::Color::Black);
                sf::FloatRect bounds = text.getLocalBounds();
                float cellX = x + (layout.colWidths[col] - bounds.size.x) / 2.f;
                float cellY = y + (layout.rowHeights[row] - bounds.size.y) / 2.f - bounds.position.y;
                text.setPosition({cellX, cellY});
                truthTableTexts.push_back(text);
                x += layout.colWidths[col];
            }
            y += layout.rowHeights[row];
        }
    }
}

void UIManager::drawUI(sf::RenderWindow& window) const {
    if (!currentFont) return;

    // Set the right panel view
    window.setView(rightPanelView);

    // Setup all UI elements
    setupBackgrounds();
    setupTitles();
    setupUITexts();

    // Draw backgrounds
    if (rightPanelBg) window.draw(*rightPanelBg);

    // Expression 1 backgrounds
    if (inputFieldBg1) window.draw(*inputFieldBg1);
    if (expressionBg1) window.draw(*expressionBg1);

    // Expression 2 backgrounds
    if (inputFieldBg2) window.draw(*inputFieldBg2);
    if (expressionBg2) window.draw(*expressionBg2);

    // Truth table background
    if (truthTableBg) window.draw(*truthTableBg);

    // Draw titles
    if (inputTitleText1) window.draw(*inputTitleText1);
    if (expressionTitleText1) window.draw(*expressionTitleText1);
    if (inputTitleText2) window.draw(*inputTitleText2);
    if (expressionTitleText2) window.draw(*expressionTitleText2);
    if (truthTableTitleText) window.draw(*truthTableTitleText);

    // Draw content
    if (inputFieldText1) window.draw(*inputFieldText1);
    if (expressionText1 && showExpression1) window.draw(*expressionText1);
    if (inputFieldText2) window.draw(*inputFieldText2);
    if (expressionText2 && showExpression2) window.draw(*expressionText2);

    // Draw truth table
    if (showTruthTable) {
        for (const auto& text : truthTableTexts) {
            window.draw(text);
        }
    }
}

void UIManager::toggleInputField(int expressionNumber) {
    activeExpressionField = expressionNumber;
    if (expressionNumber == 2) {
        showInputField2 = !showInputField2;
        if (showInputField2) {
            inputExpression2.clear();
            // Turn off the other input field
            showInputField1 = false;
        }
    } else {
        showInputField1 = !showInputField1;
        if (showInputField1) {
            inputExpression1.clear();
            // Turn off the other input field
            showInputField2 = false;
        }
    }
}