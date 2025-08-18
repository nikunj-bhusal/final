#include "ExpressionSimplifier.hpp"

#include <algorithm>
#include <iostream>
#include <sstream>
#include <stack>
#include <unordered_set>

std::string ExpressionSimplifier::simplifyExpression(const std::string& expression) const {
    if (expression.empty() || !isValidExpression(expression)) {
        return "Invalid expression";
    }

    std::string standardForm = convertToStandardForm(expression);
    int numVars = getVariableCount(standardForm);

    if (numVars == 0) {
        return expression;
    }

    // Generate truth table
    std::vector<std::vector<int>> truthTable = generateTruthTable(standardForm, numVars);

    // Extract minterms (rows where output is 1)
    std::vector<int> minterms = extractMinterms(truthTable);

    if (minterms.empty()) {
        return "0";
    }

    if (minterms.size() == (1 << numVars)) {
        return "1";
    }

    // Apply Quine-McCluskey algorithm
    return quineMcCluskey(minterms, numVars);
}

std::vector<std::string> ExpressionSimplifier::generateTruthTableDisplay(const std::string& expression) const {
    std::vector<std::string> result;

    if (expression.empty() || !isValidExpression(expression)) {
        result.push_back("Invalid expression");
        return result;
    }

    std::string standardForm = convertToStandardForm(expression);
    int numVars = getVariableCount(standardForm);

    if (numVars == 0) {
        result.push_back("No variables found");
        return result;
    }

    std::set<char> variables = getVariables(standardForm);
    std::vector<char> varList(variables.begin(), variables.end());
    std::sort(varList.begin(), varList.end());

    // Create header
    std::string header = "|";
    for (char var : varList) {
        header += " " + std::string(1, var) + " |";
    }
    header += " Output |";
    result.push_back(header);

    // Create separator
    std::string separator = "|";
    for (size_t i = 0; i < varList.size(); i++) {
        separator += "---|";
    }
    separator += "--------|";
    result.push_back(separator);

    // Generate truth table rows
    int numRows = 1 << numVars;
    for (int i = 0; i < numRows; i++) {
        std::map<char, bool> values;
        std::string row = "|";

        for (int j = 0; j < numVars; j++) {
            char var = varList[j];
            bool value = (i >> (numVars - 1 - j)) & 1;
            values[var] = value;
            row += " " + std::string(value ? "1" : "0") + " |";
        }

        bool output = evaluateExpression(standardForm, values);
        row += "   " + std::string(output ? "1" : "0") + "    |";
        result.push_back(row);
    }

    return result;
}

std::vector<std::string> ExpressionSimplifier::generateMultiOutputTruthTable(const std::vector<std::string>& expressions) const {
    std::vector<std::string> result;

    if (expressions.empty()) {
        result.push_back("No expressions provided");
        return result;
    }

    // Find all unique variables across all expressions
    std::set<char> allVariables;
    std::vector<std::string> validExpressions;

    for (const std::string& expr : expressions) {
        if (!expr.empty() && isValidExpression(expr) && expr != "0") {
            validExpressions.push_back(expr);
            std::set<char> exprVars = getVariables(expr);
            allVariables.insert(exprVars.begin(), exprVars.end());
        }
    }

    if (validExpressions.empty() || allVariables.empty()) {
        result.push_back("No valid expressions found");
        return result;
    }

    std::vector<char> varList(allVariables.begin(), allVariables.end());
    std::sort(varList.begin(), varList.end());
    int numVars = static_cast<int>(varList.size());

    // Create header
    std::string header = "|";
    for (char var : varList) {
        header += " " + std::string(1, var) + " |";
    }
    for (size_t i = 0; i < validExpressions.size(); i++) {
        header += " Out" + std::to_string(i + 1) + " |";
    }
    result.push_back(header);

    // Create separator
    std::string separator = "|";
    for (size_t i = 0; i < varList.size(); i++) {
        separator += "---|";
    }
    for (size_t i = 0; i < validExpressions.size(); i++) {
        separator += "-----|";
    }
    result.push_back(separator);

    // Generate truth table rows
    int numRows = 1 << numVars;
    for (int i = 0; i < numRows; i++) {
        std::map<char, bool> values;
        std::string row = "|";

        // Set variable values for this row
        for (int j = 0; j < numVars; j++) {
            char var = varList[j];
            bool value = (i >> (numVars - 1 - j)) & 1;
            values[var] = value;
            row += " " + std::string(value ? "1" : "0") + " |";
        }

        // Evaluate each expression
        for (const std::string& expr : validExpressions) {
            bool output = evaluateExpression(convertToStandardForm(expr), values);
            row += "  " + std::string(output ? "1" : "0") + "  |";
        }

        result.push_back(row);
    }

    return result;
}

std::vector<std::vector<int>> ExpressionSimplifier::generateTruthTable(const std::string& expression, int numVars) const {
    std::vector<std::vector<int>> table;
    std::set<char> variables = getVariables(expression);
    std::vector<char> varList(variables.begin(), variables.end());
    std::sort(varList.begin(), varList.end());

    int numRows = 1 << numVars;
    for (int i = 0; i < numRows; i++) {
        std::vector<int> row;
        std::map<char, bool> values;

        for (int j = 0; j < numVars; j++) {
            char var = varList[j];
            bool value = (i >> (numVars - 1 - j)) & 1;
            values[var] = value;
            row.push_back(value ? 1 : 0);
        }

        bool output = evaluateExpression(expression, values);
        row.push_back(output ? 1 : 0);
        table.push_back(row);
    }

    return table;
}

std::vector<int> ExpressionSimplifier::extractMinterms(const std::vector<std::vector<int>>& truthTable) const {
    std::vector<int> minterms;

    for (size_t i = 0; i < truthTable.size(); i++) {
        const auto& row = truthTable[i];
        if (!row.empty() && row.back() == 1) {
            minterms.push_back(static_cast<int>(i));
        }
    }

    return minterms;
}

std::string ExpressionSimplifier::quineMcCluskey(const std::vector<int>& minterms, int numVars) const {
    if (minterms.empty()) {
        return "0";
    }

    // Find prime implicants
    std::vector<Implicant> primeImplicants = findPrimeImplicants(minterms, numVars);

    // Generate simplified expression
    return generateSimplifiedExpression(primeImplicants, minterms, numVars);
}

std::vector<std::vector<ExpressionSimplifier::Implicant>> ExpressionSimplifier::groupByOnes(const std::vector<int>& minterms, int numVars) const {
    std::vector<std::vector<Implicant>> groups(numVars + 1);

    for (int minterm : minterms) {
        std::string binary = "";
        for (int i = numVars - 1; i >= 0; i--) {
            binary += ((minterm >> i) & 1) ? '1' : '0';
        }

        int ones = countOnes(binary);
        groups[ones].emplace_back(std::vector<int>{minterm}, binary);
    }

    return groups;
}

std::vector<ExpressionSimplifier::Implicant> ExpressionSimplifier::findPrimeImplicants(const std::vector<int>& minterms, int numVars) const {
    std::vector<std::vector<Implicant>> currentGroups = groupByOnes(minterms, numVars);
    std::vector<Implicant> primeImplicants;

    while (true) {
        std::vector<std::vector<Implicant>> nextGroups(numVars + 1);
        bool combined = false;

        for (int i = 0; i < numVars; i++) {
            for (const auto& imp1 : currentGroups[i]) {
                for (const auto& imp2 : currentGroups[i + 1]) {
                    if (canCombine(imp1.pattern, imp2.pattern)) {
                        std::string newPattern = combinePatterns(imp1.pattern, imp2.pattern);
                        std::vector<int> newTerms = imp1.terms;
                        newTerms.insert(newTerms.end(), imp2.terms.begin(), imp2.terms.end());

                        nextGroups[i].emplace_back(newTerms, newPattern);
                        combined = true;

                        // Mark as used
                        const_cast<Implicant&>(imp1).used = true;
                        const_cast<Implicant&>(imp2).used = true;
                    }
                }
            }
        }

        // Add unused implicants as prime implicants
        for (const auto& group : currentGroups) {
            for (const auto& imp : group) {
                if (!imp.used) {
                    primeImplicants.push_back(imp);
                }
            }
        }

        if (!combined) {
            break;
        }

        currentGroups = nextGroups;
    }

    return primeImplicants;
}

std::string ExpressionSimplifier::generateSimplifiedExpression(const std::vector<Implicant>& primeImplicants, const std::vector<int>& minterms,
                                                               int numVars) const {
    if (primeImplicants.empty()) {
        return "0";
    }

    std::set<char> variables = getVariables("ABCDEFGH");
    std::vector<char> varList(variables.begin(), variables.end());
    std::sort(varList.begin(), varList.end());

    std::vector<std::string> terms;

    for (const auto& imp : primeImplicants) {
        std::string term = "";

        for (int i = 0; i < numVars && i < static_cast<int>(imp.pattern.length()); i++) {
            if (imp.pattern[i] != '-') {
                if (!term.empty()) {
                    term += ".";
                }

                if (i < static_cast<int>(varList.size())) {
                    if (imp.pattern[i] == '0') {
                        term += "~" + std::string(1, varList[i]);
                    } else {
                        term += std::string(1, varList[i]);
                    }
                }
            }
        }

        if (term.empty()) {
            term = "1";
        }

        terms.push_back(term);
    }

    if (terms.empty()) {
        return "0";
    }

    if (terms.size() == 1) {
        return terms[0];
    }

    std::string result = terms[0];
    for (size_t i = 1; i < terms.size(); i++) {
        result += " + " + terms[i];
    }

    return result;
}

bool ExpressionSimplifier::canCombine(const std::string& pattern1, const std::string& pattern2) const {
    if (pattern1.length() != pattern2.length()) {
        return false;
    }

    int differences = 0;
    for (size_t i = 0; i < pattern1.length(); i++) {
        if (pattern1[i] != pattern2[i]) {
            differences++;
            if (differences > 1) {
                return false;
            }
        }
    }

    return differences == 1;
}

std::string ExpressionSimplifier::combinePatterns(const std::string& pattern1, const std::string& pattern2) const {
    std::string result = "";

    for (size_t i = 0; i < pattern1.length(); i++) {
        if (pattern1[i] == pattern2[i]) {
            result += pattern1[i];
        } else {
            result += '-';
        }
    }

    return result;
}

int ExpressionSimplifier::countOnes(const std::string& binary) const {
    int count = 0;
    for (char c : binary) {
        if (c == '1') {
            count++;
        }
    }
    return count;
}

int ExpressionSimplifier::getVariableCount(const std::string& expression) const {
    std::set<char> variables = getVariables(expression);
    return static_cast<int>(variables.size());
}

std::string ExpressionSimplifier::convertToStandardForm(const std::string& expression) const {
    std::string result = expression;

    // Remove spaces
    result.erase(std::remove(result.begin(), result.end(), ' '), result.end());

    // Convert operators to standard form
    std::replace(result.begin(), result.end(), '^', '+');  // XOR to OR for simplification

    return result;
}

bool ExpressionSimplifier::evaluateExpression(const std::string& expression, const std::map<char, bool>& values) const {
    std::string expr = expression;

    // Replace variables with their values
    for (const auto& pair : values) {
        std::string var(1, pair.first);
        std::string val = pair.second ? "1" : "0";

        size_t pos = 0;
        while ((pos = expr.find(var, pos)) != std::string::npos) {
            expr.replace(pos, var.length(), val);
            pos += val.length();
        }
    }

    // Simple evaluation for basic expressions
    // This is a simplified evaluator - in a full implementation, you'd want a proper parser
    std::stack<bool> operands;
    std::stack<char> operators;

    for (size_t i = 0; i < expr.length(); i++) {
        char c = expr[i];

        if (c == '1') {
            operands.push(true);
        } else if (c == '0') {
            operands.push(false);
        } else if (c == '~') {
            if (i + 1 < expr.length()) {
                i++;
                if (expr[i] == '1') {
                    operands.push(false);
                } else if (expr[i] == '0') {
                    operands.push(true);
                } else if (expr[i] == '(') {
                    // Handle negation of parenthesized expression
                    int parenCount = 1;
                    size_t start = i + 1;
                    i++;
                    while (i < expr.length() && parenCount > 0) {
                        if (expr[i] == '(')
                            parenCount++;
                        else if (expr[i] == ')')
                            parenCount--;
                        i++;
                    }
                    std::string subExpr = expr.substr(start, i - start - 1);
                    bool subResult = evaluateExpression(subExpr, values);
                    operands.push(!subResult);
                    i--;
                }
            }
        } else if (c == '.' || c == '+' || c == '^') {
            while (!operators.empty() && ((c == '+' && (operators.top() == '.' || operators.top() == '^')) || (c == '^' && operators.top() == '.'))) {
                char op = operators.top();
                operators.pop();

                if (operands.size() >= 2) {
                    bool b = operands.top();
                    operands.pop();
                    bool a = operands.top();
                    operands.pop();

                    if (op == '.') {
                        operands.push(a && b);
                    } else if (op == '+') {
                        operands.push(a || b);
                    } else if (op == '^') {
                        operands.push(a != b);
                    }
                }
            }
            operators.push(c);
        } else if (c == '(') {
            operators.push(c);
        } else if (c == ')') {
            while (!operators.empty() && operators.top() != '(') {
                char op = operators.top();
                operators.pop();

                if (operands.size() >= 2) {
                    bool b = operands.top();
                    operands.pop();
                    bool a = operands.top();
                    operands.pop();

                    if (op == '.') {
                        operands.push(a && b);
                    } else if (op == '+') {
                        operands.push(a || b);
                    } else if (op == '^') {
                        operands.push(a != b);
                    }
                }
            }
            if (!operators.empty()) {
                operators.pop();  // Remove '('
            }
        }
    }

    while (!operators.empty()) {
        char op = operators.top();
        operators.pop();

        if (operands.size() >= 2) {
            bool b = operands.top();
            operands.pop();
            bool a = operands.top();
            operands.pop();

            if (op == '.') {
                operands.push(a && b);
            } else if (op == '+') {
                operands.push(a || b);
            } else if (op == '^') {
                operands.push(a != b);
            }
        }
    }

    return operands.empty() ? false : operands.top();
}

bool ExpressionSimplifier::isValidExpression(const std::string& expression) const {
    if (expression.empty()) {
        return false;
    }

    // Check for valid characters
    for (char c : expression) {
        if (!(std::isalpha(c) || c == '.' || c == '+' || c == '^' || c == '~' || c == '(' || c == ')' || c == ' ' || c == '0' || c == '1')) {
            return false;
        }
    }

    return true;
}

std::set<char> ExpressionSimplifier::getVariables(const std::string& expression) const {
    std::set<char> variables;

    for (char c : expression) {
        if (std::isalpha(c) && std::isupper(c)) {
            variables.insert(c);
        }
    }

    return variables;
}
