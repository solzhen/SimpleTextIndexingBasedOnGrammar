#include "npatterns.hpp"

typedef struct { 
    int left,right;
} Tpair;

nPatternSearcher::nPatternSearcher(string input_filename) {
    string filename = input_filename;
    cout << "Applying Re-pair to " << filename << endl;
    string command = "./repairs/repairnavarro/repair " + filename;
    int retCode = system(command.c_str());
    if (retCode == 0) {
        std::cout << "Re-pair succesfull" << std::endl;
    } else {
        std::cout << "Program execution failed with code: " << retCode << std::endl;
    }

    ifstream Rfile(filename + ".R", ios::binary);
    if (!Rfile.is_open()) {
        cerr << "Error: cannot open file " << filename << ".R" << endl;        
    }
    int alph;
    Rfile.read((char*)&alph, sizeof(int));
    cout << "Alphabet size: " << alph << endl;
    char map[256];
    Rfile.read((char*)&map, alph);
    cout << "Alphabet mapping: ";
    for (int i = 0; i < alph; i++) {
        cout << map[i] << " ";
    } cout << endl;
    vector<Tpair> rules;
    Tpair rule;
    while (Rfile.read((char*)&rule, sizeof(Tpair))) {
        rules.push_back(rule);
    }
    cout << "Number of rules: " << rules.size() << endl;

    ifstream Cfile(filename + ".C", ios::binary);
    if (!Cfile.is_open()) {
        cerr << "Error: cannot open file " << filename << ".C" << endl;
    }

    int len;
    struct stat s;
    char fname[1024];
    strcpy(fname, filename.c_str());
    strcat(fname,".C");
    stat (fname,&s);
    len = s.st_size / sizeof(int);

    int* C = (int*)malloc(len * sizeof(int));
    Cfile.read((char*)C, len * sizeof(int));
    Cfile.close();
    cout << "Sequence C length: " << len << endl;
    Cfile.close();

    int nnt = rules.size() ;

    nt = alph; // ---------------------------------------------------------

    cout << ". . .Adding extra rules to get rid of sequence C" << endl;
    while (len > 1) {
        for (u_int i = 0; i < len; i = i+2) {
            if (i == len - 1) { // if we're at the last element of the sequence, it means the sequence length was odd
                C[i/2] = C[i];
            } else {
                Tpair new_rule;
                new_rule.left = C[i];
                new_rule.right = C[i+1];
                nnt = rules.size();
                rules.push_back(new_rule);
                C[i/2] = nnt;
                nnt++;
            }
        }
        len = len % 2 == 0 ? len / 2 : len / 2 + 1; // update the sequence length
    }


}