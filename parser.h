#include <iostream>
#include <queue>
#include <set>
#include <stack>
#include <string>
#include <unordered_map>
#include <vector>
using namespace std;

unordered_map<string, int> precedence = {{"+", 1}, {"-", 1}, {"*", 2}, {"/", 2}, {"^", 3}, {"sin", 4}, {"cos", 4}, {"tan", 4}, {"log", 4}};
set<char> binary = {'+', '-', '*', '/', '^'};
set<string> unary = {"sin", "cos", "tan", "log"};

typedef enum {
    WHITE, GRAY, BLACK
} COLOR;

class Node {
public:

    string label;
    vector<size_t> adj;
    bool visited;
    COLOR color;
    Node() : visited(false), color(WHITE) {}
    Node(string l) : label(l), visited(false), color(WHITE) {}

};

class DAG {
public:

    size_t n;
    vector<Node> nodes;
    DAG(size_t num) : n(num), nodes(num) {}

    void addEdge(size_t a, size_t b) { nodes[a].adj.push_back(b); }

    bool isEdge(size_t a, size_t b) { for (size_t i: nodes[a].adj) if (i == b) return true; return false; }

    void clearVisited() { for (size_t i = 0; i < nodes.size(); i++) nodes[i].visited  = false; }
  
    bool dfs_hasCycle(size_t v) { 
        if (nodes[v].color == GRAY) return true;
        if (nodes[v].color == BLACK) return false;
        nodes[v].color = GRAY;
        for (size_t nodeNum: nodes[v].adj) if (dfs_hasCycle(nodeNum)) return true; // Changed to size_t
        nodes[v].color = BLACK;
        return false;
    }

    bool hasCycle() {
        for(size_t i = 0; i < n; i++) nodes[i].color = WHITE;
        for(size_t i = 0; i < n; i++) if (nodes[i].color == WHITE && dfs_hasCycle(i)) return true;
        return false; 
    }

    static vector<string> tokenize(const string& expression) {
        vector<string> tokens;
        string previousChars;
        bool searching = false;
        bool isNum = false;
        bool hasDot = false;
        for (size_t i = 0; i < expression.length(); i++) {
            if (expression[i] == ' ') continue;
            bool loopContinue = false;
            int ascii = int(expression[i]);
            if (!searching) {
                if (expression[i] == '(' || expression[i] == ')') { tokens.push_back(string(1, expression[i])); continue; }
                for (char b: binary) if (b == expression[i]) { string nextToken(1, expression[i]); tokens.push_back(nextToken); loopContinue = true; break; }
                if (loopContinue) continue;
                for (string u: unary) if (u[0] == expression[i]) { searching = true; isNum = false; previousChars += expression[i]; loopContinue = true; break; }
                if (loopContinue) continue;
                if (ascii > 47 && ascii < 58) { searching = true; isNum = true; hasDot = false; previousChars += expression[i]; continue; }
                if ((ascii > 64 && ascii < 91) || (ascii > 96 && ascii < 123))  { string nextToken(1, expression[i]); tokens.push_back(nextToken); continue; }
            } else {
                if (isNum) {
                    if (expression[i] == '.') { if (hasDot) { throw invalid_argument("Invalid expression!"); } else { previousChars += expression[i]; hasDot = true; continue; } }
                    if (ascii > 47 && ascii < 58) { previousChars += expression[i]; continue; }
                    if (previousChars[previousChars.size() - 1] != '.') { tokens.push_back(previousChars); previousChars.clear(); searching = false; i--; continue; }
                } else {
                    for (string u: unary) if (u.size() > previousChars.size() && u.substr(0, previousChars.size()) == previousChars && u[previousChars.size()] == expression[i]) { previousChars += expression[i]; loopContinue = true; break; }
                    if (loopContinue) continue;
                    for (string u: unary) if (u == previousChars) { tokens.push_back(previousChars); previousChars.clear(); searching = false; i--; loopContinue = true; break; }
                    if (loopContinue) continue;
                    if (previousChars.size() == 1) { tokens.push_back(previousChars); previousChars.clear(); searching = false; i--; continue; }
                }
            }
            throw invalid_argument("Invalid expression!");
        }
        if (!previousChars.empty()) {
            if (!isNum) {
                bool pass = false;
                for (string u: unary) if (u == previousChars) pass = true;
                if (!pass) throw invalid_argument("Invalid expression!");
            } else {
                if (previousChars[previousChars.size() - 1] == '.') throw invalid_argument("Invalid expression!");
            }
            tokens.push_back(previousChars);
        }
        return tokens;
    }

    static DAG parse(const string& expression) {
        vector<string> tokens = tokenize(expression);
        queue<string> output;
        stack<string> operators;
        for (size_t i = 0; i < tokens.size(); i++) { 
            if (tokens[i] == "(") { operators.push("("); continue; }
            if (tokens[i] == ")") {
                while (!operators.empty() && operators.top() != "(") { output.push(operators.top()); operators.pop(); }
                if (operators.empty()) throw invalid_argument("Invalid expression!"); else { operators.pop(); continue; }
            }
            bool isAtom = true;
            for (auto& op: precedence) if (op.first == tokens[i]) isAtom = false;
            for (string u: unary) if (u == tokens[i]) isAtom = false;
            if (isAtom) output.push(tokens[i]); else {
                while (!operators.empty() && operators.top() != "(" && precedence[tokens[i]] <= precedence[operators.top()]) { output.push(operators.top()); operators.pop(); }
                operators.push(tokens[i]);
            }
        }
        while (!operators.empty()) { output.push(operators.top()); operators.pop(); }
        DAG Parsed(output.size());
        stack<size_t> nodeOrder;
        size_t totalNodes = output.size();
        for (size_t i = 0; i < totalNodes; i++) {
            Parsed.nodes[i].label = output.front();
            bool isAtom = true;
            bool isUnaryOp = false;
            for (auto& op: precedence) if (op.first == output.front()) isAtom = false;
            for (string u: unary) if (u == output.front()) { isAtom = false; isUnaryOp = true; }
            if (isAtom) nodeOrder.push(i); else if (isUnaryOp) {
                if (nodeOrder.empty()) throw invalid_argument("Invalid expression!");
                Parsed.addEdge(i, nodeOrder.top());
                nodeOrder.pop();
                nodeOrder.push(i);
            } else {
                if (nodeOrder.size() < 2) throw invalid_argument("Invalid expression!");
                Parsed.addEdge(i, nodeOrder.top());
                nodeOrder.pop();
                Parsed.addEdge(i, nodeOrder.top());
                nodeOrder.pop();
                nodeOrder.push(i);
            }
            output.pop();
        }
        return Parsed;
    }

    void printBFS() {
        queue<size_t> BFS;
        if (nodes.empty()) return;
        BFS.push(nodes.size() - 1);
        nodes[nodes.size() - 1].visited = true;
        while (!BFS.empty()) {
            size_t size = BFS.size();
            for (size_t i = 0; i < size; i++) {
                size_t currentCoord = BFS.front();
                BFS.pop();
                Node currentNode = nodes[currentCoord];
                cout << currentNode.label << endl;
                for (size_t adjacent: currentNode.adj) if (!nodes[adjacent].visited) { BFS.push(adjacent); nodes[adjacent].visited = true; }
            }
        }
        clearVisited();
    }

};