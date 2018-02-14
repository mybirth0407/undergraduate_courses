#include <iostream>
#include <string>
#include <set>
#include <map>
#include <queue>
#include <cstring>
#include <algorithm>
#include <vector>
#include <cstdio>
#include <unordered_set>

#define toNumber(c) c - 97
#define ALPHABETS 26

using namespace std;

struct info {
    int start;
    int end;
    int id;
    string str;
    
    // priority when into map.
    bool operator<(const info& other) const {
        if (start != other.start) {
            return start < other.start;
        }
        return end < other.end;
    };
};

struct TrieNode {
    TrieNode* children[ALPHABETS] = {0, };
    bool flags[ALPHABETS] = {0, };
    int string_id;

    // fail link for AC
    TrieNode* fail;
    // this node visited, output link;
    vector<int> output;
 
    TrieNode() : string_id(-1) {
    }
 
    ~TrieNode() {
        for (int i = 0; i < ALPHABETS; ++i) {
            if (children[i]) {
                delete children[i];
            }
        }
    }
};


void buildTrie(TrieNode *root, vector<string> words);
void deleteTrie(TrieNode *root);
void computeFailFunc(TrieNode* root);
map<struct info, string> searchAC(const string& s, TrieNode* trie);

vector<string> word_list;

int main() {
    int N;
    char cmd;
    string buf;

    std::ios::sync_with_stdio(false);

    cin >> N;

    struct TrieNode *root = new TrieNode();

    for (int i = 0; i < N; i++){
        cin >> buf;
        word_list.push_back(buf);
    }

    int cnt = 0;
    buildTrie(root, word_list);
    bool compute_flag = true;
    map<struct info, string> ret_map;
    auto it;
    cout << 'R' << endl;

    while (cin >> cmd){
        cin.get();
        getline(cin, buf);
        switch (cmd){
            case 'Q':
            {
                if (compute_flag) {
                    computeFailFunc(root);
                }
                ret_map = searchAC(buf, root);
                
                it = ret_map.begin();
                int cnt = ret_map.size();
                if (cnt == 0) {
                    cout << -1 << endl;
                    break;
                }
                for (; cnt != 0; cnt--, it++){
                    cout << it->second;
                    if (cnt != 1){
                        cout << "|";
                    }
                }
                cout << endl;
                compute_flag = false;
            }
            break;
            case 'A':
            {
                word_list.push_back(buf);
                root = new TrieNode();
                
                buildTrie(root, word_list);
                compute_flag = true;
            }
            break;
            case 'D':
            {
                word_list.erase(remove(word_list.begin(), word_list.end(), buf), word_list.end());
                root = new TrieNode();
                
                cnt = 0;
                buildTrie(root, word_list);
                compute_flag = true;
            }
            break;
        }
    }
    return 0;
}

// build word dictionary
void buildTrie(TrieNode *root, vector<string> words) {
    int i = 0;
    for (auto &it: words) {
        TrieNode *cur = root;
        const char *k = it.c_str();

        // string end
        while (*k != 0) {
            int next = toNumber(*k);
            if (!(cur->flags[next])) {
                cur->children[next] = new TrieNode();
                cur->flags[next] = true;
            }
            cur = cur->children[next];
            k++;
        }
        // string id mark
        cur->string_id = i++;
    }
}

// compute fail, output link
void computeFailFunc(TrieNode* root) {
    // for bfs queue
    queue<TrieNode*> q;
    // root fail link
    root->fail = root;
    q.push(root);

    // run bfs
    while (!q.empty()) {
        TrieNode* here = q.front();
        q.pop();

        for (int i = 0; i < ALPHABETS; ++i) {
            if (!(here->flags[i])) {
                continue;
            }

            TrieNode* child = here->children[i];
            // first layer
            if (here == root) {
                child->fail = root;
            }
            else {
                // medium layers
                TrieNode* t = here->fail;
                while (t != root && !(t->flags[i])) {
                    t = t->fail;
                }
                if (t->flags[i]) {
                    t = t->children[i];
                }
                child->fail = t;
            }
            // push output link
            child->output = child->fail->output;
            if (child->string_id != -1) {
                // add to output link
                child->output.push_back(child->string_id);
            }
            q.push(child);
        }
    }
}

// search word using AC, retrun map<info, string>
map<struct info, string> searchAC(const string &s, TrieNode *root) {
    map<struct info, string> ret_map;

    TrieNode* state = root;
    unordered_set<string> se;
    struct info temp;
    int next;
    for (int i = 0; i < s.size(); ++i) {
        next = toNumber(s[i]);
        while (state != root && !(state->flags[next])) {
            state = state->fail;
        }

        if (state->flags[next]) {
            state = state->children[next];
        }
        for (int j = 0; j < state->output.size(); ++j) {
            temp.end = i;
            temp.id = state->output[j];
            temp.start = temp.end - word_list[temp.id].size();

            // already exist
            if (se.find(word_list[temp.id]) != se.end()) {
                continue;
            }
            se.insert(word_list[temp.id]);
            ret_map.insert(pair<struct info, string>(temp, word_list[temp.id]));
        }
    }
    return ret_map;
}