#include <iostream>
#include <fstream>
#include <string>
using namespace std;

// Each node will store a
// character, its frequency, address of left child, and address of right child.
struct Node {
    char ch;
    int  freq;
    Node* leftNode;
    Node* rightNode;

    Node(char c, int f) :ch(c), freq(f) {
        leftNode = rightNode = NULL;
    }
};


// step 3 - making array that will store lal nodes along with their frequency
struct NodeArr {
    Node** data;
    int size;
    int cap;

    NodeArr() : size(0), cap(20) {
        data = new Node*[cap];
    }
    ~NodeArr() {
        delete[] data;
    }

    void push(Node* n) {
        if(size == cap) {
            cap *= 2;
            Node** temp = new Node*[cap];
            for(int i=0; i<size; i++) 
                temp[i] = data[i];
            delete[] data;
            data = temp;
        }
        data[size++] = n;
    }

    void removeAt(int i) {
        for(int j=i; j< size-1; j++) 
            data[j] = data[j+1];
        size--;
    }

    int minIdx() {
        int idx = 0;
        for(int i=1; i<size; i++)
            if(data[i]->freq < data[idx]->freq) 
            // finding node(index) with min freq
                idx = i;

        return idx;
    }
};

struct CodeArr {
    char chars[100];
    string codes[100];
    int size;
 
    CodeArr() : size(0) {}
 
    void add(char c, string code) {
        chars[size]  = c;
        codes[size]  = code;
        size++;
    }

    string getCode(char c) {
        for(int i=0; i<size; i++)
            if(chars[i] == c)
                return codes[i];
        return "";
    }
};

class Sender {
    private:
        string fileName;
        string content;

        string unique;              // for unique characters
        int* freq;

        NodeArr array;
        Node* root;

        CodeArr codeTable;

    public:
        Sender() {
            freq = new int[100];
            root = NULL;
        }

        ~Sender() {
            delete[] freq;
            freq = NULL;
        }

        Node* getRoot() {
            return root;
        }

        bool fileContentEmpty(){
            if(content.empty()) {
                cout << "File is empty!" << endl;
                return true;
            }   
            return false;
        }

        void menu() {
            cout << "MENU" << endl;
            cout << "1. Encode a file" << endl;
            cout << "2. Exit" << endl;
            cout << "Enter choice: ";
        }

        int input() {
            int choice;
            cin >> choice;
            return choice;
        }

        void inputFileNameandContent() {
            cout << "Enter input-text file name: ";
            cin >> fileName;

            cout << endl;

            ifstream inFile(fileName);
            if(!inFile.is_open()) {
                cout << "ERROR: Can't open file '" << fileName << endl;
                return;
            }

            // readContent
            string line = "";

            while(getline(inFile, line)){
                content += line;
            }
            inFile.close();

        }
        
        void determineFrequency() {
            int k = 0; 

            for (int i = 0; i<content.length(); i++) {
                bool found = false;
        
                for (int j = 0; j < k; j++) {
                    if (content[i] == unique[j]) {
                        freq[j]++;
                        found = true;
                        break;
                    }
                }

                // if not found -> new unique character
                if(!found) {
                    unique += content[i];
                    freq[k] = 1;
                    k++;
                }
            }
        }

        // Create nodes for each character based on their frequency
        void createNodes() {
            for(int i=0 ; i<unique.length(); i++){
                array.push(new Node(unique[i], freq[i]));
            }
        }

        // merge modes iteratively
        void mergeNodes(){
            while(array.size>1) {
                int i1= array.minIdx();
                // first min -> left child
                Node* left = array.data[i1];
                array.removeAt(i1);

                int i2 = array.minIdx();
                // second min-> right child
                Node* right = array.data[i2];
                array.removeAt(i2);

                // parent node, sum of their frequencies
                Node* parent = new Node('\0', left->freq + right->freq);
                parent->leftNode  = left;
                parent->rightNode = right;
                
                array.push(parent);
            }

            root = array.data[0];
            array.removeAt(0);
        }

        // step 6 - traverse tree and assign binary codes
        // going left -> '0', going right -> '1'
        void assignCodes(string code, Node* node) {
            /// base case
            if(node == NULL)
                return;
 
            // leaf node -> actual character
            if(node->leftNode == NULL && node->rightNode == NULL) {
                codeTable.add(node->ch, code);
                return;
            }
 
            assignCodes(code + "0", node->leftNode);
            assignCodes(code + "1", node->rightNode);
        }

        // step 7 - encode content using code table, save to output.txt
        void encodeAndSave() {
            string encoded ="";
 
            for(int i=0; i< content.length(); i++) {
                string code = codeTable.getCode(content[i]);
                if(code == "") {
                    cout <<"ERROR: No code found for character" << content[i] << endl;
                    return;
                }
                encoded += code;
            }
 
            ofstream outFile("output.txt");
            if(!outFile.is_open()) {
                cout << "ERROR: Can't open output.txt" << endl;
                return;
            }
            outFile << encoded;
            outFile.close();
 
            cout << "Encoded text saved to output.txt" << endl;
        }

        //save codes to codes.txt
        void saveCodes() {
            ofstream cFile("codes.txt");
            if(!cFile.is_open()) {
                cout << "ERROR: Can't open codes.txt!!" << endl;
                return;
            }
 
            for(int i=0; i<codeTable.size; i++) {
                cFile << codeTable.chars[i] << " " << codeTable.codes[i] << "\n";
            }
            cFile.close();
 
            cout << "Codes saved to codes.txt" << endl;
        }
 
};
int main() {
    Sender s;

    int opt;
    do {
        s.menu();
        opt = s.input();

        if(opt==1) {
            // Step 1: Take name of a text file and read its content
            s.inputFileNameandContent();

            if(! s.fileContentEmpty()) {
                // step2: determine freq of each character
                s.determineFrequency();

                // step 3: Create separate node for each character based on its frequency.
                s.createNodes();

                // Step 4 & 5: Merge the available nodes iteratively.
                s.mergeNodes();

                // Step 6:Assign binary codes to characters based on their position in the tree.
                s.assignCodes("", s.getRoot());

                // step 7:
                s.encodeAndSave();
                s.saveCodes();
            }
        }

    } while(opt==1);

    return 0;
}
