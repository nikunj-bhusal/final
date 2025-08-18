#include "UIManager.hpp"

#include <algorithm>
#include <numeric>

#include "Circuit.hpp"
#include "ExpressionSimplifier.hpp"

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

UIManager::UIManager() {
    setupRightPanelView();
    expressionSimplifier = std::make_unique<ExpressionSimplifier>();
}

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

std::unique_ptr<sf::RectangleShape> UIManager::createBackground(sf::Vector2f position, sf::Vector2f size, sf::Color fillColor) const {
    auto bg = std::make_unique<sf::RectangleShape>(size);
    bg->setPosition(position);
    bg->setFillColor(fillColor);
    bg->setOutlineThickness(2.f);
    bg->setOutlineColor(sf::Color::Black);
    return bg;
}

std::unique_ptr<sf::Text> UIManager::createText(sf::Vector2f position, const std::string& content, unsigned int fontSize) const {
    if (!currentFont) return nullptr;

    auto text = std::make_unique<sf::Text>(*currentFont);
    text->setString(content);
    text->setCharacterSize(fontSize);
    text->setFillColor(sf::Color::Black);
    text->setPosition(position);
    return text;
}

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

    if (!inputFieldBg1) {
        sf::Vector2f position = GridConfig::getGridPosition(0, 0);
        sf::Vector2f size = GridConfig::getGridAreaSize(1, 4);
        inputFieldBg1 = createBackground(position, size, sf::Color(255, 240, 240, 220));
    }
    if (!expressionBg1) {
        sf::Vector2f position = GridConfig::getGridPosition(1, 0);
        sf::Vector2f size = GridConfig::getGridAreaSize(1, 4);
        expressionBg1 = createBackground(position, size, sf::Color(240, 255, 240, 220));
    }

    if (!inputFieldBg2) {
        sf::Vector2f position = GridConfig::getGridPosition(2, 0);
        sf::Vector2f size = GridConfig::getGridAreaSize(1, 4);
        inputFieldBg2 = createBackground(position, size, sf::Color(255, 255, 240, 220));
    }
    if (!expressionBg2) {
        sf::Vector2f position = GridConfig::getGridPosition(3, 0);
        sf::Vector2f size = GridConfig::getGridAreaSize(1, 4);
        expressionBg2 = createBackground(position, size, sf::Color(240, 240, 255, 220));
    }

    if (!truthTableBg) {
        sf::Vector2f position = GridConfig::getGridPosition(4, 0);
        sf::Vector2f size = GridConfig::getGridAreaSize(8, 4);
        truthTableBg = createBackground(position, size, sf::Color(250, 250, 250, 220));
    }
}

void UIManager::setupTitles() const {
    if (!currentFont) return;

    if (!inputTitleText1) {
        sf::Vector2f position = GridConfig::getGridPosition(0, 0);
        inputTitleText1 = createText({position.x + 10.f, position.y + 5.f}, "Expression 1 - Exact:", 18);
        inputTitleText1->setStyle(sf::Text::Bold);
    }
    if (!expressionTitleText1) {
        sf::Vector2f position = GridConfig::getGridPosition(1, 0);
        expressionTitleText1 = createText({position.x + 10.f, position.y + 5.f}, "Expression 1 - Simplified:", 18);
        expressionTitleText1->setStyle(sf::Text::Bold);
    }
    if (!inputTitleText2) {
        sf::Vector2f position = GridConfig::getGridPosition(2, 0);
        inputTitleText2 = createText({position.x + 10.f, position.y + 5.f}, "Expression 2 - Exact:", 18);
        inputTitleText2->setStyle(sf::Text::Bold);
    }
    if (!expressionTitleText2) {
        sf::Vector2f position = GridConfig::getGridPosition(3, 0);
        expressionTitleText2 = createText({position.x + 10.f, position.y + 5.f}, "Expression 2 - Simplified:", 18);
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

    if (!inputFieldText1) {
        sf::Vector2f position = GridConfig::getGridPosition(0, 0);
        inputFieldText1 = createText({position.x + 10.f, position.y + 30.f}, "", 16);
        inputFieldText1->setFillColor(sf::Color(100, 0, 0));
    }
    if (!expressionText1) {
        sf::Vector2f position = GridConfig::getGridPosition(1, 0);
        expressionText1 = createText({position.x + 10.f, position.y + 30.f}, "", 16);
        expressionText1->setFillColor(sf::Color(0, 100, 0));
    }
    if (!inputFieldText2) {
        sf::Vector2f position = GridConfig::getGridPosition(2, 0);
        inputFieldText2 = createText({position.x + 10.f, position.y + 30.f}, "", 16);
        inputFieldText2->setFillColor(sf::Color(100, 100, 0));
    }
    if (!expressionText2) {
        sf::Vector2f position = GridConfig::getGridPosition(3, 0);
        expressionText2 = createText({position.x + 10.f, position.y + 30.f}, "", 16);
        expressionText2->setFillColor(sf::Color(0, 0, 100));
    }

    setupUITexts();
}

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

    float maxFieldWidth = GridConfig::getGridAreaSize(1, 4).x - 20.f;

    updateTextContent(inputFieldText1, getInputExpression(1), "No exact equation generated", maxFieldWidth);
    updateTextContent(inputFieldText2, getInputExpression(2), "No exact equation generated", maxFieldWidth);
    updateTextContent(expressionText1, currentExpression1, "No simplified expression generated", maxFieldWidth);
    updateTextContent(expressionText2, currentExpression2, "No simplified expression generated", maxFieldWidth);

    generateTruthTable();
}

void UIManager::generateTruthTable() const {
    truthTableTexts.clear();

    if (!currentFont || !expressionSimplifier) return;

    std::vector<std::string> validExpressions;
    if (!getInputExpression(1).empty() && getInputExpression(1) != "0") {
        validExpressions.push_back(getInputExpression(1));
    }
    if (!getInputExpression(2).empty() && getInputExpression(2) != "0") {
        validExpressions.push_back(getInputExpression(2));
    }

    if (validExpressions.empty()) return;

    std::set<char> allVariables;
    for (const std::string& expr : validExpressions) {
        std::set<char> exprVars = expressionSimplifier->getVariables(expr);
        allVariables.insert(exprVars.begin(), exprVars.end());
    }

    std::vector<char> varList(allVariables.begin(), allVariables.end());
    std::sort(varList.begin(), varList.end());

    if (varList.empty()) return;

    int numVars = static_cast<int>(varList.size());
    int numRows = 1 << numVars;

    sf::Vector2f startPos = GridConfig::getGridPosition(5, 0) + sf::Vector2f{10.f, 0.f};
    float cellWidth = 40.f;
    float cellHeight = 25.f;

    float x = startPos.x;
    float y = startPos.y;

    for (char var : varList) {
        sf::Text text(*currentFont);
        text.setString(std::string(1, var));
        text.setCharacterSize(20);
        text.setFillColor(sf::Color(0, 0, 150));
        text.setStyle(sf::Text::Bold);
        text.setPosition(sf::Vector2f(x + cellWidth / 2 - 5.f, y));
        truthTableTexts.push_back(text);
        x += cellWidth;
    }

    for (size_t i = 0; i < validExpressions.size(); i++) {
        sf::Text text(*currentFont);
        text.setString("Y" + std::to_string(i + 1));
        text.setCharacterSize(20);
        text.setFillColor(sf::Color(0, 0, 150));
        text.setStyle(sf::Text::Bold);
        text.setPosition(sf::Vector2f(x + cellWidth / 2 - 8.f, y));
        truthTableTexts.push_back(text);
        x += cellWidth;
    }

    for (int row = 0; row < numRows; row++) {
        x = startPos.x;
        y = startPos.y + cellHeight * (row + 1);

        std::map<char, bool> values;

        for (int col = 0; col < numVars; col++) {
            char var = varList[col];
            bool value = (row >> (numVars - 1 - col)) & 1;
            values[var] = value;

            sf::Text text(*currentFont);
            text.setString(value ? "1" : "0");
            text.setCharacterSize(20);
            text.setFillColor(sf::Color::Black);
            text.setPosition(sf::Vector2f(x + cellWidth / 2 - 5.f, y));
            truthTableTexts.push_back(text);
            x += cellWidth;
        }

        for (const std::string& expr : validExpressions) {
            bool output = expressionSimplifier->evaluateExpression(expr, values);

            sf::Text text(*currentFont);
            text.setString(output ? "1" : "0");
            text.setCharacterSize(20);
            text.setFillColor(sf::Color::Black);
            text.setPosition(sf::Vector2f(x + cellWidth / 2 - 5.f, y));
            truthTableTexts.push_back(text);
            x += cellWidth;
        }
    }
}

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

    setupBackgrounds();
    setupTitles();
    setupUITexts();

    std::vector<sf::Drawable*> backgrounds = {rightPanelBg.get(),  inputFieldBg1.get(), expressionBg1.get(),
                                              inputFieldBg2.get(), expressionBg2.get(), truthTableBg.get()};
    drawUIElements(window, backgrounds);

    std::vector<sf::Drawable*> titles = {inputTitleText1.get(), expressionTitleText1.get(), inputTitleText2.get(), expressionTitleText2.get(),
                                         truthTableTitleText.get()};
    drawUIElements(window, titles);

    if (inputFieldText1) window.draw(*inputFieldText1);
    if (expressionText1 && showExpression1) window.draw(*expressionText1);
    if (inputFieldText2) window.draw(*inputFieldText2);
    if (expressionText2 && showExpression2) window.draw(*expressionText2);

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

void UIManager::updateFromCircuit(const Circuit& circuit) {
    std::vector<std::string> outputEquations = circuit.getAllOutputEquations();

    if (!outputEquations.empty()) {
        processMultipleOutputs(outputEquations);
    } else {
        setInputExpression("", 1);
        setInputExpression("", 2);
        setCurrentExpression("", 1);
        setCurrentExpression("", 2);
        setShowExpression(false, 1);
        setShowExpression(false, 2);
        setShowTruthTable(false);
    }
}

void UIManager::processMultipleOutputs(const std::vector<std::string>& outputEquations) {
    if (!expressionSimplifier || outputEquations.empty()) {
        setInputExpression("", 1);
        setInputExpression("", 2);
        setCurrentExpression("", 1);
        setCurrentExpression("", 2);
        setShowExpression(false, 1);
        setShowExpression(false, 2);
        setShowInputField(false, 1);
        setShowInputField(false, 2);
        setShowTruthTable(false);
        setupUITexts();
        return;
    }

    std::vector<std::string> validEquations;

    if (outputEquations.size() >= 1 && !outputEquations[0].empty() && outputEquations[0] != "0") {
        setInputExpression(outputEquations[0], 1);
        setShowInputField(true, 1);

        std::string simplifiedExpression1 = expressionSimplifier->simplifyExpression(outputEquations[0]);
        setCurrentExpression(simplifiedExpression1, 1);
        setShowExpression(true, 1);

        validEquations.push_back(outputEquations[0]);
    } else {
        setInputExpression("", 1);
        setCurrentExpression("", 1);
        setShowExpression(false, 1);
        setShowInputField(false, 1);
    }

    if (outputEquations.size() >= 2 && !outputEquations[1].empty() && outputEquations[1] != "0") {
        setInputExpression(outputEquations[1], 2);
        setShowInputField(true, 2);

        std::string simplifiedExpression2 = expressionSimplifier->simplifyExpression(outputEquations[1]);
        setCurrentExpression(simplifiedExpression2, 2);
        setShowExpression(true, 2);

        validEquations.push_back(outputEquations[1]);
    } else {
        setInputExpression("", 2);
        setCurrentExpression("", 2);
        setShowExpression(false, 2);
        setShowInputField(false, 2);
    }

    if (!validEquations.empty()) {
        setShowTruthTable(true);
    } else {
        setShowTruthTable(false);
    }

    setupUITexts();
}