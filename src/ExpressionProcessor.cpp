#include "ExpressionProcessor.hpp"

#include <sstream>

namespace TableGrid {
std::vector<int> calculateColumnWidths(const std::vector<std::string> &headers, int numRows) {
    std::vector<int> widths;
    for (const auto &header : headers) {
        widths.push_back(std::max(3, static_cast<int>(header.length() + 2)));
    }
    return widths;
}

std::string generateBorder(const std::vector<int> &widths, char left, char mid, char right, char fill = '-') {
    std::string border;
    border += left;
    for (size_t i = 0; i < widths.size(); ++i) {
        border += std::string(widths[i], fill);
        if (i < widths.size() - 1) {
            border += mid;
        }
    }
    border += right;
    return border;
}

std::string generateRow(const std::vector<std::string> &cells, const std::vector<int> &widths) {
    std::string row = "|";
    for (size_t i = 0; i < cells.size() && i < widths.size(); ++i) {
        int padding = widths[i] - cells[i].length();
        int leftPad = padding / 2;
        int rightPad = padding - leftPad;
        row += std::string(leftPad, ' ') + cells[i] + std::string(rightPad, ' ') + "|";
    }
    return row;
}
}  // namespace TableGrid

void ExpressionProcessor::removeDuplicateVariables() {
    std::sort(variables.begin(), variables.end());
    variables.erase(std::unique(variables.begin(), variables.end()), variables.end());
}

bool ExpressionProcessor::isOperator(char c) { return c == '.' || c == '+' || c == '~' || c == '^'; }

int ExpressionProcessor::precedence(char op) {
    if (op == '+') return 1;
    if (op == '.') return 2;
    if (op == '~') return 3;
    if (op == '^') return 2;
    return 0;
}

bool ExpressionProcessor::applyOperation(bool a, bool b, char op) {
    switch (op) {
        case '.':
            return a && b;
        case '+':
            return a || b;
        case '^':
            return a != b;
        default:
            return false;
    }
}

std::string ExpressionProcessor::cleanExpression(const std::string &infix) {
    std::string cleaned;
    bool lastWasOperator = true;  // Track to prevent consecutive operators
    int parenCount = 0;

    for (char c : infix) {
        if (std::isspace(c)) continue;

        if (c == '(') {
            parenCount++;
            cleaned += c;
            lastWasOperator = true;
        } else if (c == ')') {
            parenCount--;
            if (parenCount < 0) return "";  // Invalid parentheses
            cleaned += c;
            lastWasOperator = false;
        } else if (isOperator(c)) {
            if (lastWasOperator && c != '~') return "";  // Invalid consecutive operators
            cleaned += c;
            lastWasOperator = (c != '~');
        } else if (std::isalpha(c)) {
            if (!lastWasOperator) return "";  // Missing operator between variables
            cleaned += c;
            lastWasOperator = false;
        }
    }

    return (parenCount == 0) ? cleaned : "";
}

std::string ExpressionProcessor::simplifyDoubleNegations(const std::string &infix) {
    std::string result = infix;
    size_t pos;
    while ((pos = result.find("~~")) != std::string::npos) {
        result.erase(pos, 2);  // Remove ~~
    }
    return result;
}

std::string ExpressionProcessor::infixToPostfix(const std::string &infix) {
    std::string cleaned = cleanExpression(simplifyDoubleNegations(infix));
    if (cleaned.empty()) return "";

    std::stack<char> s;
    std::string postfix;
    bool expectOperand = true;

    for (char c : cleaned) {
        if (std::isalpha(c)) {
            if (!expectOperand) return "";  // Invalid: two operands in a row
            postfix += c;
            expectOperand = false;
        } else if (c == '(') {
            s.push(c);
            expectOperand = true;
        } else if (c == ')') {
            while (!s.empty() && s.top() != '(') {
                postfix += s.top();
                s.pop();
            }
            if (s.empty()) return "";  // Mismatched parentheses
            s.pop();
            expectOperand = false;
        } else if (isOperator(c)) {
            if (expectOperand && c != '~') return "";  // Invalid: operator without operand
            while (!s.empty() && s.top() != '(' && precedence(s.top()) >= precedence(c)) {
                postfix += s.top();
                s.pop();
            }
            s.push(c);
            expectOperand = (c != '~');
        }
    }

    while (!s.empty()) {
        if (s.top() == '(') return "";  // Mismatched parentheses
        postfix += s.top();
        s.pop();
    }

    return postfix;
}

bool ExpressionProcessor::evaluatePostfix(const std::string &postfix) {
    if (postfix.empty()) return false;

    std::stack<bool> s;
    for (char c : postfix) {
        if (std::isalpha(c)) {
            s.push(varValues[c]);
        } else if (c == '~') {
            if (s.empty()) return false;
            bool a = s.top();
            s.pop();
            s.push(!a);
        } else {
            if (s.size() < 2) return false;
            bool b = s.top();
            s.pop();
            bool a = s.top();
            s.pop();
            s.push(applyOperation(a, b, c));
        }
    }
    return s.size() == 1 ? s.top() : false;
}

std::string ExpressionProcessor::getBinaryString(int num, int length) {
    std::string binary;
    for (int i = length - 1; i >= 0; i--) {
        binary += ((num >> i) & 1) ? '1' : '0';
    }
    return binary;
}

bool ExpressionProcessor::differsByOneBit(const std::string &a, const std::string &b) {
    if (a.size() != b.size()) return false;
    int diff = 0;
    for (size_t i = 0; i < a.size(); i++) {
        if (a[i] != b[i]) diff++;
        if (diff > 1) return false;
    }
    return diff == 1;
}

std::string ExpressionProcessor::combineTerms(const std::string &a, const std::string &b) {
    if (a.size() != b.size()) return "";
    std::string combined;
    for (size_t i = 0; i < a.size(); i++) {
        combined += (a[i] == b[i]) ? a[i] : '-';
    }
    return combined;
}

std::string ExpressionProcessor::termToExpression(const std::string &term) {
    if (term.empty()) return "";

    std::string expr;
    for (size_t i = 0; i < term.size(); i++) {
        if (term[i] == '0')
            expr += "~" + std::string(1, variables[i]) + ".";
        else if (term[i] == '1')
            expr += std::string(1, variables[i]) + ".";
    }
    if (!expr.empty()) expr = expr.substr(0, expr.size() - 1);  // Remove trailing dot
    return expr.empty() ? "1" : expr;
}

std::string ExpressionProcessor::simplifyExpression() {
    if (variables.empty() || minterms.empty()) return "0";

    bool all_true = true, all_false = true;
    for (bool m : minterms) {
        if (!m) all_true = false;
        if (m) all_false = false;
    }
    if (all_true) return "1";
    if (all_false) return "0";

    std::set<std::string> binaryMinterms;
    for (size_t i = 0; i < minterms.size(); i++) {
        if (minterms[i]) {
            binaryMinterms.insert(getBinaryString(i, variables.size()));
        }
    }

    if (binaryMinterms.empty()) return "0";

    // Quine-McCluskey algorithm for simplification
    std::set<std::string> primeImplicants = binaryMinterms;
    bool changed;
    do {
        changed = false;
        std::set<std::string> used;
        std::set<std::string> newImplicants;

        auto it1 = primeImplicants.begin();
        while (it1 != primeImplicants.end()) {
            bool combined = false;
            auto it2 = primeImplicants.begin();
            while (it2 != primeImplicants.end()) {
                if (it1 == it2) {
                    ++it2;
                    continue;
                }
                if (differsByOneBit(*it1, *it2)) {
                    std::string combined = combineTerms(*it1, *it2);
                    if (!combined.empty()) {
                        newImplicants.insert(combined);
                        used.insert(*it1);
                        used.insert(*it2);
                        combined = true;
                        changed = true;
                    }
                }
                ++it2;
            }
            if (!combined && used.find(*it1) == used.end()) {
                newImplicants.insert(*it1);
            }
            ++it1;
        }

        primeImplicants = newImplicants;
    } while (changed);

    std::string result;
    for (const std::string &term : primeImplicants) {
        std::string expr = termToExpression(term);
        if (!expr.empty()) result += "(" + expr + ")+";
    }

    if (!result.empty()) result = result.substr(0, result.size() - 1);  // Remove trailing +

    // Check for XOR pattern
    if (primeImplicants.size() == 2 && variables.size() == 2) {
        auto it = primeImplicants.begin();
        std::string term1 = *it;
        std::string term2 = *(++it);

        bool isXOR = true;
        for (size_t i = 0; i < term1.size(); i++) {
            if ((term1[i] == '0' && term2[i] != '1') || (term1[i] == '1' && term2[i] != '0') || (term2[i] == '0' && term1[i] != '1') ||
                (term2[i] == '1' && term1[i] != '0')) {
                isXOR = false;
                break;
            }
        }
        if (isXOR) {
            return std::string(1, variables[0]) + "^" + std::string(1, variables[1]);
        }
    }

    return result.empty() ? "0" : result;
}

void ExpressionProcessor::generateExpressionTruthTable(UIManager &ui) {
    variables.clear();
    std::string cleanedExpr = cleanExpression(simplifyDoubleNegations(expression));
    if (cleanedExpr.empty()) {
        std::vector<std::string> table;
        table.push_back("Invalid expression");
        ui.setTruthTable(table);
        ui.setShowTruthTable(true);
        ui.setupUITexts();
        return;
    }

    for (char c : cleanedExpr) {
        if (std::isalpha(c)) variables.push_back(c);
    }
    removeDuplicateVariables();
    int numVars = variables.size();
    minterms.resize(1 << numVars, false);
    std::vector<std::string> table;

    if (numVars == 0) {
        table.push_back("No variables found in expression");
        ui.setTruthTable(table);
        ui.setShowTruthTable(true);
        ui.setupUITexts();
        return;
    }

    // Prepare headers
    std::vector<std::string> headers;
    for (char v : variables) {
        headers.push_back(std::string(1, v));
    }
    headers.push_back("Y1");

    // Calculate column widths using grid system
    std::vector<int> colWidths = TableGrid::calculateColumnWidths(headers, 1 << numVars);

    // Generate table structure
    table.push_back(TableGrid::generateBorder(colWidths, '+', '+', '+'));
    table.push_back(TableGrid::generateRow(headers, colWidths));
    table.push_back(TableGrid::generateBorder(colWidths, '+', '+', '+'));

    // Generate truth table rows
    for (int i = 0; i < (1 << numVars); i++) {
        for (int j = 0; j < numVars; j++) {
            varValues[variables[j]] = (i >> (numVars - 1 - j)) & 1;
        }
        std::string postfix = infixToPostfix(cleanedExpr);
        bool result = evaluatePostfix(postfix);
        minterms[i] = result;

        std::vector<std::string> rowCells;
        for (int j = 0; j < numVars; j++) {
            bool val = (i >> (numVars - 1 - j)) & 1;
            rowCells.push_back(std::string(1, val ? '1' : '0'));
        }
        rowCells.push_back(std::string(1, result ? '1' : '0'));

        table.push_back(TableGrid::generateRow(rowCells, colWidths));
    }

    table.push_back(TableGrid::generateBorder(colWidths, '+', '+', '+'));

    ui.setTruthTable(table);
    ui.setShowTruthTable(true);
    ui.setupUITexts();
}

std::vector<size_t> ExpressionProcessor::findConnectedInputs(const Circuit &circuit, const std::vector<size_t> &allInputs,
                                                             const std::vector<size_t> &outputs) {
    std::set<size_t> connectedSet;

    // For each output, find all gates that can reach it (backwards traversal)
    for (size_t outputIndex : outputs) {
        std::set<size_t> visited;
        std::stack<size_t> toVisit;
        toVisit.push(outputIndex);

        while (!toVisit.empty()) {
            size_t currentGate = toVisit.top();
            toVisit.pop();

            if (visited.count(currentGate)) continue;
            visited.insert(currentGate);

            // Check if this is an input gate
            if (std::find(allInputs.begin(), allInputs.end(), currentGate) != allInputs.end()) {
                connectedSet.insert(currentGate);
                continue;
            }

            // Find all wires that connect to this gate as destination
            const auto &wires = circuit.getWires();
            for (const auto &wire : wires) {
                if (wire.getDstGate() == currentGate) {
                    toVisit.push(wire.getSrcGate());
                }
            }
        }
    }

    // Convert set to vector and maintain original order
    std::vector<size_t> result;
    for (size_t input : allInputs) {
        if (connectedSet.count(input)) {
            result.push_back(input);
        }
    }

    return result;
}

void ExpressionProcessor::generateTruthTable(const Circuit &circuit, UIManager &ui) {
    auto allInputs = circuit.getInputGates();
    auto outputs = circuit.getOutputGates();

    if (allInputs.empty()) {
        std::vector<std::string> table;
        table.push_back("No input gates found in circuit");
        ui.setTruthTable(table);
        ui.setShowTruthTable(true);
        ui.setupUITexts();
        return;
    }

    // Find inputs that are actually connected to outputs
    std::vector<size_t> connectedInputs = findConnectedInputs(circuit, allInputs, outputs);

    if (connectedInputs.empty()) {
        std::vector<std::string> table;
        table.push_back("No inputs connected to outputs");
        ui.setTruthTable(table);
        ui.setShowTruthTable(true);
        ui.setupUITexts();
        return;
    }

    int numInputs = connectedInputs.size();
    std::vector<std::string> table;

    // Prepare headers using grid system - only for connected inputs
    std::vector<std::string> headers;
    for (size_t i = 0; i < connectedInputs.size(); ++i) {
        headers.push_back(std::string(1, 'A' + i));
    }
    for (size_t i = 0; i < outputs.size(); ++i) {
        headers.push_back("Y" + std::to_string(i + 1));
    }

    // Calculate column widths using grid system
    std::vector<int> colWidths = TableGrid::calculateColumnWidths(headers, 1 << numInputs);

    // Generate table structure
    table.push_back(TableGrid::generateBorder(colWidths, '+', '+', '+'));
    table.push_back(TableGrid::generateRow(headers, colWidths));
    table.push_back(TableGrid::generateBorder(colWidths, '+', '+', '+'));

    // Generate truth table rows
    for (int i = 0; i < (1 << numInputs); ++i) {
        Circuit tempCircuit = circuit;  // Copy to avoid modifying original
        for (int j = 0; j < numInputs; ++j) {
            tempCircuit.getGates()[connectedInputs[j]].setState((i >> j) & 1);
        }
        tempCircuit.evaluateCircuit();

        std::vector<std::string> rowCells;
        // Input values
        for (size_t in : connectedInputs) {
            rowCells.push_back(std::string(1, tempCircuit.getGates()[in].getState() ? '1' : '0'));
        }
        // Output values
        for (size_t out : outputs) {
            rowCells.push_back(std::string(1, tempCircuit.getGates()[out].getState() ? '1' : '0'));
        }

        table.push_back(TableGrid::generateRow(rowCells, colWidths));
    }

    table.push_back(TableGrid::generateBorder(colWidths, '+', '+', '+'));

    ui.setTruthTable(table);
    ui.setShowTruthTable(true);
    ui.setupUITexts();
}

void ExpressionProcessor::generateLogicalExpression(const Circuit &circuit, UIManager &ui) {
    std::map<size_t, std::string> expressions;
    std::string currExpr = "";
    auto outputs = circuit.getOutputGates();

    if (outputs.empty()) {
        currExpr = "No output gates in circuit";
    } else {
        const auto &gates = circuit.getGates();
        if (!gates.empty()) {
            for (size_t i = 0; i < outputs.size(); ++i) {
                if (outputs[i] < gates.size()) {
                    std::string gateExpr = circuit.generateExpressionForGate(outputs[i], expressions);
                    if (!gateExpr.empty()) {
                        gateExpr = cleanExpression(simplifyDoubleNegations(gateExpr));
                        currExpr += "Y" + std::to_string(i + 1) + " = " + gateExpr + "\n";
                    }
                }
            }
        }
    }

    if (currExpr.empty()) {
        currExpr = "Invalid circuit configuration";
    }

    ui.setCurrentExpression(currExpr);
    ui.setShowExpression(true);
    ui.setupUITexts();
}