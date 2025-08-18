#pragma once
#include <map>
#include <set>
#include <string>
#include <vector>

class ExpressionSimplifier {
   private:
    // Helper structures for K-map simplification
    struct MinTerm {
        int value;
        std::vector<int> variables;
        std::string binary;

        MinTerm(int v, int numVars) : value(v) {
            binary = "";
            for (int i = numVars - 1; i >= 0; i--) {
                binary += ((v >> i) & 1) ? '1' : '0';
            }

            for (int i = 0; i < numVars; i++) {
                variables.push_back((v >> (numVars - 1 - i)) & 1);
            }
        }
    };

    struct Implicant {
        std::vector<int> terms;
        std::string pattern;
        bool used;

        Implicant(const std::vector<int>& t, const std::string& p) : terms(t), pattern(p), used(false) {}
    };

    // Helper methods
    std::vector<std::vector<int>> generateTruthTable(const std::string& expression, int numVars) const;
    std::vector<int> extractMinterms(const std::vector<std::vector<int>>& truthTable) const;
    std::string quineMcCluskey(const std::vector<int>& minterms, int numVars, const std::string& expression) const;
    std::vector<std::vector<Implicant>> groupByOnes(const std::vector<int>& minterms, int numVars) const;
    std::vector<Implicant> findPrimeImplicants(const std::vector<int>& minterms, int numVars) const;
    std::string generateSimplifiedExpression(const std::vector<Implicant>& primeImplicants, const std::vector<int>& minterms, int numVars,
                                             const std::string& expression) const;
    bool canCombine(const std::string& pattern1, const std::string& pattern2) const;
    std::string combinePatterns(const std::string& pattern1, const std::string& pattern2) const;
    int countOnes(const std::string& binary) const;
    int getVariableCount(const std::string& expression) const;
    std::string convertToStandardForm(const std::string& expression) const;

   public:
    ExpressionSimplifier() = default;

    // Main simplification method
    std::string simplifyExpression(const std::string& expression) const;

    // Generate truth table for an expression
    std::vector<std::string> generateTruthTableDisplay(const std::string& expression) const;

    // Generate truth table for multiple expressions
    std::vector<std::string> generateMultiOutputTruthTable(const std::vector<std::string>& expressions) const;

    // Validate expression format
    bool isValidExpression(const std::string& expression) const;

    // Get variable names from expression
    std::set<char> getVariables(const std::string& expression) const;

    // Evaluate expression with given variable values
    bool evaluateExpression(const std::string& expression, const std::map<char, bool>& values) const;
};
