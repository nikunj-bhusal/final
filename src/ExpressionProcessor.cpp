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
}

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
    bool lastWasOperator = true;
    int parenCount = 0;

    for (char c : infix) {
        if (std::isspace(c)) continue;

        if (c == '(') {
            parenCount++;
            cleaned += c;
            lastWasOperator = true;
        } else if (c == ')') {
            parenCount--;
            if (parenCount < 0) return "";
            cleaned += c;
            lastWasOperator = false;
        } else if (isOperator(c)) {
            if (lastWasOperator && c != '~') return "";
            cleaned += c;
            lastWasOperator = (c != '~');
        } else if (std::isalpha(c)) {
            if (!lastWasOperator) return "";
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
        result.erase(pos, 2);
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
            if (!expectOperand) return "";
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
            if (s.empty()) return "";
            s.pop();
            expectOperand = false;
        } else if (isOperator(c)) {
            if (expectOperand && c != '~') return "";
            while (!s.empty() && s.top() != '(' && precedence(s.top()) >= precedence(c)) {
                postfix += s.top();
                s.pop();
            }
            s.push(c);
            expectOperand = (c != '~');
        }
    }

    while (!s.empty()) {
        if (s.top() == '(') return "";
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

std::vector<int> ExpressionProcessor::getGrayCodeOrder(int numVars) {
    std::vector<int> order;
    int size = 1 << numVars;

    if (numVars <= 1) {
        for (int i = 0; i < size; i++) {
            order.push_back(i);
        }
    } else if (numVars == 2) {
        order = {0, 1, 3, 2};
    } else if (numVars == 3) {
        order = {0, 1, 3, 2, 6, 7, 5, 4};
    } else if (numVars == 4) {
        order = {0, 1, 3, 2, 6, 7, 5, 4, 12, 13, 15, 14, 10, 11, 9, 8};
    } else {
        for (int i = 0; i < size; i++) {
            order.push_back(i);
        }
    }
    return order;
}

bool ExpressionProcessor::adjacentInKMap(int a, int b, int numVars) {
    int diff = a ^ b;
    return diff > 0 && (diff & (diff - 1)) == 0;
}

std::set<std::string> ExpressionProcessor::findPrimeImplicantsKMap() {
    if (variables.empty() || minterms.empty()) return {};

    int numVars = variables.size();
    std::set<int> trueMinterms;

    for (size_t i = 0; i < minterms.size(); i++) {
        if (minterms[i]) {
            trueMinterms.insert(i);
        }
    }

    if (trueMinterms.empty()) return {};

    std::set<std::string> primeImplicants;
    std::set<int> covered;


    for (int groupSize = 1; groupSize <= (1 << numVars); groupSize *= 2) {
        std::vector<std::set<int>> groups;

        for (int start : trueMinterms) {
            if (covered.count(start)) continue;

            std::set<int> group;
            group.insert(start);

            std::vector<int> candidates(trueMinterms.begin(), trueMinterms.end());

            for (int size = 1; size < groupSize && !candidates.empty(); size *= 2) {
                std::vector<int> nextCandidates;
                for (int candidate : candidates) {
                    if (group.count(candidate)) continue;

                    bool canAdd = false;
                    for (int member : group) {
                        if (adjacentInKMap(member, candidate, numVars)) {
                            canAdd = true;
                            break;
                        }
                    }

                    if (canAdd) {
                        group.insert(candidate);
                        if (group.size() == groupSize) break;
                    } else {
                        nextCandidates.push_back(candidate);
                    }
                }
                candidates = nextCandidates;
            }

            if (group.size() == groupSize) {
                bool validGroup = true;
                for (int member : group) {
                    if (!trueMinterms.count(member)) {
                        validGroup = false;
                        break;
                    }
                }

                if (validGroup) {
                    groups.push_back(group);
                }
            }
        }

        for (const auto &group : groups) {
            if (group.size() == groupSize) {
                std::string implicant = generateImplicantFromGroup(group, numVars);
                if (!implicant.empty()) {
                    primeImplicants.insert(implicant);
                    for (int member : group) {
                        covered.insert(member);
                    }
                }
            }
        }
    }

    for (int minterm : trueMinterms) {
        if (!covered.count(minterm)) {
            primeImplicants.insert(getBinaryString(minterm, numVars));
        }
    }

    return primeImplicants;
}

std::string ExpressionProcessor::generateImplicantFromGroup(const std::set<int> &group, int numVars) {
    if (group.empty()) return "";

    std::string implicant(numVars, 'X');

    for (int bit = 0; bit < numVars; bit++) {
        int firstValue = -1;
        bool constant = true;

        for (int minterm : group) {
            int bitValue = (minterm >> (numVars - 1 - bit)) & 1;
            if (firstValue == -1) {
                firstValue = bitValue;
            } else if (firstValue != bitValue) {
                constant = false;
                break;
            }
        }

        if (constant) {
            implicant[bit] = (firstValue == 1) ? '1' : '0';
        } else {
            implicant[bit] = '-';
        }
    }

    return implicant;
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
    if (!expr.empty() && expr.back() == '.') {
        expr = expr.substr(0, expr.size() - 1);
    }
    return expr.empty() ? "1" : expr;
}

std::string ExpressionProcessor::generateExactEquation() {
    if (variables.empty() || minterms.empty()) return "0";

    bool all_true = true, all_false = true;
    for (bool m : minterms) {
        if (!m) all_true = false;
        if (m) all_false = false;
    }
    if (all_true) return "1";
    if (all_false) return "0";

    std::string result;
    bool first = true;

    for (size_t i = 0; i < minterms.size(); i++) {
        if (minterms[i]) {
            if (!first) {
                result += " + ";
            }
            first = false;

            std::string minterm;
            for (int j = variables.size() - 1; j >= 0; j--) {
                bool bit = (i >> j) & 1;
                if (!minterm.empty()) minterm += ".";
                if (!bit) minterm += "~";
                minterm += std::string(1, variables[variables.size() - 1 - j]);
            }
            result += "(" + minterm + ")";
        }
    }

    return result.empty() ? "0" : result;
}

std::string ExpressionProcessor::simplifyExpressionKMap() {
    if (variables.empty() || minterms.empty()) return "0";

    bool all_true = true, all_false = true;
    for (bool m : minterms) {
        if (!m) all_true = false;
        if (m) all_false = false;
    }
    if (all_true) return "1";
    if (all_false) return "0";

    std::set<std::string> primeImplicants = findPrimeImplicantsKMap();

    if (primeImplicants.empty()) return "0";

    std::string result;
    bool first = true;

    for (const std::string &term : primeImplicants) {
        std::string expr = termToExpression(term);
        if (!expr.empty()) {
            if (!first) {
                result += " + ";
            }
            first = false;
            result += "(" + expr + ")";
        }
    }

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
            return std::string(1, variables[0]) + " ^ " + std::string(1, variables[1]);
        }
    }

    return result.empty() ? "0" : result;
}

bool ExpressionProcessor::setupExpressionAnalysis(const std::string &expr, UIManager &ui, std::vector<std::string> &table) {
    std::string cleanedExpr = cleanExpression(simplifyDoubleNegations(expr));
    if (cleanedExpr.empty()) {
        table.push_back("Invalid expression");
        ui.setTruthTable(table);
        ui.setShowTruthTable(true);
        ui.setupUITexts();
        return false;
    }

    variables.clear();
    for (char c : cleanedExpr) {
        if (std::isalpha(c)) variables.push_back(c);
    }
    removeDuplicateVariables();

    if (variables.empty()) {
        table.push_back("No variables found in expression");
        ui.setTruthTable(table);
        ui.setShowTruthTable(true);
        ui.setupUITexts();
        return false;
    }

    return true;
}

void ExpressionProcessor::generateTableStructure(const std::vector<std::string> &headers, int numRows, std::vector<std::string> &table) {
    std::vector<int> colWidths = TableGrid::calculateColumnWidths(headers, numRows);
    table.push_back(TableGrid::generateBorder(colWidths, '+', '+', '+'));
    table.push_back(TableGrid::generateRow(headers, colWidths));
    table.push_back(TableGrid::generateBorder(colWidths, '+', '+', '+'));
}

void ExpressionProcessor::generateExpressionTruthTable(UIManager &ui) {
    std::vector<std::string> table;

    if (!setupExpressionAnalysis(expression, ui, table)) {
        return;
    }

    int numVars = variables.size();
    minterms.resize(1 << numVars, false);

    std::vector<std::string> headers;
    for (char v : variables) {
        headers.push_back(std::string(1, v));
    }
    headers.push_back("Y1");

    generateTableStructure(headers, 1 << numVars, table);

    std::vector<int> colWidths = TableGrid::calculateColumnWidths(headers, 1 << numVars);
    std::string cleanedExpr = cleanExpression(simplifyDoubleNegations(expression));

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

    std::string exactEquation = generateExactEquation();
    std::string simplifiedExpr = simplifyExpressionKMap();

    ui.setInputExpression(exactEquation, 1);
    ui.setCurrentExpression(simplifiedExpr, 1);
    ui.setShowExpression(true, 1);

    ui.setTruthTable(table);
    ui.setShowTruthTable(true);
    ui.setupUITexts();
}

std::vector<size_t> ExpressionProcessor::findConnectedInputs(const Circuit &circuit, const std::vector<size_t> &allInputs,
                                                             const std::vector<size_t> &outputs) {
    std::set<size_t> connectedSet;

    for (size_t outputIndex : outputs) {
        std::set<size_t> visited;
        std::stack<size_t> toVisit;
        toVisit.push(outputIndex);

        while (!toVisit.empty()) {
            size_t currentGate = toVisit.top();
            toVisit.pop();

            if (visited.count(currentGate)) continue;
            visited.insert(currentGate);

            if (std::find(allInputs.begin(), allInputs.end(), currentGate) != allInputs.end()) {
                connectedSet.insert(currentGate);
                continue;
            }

            const auto &wires = circuit.getWires();
            for (const auto &wire : wires) {
                if (wire.getDstGate() == currentGate) {
                    toVisit.push(wire.getSrcGate());
                }
            }
        }
    }

    std::vector<size_t> result;
    for (size_t input : allInputs) {
        if (connectedSet.count(input)) {
            result.push_back(input);
        }
    }
    return result;
}

bool ExpressionProcessor::validateCircuitForTruthTable(const Circuit &circuit, UIManager &ui, std::vector<std::string> &table,
                                                       std::vector<size_t> &connectedInputs, std::vector<size_t> &outputs) {
    auto allInputs = circuit.getInputGates();
    outputs = circuit.getOutputGates();

    if (allInputs.empty()) {
        table.push_back("No input gates found in circuit");
        ui.setTruthTable(table);
        ui.setShowTruthTable(true);
        ui.setupUITexts();
        return false;
    }

    connectedInputs = findConnectedInputs(circuit, allInputs, outputs);

    if (connectedInputs.empty()) {
        table.push_back("No inputs connected to outputs");
        ui.setTruthTable(table);
        ui.setShowTruthTable(true);
        ui.setupUITexts();
        return false;
    }

    return true;
}

void ExpressionProcessor::generateTruthTable(const Circuit &circuit, UIManager &ui) {
    std::vector<std::string> table;
    std::vector<size_t> connectedInputs, outputs;

    if (!validateCircuitForTruthTable(circuit, ui, table, connectedInputs, outputs)) {
        return;
    }

    int numInputs = connectedInputs.size();
    variables.clear();

    for (size_t i = 0; i < connectedInputs.size(); ++i) {
        variables.push_back('A' + i);
    }

    minterms.resize(1 << numInputs, false);

    std::vector<std::string> headers;
    for (size_t i = 0; i < connectedInputs.size(); ++i) {
        headers.push_back(std::string(1, 'A' + i));
    }
    for (size_t i = 0; i < outputs.size(); ++i) {
        headers.push_back("Y" + std::to_string(i + 1));
    }

    generateTableStructure(headers, 1 << numInputs, table);
    std::vector<int> colWidths = TableGrid::calculateColumnWidths(headers, 1 << numInputs);

    for (int i = 0; i < (1 << numInputs); ++i) {
        Circuit tempCircuit = circuit;
        for (int j = 0; j < numInputs; ++j) {
            tempCircuit.getGates()[connectedInputs[j]].setState((i >> j) & 1);
        }
        tempCircuit.evaluateCircuit();

        if (!outputs.empty()) {
            minterms[i] = tempCircuit.getGates()[outputs[0]].getState();
        }

        std::vector<std::string> rowCells;
        for (size_t in : connectedInputs) {
            rowCells.push_back(std::string(1, tempCircuit.getGates()[in].getState() ? '1' : '0'));
        }
        for (size_t out : outputs) {
            rowCells.push_back(std::string(1, tempCircuit.getGates()[out].getState() ? '1' : '0'));
        }

        table.push_back(TableGrid::generateRow(rowCells, colWidths));
    }

    table.push_back(TableGrid::generateBorder(colWidths, '+', '+', '+'));

    if (!outputs.empty() && !variables.empty()) {
        std::string exactEquation = generateExactEquation();
        std::string simplifiedExpr = simplifyExpressionKMap();

        ui.setInputExpression(exactEquation, 2);
        ui.setCurrentExpression(simplifiedExpr, 2);
        ui.setShowExpression(true, 2);
    }

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