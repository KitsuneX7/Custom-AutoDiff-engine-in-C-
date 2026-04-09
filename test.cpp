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
    DAG myDAG = DAG::parse("3.14*x*log(30*y)/(3+x)^4*z");
    cout << myDAG.toInfix(myDAG.nodes.size() - 1) << endl;
    DAG myDAG2 = DAG::parse("x*x*x*x*x*x");
    DAG myDAG3 = myDAG2.getPartialDerivative("x");
    cout << myDAG3.toInfix(myDAG3.nodes.size() - 1) << endl;
}