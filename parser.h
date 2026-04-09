#include <algorithm>
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

    bool isLeaf() const { return adj.size() == 0; }

};

class DAG {

public:

    vector<Node> nodes;
    vector<Node> tempNodes;
    size_t tempCoord;
    unordered_map<string, int> uniqueNodes;
    DAG(size_t num) : nodes(num) {}

    void addEdge(size_t a, size_t b) { nodes[a].adj.push_back(b); }

    bool isEdge(size_t a, size_t b) const { for (size_t i: nodes[a].adj) if (i == b) return true; return false; }

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
        for(size_t i = 0; i < nodes.size(); i++) nodes[i].color = WHITE;
        for(size_t i = 0; i < nodes.size(); i++) if (nodes[i].color == WHITE && dfs_hasCycle(i)) return true;
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
            for (string u: unary) if (u == output.front()) isUnaryOp = true;
            if (isAtom) nodeOrder.push(i); else if (isUnaryOp) {
                if (nodeOrder.empty()) throw invalid_argument("Invalid expression!");
                Parsed.addEdge(i, nodeOrder.top());
                nodeOrder.pop();
                nodeOrder.push(i);
            } else {
                if (nodeOrder.size() < 2) throw invalid_argument("Invalid expression!");
                int second = nodeOrder.top();
                nodeOrder.pop();
                int first = nodeOrder.top();
                nodeOrder.pop();
                Parsed.addEdge(i, first);
                Parsed.addEdge(i, second);
                nodeOrder.push(i);
            }
            output.pop();
        }
        return Parsed;
    }

    string toInfix(int nodeCoord, int parentPrec = 0) const {
        Node node = nodes[nodeCoord];
        if (node.isLeaf()) return node.label;
        if (node.adj.size() == 1) { return node.label + "(" + toInfix(node.adj[0], 0) + ")"; }
        int currentPrecedence = precedence.at(node.label);
        string result = toInfix(node.adj[0], currentPrecedence) + " " + node.label + " " + toInfix(node.adj[1], currentPrecedence);
        if (currentPrecedence < parentPrec) { return "(" + result + ")"; } 
        return result;
    }

    DAG getPartialDerivative(string varToDerive) { 
        deriveMemo.clear();
        transposeMemo.clear();
        size_t size = derive(nodes.size() - 1, varToDerive); 
        DAG PartialDerivative(size + 1);
        PartialDerivative.nodes = tempNodes;
        return PartialDerivative;
    }

    void printDFS() {
        cout << "DFS: ";
        stack<size_t> DFS;
        if (nodes.empty()) return;
        DFS.push(nodes.size() - 1);
        nodes[nodes.size() - 1].visited = true;
        while (!DFS.empty()) {
            size_t size = DFS.size();
            for (size_t i = 0; i < size; i++) {
                size_t currentCoord = DFS.top();
                DFS.pop();
                Node currentNode = nodes[currentCoord];
                cout << currentNode.label + " ";
                vector<size_t> aux = currentNode.adj;
                reverse(aux.begin(), aux.end());
                for (size_t adjacent: aux) if (!nodes[adjacent].visited) { DFS.push(adjacent); nodes[adjacent].visited = true; }
            }
        }
        clearVisited();
        cout << endl;
    }

    void printBFS() {
        cout << "BFS: ";
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
                cout << currentNode.label + " ";
                for (size_t adjacent: currentNode.adj) if (!nodes[adjacent].visited) { BFS.push(adjacent); nodes[adjacent].visited = true; }
            }
        }
        clearVisited();
        cout << endl;
    }

private:

    unordered_map<string, size_t> constantMemo;
    unordered_map<size_t, size_t> transposeMemo;
    unordered_map<size_t, size_t> deriveMemo;

    size_t getOrCreateConstant(const string& varToDerive) {
        if (constantMemo.count(varToDerive)) return constantMemo[varToDerive];
        tempNodes.push_back(Node(varToDerive));
        return constantMemo[varToDerive] = tempNodes.size() - 1;
    }

    size_t transpose(size_t coord) {
        if (transposeMemo.count(coord)) return transposeMemo[coord];
        Node newNode(nodes[coord].label);
        for (size_t oldChildIndex : nodes[coord].adj) { newNode.adj.push_back(transpose(oldChildIndex)); }
        tempNodes.push_back(newNode);
        return transposeMemo[coord] = tempNodes.size() - 1;
    }

    size_t derive(int coord, const string& varToDerive) {
        if (deriveMemo.count(coord)) return deriveMemo[coord];
        if (nodes[coord].isLeaf()) { tempNodes.push_back(Node(nodes[coord].label == varToDerive ? "1" : "0")); } 
        else if (nodes[coord].label == "+") {
            size_t l = derive(nodes[coord].adj[0], varToDerive);
            size_t r = derive(nodes[coord].adj[1], varToDerive);
            if (tempNodes[l].label == "0") return r;
            if (tempNodes[r].label == "0") return l;
            tempNodes.push_back(Node("+"));
            tempNodes.back().adj = {l, r};
        } else if (nodes[coord].label == "-") {
            size_t l = derive(nodes[coord].adj[0], varToDerive);
            size_t r = derive(nodes[coord].adj[1], varToDerive);
            if (tempNodes[l].label == "0") { getOrCreateConstant("-" + tempNodes[r].label); return tempNodes.size() - 1; }
            if (tempNodes[r].label == "0") return l;
            tempNodes.push_back(Node("-"));
            tempNodes.back().adj = {l, r};
        } else if (nodes[coord].label == "*") {
            size_t f_prime = derive(nodes[coord].adj[0], varToDerive);
            size_t g = transpose(nodes[coord].adj[1]);
            size_t g_prime = derive(nodes[coord].adj[1], varToDerive);
            size_t f = transpose(nodes[coord].adj[0]);
            tempNodes.push_back(Node("*"));
            tempNodes.back().adj = {f_prime, g};
            size_t term1 = tempNodes.size() - 1;
            tempNodes.push_back(Node("*"));
            tempNodes.back().adj = {f, g_prime};
            size_t term2 = tempNodes.size() - 1;
            tempNodes.push_back(Node("+"));
            tempNodes.back().adj = {term1, term2};
        } else if (nodes[coord].label == "/") {
            size_t f_prime = derive(nodes[coord].adj[0], varToDerive);
            size_t g = transpose(nodes[coord].adj[1]);
            size_t g_prime = derive(nodes[coord].adj[1], varToDerive);
            size_t f = transpose(nodes[coord].adj[0]);
            tempNodes.push_back(Node("*"));
            tempNodes.back().adj = {f_prime, g};
            size_t term1 = tempNodes.size() - 1;
            tempNodes.push_back(Node("*"));
            tempNodes.back().adj = {f, g_prime};
            size_t term2 = tempNodes.size() - 1;
            tempNodes.push_back(Node("-"));
            tempNodes.back().adj = {term1, term2};
            size_t term3 = tempNodes.size() - 1;
            tempNodes.push_back(Node("^"));
            tempNodes.back().adj = {g, getOrCreateConstant("2")};
            size_t term4 = tempNodes.size() - 1;
            tempNodes.push_back(Node("/"));
            tempNodes.back().adj = {term3, term4};
        }
        return deriveMemo[coord] = tempNodes.size() - 1;
    }

};