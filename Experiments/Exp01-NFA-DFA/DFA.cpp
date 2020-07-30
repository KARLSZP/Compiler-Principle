#include <iostream>
#include <bits/stdc++.h>
using namespace std;


bool DFA(vector<vector<int>>& adj, set<int>& terminal, string tar)
{
    int s = 0, pos = 0;
    int c = tar[pos] - 'a';
    while (pos < tar.length()) {
        // cout << s << " " << c << endl;
        s = adj[s][c];
        c = tar[++pos] - 'a';
    }
    // cout << tar << " End at " << s << endl;
    if (terminal.find(s) != terminal.end()) {
        return true;
    }
    return false;
}

int main()
{
    int N, M, tmp;
    while (cin >> N >> M && N && M) {
        vector<vector<int>> adj(N, vector<int>(M, 0));
        set<int> terminal;
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < M; j++) {
                cin >> adj[i][j];
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
            cout << (DFA(adj, terminal, s) ? "YES" : "NO") << endl;
        }
    }
    return 0;
}

/* Description
对于给出的DFA和输入的字符串，判断字符串是否是DFA识别的语言。

Input
输入有多组数据。每组数据的第一行是两个整数N（N<=50）和M（M<=26），分别代表DFA的状态数和字母表的字符数。
DFA的N个状态用整数0～N-1表示。状态0为起始状态。字母表包含的字符是小写英文字母的前M个字符。
接下来的N行，每行有M个整数。其中第i行第j列的数字k，表示DFA在状态i-1，当输入符号为第j个小写字母时，迁移到状态k。
接下来的一行包含若干个整数，代表DFA的接受状态，这一行以-1结尾。接下来的每一行是一个待识别的字符串，
字符串的长度在1到50之间且只含有小写字母。字符串"#"代表本组数据结束。N=M=0表示输入结束。

Output
对于每个待识别的字符串，如果能被给出的DFA识别，输出YES；否则输出NO。

Sample Input
4 2
1 0
1 2
1 3
1 0
3 -1
aaabb
abbab
abbaaabb
abbb
#
1 3
0 0 0
0 -1
cacba
#
0 0
Sample Output
YES
NO
YES
NO
YES
*/
