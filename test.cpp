#include "parser.h"
#include <iostream>
#include <queue>
#include <set>
#include <stack>
#include <string>
#include <unordered_map>
#include <vector>
using namespace std;

int main() {
    DAG myDAG = DAG::parse("x^10");
    cout << myDAG.toInfix(myDAG.nodes.size() - 1) << endl;
    DAG myDAG2 = myDAG;
    for (int i = 0; i < 1; i++) {
        myDAG2 = myDAG2.getPartialDerivative("x");
        cout << myDAG2.toInfix(myDAG2.nodes.size() - 1) << endl;
    }
}