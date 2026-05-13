#include <iostream>
#include <fstream>
#include <string>
using namespace std;


// stores the mapping of each code -> character (read from codes.txt)
struct CodeArr {
    char  chars[100];
    string codes[100];
    int size;

    CodeArr() : size(0) {}

    void add(char c, string code) {
        chars[size] = c;
        codes[size] = code;
        size++;
    }

    // find character for a given code
    char getChar(string code) {
        for(int i=0; i<size; i++)
            if(codes[i] == code)
                return chars[i];
        return '\0';     //not found
    }
};

class Receiver {
    private:
        string encodedText;         // content : output.txt
        string decodedText;         // final decoded result
        CodeArr codeTable;

    public:
        Receiver() {}

        bool EncodedFileEmpty(){
            if(encodedText.empty()){
                return true;
            }
            return false;
        }

        void menu() {
            cout << "MENU" << endl;
            cout << "1. Decode a file" << endl;
            cout << "2. Exit" << endl;
            cout << "Enter choice: ";
        }

        int input() {
            int choice;
            cin >> choice;
            return choice;
        }

        // Step 1 - read encoded text from output.txt
        void readEncodedFile() {
            ifstream inFile("output.txt");
            if(!inFile.is_open()) {
                cout << "ERROR: Can't open output.txt :(" << endl;
                return;
            }

            string line = "";
            while(getline(inFile, line)) {
                encodedText += line + "\n";
            }
            inFile.close();

            // removing newline added during reading
            if(!encodedText.empty() && encodedText[encodedText.length()-1] == '\n')
                encodedText = encodedText.substr(0, encodedText.length()-1);

            if(EncodedFileEmpty()) {
                cout << "ERROR: output.txt is empty!" << endl;
            }
        }

        // Step 1 - read codes from codes.txt
        void readCodesFile() {
            ifstream cFile("codes.txt");
            if(!cFile.is_open()) {
                cout << "ERROR: Can't open codes.txt" << endl;
                return;
            }

            string line = "";
            while(getline(cFile, line)) {
                if(line.length() < 3)
                    continue;

                // format per line: "X code"  e.g. "A 0"
                char ch = line[0];
                string code = "";

                // code starts after the space
                for(int i=2; i< line.length(); i++)
                    code += line[i];

                codeTable.add(ch, code);
            }
            cFile.close();
        }

        // Step 2 - iterate over encoded text, match codes, build decoded string
        void decodeText() {
            string current = "";        // accumulate bits until a match is found

            for(int i=0; i< encodedText.length(); i++) {
                current += encodedText[i];

                char found = codeTable.getChar(current);
                if(found != '\0') {
                    decodedText += found;
                    current = "";       // reset -> start looking for next code
                }
            }

            if(!current.empty()) {
                cout << "WARNING: Leftover bits that couldn't be decoded: " << current << endl;
            }
        }

        // Step 3 - save decoded text to decoded.txt and show on screen
        void saveAndDisplay() {
            ofstream outFile("decoded.txt");
            if(!outFile.is_open()) {
                cout << "ERROR: Can't open decoded.txt" << endl;
                return;
            }
            outFile << decodedText; 
            outFile.close();

            cout << "\nDecoded text:" << endl;
            cout << decodedText << endl;
            cout << "Decoded text saved to decoded.txt" << endl;
        }
};


int main() {
    Receiver r;

    int opt;

    do {    
        r.menu();
        opt = r.input();
        if(opt == 1) {
            // Step 1: Read the output.txt and codes.txt
            r.readEncodedFile();
            r.readCodesFile();

            if(! r.EncodedFileEmpty()) {
                // Step 2: Iterate over encoded text and find character against each code
                r.decodeText();

                // Step 3: decoded text to decoded.txt and  on screen
                r.saveAndDisplay();
                }
        }

    } while(opt!=2);
    
    return 0;
}
