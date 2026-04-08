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
    myDAG.printBFS();
}