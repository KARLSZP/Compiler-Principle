#include <iostream>
#include <bits/stdc++.h>

using namespace std;

class Parser {
public:
    Parser(string infix): infix(infix), pos(0) {}
    void expr() {
        term();
        while (infix[pos] == '+' || infix[pos] == '-') {
            int op = infix[pos++];
            term();
            postfix += char(op);
            res.push_back(string(1, char(op)));
        }
        int end = res.size() - 1;
        result = solve(end);
    }
    void term() {
        factor();
        while (infix[pos] == '*' || infix[pos] == '/') {
            int op = infix[pos++];
            factor();
            postfix += char(op);
            res.push_back(string(1, char(op)));
        }
    }
    void factor() {
        int op = infix[pos++];
        if (op == '(') {
            expr();
            pos++;
        }
        else {
            int integer = op - '0';
            postfix += char(op);
            while (isdigit(infix[pos])) {
                integer = 10 * integer + (infix[pos] - '0');
                postfix += infix[pos++];
            }
            res.push_back(string(to_string(integer)));
        }
    }

    void error() {
        cerr << "Syntax Error" << endl;
        exit(-1);
    }

    int solve(int& end) {
        int cur_end = end--;
        if (isdigit(res[cur_end][0])) {
            return atoi(res[cur_end].c_str());
        }
        else {
            int op2 = solve(end);
            int op1 = solve(end);
            if (res[cur_end] == "+") {
                return op1 + op2;
            }
            else if (res[cur_end] == "-") {
                return op1 - op2;
            }
            else if (res[cur_end] == "*") {
                return op1 * op2;
            }
            else if (res[cur_end] == "/") {
                return op1 / op2;
            }
            else {
                error();
            }
        }
    }

    string getPostfix() {
        return postfix;
    }

    int getResult() {
        return result;
    }

private:
    int pos;
    int result;
    string infix;
    string postfix;
    vector<string> res;
};


int main()
{
    string infix;
    while (cin >> infix && infix != "#") {
        Parser parser(infix);
        parser.expr();
        // cout << parser.getPostfix() << endl;
        cout << parser.getResult() << endl;
    }
    return 0;
}
