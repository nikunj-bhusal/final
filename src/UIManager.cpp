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

// Helper function to create background with common styling
std::unique_ptr<sf::RectangleShape> UIManager::createBackground(sf::Vector2f position, sf::Vector2f size, sf::Color fillColor) const {
    auto bg = std::make_unique<sf::RectangleShape>(size);
    bg->setPosition(position);
    bg->setFillColor(fillColor);
    bg->setOutlineThickness(2.f);
    bg->setOutlineColor(sf::Color::Black);
    return bg;
}

// Helper function to create text with common styling and word wrap
std::unique_ptr<sf::Text> UIManager::createText(sf::Vector2f position, const std::string& content, unsigned int fontSize) const {
    if (!currentFont) return nullptr;

    auto text = std::make_unique<sf::Text>(*currentFont);
    text->setString(content);
    text->setCharacterSize(fontSize);
    text->setFillColor(sf::Color::Black);
    text->setPosition(position);
    return text;
}

// Helper function to wrap text within a given width
std::string UIManager::wrapText(const std::string& text, float maxWidth, unsigned int fontSize) const {
    if (!currentFont || text.empty()) return text;

    sf::Text tempText(*currentFont);
    tempText.setCharacterSize(fontSize);

    std::string wrappedText;
    std::istringstream words(text);
    std::string word;
    std::string currentLine;

    while (words >> word) {
        std::string testLine = currentLine.empty() ? word : currentLine + " " + word;
        tempText.setString(testLine);

        if (tempText.getLocalBounds().size.x <= maxWidth) {
            currentLine = testLine;
        } else {
            if (!currentLine.empty()) {
                wrappedText += currentLine + "\n";
                currentLine = word;
            } else {
                // Single word is too long, break it
                wrappedText += word + "\n";
                currentLine = "";
            }
        }
    }

    if (!currentLine.empty()) {
        wrappedText += currentLine;
    }

    return wrappedText;
}

void UIManager::setupBackgrounds() const {
    if (!rightPanelBg) {
        sf::Vector2f panelSize = rightPanelView.getSize();
        rightPanelBg = createBackground({0.f, 0.f}, panelSize, sf::Color(240, 240, 240, 200));
    }

    // Expression 1 backgrounds - Enhanced colors for better distinction
    if (!inputFieldBg1) {
        sf::Vector2f position = GridConfig::getGridPosition(0, 0);
        sf::Vector2f size = GridConfig::getGridAreaSize(1, 4);
        inputFieldBg1 = createBackground(position, size, sf::Color(255, 240, 240, 220));  // Light pink for exact equation
    }
    if (!expressionBg1) {
        sf::Vector2f position = GridConfig::getGridPosition(1, 0);
        sf::Vector2f size = GridConfig::getGridAreaSize(1, 4);
        expressionBg1 = createBackground(position, size, sf::Color(240, 255, 240, 220));  // Light green for simplified
    }

    // Expression 2 backgrounds
    if (!inputFieldBg2) {
        sf::Vector2f position = GridConfig::getGridPosition(2, 0);
        sf::Vector2f size = GridConfig::getGridAreaSize(1, 4);
        inputFieldBg2 = createBackground(position, size, sf::Color(255, 255, 240, 220));  // Light yellow for exact equation
    }
    if (!expressionBg2) {
        sf::Vector2f position = GridConfig::getGridPosition(3, 0);
        sf::Vector2f size = GridConfig::getGridAreaSize(1, 4);
        expressionBg2 = createBackground(position, size, sf::Color(240, 240, 255, 220));  // Light blue for simplified
    }

    // Truth table background
    if (!truthTableBg) {
        sf::Vector2f position = GridConfig::getGridPosition(4, 0);
        sf::Vector2f size = GridConfig::getGridAreaSize(8, 4);
        truthTableBg = createBackground(position, size, sf::Color(250, 250, 250, 220));
    }
}

void UIManager::setupTitles() const {
    if (!currentFont) return;

    // Enhanced titles with better descriptions
    if (!inputTitleText1) {
        sf::Vector2f position = GridConfig::getGridPosition(0, 0);
        inputTitleText1 = createText({position.x + 10.f, position.y + 5.f}, "Expression 1 - Exact Equation (SOP):", 18);
        inputTitleText1->setStyle(sf::Text::Bold);
    }
    if (!expressionTitleText1) {
        sf::Vector2f position = GridConfig::getGridPosition(1, 0);
        expressionTitleText1 = createText({position.x + 10.f, position.y + 5.f}, "Expression 1 - K-Map Simplified:", 18);
        expressionTitleText1->setStyle(sf::Text::Bold);
    }
    if (!inputTitleText2) {
        sf::Vector2f position = GridConfig::getGridPosition(2, 0);
        inputTitleText2 = createText({position.x + 10.f, position.y + 5.f}, "Expression 2 - Exact Equation (SOP):", 18);
        inputTitleText2->setStyle(sf::Text::Bold);
    }
    if (!expressionTitleText2) {
        sf::Vector2f position = GridConfig::getGridPosition(3, 0);
        expressionTitleText2 = createText({position.x + 10.f, position.y + 5.f}, "Expression 2 - K-Map Simplified:", 18);
        expressionTitleText2->setStyle(sf::Text::Bold);
    }
    if (!truthTableTitleText) {
        sf::Vector2f position = GridConfig::getGridPosition(4, 0);
        truthTableTitleText = createText({position.x + 10.f, position.y + 5.f}, "Truth Table Analysis:", 20);
        truthTableTitleText->setStyle(sf::Text::Bold);
    }
}

void UIManager::initializeUITexts() {
    if (!currentFont) return;
    setupBackgrounds();
    setupTitles();

    // Initialize text fields with appropriate styling
    if (!inputFieldText1) {
        sf::Vector2f position = GridConfig::getGridPosition(0, 0);
        inputFieldText1 = createText({position.x + 10.f, position.y + 30.f}, "", 16);
        inputFieldText1->setFillColor(sf::Color(100, 0, 0));  // Dark red for exact equations
    }
    if (!expressionText1) {
        sf::Vector2f position = GridConfig::getGridPosition(1, 0);
        expressionText1 = createText({position.x + 10.f, position.y + 30.f}, "", 16);
        expressionText1->setFillColor(sf::Color(0, 100, 0));  // Dark green for simplified
    }
    if (!inputFieldText2) {
        sf::Vector2f position = GridConfig::getGridPosition(2, 0);
        inputFieldText2 = createText({position.x + 10.f, position.y + 30.f}, "", 16);
        inputFieldText2->setFillColor(sf::Color(100, 100, 0));  // Dark yellow for exact equations
    }
    if (!expressionText2) {
        sf::Vector2f position = GridConfig::getGridPosition(3, 0);
        expressionText2 = createText({position.x + 10.f, position.y + 30.f}, "", 16);
        expressionText2->setFillColor(sf::Color(0, 0, 100));  // Dark blue for simplified
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
    if (truthTable.size() < 3) return layout;

    // Parse table structure
    std::vector<std::string> headers;
    std::vector<std::vector<std::string>> rows;
    bool isHeader = true;

    for (const auto& line : truthTable) {
        if (line.find('+') != std::string::npos) continue;  // Skip borders

        std::vector<std::string> cells;
        std::string cell;
        bool insideCell = false;

        for (size_t i = 0; i < line.size(); ++i) {
            char c = line[i];
            if (c == '|') {
                if (insideCell) {
                    // Trim whitespace
                    while (!cell.empty() && cell.back() == ' ') cell.pop_back();
                    while (!cell.empty() && cell.front() == ' ') cell.erase(0, 1);
                    cells.push_back(cell);
                    cell.clear();
                }
                insideCell = !insideCell;
            } else if (insideCell) {
                cell += c;
            }
        }

        if (!cells.empty()) {
            if (isHeader) {
                headers = cells;
                isHeader = false;
            } else {
                rows.push_back(cells);
            }
        }
    }

    // Calculate optimal column widths
    layout.cells.push_back(headers);
    layout.cells.insert(layout.cells.end(), rows.begin(), rows.end());
    layout.colWidths.resize(headers.size(), 0.f);

    sf::Text tempText(font);
    tempText.setCharacterSize(16);

    for (size_t col = 0; col < headers.size(); ++col) {
        float maxColWidth = 0.f;

        // Check header width
        tempText.setString(headers[col]);
        maxColWidth = std::max(maxColWidth, tempText.getLocalBounds().size.x + 10.f);

        // Check data widths
        for (const auto& row : rows) {
            if (col < row.size()) {
                tempText.setString(row[col]);
                maxColWidth = std::max(maxColWidth, tempText.getLocalBounds().size.x + 10.f);
            }
        }

        layout.colWidths[col] = std::min(maxColWidth, maxWidth / headers.size());
    }

    // Set uniform row height
    tempText.setString("Ag");  // Test string with ascenders and descenders
    float rowHeight = tempText.getLocalBounds().size.y + 15.f;
    layout.rowHeights.resize(layout.cells.size(), rowHeight);

    // Adjust if total width exceeds available space
    float totalWidth = std::accumulate(layout.colWidths.begin(), layout.colWidths.end(), 0.f);
    if (totalWidth > maxWidth) {
        float scale = maxWidth / totalWidth;
        for (auto& width : layout.colWidths) {
            width *= scale;
        }
    }

    layout.startPos = GridConfig::getGridPosition(5, 0) + sf::Vector2f{10.f, 30.f};
    return layout;
}

// Helper function to update text content with word wrapping
void UIManager::updateTextContent(std::unique_ptr<sf::Text>& textPtr, const std::string& content, const std::string& defaultContent,
                                  float maxWidth) const {
    if (textPtr) {
        std::string displayText = content.empty() ? defaultContent : content;
        if (maxWidth > 0) {
            displayText = wrapText(displayText, maxWidth, textPtr->getCharacterSize());
        }
        textPtr->setString(displayText);
    }
}

void UIManager::setupUITexts() const {
    if (!currentFont) return;

    float maxFieldWidth = GridConfig::getGridAreaSize(1, 4).x - 20.f;  // Account for padding

    // Update text content with word wrapping for better display
    updateTextContent(inputFieldText1, getInputExpression(1), "No exact equation generated", maxFieldWidth);
    updateTextContent(inputFieldText2, getInputExpression(2), "No exact equation generated", maxFieldWidth);
    updateTextContent(expressionText1, currentExpression1, "No simplified expression generated", maxFieldWidth);
    updateTextContent(expressionText2, currentExpression2, "No simplified expression generated", maxFieldWidth);

    // Setup Truth Table with improved formatting
    truthTableTexts.clear();
    if (!truthTable.empty()) {
        sf::Vector2f tableSize = GridConfig::getGridAreaSize(8, 4) - sf::Vector2f{20.f, 40.f};
        TableLayout layout = computeTableLayout(truthTable, *currentFont, tableSize.x, tableSize.y);

        float y = layout.startPos.y;
        for (size_t row = 0; row < layout.cells.size(); ++row) {
            float x = layout.startPos.x;
            for (size_t col = 0; col < layout.cells[row].size(); ++col) {
                sf::Text text(*currentFont);
                text.setString(layout.cells[row][col]);
                text.setCharacterSize(16);

                // Different styling for header vs data
                if (row == 0) {
                    text.setFillColor(sf::Color(0, 0, 150));  // Blue for headers
                    text.setStyle(sf::Text::Bold);
                } else {
                    text.setFillColor(sf::Color::Black);
                }

                // Center text in cell
                sf::FloatRect bounds = text.getLocalBounds();
                float cellX = x + (layout.colWidths[col] - bounds.size.x) / 2.f;
                float cellY = y + (layout.rowHeights[row] - bounds.size.y) / 2.f;
                text.setPosition({cellX, cellY});

                truthTableTexts.push_back(text);
                x += layout.colWidths[col];
            }
            y += layout.rowHeights[row];
        }
    }
}

// Helper function to draw UI elements with null checking
void UIManager::drawUIElements(sf::RenderWindow& window, const std::vector<sf::Drawable*>& elements) const {
    for (const auto* element : elements) {
        if (element) {
            window.draw(*element);
        }
    }
}

void UIManager::drawUI(sf::RenderWindow& window) const {
    if (!currentFont) return;

    window.setView(rightPanelView);

    // Setup all UI elements
    setupBackgrounds();
    setupTitles();
    setupUITexts();

    // Draw backgrounds
    std::vector<sf::Drawable*> backgrounds = {rightPanelBg.get(),  inputFieldBg1.get(), expressionBg1.get(),
                                              inputFieldBg2.get(), expressionBg2.get(), truthTableBg.get()};
    drawUIElements(window, backgrounds);

    // Draw titles
    std::vector<sf::Drawable*> titles = {inputTitleText1.get(), expressionTitleText1.get(), inputTitleText2.get(), expressionTitleText2.get(),
                                         truthTableTitleText.get()};
    drawUIElements(window, titles);

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
            showInputField1 = false;
        }
    } else {
        showInputField1 = !showInputField1;
        if (showInputField1) {
            inputExpression1.clear();
            showInputField2 = false;
        }
    }
}