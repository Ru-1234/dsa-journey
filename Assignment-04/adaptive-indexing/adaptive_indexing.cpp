#include <iostream>
#include <fstream>
#include <string>
using namespace std;

int toInt(const string& s) {
    if(s.empty()) 
        return 0;

    int val = 0;
    int i = 0;
    bool neg = false;
    if(s[0] == '-') { 
        neg = true; 
        i++; 
    }

    for(; i<(int)s.length(); i++) {
        if(s[i]<'0' || s[i] > '9') 
            break;

        val = val * 10 + (s[i] - '0');
    }

    return neg ? -val : val;
}

double toDouble(const string& s) {

    if(s.empty()) 
        return 0.0f;

    double val = 0.0f;
    double dec = 0.0f;

    bool inDec = false;
    double divid = 10.0f;

    int i= 0;
    bool neg= false;
    if(s[0] == '-') { 
        neg = true; 
        i++; 
    }

    for(; i<(int)s.length(); i++) {
        if(s[i] == '.') { 
            inDec = true; 
            continue; 
        }

        if(s[i] <'0' || s[i] > '9') 
            break;
        if(!inDec) 
            val = val * 10 + (s[i] - '0');
        else{ 
            dec += (s[i] - '0') / divid; 
            divid *= 10; }
    }
    return neg ? -(val + dec) : (val + dec);
}

string getField(const string& line, int& pos) {
    string field = "";
    while(pos<(int)line.length() && line[pos] != ',') {
        field += line[pos];
        pos++;
    }

    if(pos<(int)line.length()) 
        pos++;  // skiping comma
    return field;
}

// strip \r from end of line 
void stripCR(string& s) {
    if(!s.empty() && s[s.length()-1] == '\r')
        s.erase(s.length()-1, 1);
}

struct Transaction {
    int transactionID;
    int timestamp;
    double amount;
    double riskScore;   // amount * alpha
};

struct TreeNode {
    Transaction data;

    //BT ptrs(level-order insert)
    TreeNode* btLeft;
    TreeNode* btRight;

    //BST (order : amount)
    TreeNode* bstLeft;
    TreeNode* bstRight;

    //AVL (order : riskScore)
    TreeNode* avlLeft;
    TreeNode* avlRight;
    int avlHeight;

    // parent pointer for iterative AVL rebalance 
    TreeNode* avlParent;

    TreeNode(Transaction t) : data(t), btLeft(NULL),   btRight(NULL),bstLeft(NULL),  
    bstRight(NULL), avlLeft(NULL),  avlRight(NULL),avlHeight(1), avlParent(NULL) {}
};


// queue for level-order BT insertion
struct Queue {
    TreeNode** data;
    int front, back, cap;

    Queue() : front(0), back(0), cap(500) {
        data = new TreeNode*[cap];
    }
    ~Queue() { delete[] data; }

    void enqueue(TreeNode* n) {
        if(back == cap) {
            cap *= 2;
            TreeNode** temp = new TreeNode*[cap];
            for(int i = front; i<back; i++)
                temp[i] = data[i];
            delete[] data;
            data = temp;
        }
        data[back++] = n;
    }

    TreeNode* dequeue() {
        if(front == back) return NULL;
        return data[front++];
    }

    bool isEmpty() { return front == back; }
};

struct Stack {
    TreeNode** data;
    int top, cap;

    Stack() : top(0), cap(500) {
        data = new TreeNode*[cap];
    }
    ~Stack() { delete[] data; }

    void push(TreeNode* n) {
        if(top == cap) {
            cap *= 2;
            TreeNode** temp = new TreeNode*[cap];
            for(int i = 0; i<top; i++)
                temp[i] = data[i];
            delete[] data;
            data = temp;
        }
        data[top++] = n;
    }

    TreeNode* pop() {
        if(top == 0)
            return NULL;

        return data[--top];
    }

    bool isEmpty() { return top == 0; }
};

int avlHeight(TreeNode* n) {
    return n? n->avlHeight : 0;
}

int avlBalance(TreeNode* n) {
    return n? avlHeight(n->avlLeft) - avlHeight(n->avlRight) : 0;
}

void avlUpdateHeight(TreeNode* n) {
    if(n==NULL) 
        return;
    int lh = avlHeight(n->avlLeft);
    int rh = avlHeight(n->avlRight);

    n->avlHeight = 1 + (lh>rh ? lh : rh);
}

TreeNode* avlRotateRight(TreeNode* y) {
    TreeNode* x  = y->avlLeft;
    TreeNode* T2 = x->avlRight;

    x->avlRight = y;
    y->avlLeft  = T2;

    x->avlParent = y->avlParent;
    y->avlParent = x;
    if(T2) T2->avlParent = y;

    avlUpdateHeight(y);
    avlUpdateHeight(x);
    return x;
}

TreeNode* avlRotateLeft(TreeNode* x) {
    TreeNode* y  = x->avlRight;
    TreeNode* T2 = y->avlLeft;

    y->avlLeft  = x;
    x->avlRight = T2;

    y->avlParent = x->avlParent;
    x->avlParent = y;
    if(T2) T2->avlParent = x;

    avlUpdateHeight(x);
    avlUpdateHeight(y);
    return y;
}

TreeNode* avlInsert(TreeNode* root, TreeNode* nn) {
    nn->avlLeft = nn->avlRight = NULL;
    nn->avlHeight = 1;
    nn->avlParent = NULL;

    if(root == NULL) 
        return nn;

    TreeNode* curr = root;
    TreeNode* parent = NULL;

    while(curr != NULL) {
        parent = curr;
        if(nn->data.riskScore<curr->data.riskScore)
            curr = curr->avlLeft;
        else
            curr = curr->avlRight;
    }

    nn->avlParent = parent;
    if(nn->data.riskScore<parent->data.riskScore)
        parent->avlLeft  = nn;
    else
        parent->avlRight = nn;

    TreeNode* node = nn->avlParent;

    while(node != NULL) {
        avlUpdateHeight(node);
        int bal = avlBalance(node);

        if(bal > 1 || bal < -1) {
            TreeNode* par = node->avlParent;
            TreeNode* newSub = NULL;

            if(bal>1) {
                if(avlBalance(node->avlLeft) >= 0) {
                    // LL
                    newSub = avlRotateRight(node);
                } else {
                    // LR
                    node->avlLeft = avlRotateLeft(node->avlLeft);
                    if(node->avlLeft) 
                        node->avlLeft->avlParent = node;
                    newSub = avlRotateRight(node);
                }
            } else {
                if(avlBalance(node->avlRight) <= 0) {
                    // RR
                    newSub = avlRotateLeft(node);
                } else {
                    // RL
                    node->avlRight = avlRotateRight(node->avlRight);
                    if(node->avlRight) 
                        node->avlRight->avlParent = node;
                    newSub = avlRotateLeft(node);
                }
            }

            newSub->avlParent = par;

            if(par == NULL) {
                root = newSub;
            } else {
                if(par->avlLeft == node)
                    par->avlLeft  = newSub;
                else
                    par->avlRight = newSub;
            }

            break;
        }

        node = node->avlParent;
    }

    return root;
}

TreeNode* avlMin(TreeNode* node) {
    while(node->avlLeft) 
        node = node->avlLeft;

    return node;
}

TreeNode* avlDelete(TreeNode* root, double key, int id) {

    //sfind the node 
    TreeNode* target = root;
    while(target != NULL) {
        if(target->data.transactionID == id)
            break;
        if(key < target->data.riskScore)
            target = target->avlLeft;
        else
            target = target->avlRight;
    }
    if(target == NULL) 
        return root;

    //  BST removal, where to start rebalance 
    TreeNode* rebalanceFrom = NULL;

    if(target->avlLeft == NULL || target->avlRight == NULL) {
        // 0 or 1 child
        TreeNode* child = (target->avlLeft != NULL) ? target->avlLeft : target->avlRight;
        TreeNode* par= target->avlParent;

        if(child != NULL) 
            child->avlParent = par;

        if(par == NULL) {
            root = child;
        } else {
            if(par->avlLeft == target)
                par->avlLeft  = child;
            else
                par->avlRight = child;
        }
        rebalanceFrom = par;

    } 
    else {
        TreeNode* succ    = avlMin(target->avlRight);
        TreeNode* succPar = succ->avlParent;

        TreeNode* succRight = succ->avlRight;
        if(succPar == target)
            succPar->avlRight = succRight;
        else
            succPar->avlLeft  = succRight;

        if(succRight != NULL) 
            succRight->avlParent = succPar;

        target->data = succ->data;

        rebalanceFrom = (succPar == target) ? target : succPar;
    }

    TreeNode* node = rebalanceFrom;

    while(node != NULL) {
        avlUpdateHeight(node);
        int bal = avlBalance(node);

        if(bal > 1 || bal < -1) {
            TreeNode* par    = node->avlParent;
            TreeNode* newSub = NULL;

            if(bal > 1) {
                if(avlBalance(node->avlLeft) >= 0) {
                    newSub = avlRotateRight(node);
                } else {
                    node->avlLeft = avlRotateLeft(node->avlLeft);
                    if(node->avlLeft) node->avlLeft->avlParent = node;
                    newSub = avlRotateRight(node);
                }
            } else {
                if(avlBalance(node->avlRight) <= 0) {
                    newSub = avlRotateLeft(node);
                } else {
                    node->avlRight = avlRotateRight(node->avlRight);
                    if(node->avlRight) node->avlRight->avlParent = node;
                    newSub = avlRotateLeft(node);
                }
            }

            newSub->avlParent = par;

            if(par == NULL) {
                root = newSub;
            } else {
                if(par->avlLeft == node)
                    par->avlLeft  = newSub;
                else
                    par->avlRight = newSub;
            }
            // unlike insert, delete may unbalance nodes higher up; continue
            node = newSub->avlParent;

        } else {
            node = node->avlParent;
        }
    }

    return root;
}

TreeNode* bstInsert(TreeNode* root, TreeNode* nn) {
    if(root == NULL) 
        return nn;

    TreeNode* curr = root;
    TreeNode* parent = NULL;

    while(curr != NULL) {
        parent = curr;
        if(nn->data.amount < curr->data.amount)
            curr = curr->bstLeft;
        else
            curr = curr->bstRight;
    }

    if(nn->data.amount < parent->data.amount)
        parent->bstLeft  = nn;
    else
        parent->bstRight = nn;

    return root;
}

class Engine {
    private:
        TreeNode* btRoot;
        TreeNode* bstRoot;
        TreeNode* avlRoot;

        TreeNode** allNodes;
        int nodeCount;
        int nodeCap;

        // anomaly etc nodes
        TreeNode** copyNodes;
        int copyCount;
        int copyCap;

        string csvFile;
        double alpha;     //riskScore = amount * alpha

        TreeNode* anomalyRoot;
        TreeNode* rangeRoot;

    public:
        Engine() : btRoot(NULL), bstRoot(NULL), avlRoot(NULL), nodeCount(0), nodeCap(10000), copyCount(0), copyCap(1000), alpha(1.0), anomalyRoot(NULL), rangeRoot(NULL) {
            allNodes  = new TreeNode*[nodeCap];
            copyNodes = new TreeNode*[copyCap];
        }

        ~Engine() {
            for(int i = 0; i < nodeCount; i++)
                delete allNodes[i];
            for(int i = 0; i < copyCount; i++)
                delete copyNodes[i];
            delete[] allNodes;
            delete[] copyNodes;
        }

        void addToList(TreeNode* n) {
            if(nodeCount == nodeCap) {
                nodeCap *= 2;
                TreeNode** temp = new TreeNode*[nodeCap];
                for(int i = 0; i < nodeCount; i++)
                    temp[i] = allNodes[i];
                delete[] allNodes;
                allNodes = temp;
            }
            allNodes[nodeCount++] = n;
        }

        // tracking anomaly/range copies separately so nodeCount stays accurate
        void addToCopyList(TreeNode* n) {
            if(copyCount == copyCap) {
                copyCap *= 2;
                TreeNode** temp = new TreeNode*[copyCap];
                for(int i = 0; i < copyCount; i++)
                    temp[i] = copyNodes[i];
                delete[] copyNodes;
                copyNodes = temp;
            }
            copyNodes[copyCount++] = n;
        }

        void clearCopies() {
            for(int i = 0; i < copyCount; i++)
                delete copyNodes[i];
            copyCount   = 0;
            anomalyRoot = NULL;
            rangeRoot   = NULL;
        }

        // columns: TransactionID(0), isFraud(1), TransactionDT(2), TransactionAmt(3)
        bool parseLine(const string& line, Transaction& t) {
            int pos = 0;

            string idStr  = getField(line, pos);    // col 0
            string skip   = getField(line, pos);    // col 1  isFraud leave
            string dtStr  = getField(line, pos);    // col 2
            string amtStr = getField(line, pos);    // col 3

            if(idStr.empty() || amtStr.empty()) return false;

            t.transactionID = toInt(idStr);
            t.timestamp     = toInt(dtStr);
            t.amount        = toDouble(amtStr);
            t.riskScore     = t.amount * alpha;

            return true;
        }

        void insertAll(TreeNode* node) {

            // Binary Tree -> level-order using queue
            if(btRoot == NULL) {
                btRoot = node;
            } else {
                Queue q;
                q.enqueue(btRoot);
                while(!q.isEmpty()) {
                    TreeNode* curr = q.dequeue();
                    if(curr->btLeft == NULL) { curr->btLeft = node; break; }
                    else q.enqueue(curr->btLeft);

                    if(curr->btRight == NULL) { curr->btRight = node; break; }
                    else q.enqueue(curr->btRight);
                }
            }

            // BST -> ordered by amount
            bstRoot = bstInsert(bstRoot, node);

            // AVL -> ordered by riskScore (iterative, no recursion)
            avlRoot = avlInsert(avlRoot, node);
        }

        void resetPointers(TreeNode* n) {
            n->btLeft = n->btRight   = NULL;
            n->bstLeft = n->bstRight = NULL;
            n->avlLeft = n->avlRight = NULL;
            n->avlHeight  = 1;
            n->avlParent  = NULL;
        }

        //step 1: load csv
        void loadCSV(int limit) {
            cout << "Enter CSV file name: ";
            getline(cin, csvFile);
            stripCR(csvFile);

            ifstream inFile(csvFile.c_str());
            if(!inFile.is_open()) {
                cout << "ERROR! Can't open '" << csvFile << "'" << endl;
                return;
            }

            // header -> skip
            string line = "";
            getline(inFile, line);

            int count = 0;
            while(count < limit && getline(inFile, line)) {
                stripCR(line);
                if(line.empty()) continue;

                Transaction t;
                if(!parseLine(line, t)) continue;

                TreeNode* node = new TreeNode(t);
                addToList(node);
                insertAll(node);
                count++;
            }
            inFile.close();

            cout << "Loaded " << count << " transactions " << endl;
        }

        void recalibrate(double newAlpha) {
            alpha   = newAlpha;
            avlRoot = NULL;

            for(int i = 0; i < nodeCount; i++) {
                allNodes[i]->data.riskScore = allNodes[i]->data.amount * alpha;
                allNodes[i]->avlLeft   = NULL;
                allNodes[i]->avlRight  = NULL;
                allNodes[i]->avlHeight = 1;
                allNodes[i]->avlParent = NULL;
            }

            // reinserting all into fresh AVL with updated scores
            for(int i = 0; i < nodeCount; i++)
                avlRoot = avlInsert(avlRoot, allNodes[i]);

            cout << "Recalibrated. alpha = " << alpha << endl;
        }

        void reload(int k) {
            if(k > nodeCount) {
                cout << "ERROR: k=" << k << " exceeds loaded=" << nodeCount << endl;
                return;
            }

            int reloadFrom = nodeCount - k;

            // clearing all three trees
            btRoot = bstRoot = avlRoot = NULL;
            for(int i = 0; i < nodeCount; i++)
                resetPointers(allNodes[i]);

            // reinserting first (nodeCount - k) nodes
            for(int i = 0; i < reloadFrom; i++)
                insertAll(allNodes[i]);

            // reread last k lines from CSV
            ifstream inFile(csvFile.c_str());
            if(!inFile.is_open()) {
                cout << "ERROR: Can't reopen '" << csvFile << "'" << endl;
                return;
            }

            // header + first reloadFrom data lines -> skip
            string line = "";
            getline(inFile, line);
            for(int i = 0; i < reloadFrom; i++)
                getline(inFile, line);

            // reload last k
            for(int i = reloadFrom; i < nodeCount; i++) {
                if(!getline(inFile, line)) 
                    break;
                stripCR(line);

                Transaction t;
                if(parseLine(line, t)) {
                    allNodes[i]->data = t;
                    insertAll(allNodes[i]);
                }
            }
            inFile.close();

            cout << "Reloaded last " << k << " transactions" << endl;
        }


        void extractAnomalies(double threshold) {
            // free previous copies before building new anomaly BST
            clearCopies();

            Stack s;
            TreeNode* curr = bstRoot;

            while(curr!=NULL || !s.isEmpty()) {
                while(curr != NULL) { 
                    s.push(curr); 
                    curr = curr->bstLeft; 
                }
                curr = s.pop();

                if(curr->data.amount >= threshold) {
                    // fresh copy so anomalyRoot is independent of main trees
                    TreeNode* copy = new TreeNode(curr->data);
                    copy->bstLeft = copy->bstRight = NULL;
                    addToCopyList(copy);
                    anomalyRoot = bstInsert(anomalyRoot, copy);
                }

                curr = curr->bstRight;
            }

            cout << "Anomaly BST built for amount >= " << threshold << endl;
        }

        // BST depth by walking on amount (BST is ordered by amount, not ID)
        int bstDepth(int id, double amount) {
            int depth = 0;
            TreeNode* curr = bstRoot;

            while(curr != NULL) {
                if(curr->data.transactionID == id)
                    return depth;

                if(amount < curr->data.amount)
                    curr = curr->bstLeft;
                else
                    curr = curr->bstRight;

                depth++;
            }

            return -1;
        }

        // AVL depth by walking on riskScore (AVL is ordered by riskScore)
        int avlDepth(int id, double riskScore) {
            int depth = 0;
            TreeNode* curr = avlRoot;

            while(curr != NULL) {
                if(curr->data.transactionID == id)
                    return depth;

                if(riskScore < curr->data.riskScore)
                    curr = curr->avlLeft;
                else
                    curr = curr->avlRight;

                depth++;
            }

            return -1;
        }

        void detectDrift(int D) {
            cout << "Drift nodes (depth diff > " << D << "):" << endl;
            int found = 0;

            for(int i = 0; i < nodeCount; i++) {
                int    id   = allNodes[i]->data.transactionID;
                double amt  = allNodes[i]->data.amount;
                double risk = allNodes[i]->data.riskScore;

                int bd = bstDepth(id, amt);
                int ad = avlDepth(id, risk);

                if(bd == -1 || ad == -1) 
                    continue;

                int diff = bd - ad;
                if(diff < 0) diff = -diff;

                if(diff > D) {
                    cout << " ID=" << id << " BSTdepth=" << bd
                    << " AVLdepth=" << ad << " diff=" << diff << endl;
                    found++;
                }
            }

            if(found == 0)
                cout << "  No drift nodes found " << endl;
        }

        // extract nodes with timestamp in [T1, T2] into a separate AVL
        // copies are used so main avlRoot is never modified
        void temporalRange(int T1, int T2) {
            // free previous copies before building new range AVL
            clearCopies();

            Stack s;
            TreeNode* curr = bstRoot;

            while(curr != NULL || !s.isEmpty()) {
                while(curr != NULL) {
                    s.push(curr);
                    curr = curr->bstLeft;
                }

                curr = s.pop();

                if(curr->data.timestamp >= T1 && curr->data.timestamp <= T2) {
                    // copy with fresh AVL pointers; does not touch main avlRoot
                    TreeNode* copy = new TreeNode(curr->data);
                    copy->avlLeft = copy->avlRight = NULL;
                    copy->avlHeight = 1;
                    copy->avlParent = NULL;
                    addToCopyList(copy);
                    rangeRoot = avlInsert(rangeRoot, copy);
                }

                curr = curr->bstRight;
            }

            cout << "Range AVL built for [" << T1 << ", " << T2 << "]" << endl;
        }


        bool verifyBST() {
            Stack s;
            TreeNode* curr = bstRoot;
            double prev = -1.0;

            while(curr != NULL || !s.isEmpty()) {

                while(curr != NULL) { 
                    s.push(curr); 
                    curr = curr->bstLeft; 
                }
                curr = s.pop();

                if(curr->data.amount < prev) 
                    return false;

                prev = curr->data.amount;
                curr = curr->bstRight;
            }
            return true;
        }

        bool verifyAVL(TreeNode* root) {
            if(root == NULL) return true;

            Stack s;
            s.push(root);

            while(!s.isEmpty()) {
                TreeNode* curr = s.pop();

                int bal = avlBalance(curr);
                if(bal > 1 || bal < -1)
                    return false;

                if(curr->avlLeft)  
                    s.push(curr->avlLeft);
                if(curr->avlRight) 
                    s.push(curr->avlRight);
            }

            return true;
        }

        bool verifyBT() {
            if(btRoot == NULL) 
                return nodeCount == 0;
            int count = 0;

            Queue q;
            q.enqueue(btRoot);
            while(!q.isEmpty()){
                TreeNode* curr = q.dequeue();
                count++;
                if(curr->btLeft)  
                    q.enqueue(curr->btLeft);
                if(curr->btRight) 
                   q.enqueue(curr->btRight);
            }
            // count must = original node count (copies in separate list)
            return count == nodeCount;
        }

        void verifyAll() {
            cout << "Consistency Check:" << endl;
            cout << " BST sorted: " << (verifyBST()       ? "OK" : "FAIL") << endl;
            cout << " AVL balanced: " << (verifyAVL(avlRoot) ? "OK" : "FAIL") << endl;
            cout << " BT count: " << (verifyBT()         ? "OK" : "FAIL") << endl;
        }

        void saveCheckpoint(const string& fname) {
            ofstream f(fname.c_str());
            if(!f.is_open()) {
                cout << "ERROR: Can't open '" << fname << "'" << endl;
                return;
            }

            f << nodeCount << " " << alpha << "\n";

            for(int i = 0; i < nodeCount; i++) {
                f << allNodes[i]->data.transactionID << ","
                  << allNodes[i]->data.timestamp     << ","
                  << allNodes[i]->data.amount        << ","
                  << allNodes[i]->data.riskScore     << "\n";
            }
            f.close();

            cout << "Checkpoint saved to '" << fname << "'" << endl;
        }

        void loadCheckpoint(const string& fname) {
            ifstream f(fname.c_str());
            if(!f.is_open()) {
                cout << "ERROR: Can't open '" << fname << "'" << endl;
                return;
            }

            // clear existing data
            for(int i = 0; i < nodeCount; i++)
                delete allNodes[i];
            nodeCount = 0;
            btRoot = bstRoot = avlRoot = NULL;

            string line = "";
            getline(f, line);

            int pos = 0;
            toInt(getField(line, pos));         // advance past node count
            alpha = toDouble(getField(line, pos));

            while(getline(f, line)) {
                if(line.empty()) continue;
                int p = 0;

                Transaction t;
                t.transactionID = toInt(getField(line, p));
                t.timestamp     = toInt(getField(line, p));
                t.amount        = toDouble(getField(line, p));
                t.riskScore     = toDouble(getField(line, p));

                TreeNode* node = new TreeNode(t);
                addToList(node);
                insertAll(node);
            }
            f.close();

            cout << "Checkpoint loaded. " << nodeCount << " transactions " << endl;
        }

        void printBST(int n) {
            cout << "BST in-order (first " << n << "):" << endl;
            Stack s;
            TreeNode* curr = bstRoot;
            int count = 0;

            while((curr != NULL || !s.isEmpty()) && count < n) {
                while(curr != NULL) { 
                    s.push(curr); 
                    curr = curr->bstLeft; 
                }
                curr = s.pop();
                cout << " ID=" << curr->data.transactionID
                     << " Amt=" << curr->data.amount << endl;
                count++;
                curr = curr->bstRight;
            }
        }

        void printBT(int n) {
            cout << "BT level-order (first " << n << "):" << endl;
            if(btRoot == NULL) 
                return;

            Queue q;
            q.enqueue(btRoot);
            int count = 0;

            while(!q.isEmpty() && count < n) {
                TreeNode* curr = q.dequeue();
                cout << " ID=" << curr->data.transactionID
                     << " Amt=" << curr->data.amount << endl;
                count++;

                if(curr->btLeft)  
                    q.enqueue(curr->btLeft);
                if(curr->btRight) 
                    q.enqueue(curr->btRight);
            }
        }

        void printAVL(int n) {
            cout << "AVL in-order (first " << n << "):" << endl;
            Stack s;
            TreeNode* curr = avlRoot;
            int count = 0;

            while((curr != NULL || !s.isEmpty()) && count < n) {
                while(curr != NULL) { s.push(curr); curr = curr->avlLeft; }
                curr = s.pop();
                cout << "  ID=" << curr->data.transactionID
                     << "  Risk=" << curr->data.riskScore << endl;
                count++;
                curr = curr->avlRight;
            }
        }

    // print anomaly BST inorder (ordered by amount, same as main BST)
        void printAnomalyBST(int n) {
            if(anomalyRoot == NULL) {
                cout << "Anomaly BST is empty. Run option 4 first." << endl;
                return;
            }
 
            cout << "Anomaly BST in-order (first " << n << "):" << endl;
            Stack s;
            TreeNode* curr = anomalyRoot;
            int count = 0;
 
            while((curr != NULL || !s.isEmpty()) && count < n) {
                while(curr != NULL) { 
                    s.push(curr); 
                    curr = curr->bstLeft; 
                }
                curr = s.pop();
                cout << "  ID=" << curr->data.transactionID
                     << "  Amt=" << curr->data.amount
                     << "  Risk=" << curr->data.riskScore
                     << "  TS=" << curr->data.timestamp << endl;
                count++;
                curr = curr->bstRight;
            }
        }
 
        // print range AVL in-order (ordered by riskScore, same as main AVL)
        void printRangeAVL(int n) {
            if(rangeRoot == NULL) {
                cout << "Range AVL is empty. Run option 6 first." << endl;
                return;
            }
 
            cout << "Range AVL in-order (first " << n << "):" << endl;
            Stack s;
            TreeNode* curr = rangeRoot;
            int count = 0;
 
            while((curr != NULL || !s.isEmpty()) && count < n) {
                while(curr != NULL) { 
                    s.push(curr); 
                    curr = curr->avlLeft; 
                }
                curr = s.pop();
                cout << "  ID=" << curr->data.transactionID
                     << "  Risk=" << curr->data.riskScore
                     << "  TS=" << curr->data.timestamp
                     << "  Amt=" << curr->data.amount << endl;
                count++;
                curr = curr->avlRight;
            }
        }

        void menu() {
            cout << "\nMENU" << endl;
            cout << "1.  Load CSV"    << endl;
            cout << "2.  Recalibrate" << endl;
            cout << "3.  Reload k"    << endl;
            cout << "4.  Anomaly Extract"  << endl;
            cout << "5.  Drift Detection"  << endl;
            cout << "6.  Temporal Range"   << endl;
            cout << "7.  Verify"      << endl;
            cout << "8.  Save Checkpoint"  << endl;
            cout << "9.  Load Checkpoint"  << endl;
            cout << "10. Print BST"   << endl;
            cout << "11. Print BT"    << endl;
            cout << "12. Print AVL"   << endl;
            cout << "0.  Exit"        << endl;
            cout << "Enter choice: ";
        }
};


int main() {
    Engine engine;

    int opt;
    do {
        engine.menu();
        cin >> opt;

        if(opt == 1) {
            int limit;
            cout << "How many rows to load: ";
            cin  >> limit;
            cin.ignore();
            engine.loadCSV(limit);
        }
        else if(opt == 2) {
            double a;
            cout << "Enter new alpha: ";
            cin  >> a;
            engine.recalibrate(a);
        }
        else if(opt == 3) {
            int k;
            cout << "Enter k: ";
            cin  >> k;
            engine.reload(k);
        }
        else if(opt == 4) {
            double x;
            cout << "Enter threshold X: ";
            cin  >> x;
            engine.extractAnomalies(x);
        }
        else if(opt == 5) {
            int d;
            cout << "Enter depth diff D: ";
            cin  >> d;
            engine.detectDrift(d);
        }
        else if(opt == 6) {
            int t1, t2;
            cout << "Enter T1: "; cin >> t1;
            cout << "Enter T2: "; cin >> t2;
            engine.temporalRange(t1, t2);
        }
        else if(opt == 7) {
            engine.verifyAll();
        }
        else if(opt == 8) {
            string fname;
            cout << "Enter file name: ";
            cin  >> fname;
            engine.saveCheckpoint(fname);
        }
        else if(opt == 9) {
            string fname;
            cout << "Enter file name: ";
            cin  >> fname;
            engine.loadCheckpoint(fname);
        }
        else if(opt == 10) {
            int n; 
            cout << "How many: "; 
            cin >> n;
            engine.printBST(n);
        }
        else if(opt == 11) {
            int n; 
            cout << "How many: "; 
            cin >> n;
            engine.printBT(n);
        }
        else if(opt == 12) {
            int n; 
            cout << "How many: "; 
            cin >> n;
            engine.printAVL(n);
        }
        else if(opt != 0) {
            cout << "Invalid choice " << endl;
        }

    }while(opt != 0);

    return 0;
}
