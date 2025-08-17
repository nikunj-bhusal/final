#pragma once
#include <SFML/Graphics.hpp>

namespace UIConstants {
constexpr float LEFT_MARGIN = 50.f;
constexpr float TOP_MARGIN = 40.f;
constexpr float SPACING = 25.f;
constexpr float OUTLINE_THICKNESS = 2.f;
constexpr float TITLE_HEIGHT = 40.f;
constexpr float SECTION_SPACING = 30.f;
constexpr float LINE_HEIGHT = 20.f;
constexpr float SECTION_WIDTH = 280.f;
constexpr float BOX_WIDTH = 180.f;
constexpr float BOX_HEIGHT = 60.f;
constexpr float BOX_Y_SPACING = BOX_HEIGHT + SPACING;
constexpr float FONT_TITLE = 20;
constexpr float FONT_LABEL = 18;
constexpr float FONT_INSTRUCTION = 18;
constexpr float FONT_CONTENT = 18;
constexpr float CELL_PADDING = 10.f;
constexpr float GRID_LINE_THICKNESS = 2.f;
constexpr float GRID_SIZE = 50.f;
constexpr float ZOOM_IN_FACTOR = 0.95f;
constexpr float ZOOM_OUT_FACTOR = 1.05f;

const sf::Color COLOR_SELECTED = sf::Color::Yellow;
const sf::Color COLOR_HOVERED = sf::Color(220, 220, 220);
const sf::Color COLOR_DEFAULT = sf::Color::White;
const sf::Color COLOR_OUTLINE = sf::Color::Black;
const sf::Color COLOR_BG = sf::Color(200, 200, 200, 200);
const sf::Color COLOR_TITLE_BG = sf::Color::Black;
const sf::Color COLOR_TITLE_TEXT = sf::Color::White;
const sf::Color COLOR_LABEL_TEXT = sf::Color::Black;
const sf::Color COLOR_INSTR_TEXT = sf::Color::Black;
const sf::Color COLOR_PANEL_BG = sf::Color(220, 220, 220, 200);
const sf::Color COLOR_CONTENT_TEXT = sf::Color::Black;
const sf::Color COLOR_INPUT_BG = sf::Color(255, 255, 200);
const sf::Color COLOR_EXPRESSION_BG = sf::Color(200, 255, 200);
const sf::Color COLOR_TRUTH_BG = sf::Color(200, 200, 255);
const sf::Color GRID_COLOR = sf::Color(60, 60, 60);
const sf::Color PIN_OUTLINE_COLOR = sf::Color::Black;
const sf::Color PIN_HIGH_COLOR = sf::Color::Red;
const sf::Color PIN_LOW_COLOR = sf::Color::White;
const sf::Color GATE_SELECTED_OUTLINE = sf::Color::Yellow;
const sf::Color GATE_DEFAULT_OUTLINE = sf::Color::Black;
const sf::Color GATE_DEFAULT_FILL = sf::Color(200, 200, 200);
const sf::Color GATE_IO_FILL = sf::Color(128, 128, 128);
}  // namespace UIConstants
#include <SFML/Graphics.hpp>

class WindowConfig {
   public:
    static constexpr float PALETTE_SCALE = 0.18f;
    static constexpr float RIGHT_PANEL_SCALE = 0.25f;

    static sf::Vector2f getWindowSize() {
        sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
        return sf::Vector2f(desktop.size.x, desktop.size.y);
    }

    static sf::Vector2f getPaletteSize() {
        sf::Vector2f windowSize = getWindowSize();
        return sf::Vector2f(windowSize.x * PALETTE_SCALE, windowSize.y);
    }

    static sf::Vector2f getRightPanelSize() {
        sf::Vector2f windowSize = getWindowSize();
        return sf::Vector2f(windowSize.x * RIGHT_PANEL_SCALE, windowSize.y);
    }

    static sf::Vector2f getCanvasSize() {
        sf::Vector2f windowSize = getWindowSize();
        return sf::Vector2f(windowSize.x, windowSize.y);
    }
};

class GridConfig {
   public:
    static constexpr int GRID_ROWS = 12;
    static constexpr int GRID_COLS = 4;
    static constexpr float GRID_PADDING = 10.f;
    static constexpr float GRID_MARGIN = 15.f;

    static sf::Vector2f getGridCellSize() {
        sf::Vector2f panelSize = WindowConfig::getRightPanelSize();
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
