// plagiarism_enhanced.cpp
// Enhanced version of your plagiarism detector
// - Synonym matching (simple dictionary-based normalization)
// - Command-line menu (Scan / Show Report / Export Report / Exit)
// - ASCII table with borders, animated loading spinner
// - Final summary page with total files & comparisons
//
// Compile with: g++ -std=c++17 plagiarism_enhanced.cpp -o plagiarism_enhanced

#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string>
#include <thread>
#include <chrono>

using namespace std;
namespace fs = std::filesystem;

// ANSI Colors
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define YELLOW  "\033[33m"
#define GREEN   "\033[32m"
#define CYAN    "\033[36m"
#define MAGENTA "\033[35m"

// ======= Config =======
const int MAX_FILES = 200;
const int MAX_TOKENS = 20000;
const int MAX_FPS = 20000;
const int K_GRAM = 3;

// ========================== LINKED LIST NODE FOR FINGERPRINTS =========================
struct Node {
    long long fingerprint;
    string fileID;
    Node* next;
};

// Insert at end of linked list (used for fingerprints)
void insert(Node** head, long long fp, const string& fileID) {
    Node* newNode = new Node{ fp, fileID, nullptr };
    if (*head == nullptr) {
        *head = newNode;
    }
    else {
        Node* temp = *head;
        while (temp->next) temp = temp->next;
        temp->next = newNode;
    }
}

// Count fingerprints of a file
int countFileFingerprints(Node* head, const string& file) {
    int cnt = 0;
    for (Node* temp = head; temp; temp = temp->next)
        if (temp->fileID == file) cnt++;
    return cnt;
}

// Count common fingerprints between two files
int countIntersection(Node* head, const string& fileA, const string& fileB) {
    int common = 0;
    for (Node* i = head; i; i = i->next) {
        if (i->fileID != fileA) continue;
        for (Node* j = head; j; j = j->next) {
            if (j->fileID != fileB) continue;
            if (i->fingerprint == j->fingerprint) {
                common++;
                break; // avoid double count
            }
        }
    }
    return common;
}

// ========================== LINKED LIST QUEUE FOR TOKENS =========================
struct TokenNode {
    string word;
    TokenNode* next;
};

// Queue struct to preserve order
struct TokenQueue {
    TokenNode* front;
    TokenNode* rear;
};

// Initialize queue
void initQueue(TokenQueue* q) {
    q->front = q->rear = nullptr;
}

// Enqueue token at the end
void enqueue(TokenQueue* q, const string& word) {
    TokenNode* newNode = new TokenNode{ word, nullptr };
    if (!q->rear) {
        q->front = q->rear = newNode;
    }
    else {
        q->rear->next = newNode;
        q->rear = newNode;
    }
}

// Convert token queue to array
int queueToArray(TokenQueue* q, string arr[], int maxSize) {
    int count = 0;
    TokenNode* temp = q->front;
    while (temp && count < maxSize) {
        arr[count++] = temp->word;
        temp = temp->next;
    }
    return count;
}

// ========================== READ FILE CONTENT ==============================
string readFile(const string& path) {
    ifstream file(path, ios::binary);
    if (!file.is_open()) return "";
    stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// ========================== SYNONYM DICTIONARY =============================
// Simple small dictionary for demo. You can expand this list.
struct SynEntry {
    const char* canonical;
    const char* variants[8]; // up to 7 variants + null
};

SynEntry synonyms[] = {
    { "big",    {"big", "large", "huge", "vast", nullptr} },
    { "small",  {"small", "tiny", "mini", "little", nullptr} },
    { "fast",   {"fast", "quick", "speedy", nullptr} },
    { "slow",   {"slow", "sluggish", nullptr} },
    { "start",  {"start", "begin", "commence", nullptr} },
    { "end",    {"end", "finish", "terminate", nullptr} },
    { "use",    {"use", "utilize", "utilise", nullptr} },
    { nullptr,  {nullptr} }
};

// Normalize token by mapping known synonyms to canonical term
string normalizeToken(const string& tok) {
    if (tok.empty()) return tok;
    // lower already ensured in tokenizer, but we keep defensive step
    string s = tok;
    for (char& c : s) c = tolower(c);

    // check synonyms
    for (int i = 0; synonyms[i].canonical != nullptr; i++) {
        const char* canon = synonyms[i].canonical;
        for (int j = 0; synonyms[i].variants[j] != nullptr; j++) {
            string var = synonyms[i].variants[j];
            if (s == var) return string(canon);
        }
    }
    return s;
}

// ========================== BUILD K-GRAM FINGERPRINTS =======================
void buildFingerprints(string tokens[], int n, long long fps[], int* fpCount, int k = K_GRAM) {
    *fpCount = 0;
    if (n < k) return;
    for (int i = 0; i <= n - k; i++) {
        string gram = "";
        for (int j = 0; j < k; j++) gram += tokens[i + j] + "#";
        long long h = 0;
        for (unsigned char c : gram) h = h * 131 + c;
        fps[(*fpCount)++] = h;
    }
}

// ========================== SIMILARITY CALCULATION =========================
double similarity(Node* head, const string& fileA, const string& fileB) {
    int common = countIntersection(head, fileA, fileB);
    int total = max(countFileFingerprints(head, fileA), countFileFingerprints(head, fileB));
    if (total == 0) return 0.0;
    return (double)common / total * 100.0;
}

// ========================== PROGRESS BAR / LOADING SPINNER ==================
void progressBar(int current, int total) {
    int width = 40;
    float ratio = (total == 0) ? 1.0f : (float)current / total;
    if (ratio > 1.0f) ratio = 1.0f;
    int filled = (int)(ratio * width);
    cout << "\r[";
    for (int i = 0; i < filled; i++) cout << "#";
    for (int i = filled; i < width; i++) cout << "-";
    cout << "] " << fixed << setprecision(0) << ratio * 100 << "%";
    cout.flush();
}

void spinnerAnimation(const string& msg, int ms = 1000) {
    const char spinChars[] = { '|','/','-','\\' };
    cout << msg << " ";
    int cycles = max(1, ms / 100);
    for (int i = 0; i < cycles; i++) {
        cout << "\b" << spinChars[i % 4];
        cout.flush();
        this_thread::sleep_for(chrono::milliseconds(100));
    }
    cout << "\b "; // remove spinner
    cout << endl;
}

// ========================== REPORT STORAGE =================================
struct PairResult {
    string a;
    string b;
    double sim;
};

PairResult results[MAX_FILES * MAX_FILES / 2];
int resultCount = 0;
int totalFilesGlobal = 0;
int totalComparisonsGlobal = 0;

// ========================== MAIN FUNCTIONALITY =============================
void scanFolderAndProcess(const string& folder, Node*& allFingerprints, string files[], int& fileCount) {
    fileCount = 0;
    allFingerprints = nullptr;
    resultCount = 0;

    if (!fs::exists(folder)) {
        cout << RED << "❌ Folder not found: " << RESET << folder << endl;
        return;
    }

    // Count files
    int totalFiles = 0;
    for (auto& entry : fs::directory_iterator(folder))
        if (entry.is_regular_file()) totalFiles++;
    if (totalFiles == 0) {
        cout << RED << "❌ No files found in folder!" << RESET << endl;
        return;
    }

    // Animated loading screen (brief)
    spinnerAnimation("Preparing to scan files...", 800);

    cout << CYAN << "\nReading and processing files...\n" << RESET;

    int processed = 0;
    for (auto& entry : fs::directory_iterator(folder)) {
        if (entry.is_regular_file()) {
            string filename = entry.path().filename().string();
            files[fileCount++] = filename;

            string content = readFile(entry.path().string());

            // Tokenize using queue with normalization (synonym mapping)
            TokenQueue tokens;
            initQueue(&tokens);
            string word = "";
            for (unsigned char c : content) {
                if (isalnum(c)) word += (char)tolower(c);
                else if (!word.empty()) {
                    string norm = normalizeToken(word);
                    enqueue(&tokens, norm);
                    word.clear();
                }
            }
            if (!word.empty()) {
                string norm = normalizeToken(word);
                enqueue(&tokens, norm);
                word.clear();
            }

            // Convert queue to array
            static string tokenArr[MAX_TOKENS];
            int nTokens = queueToArray(&tokens, tokenArr, MAX_TOKENS);

            // Build fingerprints
            static long long fps[MAX_FPS];
            int fpCount;
            buildFingerprints(tokenArr, nTokens, fps, &fpCount);

            for (int i = 0; i < fpCount; i++)
                insert(&allFingerprints, fps[i], filename);

            processed++;
            progressBar(processed, totalFiles);
            this_thread::sleep_for(chrono::milliseconds(80)); // small pause so progress is visible
        }
    }

    cout << "\n" << GREEN << "✔ Files scanned: " << fileCount << RESET << endl;

    totalFilesGlobal = fileCount;
    totalComparisonsGlobal = (fileCount * (fileCount - 1)) / 2;
}

// Print a table with borders for results
void printTableResults() {
    if (resultCount == 0) {
        cout << YELLOW << "No comparison results available. Please scan a folder first.\n" << RESET;
        return;
    }

    // Table header
    const int w1 = 25, w2 = 25, w3 = 12;
    cout << string(w1 + w2 + w3 + 7, '=') << "\n";
    cout << "| " << left << setw(w1) << "File 1"
        << " | " << left << setw(w2) << "File 2"
        << " | " << left << setw(w3) << "Similarity" << " |\n";
    cout << string(w1 + w2 + w3 + 7, '=') << "\n";

    // Rows
    for (int i = 0; i < resultCount; i++) {
        string color = GREEN;
        if (results[i].sim >= 60) color = RED;
        else if (results[i].sim >= 30) color = YELLOW;

        cout << "| " << left << setw(w1) << results[i].a
            << " | " << left << setw(w2) << results[i].b
            << " | " << color << right << setw(w3 - 1) << fixed << setprecision(2) << results[i].sim << "%" << RESET
            << " |\n";
    }

    cout << string(w1 + w2 + w3 + 7, '=') << "\n";
}

// Compare files and populate results array (and optionally print progress)
void compareFiles(Node* allFingerprints, string files[], int fileCount) {
    if (fileCount <= 1) {
        cout << YELLOW << "Not enough files to compare (need at least 2).\n" << RESET;
        return;
    }

    spinnerAnimation("Preparing comparisons...", 700);
    cout << CYAN << "\nComparing files...\n" << RESET;

    int totalComparisons = (fileCount * (fileCount - 1)) / 2;
    int done = 0;
    resultCount = 0;

    for (int i = 0; i < fileCount; i++) {
        for (int j = i + 1; j < fileCount; j++) {
            double sim = similarity(allFingerprints, files[i], files[j]);

            // Save result
            results[resultCount++] = { files[i], files[j], sim };

            done++;
            progressBar(done, totalComparisons);
            this_thread::sleep_for(chrono::milliseconds(90)); // visible progress
        }
    }

    cout << "\n\n" << GREEN << "Comparison complete." << RESET << endl;
}

// Export CSV from results[]
void exportCSV(const string& fname = "plagiarism_report.csv") {
    if (resultCount == 0) {
        cout << YELLOW << "No results to export. Please run comparisons first.\n" << RESET;
        return;
    }
    ofstream csv(fname);
    csv << "File1,File2,Plagiarism\n";
    for (int i = 0; i < resultCount; i++) {
        csv << results[i].a << "," << results[i].b << "," << results[i].sim << "\n";
    }
    csv.close();
    cout << GREEN << " CSV file generated: " << fname << RESET << endl;
}

// Final summary page
void printSummary(int fileCount) {
    cout << MAGENTA << "\n===== FINAL SUMMARY =====\n" << RESET;
    cout << "Total Files: " << fileCount << "\n";
    cout << "Total Comparisons: " << (fileCount * (fileCount - 1)) / 2 << "\n\n";
}

// ========================== MAIN PROGRAM ===================================
int main() {
    Node* allFingerprints = nullptr;
    string files[MAX_FILES];
    int fileCount = 0;
    bool hasScanned = false;

    while (true) {
        // Command-line menu
        cout << CYAN << "\n================ PLAGIARISM DETECTOR =================\n" << RESET;
        cout << "1. Scan Folder\n";
        cout << "2. Show Plagiarism Report\n";
        cout << "3. Export Report (CSV)\n";
        cout << "0. Exit\n";
        cout << CYAN << "Choose an option: " << RESET;

        int choice;
        if (!(cin >> choice)) {
            cin.clear();
            string dummy;
            getline(cin, dummy);
            cout << RED << "Invalid input. Try again.\n" << RESET;
            continue;
        }

        if (choice == 0) {
            cout << CYAN << "Exiting... Goodbye!\n" << RESET;
            break;
        }

        if (choice == 1) {
            string folder;
            cout << CYAN << "Enter folder name containing student files: " << RESET;
            cin >> folder;

            // Clear previous data if any
            allFingerprints = nullptr;
            fileCount = 0;

            scanFolderAndProcess(folder, allFingerprints, files, fileCount);
            if (fileCount > 0) {
                compareFiles(allFingerprints, files, fileCount);
                hasScanned = true;
                printSummary(fileCount);
            }
        }
        else if (choice == 2) {
            if (!hasScanned || resultCount == 0) {
                cout << YELLOW << "No report available. Please scan a folder first (Option 1).\n" << RESET;
            }
            else {
                printTableResults();
                // Print top summary
                cout << "\n";
                // Show highest plagiarism pair
                double highest = -1.0;
                int idx = -1;
                for (int i = 0; i < resultCount; i++) {
                    if (results[i].sim > highest) { highest = results[i].sim; idx = i; }
                }
                if (idx != -1) {
                    cout << MAGENTA << "Highest Plagiarism: " << RESET
                        << results[idx].a << " <-> " << results[idx].b
                        << " : " << fixed << setprecision(2) << highest << "%\n";
                }
                printSummary(totalFilesGlobal);
            }
        }
        else if (choice == 3) {
            if (!hasScanned || resultCount == 0) {
                cout << YELLOW << "Nothing to export. Please scan & compare first.\n" << RESET;
            }
            else {
                exportCSV();
            }
        }
        else {
            cout << RED << "Invalid option. Choose 0,1,2 or 3.\n" << RESET;
        }
    }

    return 0;
}
