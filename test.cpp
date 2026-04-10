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
    DAG myDAG = DAG::parse("4*x-log(321)/(3+2-1231*x^2)");
    cout << myDAG.toInfix() << endl;
    DAG myDAG2 = myDAG.getPartialDerivative("x");
    cout << myDAG2.toInfix() << endl;
}