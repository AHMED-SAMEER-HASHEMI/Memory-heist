#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <climits>
#include <algorithm>

using namespace std;

// =============================================================================
// DAA CONCEPTS USED:
// 1. DIJKSTRA'S (Greedy) - Finding the lowest-risk path.
// 2. MINIMAX (Backtracking) - AI predicting and blocking player moves.
// 3. KMP (String Matching) - Efficiently detecting repetitive playstyles.
// =============================================================================

enum NodeType { SERVER, FIREWALL, VAULT, SCANNER };

struct Node {
    int id;
    NodeType type;
    int securityLevel;

    Node(int _id, NodeType _type, int _sl) 
        : id(_id), type(_type), securityLevel(_sl) {}
};

// --- 1. KMP ALGORITHM (Pattern Detection) ---
vector<int> computeLPS(string pat) {
    int m = pat.length();
    vector<int> lps(m);
    int len = 0; lps[0] = 0;
    int i = 1;
    while (i < m) {
        if (pat[i] == pat[len]) { len++; lps[i] = len; i++; }
        else {
            if (len != 0) len = lps[len - 1];
            else { lps[i] = 0; i++; }
        }
    }
    return lps;
}

bool kmpSearch(string txt, string pat) {
    int n = txt.length(), m = pat.length();
    if (m == 0) return false;
    vector<int> lps = computeLPS(pat);
    int i = 0, j = 0;
    while (i < n) {
        if (pat[j] == txt[i]) { i++; j++; }
        if (j == m) return true;
        else if (i < n && pat[j] != txt[i]) {
            if (j != 0) j = lps[j - 1];
            else i++;
        }
    }
    return false;
}

// --- 2. DIJKSTRA'S ALGORITHM (Pathfinding) ---
struct PathNode {
    int id, dist;
    bool operator>(const PathNode& other) const { return dist > other.dist; }
};

vector<int> runDijkstra(int start, int end, int n, const vector<vector<pair<int, int> > >& adj) {
    vector<int> dist(n, INT_MAX);
    vector<int> parent(n, -1);
    priority_queue<PathNode, vector<PathNode>, greater<PathNode> > pq;

    dist[start] = 0;
    PathNode first = {start, 0};
    pq.push(first);

    while (!pq.empty()) {
        int u = pq.top().id;
        int d = pq.top().dist;
        pq.pop();
        if (d > dist[u]) continue;
        for (size_t i = 0; i < adj[u].size(); i++) {
            int v = adj[u][i].first;
            int weight = adj[u][i].second;
            if (dist[u] + weight < dist[v]) {
                dist[v] = dist[u] + weight;
                parent[v] = u;
                PathNode next = {v, dist[v]};
                pq.push(next);
            }
        }
    }
    vector<int> path;
    for (int v = end; v != -1; v = parent[v]) path.insert(path.begin(), v);
    return path;
}

// --- 3. MINIMAX ALGORITHM (AI Defense) ---
int minimax(int node, int depth, bool isMax, int target, const vector<Node>& nodes, const vector<vector<pair<int, int> > >& adj) {
    if (depth == 0 || node == target) return nodes[node].securityLevel;
    if (isMax) {
        int best = -1000;
        for (size_t i = 0; i < adj[node].size(); i++) 
            best = max(best, minimax(adj[node][i].first, depth - 1, false, target, nodes, adj));
        return best;
    } else {
        int best = 1000;
        for (size_t i = 0; i < adj[node].size(); i++) 
            best = min(best, minimax(adj[node][i].first, depth - 1, true, target, nodes, adj));
        return best;
    }
}

// --- GAME ENGINE ---
class GameEngine {
public:
    int playerPos, targetNode, detection;
    string history;
    vector<Node> nodes;
    vector<vector<pair<int, int> > > adj;

    GameEngine() : playerPos(0), targetNode(9), detection(0), history("") {
        adj.resize(10);
        for (int i = 0; i < 10; i++) nodes.push_back(Node(i, SERVER, 10));
        nodes[5].type = FIREWALL; nodes[5].securityLevel = 30;
        nodes[9].type = VAULT;    nodes[9].securityLevel = 50;
        setupEdges();
    }

    void setupEdges() {
        autoAddE(0, 1, 5);  autoAddE(0, 2, 8);
        autoAddE(1, 3, 12); autoAddE(2, 4, 10);
        autoAddE(3, 5, 15); autoAddE(4, 5, 10);
        autoAddE(5, 6, 20); autoAddE(6, 7, 10);
        autoAddE(7, 8, 15); autoAddE(8, 9, 30);
    }

    void autoAddE(int u, int v, int w) {
        adj[u].push_back(make_pair(v, w));
        adj[v].push_back(make_pair(u, w));
    }

    void updateWeight(int u, int v, int w) {
        for (size_t i = 0; i < adj[u].size(); i++) if (adj[u][i].first == v) adj[u][i].second = w;
        for (size_t i = 0; i < adj[v].size(); i++) if (adj[v][i].first == u) adj[v][i].second = w;
    }

    void showTutorial() {
        cout << "=====================================================" << endl;
        cout << "          MISSION BRIEFING: MEMORY HEIST             " << endl;
        cout << "=====================================================" << endl;
        cout << " GOAL: Reach Node 9 (The Vault) before Detection hits 100%\n" << endl;
        cout << " ACTIONS:\n (H)ack: Fast progress, +20% Detection.\n (S)tealth: Slow, +5% Detection." << endl;
        cout << " SYSTEM:\n - Dijkstra: Calculates the safest route for you.\n - KMP: Monitors if you spam the Hack action.\n - Minimax: The AI predicts your move and shields nodes." << endl;
        cout << "=====================================================\n" << endl;
    }

    void printStats() {
        cout << "\n[NODE: " << playerPos << " | DETECTION: " << detection << "%]" << endl;
        vector<int> p = runDijkstra(playerPos, targetNode, 10, adj);
        cout << ">> DIJKSTRA SCAN (Safest Path): ";
        for (size_t i = 0; i < p.size(); i++) cout << p[i] << (i == p.size()-1 ? "" : " -> ");
        cout << "\n>> Neighbors: ";
        for (size_t i = 0; i < adj[playerPos].size(); i++) 
            cout << adj[playerPos][i].first << "(Risk:" << adj[playerPos][i].second << ") ";
        cout << endl;
    }

    void run() {
        showTutorial();
        while (detection < 100 && playerPos != targetNode) {
            printStats();
            cout << "Action (H/S) & Move To ID: ";
            char cmd; int next; cin >> cmd >> next;
            cmd = toupper(cmd); history += cmd;

            if (kmpSearch(history, "HHH")) {
                cout << " [!] KMP Pattern Detected: Rapid Hacking. Security Hardened!" << endl;
                detection += 20;
            }

            int bestMove = -1; int maxS = -1000;
            for (size_t i = 0; i < adj[next].size(); i++) {
                int s = minimax(adj[next][i].first, 2, true, targetNode, nodes, adj);
                if (s > maxS) { maxS = s; bestMove = adj[next][i].first; }
            }
            if (bestMove != -1) {
                nodes[bestMove].securityLevel += 20;
                updateWeight(next, bestMove, nodes[bestMove].securityLevel);
                cout << " [!] AI Minimax predicted move. Shielded Node " << bestMove << endl;
            }

            playerPos = next;
            detection += (cmd == 'H' ? 15 : 5);
        }
        
        if (playerPos == targetNode) cout << "\n>>> MISSION SUCCESS: VAULT BYPASSED <<<\n";
        else cout << "\n>>> MISSION FAILED: TRACED BY AI <<<\n";

        printFinalAnalysis();
    }

    void printFinalAnalysis() {
        cout << "\n--- DAA ALGORITHM COMPLEXITY ANALYSIS ---" << endl;
        cout << "1. Dijkstra: O(E log V) - Used for Safest Path." << endl;
        cout << "2. Minimax:  O(b^d)     - Used for Adversarial Prediction." << endl;
        cout << "3. KMP:      O(N + M)   - Used for Linear Pattern Matching." << endl;
        cout << "-----------------------------------------" << endl;
    }
};

int main() {
    GameEngine game;
    game.run();
    cout << "\nPress any key to exit...";
    cin.ignore(); cin.get();
    return 0;
}
