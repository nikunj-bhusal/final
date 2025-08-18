#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "ExpressionSimplifier.hpp"

class Circuit;

class UIManager {
   private:
    const sf::Font* currentFont = nullptr;
    std::optional<sf::Text> text;
    std::string currentExpression1;
    std::string currentExpression2;
    std::vector<std::string> truthTable;
    std::string inputExpression1;
    std::string inputExpression2;
    int activeExpressionField = 1;

    std::unique_ptr<ExpressionSimplifier> expressionSimplifier;

    bool showTruthTable = false;
    bool showExpression1 = false;
    bool showExpression2 = false;
    bool showInputField1 = false;
    bool showInputField2 = false;

    sf::View rightPanelView;

    mutable std::unique_ptr<sf::Text> inputFieldText1;
    mutable std::unique_ptr<sf::RectangleShape> inputFieldBg1;
    mutable std::unique_ptr<sf::Text> expressionText1;
    mutable std::unique_ptr<sf::Text> expressionTitleText1;
    mutable std::unique_ptr<sf::Text> inputTitleText1;
    mutable std::unique_ptr<sf::RectangleShape> expressionBg1;

    mutable std::unique_ptr<sf::Text> inputFieldText2;
    mutable std::unique_ptr<sf::RectangleShape> inputFieldBg2;
    mutable std::unique_ptr<sf::Text> expressionText2;
    mutable std::unique_ptr<sf::Text> expressionTitleText2;
    mutable std::unique_ptr<sf::Text> inputTitleText2;
    mutable std::unique_ptr<sf::RectangleShape> expressionBg2;

    mutable std::vector<sf::Text> truthTableTexts;
    mutable std::unique_ptr<sf::Text> truthTableTitleText;
    mutable std::unique_ptr<sf::RectangleShape> truthTableBg;
    mutable std::unique_ptr<sf::RectangleShape> rightPanelBg;

    void setupRightPanelView();
    void setupBackgrounds() const;
    void setupTitles() const;

    std::unique_ptr<sf::RectangleShape> createBackground(sf::Vector2f position, sf::Vector2f size, sf::Color fillColor) const;
    std::unique_ptr<sf::Text> createText(sf::Vector2f position, const std::string& content, unsigned int fontSize) const;
    std::string wrapText(const std::string& text, float maxWidth, unsigned int fontSize) const;
    void updateTextContent(std::unique_ptr<sf::Text>& textPtr, const std::string& content, const std::string& defaultContent,
                           float maxWidth = 0.f) const;
    void drawUIElements(sf::RenderWindow& window, const std::vector<sf::Drawable*>& elements) const;
    void generateTruthTable() const;

   public:
    UIManager();
    void setFont(const sf::Font& font);
    void initializeUITexts();
    void drawUI(sf::RenderWindow& window) const;
    void setupUITexts() const;
    void toggleInputField(int expressionNumber = 1);

    bool isInputFieldActive(int expressionNumber = 1) const { return expressionNumber == 2 ? showInputField2 : showInputField1; }
    int getActiveExpressionField() const { return activeExpressionField; }
    bool getShowTruthTable() const { return showTruthTable; }
    void setShowTruthTable(bool show) { showTruthTable = show; }
    bool getShowExpression(int expressionNumber = 1) const { return expressionNumber == 2 ? showExpression2 : showExpression1; }
    void setShowExpression(bool show, int expressionNumber = 1) {
        if (expressionNumber == 2)
            showExpression2 = show;
        else
            showExpression1 = show;
    }
    bool getShowInputField(int expressionNumber = 1) const { return expressionNumber == 2 ? showInputField2 : showInputField1; }
    void setShowInputField(bool show, int expressionNumber = 1) {
        if (expressionNumber == 2)
            showInputField2 = show;
        else
            showInputField1 = show;
    }

    const std::string& getInputExpression(int num = 1) const { return num == 2 ? inputExpression2 : inputExpression1; }
    void setInputExpression(const std::string& expr, int num = 1) {
        if (num == 2)
            inputExpression2 = expr;
        else
            inputExpression1 = expr;
    }
    const std::string& getCurrentExpression(int num = 1) const { return num == 2 ? currentExpression2 : currentExpression1; }
    void setCurrentExpression(const std::string& expr, int num = 1) {
        if (num == 2)
            currentExpression2 = expr;
        else
            currentExpression1 = expr;
    }

    const std::vector<std::string>& getTruthTable() const { return truthTable; }
    void setTruthTable(const std::vector<std::string>& table) { truthTable = table; }

    void updateFromCircuit(const Circuit& circuit);
    void processMultipleOutputs(const std::vector<std::string>& outputEquations);
};