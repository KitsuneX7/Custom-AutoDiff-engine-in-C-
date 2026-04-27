#include "parser.h"
#include <iostream>
#include <string>
using namespace std; 

int main() {
    DAG myDAG = DAG::parse("((2^(log((a^2*sin(b)^2 + 1)/(c^50 + d^4 + 1))) + cos((e^f)/(g^2+1))) / (tan((h*i)/(j^2+1)) + log(a^2+b^2+1))) - (1 / ((c*d)/(e^2+1) + (f*g)/(h^2+1)))^(sin(i*j)) + (a*j - b*i)^4 * cos(c+d+e+f)");
    DAG next = myDAG;
    vector<string> variables = {"a", "b", "c", "d", "e", "f", "g", "h", "i", "j"};
    for (string var: variables) {
        for (int i = 0; i < 10; i++) {
            next = next.getPartialDerivative(var);
        }
    }
}