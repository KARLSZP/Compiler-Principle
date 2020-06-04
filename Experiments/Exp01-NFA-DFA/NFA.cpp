#include <iostream>
#include <bits/stdc++.h>
using namespace std;

set<int> epsilon_closure(vector<vector<set<int>>>& adj, set<int> T)
{
    set<int> res(T);
    stack<int> stk;
    for (int t : T) {
        stk.push(t);
    }
    while (!stk.empty()) {
        int cur_s = stk.top();
        stk.pop();
        for (auto u : adj[cur_s][0]) {
            if (res.find(u) == res.end()) {
                res.insert(u);
                stk.push(u);
            }
        }
    }
    return res;
}

set<int> move(vector<vector<set<int>>>& adj, set<int> S, int c)
{
    set<int> res;
    for (int s : S) {
        for (int next : adj[s][c + 1]) {
            res.insert(next);
        }
    }
    return res;
}

bool NFA(vector<vector<set<int>>>& adj, set<int>& terminal, string tar)
{
    set<int> s = epsilon_closure(adj, {0});
    int pos = 0;
    int c = tar[pos] - 'a';
    while (pos < tar.length()) {
        s = epsilon_closure(adj, move(adj, s, c));
        c = tar[++pos] - 'a';
    }
    // cout << tar << " End at " << s << endl;
    for (int t : terminal) if (s.find(t) != s.end()) {
            return true;
        }
    return false;
}


int main()
{
    int N, M, tmp;
    while (cin >> N >> M && N && M) {
        vector<vector<set<int>>> adj(N, vector<set<int>>(M, set<int>()));
        vector<set<int>> epsilon_closures;
        set<int> terminal;
        string inp;
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < M; j++) {
                cin >> inp;
                // cout << inp << endl;
                for (int k = 0; k < inp.length(); k++) {
                    if (!isdigit(inp[k])) {
                        continue;
                    }
                    int s = 0;
                    while (isdigit(inp[k])) {
                        s = s * 10 + (inp[k] - '0');
                        k++;
                    }
                    // cout << "sum: " << s << endl;
                    adj[i][j].insert(s);
                }
            }
        }
        while (cin >> tmp && tmp != -1) {
            terminal.insert(tmp);
        }

        vector<string> targets;
        string target;
        while (cin >> target && target != "#") {
            targets.push_back(target);
        }

        for (string s : targets) {
            cout << (NFA(adj, terminal, s) ? "YES" : "NO") << endl;
        }
    }
    return 0;
}