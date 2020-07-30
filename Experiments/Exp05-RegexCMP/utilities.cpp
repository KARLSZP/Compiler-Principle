#include <iostream>
#include <bits/stdc++.h>

using namespace std;


int main(int argc, char const* argv[])
{
    string exp = "(a|b)*+a|c+d*|(a|bc)*";

    set<int> s = {1, 3, 2, 4, 5};
    set<int> t = {1, 2, 4, 5, 3};
    for (auto i = s.begin(); i != s.end(); i++) {
        cout << *i << endl;
    }
    return 0;
}