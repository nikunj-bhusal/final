#pragma once
#include <algorithm>
#include <limits>
#include <map>
#include <set>
#include <stack>
#include <string>
#include <vector>

#include "Circuit.hpp"
#include "UIManager.hpp"

class ExpressionProcessor {
   private:
    std::string expression;
    std::vector<char> variables;
    std::map<char, bool> varValues;
    std::vector<bool> minterms;

    void removeDuplicateVariables();
    bool isOperator(char c);
    int precedence(char op);
    bool applyOperation(bool a, bool b, char op);
    std::string infixToPostfix(const std::string &infix);
    bool evaluatePostfix(const std::string &postfix);
    std::string getBinaryString(int num, int length);
    std::string termToExpression(const std::string &term);
    std::vector<size_t> findConnectedInputs(const Circuit &circuit, const std::vector<size_t> &allInputs, const std::vector<size_t> &outputs);
    std::string cleanExpression(const std::string &infix);
    std::string simplifyDoubleNegations(const std::string &infix);

    std::vector<int> getGrayCodeOrder(int numVars);
    bool adjacentInKMap(int a, int b, int numVars);
    std::set<std::string> findPrimeImplicantsKMap();
    std::string generateImplicantFromGroup(const std::set<int> &group, int numVars);
    std::string generateExactEquation();
    std::string simplifyExpressionKMap();

    bool setupExpressionAnalysis(const std::string &expr, UIManager &ui, std::vector<std::string> &table);
    void generateTableStructure(const std::vector<std::string> &headers, int numRows, std::vector<std::string> &table);
    bool validateCircuitForTruthTable(const Circuit &circuit, UIManager &ui, std::vector<std::string> &table, std::vector<size_t> &connectedInputs,
                                      std::vector<size_t> &outputs);

    bool differsByOneBit(const std::string &a, const std::string &b);
    std::string combineTerms(const std::string &a, const std::string &b);

   public:
    void setExpression(const std::string &expr) { expression = expr; }
    const std::string &getExpression() const { return expression; }
    void generateTruthTable(const Circuit &circuit, UIManager &ui);
    void generateExpressionTruthTable(UIManager &ui);
    std::string simplifyExpression() { return simplifyExpressionKMap(); }
    void generateLogicalExpression(const Circuit &circuit, UIManager &ui);
    void readExpression();
    void generateCircuitFromExpression(const std::string &expr);
};