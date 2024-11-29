#include "patterns.hpp"


// MACROS
#define seqR2normalized(c, result) \
    do { \
        if (c < 256)  \
            (result) = rank_bbbb(c + 1) - 1; \
        else\
            (result) = c - 257 + rank_bbbb(257); \
    } while (0)
#define normalized2seqR(c, result) \
    do { \
        if (c < rank_bbbb(257)) \
            (result) = select_bbbb(c + 1); \
        else \
            (result) = c - rank_bbbb(257) + 257; \
    } while (0)

PatternSearcher::PatternSearcher(string input_filename) {
    FILE *input;
    DICT *dict;
    input  = fopen(input_filename.c_str(), "rb");
    dict = RunRepair(input);
    fclose(input);

    std::cout << "original text length: " << dict->txt_len << std::endl;
    std::cout << "number of rules: " << dict->num_rules - 256 << std::endl;
    std::cout << "sequence length: " << dict->seq_len << std::endl;

    RULE *rules = dict->rule; // set or rules 
    CODE *comp_seq = dict->comp_seq; // sequence C

    int len = dict->txt_len; // length of the text
    cout << "Text length: " << len << endl;

    cout << ". . .Adding extra rules to get rid of sequence C" << endl;

    while (dict->seq_len > 1) {
        for (u_int i = 0; i < dict->seq_len; i = i+2) {
            if (dict->num_rules >= dict->buff_size) {
                dict->buff_size *= 2;
                dict->rule = (RULE*)realloc(dict->rule, sizeof(RULE) * dict->buff_size);
                if (!dict->rule) {
                    fprintf(stderr, "Memory allocation failed!\n");
                    exit(EXIT_FAILURE);
                }
            }
            if (i == dict->seq_len - 1) { // if we're at the last element of the sequence, it means the sequence length was odd
                comp_seq[i/2] = comp_seq[i];
            }
            else { // This works, but in the future, I may need to expose the AddNewPair function from repair.c.
                RULE new_rule;
                new_rule.left = comp_seq[i];
                new_rule.right = comp_seq[i+1];
                rules[dict->num_rules] = new_rule; // append the new rule to the rules
                comp_seq[i/2] = dict->num_rules; // update the sequence C
                dict->num_rules++; // increment the number of rules
            }
        }
        dict->seq_len = dict->seq_len % 2 == 0 ? dict->seq_len / 2 : dict->seq_len / 2 + 1; // update the sequence length
    }

    bit_vector bbbb(257, 0); // bit vector to mark which symbols are in the alphabet used by text
    int_vector<> sequenceR((dict->num_rules - 257) * 2, 0, sizeof(CODE) * 8);
    for (u_int i = 0; i < sequenceR.size(); i = i + 2) {
        sequenceR[i] = rules[i/2 + 257].left;
        sequenceR[i + 1] = rules[i/2 + 257].right;
        if (sequenceR[i] <= 256) {
            bbbb[sequenceR[i]] = 1;
        }
        if (sequenceR[i + 1] <= 256) {
            bbbb[sequenceR[i + 1]] = 1;
        }
    }
    rank_support_v<1> rank_bbbb(&bbbb);
    select_support_mcl<1, 1> select_bbbb(&bbbb);
    vector<char> rank(257, 0);
    vector<char> select(257, 0);
    for (int i = 0; i < 257; i++) {
        rank[i] = rank_bbbb(i);
        if (i==0) continue;
        select[i] = select_bbbb(i);
    }
    u_int max_terminal = 0;    
    for (u_int i = 1; i <= rank_bbbb(257); i++) {
        if (select_bbbb(i) > max_terminal) {
            max_terminal = select_bbbb(i);
        }
    }
    sl = select; //--------------------------------------------------
    rk = rank; //--------------------------------------------------

    int_vector<> normalized_sequenceR(sequenceR.size(), 0, sizeof(CODE) * 8);
    int sz = sequenceR.size(); // size of the sequence R
    int r;
    int max_normalized = 0; // maximum symbol in the normalized alphabet
    for (int i = 0; i < sz; i++) {
        seqR2normalized(sequenceR[i], r);
        normalized_sequenceR[i] = r;
        if (r > max_normalized) {
            max_normalized = r;
        }
    }
    int n_terminals = rk[256];
    int n_non_terminals = sz / 2;
    nt = n_terminals; //--------------------------------------------------
    cout << "Max Symbol: " << max_normalized << endl;
    cout << "N. of Terminals: " << n_terminals << endl;
    cout << "N. of Non-terminals: " << n_non_terminals << endl;
    free(dict);
    ARSSequence arsSequence(normalized_sequenceR, max_normalized + 1); 
    R = arsSequence; //--------------------------------------------------  
    cout << "------------------------" << endl;
    cout << "Sorting rules by lexicographic order of left side reverse expansion" << endl;
    int_vector indexMap(n_non_terminals);
    int_vector reverseIndexMap(n_non_terminals);

    for (int i = 0; i < n_non_terminals; i++) {
        indexMap[i] = i;
        reverseIndexMap[i] = i;
    }
    
    totalComparisons = static_cast<int>(n_non_terminals * std::log2(std::max(1, n_non_terminals)));
    if (totalComparisons == 0) totalComparisons = 1; // Prevent division by zero
    comparisons = 0;
    lastProgress = -1;
    auto progressComparatorRev = [&](int a, int b) {
        comparisons++;
        int progress = min((comparisons * 100) / totalComparisons, 100);
        if (progress > lastProgress) {
            std::cout << "\rSorting progress: " << progress << "%" << std::flush;
            lastProgress = progress;
        }
        return this->compareRulesLazy(a, b, true);
    };
    sort(
        reverseIndexMap.begin(), 
        reverseIndexMap.end(), 
        progressComparatorRev
    );
    cout << "\rSorting progress: 100%" << endl;
    cout << "Sorting succesfull" << endl;
    cout << "------------------------" << endl;

/*     sort(
        reverseIndexMap.begin(), 
        reverseIndexMap.end(), 
        [&](int a, int b) { 
            return compareRulesLazy(a, b, true); 
        }
    ); */
    cout << "Reordering Sequence" << endl;

    int last = 0;
    int current = 0;

    vector<int> distance_of_find(reverseIndexMap.size(), 0);
    for (int i = 0; i < reverseIndexMap.size(); i++) {
        distance_of_find[reverseIndexMap[i]] = i;        
    }
    int_vector<> sortedSequenceR = int_vector(n_non_terminals * 2 + 1, 0);
    for (u_int i = 0; i < reverseIndexMap.size(); i++) {
        current = i*100/reverseIndexMap.size();
        if (current > last) {
            cout << "\r" << current << "%" << flush;
            last = current;
        }
        int a_i = reverseIndexMap[i]; // index in ARSS of the i-th smallest rule (sorted by the reversed left side)
        int b_i = normalized_sequenceR[a_i*2]; // left side of the rule
        int c_i = normalized_sequenceR[a_i*2+1]; // right side of the rule
        int n_b_i, n_c_i;
        if (b_i < n_terminals) {
            n_b_i = b_i;
        } else {
            n_b_i = distance_of_find[b_i - n_terminals]  + n_terminals;
        }
        if (c_i < n_terminals) {
            n_c_i = c_i;
        } else {
            n_c_i = distance_of_find[c_i - n_terminals] + n_terminals;
        }
        sortedSequenceR[i*2] = n_b_i; // update the left side of the rule
        sortedSequenceR[i*2+1] = n_c_i; // update the right side of the rule
    } cout << "\r100%" << endl;
    int S_i = distance(reverseIndexMap.begin(), find(reverseIndexMap.begin(), reverseIndexMap.end(), n_non_terminals-1)); // index of the initial symbol in the sorted sequence   
    cout << "Initial rule S: " << S_i << endl;
    sortedSequenceR[n_non_terminals*2] = S_i; // update the initial symbol
    R = ARSSequence(sortedSequenceR, max_normalized + 1 + 1); //--------------------------------------------------

    cout << "Sequence Reordered" << endl;
    cout << "------------------------" << endl;
    cout << "Sorting rules by lexicographic order of right side expansion" << endl;

    totalComparisons = static_cast<int>(n_non_terminals * std::log2(std::max(1, n_non_terminals)));
    if (totalComparisons == 0) totalComparisons = 1; // Prevent division by zero
    comparisons = 0;
    lastProgress = -1;
    auto progressComparator = [&](int a, int b) {
        comparisons++;
        int progress = min((comparisons * 100) / totalComparisons, 100);
        if (progress > lastProgress) {
            std::cout << "\rSorting progress: " << progress << "%" << std::flush;
            lastProgress = progress;
        }
        return this->compareRulesLazy(a, b);
    };
    sort(
        indexMap.begin(), 
        indexMap.end(), 
        progressComparator
    );
    std::cout << "\rSorting progress: 100%" << std::endl;
    cout << "Sorting succesfull" << endl;
    cout << "------------------------" << endl;

    cout << "Creating points for grid" << endl;
    std::vector<Point> points(n_non_terminals);

    vector<int> distance_of_find_col(indexMap.size(), 0);
    for (int i = 0; i < indexMap.size(); i++) {
        distance_of_find_col[indexMap[i]] = i;        
    }

    u_int j, k;
    for (u_int i = 0; i < indexMap.size(); i++) {
        k = distance_of_find_col[i];
        points[i] = Point(k + 1, i + 1); // 1-indexed
    }
    cout << "Points created" << endl;
    cout << "------------------------" << endl;
    cout << "Creating Grid" << endl;
    G = Grid(points, n_non_terminals, n_non_terminals); //--------------------------------------------------
    S = S_i; //--------------------------------------------------
    cout << "Grid created" << endl;
    cout << "------------------------" << endl;
    cout << "Precalculating lengths" << endl;
    l = int_vector(n_non_terminals, 0); //--------------------------------------------------
    for (int i = 0; i < n_non_terminals; i++) {
        l[i] = ruleLength(i);
    }
    cout << "Lengths precalculated" << endl;
    cout << "------------------------" << endl;
}

int PatternSearcher::ruleLength(int i) {
    if (l[i] != 0) {
        return l[i];
    }
    int left, right;
    if (R[i*2] < nt) {
        left = 1;
    } else {
        left = ruleLength(R[i*2] - nt);
    }
    if (R[i*2+1] < nt) {
        right = 1;
    } else {
        right = ruleLength(R[i*2+1] - nt);
    }
    l[i] = left + right;
    return l[i];
}

string PatternSearcher::expandRule(int i, unordered_map<int, string> &memo) {
    if (memo.find(i) != memo.end()) {
        return memo[i];
    }
    string left, right;
    if (R[i] < nt) {
        left = string(1, sl[ R[i] + 1 ]);
    } else {        
        left = expandRule(2*(R[i]-nt), memo);
    }
    if (R[i + 1] < nt) {
        right = string(1, sl[ R[i+1] + 1 ]);
    } else {
        right = expandRule(2*(R[i+1]-nt), memo);
    }
    string result = left + right;
    memo[i] = result;
    return result;
}

string PatternSearcher::expandRightSideRule(int i, unordered_map<int, string> &memo) {
    string right;
    if (R[i+1] < nt) {
        right = string(1, sl[ R[i+1] + 1 ]);
    } else {
        right = expandRule(2*(R[i+1]-nt), memo);
    }
    return right;
}

string PatternSearcher::expandLeftSideRule(int i, unordered_map<int, string> &memo) {
    string left;
    if (R[i] < nt) {
        left = string(1, sl[R[i] +1 ]);
    } else {
        left = expandRule(2*(R[i]-nt), memo);
    }
    return left;
}

Generator<char> PatternSearcher::expandRuleLazy(int i, bool rev) {
    int f,s;
    f = rev? i+1: i;
    s = rev? i: i+1;
    if (R[f] < nt) { 
        co_yield sl[R[f] + 1];
    } else {
        auto first_gen = expandRuleLazy(2*(R[f]-nt), rev);
        for (char c : first_gen) {
            co_yield c;
        }
    }
    if (R[s] < nt) {
        co_yield sl[R[s] + 1];
    } else {
        auto second_gen = expandRuleLazy(2*(R[s]-nt), rev);
        for (char c : second_gen) {
            co_yield c;
        }
    }
}

Generator<char> PatternSearcher::expandRuleSideLazy(int i, bool left) {   
    int lr_i = left? i: i+1;
    if (R[lr_i] < nt) {
        co_yield sl[R[lr_i] + 1];
    } else {
        auto gen = expandRuleLazy(2*(R[lr_i]-nt), left);
        for (char c : gen) {
            co_yield c;
        }
    }
}

bool PatternSearcher::compareRulesLazy(int i, int j, bool rev) {
    //cout << "Comparing rules " << i << " and " << j << endl;
    // Set up generators for each rule's expansion
    auto gen_i = expandRuleSideLazy(2 * i, rev);
    auto gen_j = expandRuleSideLazy(2 * j, rev);
    // Create iterators to lazily consume characters from both expansions
    auto it_i = gen_i.begin();
    auto it_j = gen_j.begin();
    // Compare characters from both sequences one by one
    while (it_i != gen_i.end() && it_j != gen_j.end()) {
        char char_i = *it_i;
        char char_j = *it_j;
        if (char_i != char_j) {
            return char_i < char_j;
        }
        ++it_i;
        ++it_j;
    }
    // If one sequence is shorter, the shorter one is considered "less"
    return (it_i == gen_i.end()) && (it_j != gen_j.end());
}

void PatternSearcher::secondaries(vector<int> *occurences, u_int A_i, u_int offset, bool terminal) {
    if (DEBUG) cout << "A_i: " << A_i << " offset: " << offset << " terminal: " << terminal << endl;
    if (!terminal && A_i == S) { // append the occurence to the report
        if (DEBUG) cout << "Appending " << offset << endl;
        occurences->push_back(offset);
        return;
    }
    int c = terminal? A_i: A_i + nt; // symbol representing the rule
    if (DEBUG) cout << "c: " << c << endl;
    if (DEBUG) cout << "number of " << c << " in R: " << R.rank(c, R.size()-1-1) << endl;
    for (int j=1; j <= R.rank(c, R.size()-1-1); j++) { // for each j-th occurence of c
        int k = R.select(c, j); // position of the j-th occurence of c
        int D_i = k / 2; // Rule index that derives the symbol at position k
        int offset_prime = offset;
        if (k % 2 == 1) { // if k is odd, then the symbol at position k is the right side of the rule D
            if ((u_int)R[k-1] < nt) 
                offset_prime += 1; // we add 1 if the left side of the rule D is a terminal
            else
                offset_prime += l[R[k-1] - nt]; // we add the length of the left side of the rule D        
        }
        secondaries(occurences, D_i, offset_prime, false);
    }
}
int PatternSearcher::ruleAt(int i)
{
    if (i < l[R[S*2] - nt]) { // rule is at left side
        return ruleAt(R[S*2] - nt, i);
    } else {
        return ruleAt(R[S*2+1] - nt, i - l[S]);
    }
}
int PatternSearcher::ruleAt(int r_i, int i)
{
    if (DEBUG) cout << "r_i: " << r_i << " i: " << i << " l[r_i]:" << l[r_i] << " l[r_i.left]: " << l[R[r_i*2] - nt] << " l[r_i.right]: " << l[R[r_i*2 + 1] - nt] << endl;
    if (R[r_i*2] < nt && i == 0) return r_i;
    if (R[r_i*2] < nt && R[r_i*2 + 1] < nt) return r_i;
    if (R[r_i*2] >= nt && i < l[R[r_i*2] - nt]) return ruleAt(R[r_i*2] - nt, i);
    if (R[r_i*2] >= nt && i >= l[R[r_i*2] - nt]) {
        if (R[r_i*2 + 1] < nt) return r_i;
        return ruleAt(R[r_i*2 + 1] - nt, i - l[R[r_i*2] - nt]);
    }   
};

void PatternSearcher::search(vector<int> *occurences, string P) {    
    u_int m = P.size();
    if (m == 1) {
        secondaries(occurences, rk[P.at(0)], 0, true);
    } else {
        u_int t;
        for (t = 1; t < m; t++) {
            string P_left = P.substr(0, t); // P_<
            string P_right = P.substr(t, m-t); // P_>
            if (DEBUG) cout << "P_<: " << P_left << endl;
            if (DEBUG) cout << "P_>: " << P_right << endl;
            uint s_x, e_x, s_y, e_y;
            int left = 0, right = G.getRows() - 1;
            int result = -1;
            while (left <= right) {
                int mid = left + (right - left) / 2;
                int r_i = mid;
                int compare = compareRuleWithPatternLazy(r_i, P_left, true);
                if (DEBUG) cout << "compare: " << compare << endl;
                if (compare >= 0) {
                    if (compare == 0) {
                        result = mid;
                    }
                    right = mid - 1;
                } else {
                    left = mid + 1;
                }
            }
            if (result == -1) continue;
            s_y = result + 1;
            if (DEBUG) cout << "s_y: " << s_y << endl;

            left = 0, right = G.getRows() - 1;
            result = -1;
            while (left <= right) {
                int mid = left + (right - left) / 2;
                int r_i = mid;
                int compare = compareRuleWithPatternLazy(r_i, P_left, true);
                if (DEBUG) cout << "compare: " << compare << endl;
                if (compare <= 0) {
                    if (compare == 0) {
                        result = mid;
                    }
                    left = mid + 1;
                } else {
                    right = mid - 1;
                }
            }
            if (result == -1) continue;
            e_y = result + 1;
            if (DEBUG) cout << "e_y: " << e_y << endl;

            left = 0, right = G.getColumns() - 1;
            result = -1;
            while (left <= right) {
                int mid = left + (right - left) / 2;
                int r_i = G.access(mid+1)-1;
                int compare = compareRuleWithPatternLazy(r_i, P_right);
                if (DEBUG) cout << "compare: " << compare << endl;
                if (compare >= 0) {
                    if (compare == 0) {
                        result = mid;
                    }
                    right = mid - 1;
                } else {
                    left = mid + 1;
                }
            }
            if (result == -1) continue;
            s_x = result + 1;
            if (DEBUG) cout << "s_x: " << s_x << endl;

            left = 0, right = G.getColumns() - 1;
            result = -1;
            while (left <= right) {
                int mid = left + (right - left) / 2;
                int r_i = G.access(mid+1)-1;
                int compare = compareRuleWithPatternLazy(r_i, P_right);
                if (DEBUG) cout << "compare: " << compare << endl;
                if (compare <= 0) {
                    if (compare == 0) {
                        result = mid;
                    }
                    left = mid + 1;
                } else {
                    right = mid - 1;
                }
            }
            if (result == -1) continue;
            e_x = result + 1;
            if (DEBUG) {
                cout << "e_x: " << e_x << endl;
                cout << "s_x: " << s_x << " e_x: " << e_x << " s_y: " << s_y << " e_y: " << e_y << endl;
                cout << "------------------------" << endl;
            }
            vector<Point> points = G.report(s_x, e_x, s_y, e_y);
            if (DEBUG) cout << "Points: " << points.size() << endl;
            for (Point p : points) {
                int r_i = p.second - 1;
                if (R[r_i*2] < nt) { //if left side is terminal
                    secondaries(occurences, r_i, 0);
                } else {
                    secondaries(occurences, r_i, l[R[r_i*2] - nt] - t);
                }
            }
        }
    }
}

template <typename Iterator>
inline int PatternSearcher::compareRuleWithPatternLazyImpl(int i, Iterator pattern_begin, Iterator pattern_end, bool rev) {      
    auto gen = expandRuleSideLazy(2*i, rev);
    auto it = gen.begin();
    while (it != gen.end() && pattern_begin != pattern_end) {
        char c = *it;
        char p = *pattern_begin;
        if (DEBUG) cout << "..comparing " << c << " with " << p << endl;
        if (c < p) return -1;
        if (c > p) return 1;   
        ++it;
        ++pattern_begin;
    }
    if (it == gen.end() && pattern_begin != pattern_end) return -1;  
    return 0;
}
int PatternSearcher::compareRuleWithPatternLazy(int i, string pattern, bool rev) {
    if (DEBUG) cout << "Comparing rule " << i << " with pattern " << pattern << " leftside?: " << rev << endl;
    if (rev) {
        return compareRuleWithPatternLazyImpl(i, pattern.rbegin(), pattern.rend(), rev);
    } else {
        return compareRuleWithPatternLazyImpl(i, pattern.begin(), pattern.end(), rev);
    }
}

// Explicit template instantiations for the required types
template int PatternSearcher::compareRuleWithPatternLazyImpl<std::string::iterator>(int, std::string::iterator, std::string::iterator, bool);
template int PatternSearcher::compareRuleWithPatternLazyImpl<std::string::reverse_iterator>(int, std::string::reverse_iterator, std::string::reverse_iterator, bool);