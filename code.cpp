#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <sstream>
using namespace std;
class Student {
public:
    string name;
    int rollNo;
    vector<pair<string, string>> preferences;
    int mains;
    int advance;

    Student(string n, int r, vector<pair<string, string>> prefs, int m, int a)
        : name(n), rollNo(r), preferences(prefs), mains(m), advance(a) {}
};
class PriorityQueue 
{
private:
    vector<Student*> data;
    bool isForIIT; 

    void heapifyUp(int index) {
        while (index > 0) {
            int parent = (index - 1) / 2;
            if (compare(data[index], data[parent])) {
                swap(data[index], data[parent]);
                index = parent;
            } else {
                break;
            }
        }
    }
    void heapifyDown(int index) {
        int left = 2 * index + 1;
        int right = 2 * index + 2;
        int smallest = index;

        if (left < data.size() && compare(data[left], data[smallest])) {
            smallest = left;
        }
        if (right < data.size() && compare(data[right], data[smallest])) {
            smallest = right;
        }
        if (smallest != index) {
            swap(data[index], data[smallest]);
            heapifyDown(smallest);
        }
    }

    bool compare(Student* s1, Student* s2) {
        if (isForIIT) {
            return s1->advance < s2->advance;
        } else {
            return s1->mains < s2->mains;
        }
    }

public:
    PriorityQueue(bool iitQueue) : isForIIT(iitQueue) {}

    void push(Student* student) {
        data.push_back(student);
        heapifyUp(data.size() - 1);
    }

    void pop() {
        if (!data.empty()) {
            data[0] = data.back();
            data.pop_back();
            heapifyDown(0);
        }
    }

    Student* top() {
        return data.empty() ? nullptr : data[0];
    }

    bool empty() {
        return data.empty();
    }
};
struct BNode {
    string bName;
    int availableSeats;
    BNode(string branch, int seats) : bName(branch), availableSeats(seats) {}
};
struct CNode {
    string cName;
    unordered_map<string, BNode*> branches;
    CNode* left;
    CNode* right;
    CNode(string college) : cName(college), left(NULL), right(NULL) {
        branches["CS"] = new BNode("CS", 1);
        branches["EC"] = new BNode("EC", 1);
        branches["EE"] = new BNode("EE", 1);
        branches["CV"] = new BNode("CV", 1);
        branches["ME"] = new BNode("ME", 1);
    }
};
class CTree {
public:
    CNode* root = NULL;
    CNode* insert(CNode* node, const string& college) {
        if (!node)
            return new CNode(college);

        if (college.find("IIT") != string::npos) {
            node->left = insert(node->left, college);
        } else if (college.find("NIT") != string::npos) {
            node->right = insert(node->right, college); 
        }

        return node;
    }

    void insertC(const string& college) {
        root = insert(root, college);
    }
    
    CNode* searchCollege(CNode* node, const string& collegeName) {
        if (!node) return NULL;
        if (node->cName == collegeName) return node;
        if (collegeName.find("IIT") != string::npos) return searchCollege(node->left, collegeName);
        return searchCollege(node->right, collegeName);
    }
    CNode* findCollege(const string& collegeName) {
        return searchCollege(root, collegeName);
    }
};
void allocateSeats(PriorityQueue& iitQueue, PriorityQueue& nitQueue, 
                   CTree& cTree, unordered_map<int, string>& allocationMap) {
   
    while (!iitQueue.empty()) {
        Student* student = iitQueue.top();
        iitQueue.pop();

        for (const pair<string,string>& pref : student->preferences) {
            const string& collegeName = pref.first;
            const string& branchName = pref.second;

            if (collegeName.find("IIT") != string::npos) {  
                CNode* college = cTree.findCollege(collegeName);
                if (college && college->branches.count(branchName) > 0) {
                    BNode* branch = college->branches[branchName];
                    if (branch->availableSeats > 0) {
                        allocationMap[student->rollNo] = "Allocated " + branch->bName + " at " + college->cName;
                        branch->availableSeats--;
                        break;
                    }
                }
            }
        }
    }
    while (!nitQueue.empty()) {
        Student* student = nitQueue.top();
        nitQueue.pop();

        for (const pair<string,string>& pref : student->preferences) {
            const string& collegeName = pref.first;
            const string& branchName = pref.second;

            if (collegeName.find("NIT") != string::npos) { 
                CNode* college = cTree.findCollege(collegeName);
                if (college && college->branches.count(branchName) > 0) {
                    BNode* branch = college->branches[branchName];
                    if (branch->availableSeats > 0) {
                        allocationMap[student->rollNo] = "Allocated " + branch->bName + " at " + college->cName;
                        branch->availableSeats--;
                        break;
                    }
                }
            }
        }
    }
}
int main() {
    CTree cTree;
    cTree.insertC("IIT1");
    cTree.insertC("IIT2");
    cTree.insertC("IIT3");
    cTree.insertC("IIT4");
    cTree.insertC("IIT5");
    cTree.insertC("NIT1");
    cTree.insertC("NIT2");
    cTree.insertC("NIT3");
    cTree.insertC("NIT4");
    cTree.insertC("NIT5");
    PriorityQueue iitQueue(true);  // IIT priority queue based on `advance` rank
    PriorityQueue nitQueue(false); // NIT priority queue based on `mains` rank
    unordered_map<int, string> allocationMap;
    int fa = 1;
    while(fa) {
        string name;
        int rollNo, mains, advance;
        cout << "Enter student name, roll no, mains rank, and advance rank: ";
        cin >> name >> rollNo >> mains >> advance;
        vector<pair<string, string>> preferences;
        cout << "Enter 10 preferences (e.g., IIT1 CS): \n";
        bool onlyIIT = true, onlyNIT = true;
        for (int j = 0; j < 10; j++) {
            string college, branch;
            cin >> college >> branch;
            preferences.push_back({college, branch});
            if (college.find("IIT") != string::npos) onlyNIT = false;
            if (college.find("NIT") != string::npos) onlyIIT = false;
        }

        Student* student = new Student(name, rollNo, preferences, mains, advance);

        if ((advance < mains && !onlyNIT) || (onlyIIT && mains < advance))
            iitQueue.push(student);
        else
            nitQueue.push(student);

        cout << "Enter 1 if you want to continue or else press 0: ";
        cin >> fa;
    }

    allocateSeats(iitQueue, nitQueue, cTree, allocationMap);

    int queryRollNo;
    string f = "Yes";
    while(f == "Yes") {
        cout << "Enter roll number to check allocation: ";
        cin >> queryRollNo;
        if (allocationMap.find(queryRollNo) != allocationMap.end()) {
            cout << "Allocation for Roll No " << queryRollNo << ": " << allocationMap[queryRollNo] << endl;
        } else {
            cout << "No allocation found for Roll No " << queryRollNo << endl;
        }
        cout << "Enter Yes to check for another student: ";
        cin >> f;
    }

    return 0;
}
