#include "parser.h"
#include <iostream>
#include <string>
using namespace std; 

int main() {
    // test 1
    DAG myDAG = DAG::parse("log(2*x^2+3*y/4-z^3)");
    cout << myDAG.toInfix() << endl;
    DAG myDAG2 = myDAG.getPartialDerivative("x");
    cout << myDAG2.toInfix() << endl;
    vector<double> vars = {1, 2, 0.5};
    myDAG.printVariables();
    double result = myDAG.calculate(vars);
    cout << result << endl;
    double result2 = myDAG2.calculate(vars);
    cout << result2 << endl;
    //test 2
    DAG test2 = DAG::parse("x^10");
    string toPrint;
    do {
        test2 = test2.getPartialDerivative("x");
        toPrint = test2.toInfix();
        cout << toPrint << endl;
    } while (toPrint != "0");
}