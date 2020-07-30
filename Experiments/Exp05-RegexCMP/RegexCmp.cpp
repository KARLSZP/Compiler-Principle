#include <iostream>
#include <bits/stdc++.h>

using namespace std;

// Definitions
#define EPS "[EPS]"
static int idx = 0;
static char state = 'A';

unordered_map<char, int> sym_list = {
    {'+', 0}, {'?', 1 }, {'*', 2}
};

unordered_map<char, int> priority = {
    {'*', 3}, {'@', 2}, {'|', 1}
};

struct Edge;
struct Node;
struct NFA;
struct DtranElem;

vector<string> pattern_list;
vector<vector<string>> NFAmap;
unordered_map<int, Node*> nodes;
vector<DtranElem*> Dtrans;
vector<vector<char>> miniDtrans;

/**
 * (Number of DFA state * (DFA state, [pattern1, ..., ACC]))
 * Sample: (DFA state, [pattern1, ..., ACC])
 *         (A, [B, C, \, D, N(No)])
 */
vector<pair<char, vector<char>>> minimizedDtrans;


/**
 * Init all global vars.
 */
void init()
{
    pattern_list.clear();
    NFAmap.clear();
    nodes.clear();
    Dtrans.clear();
    miniDtrans.clear();
    minimizedDtrans.clear();
    idx = 0;
    state = 'A';
}

struct Node {
    int id;
    unordered_map<string, vector<Edge*>> in;
    unordered_map<string, vector<Edge*>> out;
    Node(int id): id(id) {}
};

struct Edge {
    Node* from;
    Node* to;
    string by;
    Edge(Node* from, Node* to, string by): from(from), to(to), by(by) {}
};


struct NFA {
    Node* start;
    Node* end;
    NFA(Node* start, Node* end): start(start), end(end) {}
};

struct DtranElem {
    set<Node*> nfa_set;
    char dfa_state;
    vector<DtranElem*> to;
    DtranElem(set<Node*> nfa_set, char dfa_state, vector<DtranElem*> to):
        nfa_set(nfa_set), dfa_state(dfa_state), to(to) {}
    DtranElem(set<Node*> nfa_set, char dfa_state):
        nfa_set(nfa_set), dfa_state(dfa_state) {}
};


/**
 * Parse and get Pattern List.
 * e.g.: exp = a+bc
 *  ==> pattern list = {a, b, c}
 */
void getPatternList(string exp)
{
    vector<int> existed(26, 0);
    for (auto i : exp) {
        if (islower(i) && !existed[i - 'a']) {
            pattern_list.push_back(string(1, i));
            existed[i - 'a'] = 1;
        }
    }
    sort(pattern_list.begin(), pattern_list.end());
}

/**
 * Add catenation symbol `@`
 * to correct place.
 */
string AddCatSymbol(string exp)
{
    int i = 0, j, n = exp.length();
    while (i < n) {
        if (((exp[i] != '(' && exp[i] != '@' && exp[i] != '|')
             || exp[i] == ')'
             || exp[i] == '*')
            && (islower(exp[i + 1]))) {
            exp += exp[n - 1];
            for (j = n - 1; j > i + 1; j--) {
                exp[j] = exp[j - 1];
            }
            exp[i + 1] = '@';
            n++;
            i++;
        }
        i++;
    }
    return exp;
}

/**
 * Transfer expression `exp`
 * to postfix type.
 */
string RegExpToPost(string exp)
{
    exp = AddCatSymbol(exp);
    int i = 0, j = 0, n = exp.length();
    char ch, cl;
    string postfix(n, ' ');
    stack<char> s;
    s.push('#');
    ch = exp[i];
    while (i < n) {
        if (ch == '(') {
            s.push(ch);
            ch = exp[++i];
        }
        else if (ch == ')') {
            while (s.top() != '(') {
                postfix[j++] = s.top();
                s.pop();
            }
            s.pop();
            ch = exp[++i];
        }
        else if ((ch == '|') || (ch == '*') || (ch == '@')) {
            cl = s.top();
            while (priority[cl] >= priority[ch]) {
                postfix[j++] = cl;
                s.pop();
                cl = s.top();
            }
            s.push(ch);
            ch = exp[++i];
        }
        else {
            postfix[j++] = ch;
            ch = exp[++i];
        }
    }
    ch = s.top();
    s.pop();
    while ((ch == '|') || (ch == '*') || (ch == '@')) {
        postfix[j++] = ch;
        ch = s.top();
        s.pop();
    }
    while (postfix.back() == ' ') {
        postfix.pop_back();
    }
    return postfix;
}


// NFA Section
NFA* ModifyNFAWithSymbol(NFA* nfa, char sym)
{
    // {'+', 0}, {'?', 1 }, {'*', 2}
    int sym_int = sym_list[sym];
    switch (sym_int) {
    case 0: {
        /**
         * NFA_start --> NFA_end
         * ==>
         *                       |------EPS------
         *                       v              |
         * new_start --EPS--> NFA_start --> NFA_end --EPS--> new_end
         */
        Node* new_start = new Node(idx++);
        Node* new_end = new Node(idx++);
        Edge* new_edge1 = new Edge(new_start, nfa->start, EPS);
        Edge* new_edge2 = new Edge(nfa->end, new_end, EPS);
        Edge* new_edge3 = new Edge(nfa->end, nfa->start, EPS);
        // new start edge
        new_start->out[new_edge1->by].push_back(new_edge1);
        nfa->start->in[new_edge1->by].push_back(new_edge1);
        // new end edge
        new_end->in[new_edge2->by].push_back(new_edge2);
        nfa->end->out[new_edge2->by].push_back(new_edge2);
        // symbol edge
        nfa->end->out[new_edge3->by].push_back(new_edge3);
        nfa->start->in[new_edge3->by].push_back(new_edge3);

        nfa->start = new_start;
        nfa->end = new_end;
        break;
    }

    case 1: {
        /**
         * NFA_start --> NFA_end
         * ==>
         *       ----------------------EPS---------------------|
         *       |                                             v
         * new_start --EPS--> NFA_start --> NFA_end --EPS--> new_end
         */
        Node* new_start = new Node(idx++);
        Node* new_end = new Node(idx++);
        Edge* new_edge1 = new Edge(new_start, nfa->start, EPS);
        Edge* new_edge2 = new Edge(nfa->end, new_end, EPS);
        Edge* new_edge3 = new Edge(new_start, new_end, EPS);
        // new start edge
        new_start->out[new_edge1->by].push_back(new_edge1);
        nfa->start->in[new_edge1->by].push_back(new_edge1);
        // new end edge
        new_end->in[new_edge2->by].push_back(new_edge2);
        nfa->end->out[new_edge2->by].push_back(new_edge2);
        // symbol edge
        new_start->out[new_edge3->by].push_back(new_edge3);
        new_end->in[new_edge3->by].push_back(new_edge3);
        nfa->start = new_start;
        nfa->end = new_end;
        break;
    }

    case 2: {
        /**
         * NFA_start --> NFA_end
         * ==>
         *       ----------------------EPS---------------------|
         *       |                                             v
         * new_start --EPS--> NFA_start --> NFA_end --EPS--> new_end
         *                       ^              |
         *                       |------EPS------
         */
        Node* new_start = new Node(idx++);
        Node* new_end = new Node(idx++);
        Edge* new_edge1 = new Edge(new_start, nfa->start, EPS);
        Edge* new_edge2 = new Edge(nfa->end, new_end, EPS);
        Edge* new_edge3 = new Edge(new_start, new_end, EPS);
        Edge* new_edge4 = new Edge(nfa->end, nfa->start, EPS);
        // new start edge
        new_start->out[new_edge1->by].push_back(new_edge1);
        nfa->start->in[new_edge1->by].push_back(new_edge1);
        // new end edge
        new_end->in[new_edge2->by].push_back(new_edge2);
        nfa->end->out[new_edge2->by].push_back(new_edge2);
        // symbol edge1
        new_start->out[new_edge3->by].push_back(new_edge3);
        new_end->in[new_edge3->by].push_back(new_edge3);
        // symbol edge2
        nfa->end->out[new_edge4->by].push_back(new_edge4);
        nfa->start->in[new_edge4->by].push_back(new_edge4);
        nfa->start = new_start;
        nfa->end = new_end;
        break;
    }
    default:
        cout << "Invalid Sample." << endl;
        exit(0);
        break;
    }

    return nfa;
}

/**
 * NFA `and` operation
 *
 * NFA1_start --> NFA1_end `and` NFA2_start --> NFA2_end
 * ==>
 *
 * NFA1_start --> NFA1_end --EPS--> NFA2_start --> NFA2_end
 *
 * # Note: nfa1, nfa2 will be freed.
 */
NFA* NFAand(NFA* nfa1, NFA* nfa2)
{
    Edge* new_edge = new Edge(nfa1->end, nfa2->start, EPS);
    nfa1->end->out[new_edge->by].push_back(new_edge);
    nfa2->start->in[new_edge->by].push_back(new_edge);
    NFA* new_nfa = new NFA(nfa1->start, nfa2->end);
    free(nfa1);
    free(nfa2);
    return new_nfa;
}

/**
 * NFA `or` operation
 *
 * NFA1_start --> NFA1_end `and` NFA2_start --> NFA2_end
 * ==>
 *
 *               NFA1_start --> NFA1_end
 *             /                        \
 *           EPS                        EPS
 *           /                            \
 * new_start                              new_end
 *           \                            /
 *           EPS                        EPS
 *             \                        /
 *               NFA2_start --> NFA2_end
 *
 * # Note: nfa1, nfa2 will be freed.
 */
NFA* NFAor(NFA* nfa1, NFA* nfa2)
{
    Node* new_start = new Node(idx++);
    Node* new_end = new Node(idx++);
    Edge* new_edge1 = new Edge(new_start, nfa1->start, EPS);
    Edge* new_edge2 = new Edge(new_start, nfa2->start, EPS);
    Edge* new_edge3 = new Edge(nfa1->end, new_end, EPS);
    Edge* new_edge4 = new Edge(nfa2->end, new_end, EPS);

    // new start edges
    new_start->out[new_edge1->by].push_back(new_edge1);
    nfa1->start->in[new_edge1->by].push_back(new_edge1);
    new_start->out[new_edge2->by].push_back(new_edge2);
    nfa2->start->in[new_edge2->by].push_back(new_edge2);

    // new end edges
    nfa1->end->out[new_edge3->by].push_back(new_edge3);
    new_end->in[new_edge3->by].push_back(new_edge3);
    nfa2->end->out[new_edge4->by].push_back(new_edge4);
    new_end->in[new_edge4->by].push_back(new_edge4);

    NFA* new_nfa = new NFA(new_start, new_end);
    free(nfa1);
    free(nfa2);
    return new_nfa;
}

/**
 * Build a simple NFA with `by`.
 */
NFA* buildBaseNFA(string by)
{
    by = by == "~" ? EPS : by;
    Node* start = new Node(idx++);
    Node* end = new Node(idx++);
    Edge* edge = new Edge(start, end, by);
    start->out[by].push_back(edge);
    end->in[by].push_back(edge);
    return new NFA(start, end);
}

/**
 * Build a map of NFA with
 * start node `nfa`.
 */
void buildNFAMap(NFA* nfa)
{
    NFAmap = vector<vector<string>>(idx, vector<string>(idx, "\0"));
    vector<int> visited = vector<int>(idx, 0);
    queue<Node*> q;
    q.push(nfa->start);
    while (!q.empty()) {
        Node* tmp = q.front();
        q.pop();
        visited[tmp->id] = 1;
        nodes[tmp->id] = tmp;
        for (auto item : tmp->out) {
            for (auto e : item.second) {
                NFAmap[e->from->id][e->to->id] = e->by;
                if (!visited[e->to->id]) {
                    q.push(e->to);
                }
            }
        }
    }
}


// NFA to DFA Section

/**
 * Build a closure from node
 * through `by`.
 */
set<Node*> Closure(Node* node, string by)
{
    set<Node*> s;
    stack<Node*> stk;
    s.insert(node);
    stk.push(node);
    while (!stk.empty()) {
        auto tmp = stk.top();
        stk.pop();
        for (int i = 0; i < idx; i++) {
            if (NFAmap[tmp->id][i] == by && s.find(nodes[i]) == s.end()) {
                s.insert(nodes[i]);
                stk.push(nodes[i]);
            }
        }
    }
    return s;
}

/**
 * Build a epsilon-closure of all nodes
 * from `node_set`.
 */
set<Node*> epsClosure(set<Node*> node_set)
{
    set<Node*> s(node_set.begin(), node_set.end());
    stack<Node*> stk;
    for (auto i : node_set) {
        stk.push(i);
    }
    while (!stk.empty()) {
        auto tmp = stk.top();
        stk.pop();
        for (int i = 0; i < idx; i++) {
            if (NFAmap[tmp->id][i] == EPS && s.find(nodes[i]) == s.end()) {
                s.insert(nodes[i]);
                stk.push(nodes[i]);
            }
        }
    }
    return s;
}

/**
 * Get a set of nodes, which is generated by:
 * Moving all nodes in `node_set` through `by`.
 */
set<Node*> move(set<Node*> node_set, string by)
{
    set<Node*> s;
    for (auto node : node_set) {
        for (int i = 0; i < idx; i++) {
            if (NFAmap[node->id][i] == by) {
                s.insert(nodes[i]);
            }
        }
    }
    return s;
}

/**
 * Set State for DFA.
 */
char getDFAState(set<Node*> s)
{
    if (s.empty()) {
        return '\0';
    }
    for (auto elem : Dtrans) {
        if (elem->nfa_set == s) {
            return elem->dfa_state;
        }
    }
    return state++;
}

/**
 * Build Dtrans from given nfa `init_nfa_set`.
 */
void buildDtrans(set<Node*> init_nfa_set)
{
    DtranElem* A = new DtranElem(init_nfa_set, getDFAState(init_nfa_set));
    queue<DtranElem*> s;
    s.push(A);
    while (!s.empty()) {
        auto tmp = s.front();
        s.pop();
        for (auto by : pattern_list) {
            auto new_nfa_set = epsClosure(move(tmp->nfa_set, by));
            auto new_dfa_state = getDFAState(new_nfa_set);
            if (new_dfa_state == '\0') {
                tmp->to.push_back(NULL);
            }
            else if (new_dfa_state <= tmp->dfa_state) {
                tmp->to.push_back(Dtrans[new_dfa_state - 'A']);
            }
            else {
                DtranElem* new_elem = new DtranElem(new_nfa_set, new_dfa_state);
                tmp->to.push_back(new_elem);
                s.push(new_elem);
            }
        }
        Dtrans.push_back(tmp);
    }
}

// DFA Minimization Section

/**
 * Build minimized Dtrans,
 * in `vector<vector<char>>` type.
 */
void buildMiniDtrans(int end_node_id)
{
    for (auto elem : Dtrans) {
        vector<char> tmp;
        for (auto i : elem->to) {
            tmp.push_back(i != NULL ? i->dfa_state : '\0');
        }
        tmp.push_back(
            elem->nfa_set.find(
                nodes[end_node_id]) != elem->nfa_set.end() ? 'Y' : 'N');
        miniDtrans.push_back(tmp);
    }
}

/**
 * For a partitions in DFA,
 * Find a group of DFA states which
 *  are accessible through `dfa_state`
 */
vector<int> getToGroup(vector<vector<char>> partitions, char dfa_state)
{
    vector<int> group;
    auto tmp = miniDtrans[int(dfa_state - 'A')];
    for (int i = 0; i < pattern_list.size(); i++) {
        if (tmp[i] == '\0') {
            group.push_back(-1);
        }
        bool flag = true;
        for (int j = 0; j < partitions.size(); j++) {
            for (int k = 0; k < partitions[j].size(); k++) {
                if (tmp[i] == partitions[j][k]) {
                    group.push_back(j);
                    flag = false;
                    break;
                }
            }
            if (!flag) {
                break;
            }
        }
    }
    return group;
}

/**
 * Do minimization.
 */
void minimizeDFA()
{
    // init
    vector<vector<char>> partitions(2, vector<char>());
    for (int i = 0; i < miniDtrans.size(); i++) {
        if (miniDtrans[i].back() == 'N') {
            partitions[0].push_back(char(i + 'A'));
        }
        else {
            partitions[1].push_back(char(i + 'A'));
        }
    }

    // Keep trying to split
    // until no change take place.
    while (1) {
        bool nochange = true;

        // for each partitions, partialize it
        int len = partitions.size();
        for (int i = 0; i < len; i++) {
            // single-content partition can never split
            if (partitions[i].size() <= 1) {
                continue;
            }

            vector<vector<char>> new_partitions;
            vector<bool> visited(partitions[i].size(), false);
            for (int j = 0; j < partitions[i].size(); j++) {
                if (visited[j]) {
                    continue;
                }
                vector<char> new_partition(1, partitions[i][j]);
                vector<int> group = getToGroup(partitions, partitions[i][j]);
                for (int k = j + 1; k < partitions[i].size(); k++) {
                    if (!visited[k]) {
                        auto tmp = getToGroup(partitions, partitions[i][k]);
                        if (tmp == group) {
                            new_partition.push_back(partitions[i][k]);
                            visited[k] = true;
                        }
                    }
                }
                new_partitions.push_back(new_partition);
            }
            if (new_partitions.size() > 1 || partitions[i] != new_partitions[0]) {
                nochange = false;
            }

            partitions[i].resize(new_partitions[0].size());
            copy(new_partitions[0].begin(), new_partitions[0].end(), partitions[i].begin());
            if (new_partitions.size() > 1) {
                partitions.insert(partitions.end(),
                                  new_partitions.begin() + 1,
                                  new_partitions.end());
            }
        }
        if (nochange) {
            break;
        }
    }

    vector<char> group_map(26, '~');
    for (auto p : partitions) {
        if (p.empty()) {
            continue;
        }
        for (int j = 1; j < p.size(); j++) {
            group_map[p[j] - 'A'] = p[0];
        }
        if (p.size() > 0) {
            group_map[p[0] - 'A'] = p[0];
        }
    }

    for (auto p : partitions) {
        if (p.empty()) {
            continue;
        }
        auto tmp = miniDtrans[p[0] - 'A'];
        for (int i = 0; i < tmp.size() - 1; i++) {
            tmp[i] = group_map[tmp[i] - 'A'];
        }
        minimizedDtrans.push_back(make_pair(p[0], tmp));
    }

    sort(minimizedDtrans.begin(), minimizedDtrans.end(), [](auto x, auto y) {
        return (x.second.back() == y.second.back()) ? x.first < y.first : x.second.back() < y.second.back();
    });

    for (int i = 0; i < minimizedDtrans.size(); i++) {
        if (minimizedDtrans[i].first != 'A' + i) {
            char from = minimizedDtrans[i].first, to = 'A' + i;
            for (auto& j : minimizedDtrans) {
                for (auto& k : j.second) {
                    k = (k == from) ? to : k;
                }
            }
            minimizedDtrans[i].first = to;
        }
    }

}

// === BEGIN Debug function ===

void printMap()
{
    cout << setw(5) << " ";
    for (int i = 0; i < idx; i++) {
        cout << setw(5) << i << " ";
    }
    cout << endl;
    for (int i = 0; i < NFAmap.size(); i++) {
        cout << setw(5) << i << " ";
        for (auto j : NFAmap[i]) {
            cout << setw(5) << j << " ";
        }
        cout << endl;
    }
}

void printDtrans()
{
    cout << endl << "DFA ";
    for (auto j : pattern_list) {
        cout << setw(4) << j << " ";
    }
    cout << endl;
    for (auto i : Dtrans) {
        cout << setw(3) << i->dfa_state << " ";

        for (auto j : i->to) {
            cout << setw(4) << (j != NULL ? j->dfa_state : ' ') << " ";
        }

        cout << "  { ";

        for (auto j : i->nfa_set) {
            cout << j->id << " ";
        }
        cout << "}" << endl;
    }
}

void printMiniDtrans()
{
    cout << endl << "DFA ";
    for (auto j : pattern_list) {
        cout << setw(4) << j << " ";
    }
    cout << setw(4) << "ACC" << endl;
    for (int i = 0; i < miniDtrans.size(); i++) {
        cout << setw(3) << char('A' + i) << " ";

        for (auto j : miniDtrans[i]) {
            cout << setw(4) << (j != '\0' ? j : ' ') << " ";
        }

        cout << endl;
    }
}

void printMinimizedDtrans()
{
    cout << endl << "DFA ";
    for (auto j : pattern_list) {
        cout << setw(4) << j << " ";
    }
    cout << setw(4) << "ACC" << endl;
    for (auto i : minimizedDtrans) {
        cout << setw(3) << i.first << " ";
        for (auto j : i.second) {
            cout << setw(4) << (j != '\0' ? j : ' ') << " ";
        }
        cout << endl;
    }
}

// === END Debug function ===


// Search for relationship
bool dfs(vector<pair<char, vector<char>>> dfa1,
         vector<pair<char, vector<char>>> dfa2,
         int l, int r, vector<vector<bool>>& vis)
{
    vis[l][r] = 1;
    if ((dfa1[l].second.back() == 'Y' &&  dfa2[r].second.back() == 'N') || \
        (dfa1[l].second.back() == 'N' &&  dfa2[r].second.back() == 'Y')) {
        return true;
    }
    for (int i = 0; i < dfa1[0].second.size() - 1; i++) {
        int nl = dfa1[l].second[i] - 'A';
        int nr = dfa2[r].second[i] - 'A';
        if (nl == -65 && nr == -65) {
            continue;
        }
        else if (nl == -65 || nr == -65) {
            return false;
        }
        if (!vis[nl][nr] && dfs(dfa1, dfa2, nl, nr, vis)) {
            return true;
        }
    }
    return false;
}

bool contain(vector<pair<char, vector<char>>> dfa1,
             vector<pair<char, vector<char>>> dfa2)
{
    vector<vector<int>> vis(dfa1.size(), vector<int>(dfa2.size(), 0));
    vector<pair<int, int>> q;
    vis[0][0] = 1;
    q.push_back({0, 0});
    while (!q.empty()) {
        int l = q.back().first, r = q.back().second;
        q.pop_back();
        if (dfa1[l].second.back() == 'N' && dfa2[r].second.back() == 'Y') {
            return 0;
        }
        for (int i = 0; i < dfa1[l].second.size() - 1; i++) {
            int nl = dfa1[l].second[i] - 'A',
                nr = dfa2[r].second[i] - 'A';
            if (nl < 0 && nr >= 0) {
                return 0;
            }
            if (nl >= 0 && nr >= 0 && !vis[nl][nr]) {
                vis[nl][nr] = 1;
                q.push_back({nl, nr});
            }
        }
    }
    return 1;
}



int main(int argc, char const* argv[])
{
    int k = 0;
    cin >> k;
    while (k--) {
        string exp1, exp2;
        cin >> exp1 >> exp2;
        for (auto& i : exp1) {
            if (i == 'E') {
                i = '~';
            }
        }
        for (auto& i : exp2) {
            if (i == 'E') {
                i = '~';
            }
        }

        // First Case
        init();
        stack<NFA*> s;
        string exp = exp1;
        getPatternList(exp);
        exp = RegExpToPost(exp);

        int i = 0, n = exp.length();
        while (i < n) {
            if (islower(exp[i]) || exp[i] == '~') {
                s.push(buildBaseNFA(exp.substr(i, 1)));
            }
            else {
                switch (exp[i]) {
                case '|': {
                    NFA* nfa1 = s.top();
                    s.pop();
                    NFA* nfa2 = s.top();
                    s.pop();
                    s.push(NFAor(nfa1, nfa2));
                    break;
                }
                case '@': {
                    NFA* nfa1 = s.top();
                    s.pop();
                    NFA* nfa2 = s.top();
                    s.pop();
                    s.push(NFAand(nfa2, nfa1));
                    break;
                }
                default:
                    NFA* nfa = s.top();
                    s.pop();
                    s.push(ModifyNFAWithSymbol(nfa, exp[i]));
                    break;
                }
            }
            i++;
        }

        while (s.size() > 1) {
            NFA* nfa1 = s.top();
            s.pop();
            NFA* nfa2 = s.top();
            s.pop();
            s.push(NFAand(nfa2, nfa1));
        }

        NFA* final_nfa = s.top();

        buildNFAMap(final_nfa);
        auto nfa_A = Closure(final_nfa->start, EPS);
        buildDtrans(nfa_A);
        buildMiniDtrans(final_nfa->end->id);
        minimizeDFA();
        // printMinimizedDtrans();
        auto dfa1 = minimizedDtrans;

        //==========================================//
        // Second Case
        init();
        s = stack<NFA*>();
        exp = exp2;
        getPatternList(exp);
        exp = RegExpToPost(exp);

        i = 0, n = exp.length();
        while (i < n) {
            if (islower(exp[i]) || exp[i] == '~') {
                s.push(buildBaseNFA(exp.substr(i, 1)));
            }
            else {
                switch (exp[i]) {
                case '|': {
                    NFA* nfa1 = s.top();
                    s.pop();
                    NFA* nfa2 = s.top();
                    s.pop();
                    s.push(NFAor(nfa1, nfa2));
                    break;
                }
                case '@': {
                    NFA* nfa1 = s.top();
                    s.pop();
                    NFA* nfa2 = s.top();
                    s.pop();
                    s.push(NFAand(nfa2, nfa1));
                    break;
                }
                default:
                    NFA* nfa = s.top();
                    s.pop();
                    s.push(ModifyNFAWithSymbol(nfa, exp[i]));
                    break;
                }
            }
            i++;
        }

        while (s.size() > 1) {
            NFA* nfa1 = s.top();
            s.pop();
            NFA* nfa2 = s.top();
            s.pop();
            s.push(NFAand(nfa2, nfa1));
        }

        final_nfa = s.top();

        buildNFAMap(final_nfa);
        nfa_A = Closure(final_nfa->start, EPS);
        buildDtrans(nfa_A);
        buildMiniDtrans(final_nfa->end->id);
        minimizeDFA();
        // printMinimizedDtrans();
        auto dfa2 = minimizedDtrans;

        //==========================================//
        // Check Relationship
        vector<vector<bool>> vis(dfa1.size(), vector<bool>(dfa2.size(), false));
        if (!dfs(dfa1, dfa2, 0, 0, vis)) {
            auto flag1 = contain(dfa1, dfa2);
            auto flag2 = contain(dfa2, dfa1);
            if (flag1 && flag2) {
                cout << "=" << endl;
            }
            else if (flag1) {
                cout << ">" << endl;
            }
            else {
                cout << "<" << endl;
            }
        }
        else {
            auto flag1 = contain(dfa1, dfa2);
            auto flag2 = contain(dfa2, dfa1);
            if (flag1 && flag2) {
                cout << "=" << endl;
            }
            else if (flag1) {
                cout << ">" << endl;
            }
            else if (flag2) {
                cout << "<" << endl;
            }
            else {
                cout << "!" << endl;
            }
        }
    }
    return 0;
}
