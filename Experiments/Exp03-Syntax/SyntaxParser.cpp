#include <iostream>
#include <bits/stdc++.h>

#define EPS "#"
#define ID "0"
#define DOLLAR "$"

using namespace std;

unordered_map<string, set<string>> pro;
unordered_map<string, set<string>> pro_bak;
unordered_map<string, set<string>> First;
unordered_map<string, set<string>> Follow;
unordered_map<string, unordered_map<string, string>> PredictTable;
unordered_map<string, bool> built;
stack<string> s;
vector<string> path;
vector<string> matched;
vector<string> report;
string pre_path;



void PrintTable();
void printAll();

void _init()
{
    cout << "Please input productions rules in form of:" << endl;
    cout << "(1) Replace '->' with ':'; (2) Whitespaces will be ignored." << endl;
    cout << "(3) ID should be 'a-z' or '0-9'; (4) '#' stands for Epsilon." << endl;
    cout << "e.g.:\n>> E : E+T  (E -> E + T)" << endl;
    cout << "End with a single '#'." << endl;
    string tmp;
    while (cout << ">> " && getline(cin, tmp) && tmp != EPS) {
        if (tmp.length() == 0) {
            continue;
        }
        int i = 0, flag = 0;
        string keys, prod;
        while (i < tmp.length()) {
            while (tmp[i] == ' ') {
                i++;
            }
            if (!flag && tmp[i] == ':') {
                flag = 1;
                i++;
                continue;
            }
            if (!flag) {
                if (i != tmp.length() - 1 && tmp[i + 1] == '\'') {
                    keys = tmp.substr(i, 2);
                    i += 2;
                }
                else {
                    keys = tmp.substr(i, 1);
                    i++;
                }
            }
            if (flag && tmp[i] != ' ') {
                prod += tmp[i];
                i++;
            }
        }
        pro[keys].insert(prod);
    }
    pro_bak = unordered_map<string, set<string>>(pro);
    // pro["S"] = {"0TUV", "1V"};
    // pro["T"] = {"U", "UU"};
    // pro["U"] = {EPS, "1V"};
    // pro["V"] = {EPS, "2V"};
}

void recover()
{
    pro = unordered_map<string, set<string>>(pro_bak);
}

void FIRST(string X)
{
    if (built[X]) {
        return;
    }
    built[X] = true;
    for (auto it = pro[X].begin(); it != pro[X].end(); it++) {
        for (int i = 0; i < it->length(); i++) {
            if (!isupper(it->at(i)) && it->at(i) != '\'') {
                First[X].insert(it->substr(i, 1));
                break;
            }
            if (isupper(it->at(i))) {
                string Y;
                if (i != it->length() - 1 && it->at(i + 1) == '\'') {
                    Y = it->substr(i, 2);
                }
                else {
                    Y = it->at(i);
                }
                FIRST(Y);
                bool flag = true;
                for (auto it1 = First[Y].begin(); it1 != First[Y].end(); it1++) {
                    if (*it1 == EPS) {
                        flag = false;
                    }
                    First[X].insert(*it1);
                }
                if (flag) {
                    break;
                }
            }
        }
    }
}

void FOLLOW()
{
    while (1) {
        bool _continue = false;
        for (auto i : pro) {
            // cout << i.first << endl;
            for (auto it = i.second.begin(); it != i.second.end(); it++) {
                bool flag = true;
                for (int j = it->length() - 1; j >= 0; j--) {
                    if (it->at(j) == '\'') {
                        string tmp = it->substr(j - 1, 2);
                        if (flag) {
                            int size1 = Follow[tmp].size();
                            Follow[tmp].insert(Follow[i.first].begin(), Follow[i.first].end());
                            if (size1 < Follow[tmp].size()) {
                                _continue = true;
                            }
                            if (!pro[tmp].count(EPS)) {
                                flag = false;
                            }
                        }
                        for (int k = j + 1; k < it->length(); k++) {
                            if (isupper(it->at(k))) {
                                string s;
                                if (k != it->length() - 1 && it->at(k + 1) == '\'') {
                                    s = it->substr(k, 2);
                                }
                                else {
                                    s = it->at(k);
                                }
                                int size1 = Follow[tmp].size();
                                for (auto it1 = First[s].begin(); it1 != First[s].end(); it1++) {
                                    if (*it1 != EPS) {
                                        Follow[tmp].insert(*it1);
                                    }
                                }
                                if (size1 < Follow[tmp].size()) {
                                    _continue = true;
                                }
                                if (!pro[s].count(EPS)) {
                                    break;
                                }
                            }
                            else if (it->at(k) != '\'') {
                                int size1 = Follow[tmp].size();
                                Follow[tmp].insert("" + it->substr(k, 1));
                                if (size1 < Follow[tmp].size()) {
                                    _continue = true;
                                }
                                break;
                            }
                        }
                        j--;
                    }
                    else if (isupper(it->at(j))) {
                        string tmp = it->substr(j, 1);
                        if (flag) {
                            int size1 = Follow[tmp].size();
                            Follow[tmp].insert(Follow[i.first].begin(), Follow[i.first].end());
                            if (size1 < Follow[tmp].size()) {
                                _continue = true;
                            }
                            if (!pro[tmp].count(EPS)) {
                                flag = false;
                            }
                        }
                        for (int k = j + 1; k < it->length(); k++) {
                            if (isupper(it->at(k))) {
                                string s;
                                if (k != it->length() - 1 && it->at(k + 1) == '\'') {
                                    s = it->substr(k, 2);
                                }
                                else {
                                    s = it->at(k);
                                }
                                int size1 = Follow[tmp].size();
                                for (auto it1 = First[s].begin(); it1 != First[s].end(); it1++) {
                                    if (*it1 != EPS) {
                                        Follow[tmp].insert(*it1);
                                    }
                                }
                                if (size1 < Follow[tmp].size()) {
                                    _continue = true;
                                }
                                if (!pro[s].count(EPS)) {
                                    break;
                                }
                            }
                            else if (it->at(k) != '\'') {
                                int size1 = Follow[tmp].size();
                                Follow[tmp].insert("" + it->substr(k, 1));
                                if (size1 < Follow[tmp].size()) {
                                    _continue = true;
                                }
                                break;
                            }
                        }
                    }
                    else {
                        flag = false;
                    }
                }
            }
        }
        if (!_continue) {
            break;
        }
    }
    for (auto& i : Follow) {
        i.second.insert(DOLLAR);
    }
}

void PREDICT_TABLE()
{
    for (auto i : pro) {
        string A = i.first;
        for (auto j : i.second) {
            if (j == EPS) {
                for (auto k : Follow[A]) {
                    PredictTable[A][k] = j;
                }
            }
            else if (!isupper(j[0])) {
                PredictTable[A][j.substr(0, 1)] = j;
            }
            else if (isupper(j[0])) {
                string s;
                if (j.length() > 1 && j[1] == '\'') {
                    s = j.substr(0, 2);
                }
                else {
                    s = j[0];
                }
                for (auto k : First[s]) {
                    if (k == EPS) {
                        for (auto x : Follow[A]) {
                            PredictTable[A][x] = j;
                        }
                    }
                    else {
                        PredictTable[A][k] = j;
                    }
                }
            }
        }
    }
}


void error()
{
    puts("Syntax Error\n");
    return;
}

void printPath(bool _print)
{
    string output;
    for (auto i : matched) {
        output += i;
    }
    for (auto pit = path.rbegin(); pit != path.rend(); pit++) {
        output += *pit;
    }
    if (pre_path.empty() || pre_path != output) {
        report.push_back(output);
        pre_path = output;
    }
    if (_print) {
        for (auto i : report) {
            cout << i << endl;
        }
        cout << endl;
    }
    return;
}

void Parse(string buffer)
{
    s.push(DOLLAR);
    s.push("E");
    path.push_back("E");
    auto it = buffer.begin();
    while (s.top() != DOLLAR) {
        printPath(0);
        string X = s.top();
        string a;
        string tmp_id;
        if (isdigit(*it)) {
            a = ID;
            tmp_id = *it;
        }
        else if (it == buffer.end()) {
            a = DOLLAR;
        }
        else {
            a = *it;
        }
        if (X == a) {
            s.pop();
            path.pop_back();
            matched.push_back(tmp_id == "" ? a : tmp_id);
            it++;
        }
        else if (!isupper(X[0])) {
            error();
            return;
        }
        else if (PredictTable[X].find(a) == PredictTable[X].end()) {
            error();
            return;
        }
        else {
            string& tmp = PredictTable[X][a];
            s.pop();
            path.pop_back();
            if (tmp == EPS) {
                continue;
            }
            if (tmp == ID) {
                s.push(ID);
                path.push_back(tmp_id);
                continue;
            }
            for (int j = tmp.length() - 1; j >= 0; j--) {
                if (tmp[j] == '\'') {
                    s.push(tmp.substr(j - 1, 2));
                    path.push_back(tmp.substr(j - 1, 2));
                    j--;
                }
                else {
                    s.push(tmp.substr(j, 1));
                    path.push_back(tmp.substr(j, 1));
                }
            }
        }
    }
    if (it != buffer.end()) {
        error();
        return;
    }
    printPath(1);
}

void init()
{
    pro.clear();
    First.clear();
    Follow.clear();
    PredictTable.clear();
    built.clear();
    path.clear();
    matched.clear();
    report.clear();
    pre_path.clear();
    s = stack<string>();
    recover();
    for (auto i : pro) {
        FIRST(i.first);
    }
    FOLLOW();
    PREDICT_TABLE();
}


int main(int argc, char** argv)
{
    if (argc == 1) {
        cout << "\n=====================================================" << endl;
        cout << "SyntaxParser:" << endl;
        cout << "Working under [Non-Parse] mode." << endl;
        cout << "Only to Generate FIRST, FOLLOW set and PredictTable." << endl;
        cout << "NOTE: Use `./SyntaxParser parse` to parse sentence." << endl;
        cout << "Copyright @ Karl - karlszp.github.io" << endl;
        cout << "=====================================================\n" << endl;
        _init();
        init();
        printAll();
    }
    else if (argc == 2 && !strcmp(argv[1], "parse")) {
        cout << "\n=====================================================" << endl;
        cout << "SyntaxParser:" << endl;
        cout << "Working under [Parse] mode." << endl;
        cout << "Generate FIRST, FOLLOW set and PredictTable," << endl;
        cout << "and Parse user's inputs." << endl;
        cout << "NOTE: Use `./SyntaxParser` to generate sets and table only." << endl;
        cout << "Copyright @ Karl - karlszp.github.io" << endl;
        cout << "=====================================================\n" << endl;
        _init();
        init();
        printAll();
        cout << "Parse sentence(Stop parsing with a single '#'.)";
        cout << "Note: whitespaces separate input sentences.\n>>";
        string buf;
        while (cin >> buf && buf != "#") {
            init();
            Parse(buf);
            cout << "\nParse sentence(Stop parsing with a single '#'.)\n>> ";
        }
    }
    else {
        cout << "Please ONLY USE `./SyntaxParser` or `./SyntaxParser parse`!" << endl;
        exit(0);
    }
    return 0;
}


/* Debug */

void PrintTable()
{
    for (auto i : PredictTable) {
        for (auto j : i.second) {
            cout << left << setw(2) << i.first << "- " << j.first << ": ";
            cout << left << setw(2) << i.first << "-> " << j.second << endl;
        }
    }
    return;
}


void printAll()
{
    cout << "======FIRST=======" << endl;
    for (auto i : First) {
        cout << left << setw(2) << i.first << ": {";
        for (auto j : i.second) {
            cout << " " << j ;
        }
        cout << " }" << endl;
    }
    cout << "======FOLLOW======" << endl;
    for (auto i : Follow) {
        cout << left << setw(2) << i.first << ": {";
        for (auto j : i.second) {
            cout << " " << j;
        }
        cout << " }" << endl;
    }
    cout << "==PREDICT==TABLE==" << endl;
    PrintTable();
    cout << "==================" << endl;
}

// E:TE'
// E':+TE'
// E':#
// T:FT'
// T':*FT'
// T':#
// F:(E)
// F:0
// #