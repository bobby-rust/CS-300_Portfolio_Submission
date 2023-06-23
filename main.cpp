/**
 * Author: Bobby Rust
 * 
 * --- Note: Please use -std=c++14 or higher compiler flag ---
*/

#include <iostream>
#include <fstream>
#include <string>
#include <sstream> 
#include <vector>
#include <algorithm>
#include <memory>
#include <limits>

using namespace std;

template <typename T>
bool vectorContains(const vector<T>& vec, const T& value) {
    return count(vec.begin(), vec.end(), value) != 0;
}

struct Course {
    string courseNumber;
    string courseName;
    shared_ptr<vector<shared_ptr<Course>>> prerequisites; 
    
    Course() {
        prerequisites = make_shared<vector<shared_ptr<Course>>>();
    }
};

// Returns true if c1 is a prerequisite of c2
bool isPrerequisiteOf(const shared_ptr<Course> c1, const shared_ptr<Course> c2) {
    return vectorContains(*c2->prerequisites, c1);
}

// data structure for bst node
struct Node {
    shared_ptr<Course> course;
    Node* left;
    Node* right;

    // default constructor
    Node() {
        left = nullptr;
        right = nullptr;            
    }

    // parameterized constructor
    Node(const shared_ptr<Course> aCourse) : Node() {
        course = aCourse;
    }
};

class BST {
private:
    Node* root;
    void addNode(Node* node, shared_ptr<Course> course);
    void inOrder(const Node* node) const;
    void deleteSubtree(Node* node);
    Node* findRelatedNode(Node* node, const shared_ptr<Course> course) const;
    Node* findDependentCourse(Node* node, const shared_ptr<Course> course) const;
public:
    BST();
    virtual ~BST();
    void InOrder() const;
    void Insert(const shared_ptr<Course> course);
    Node* Search(Node* node, const string courseNumber) const;
    Node* getRoot();
};

Node* BST::getRoot() {
    return this->root;
}

/**
 * Performs a reverse inorder traversal of tree with root = node (searches subtree with root = node) until finding a related node
 * A related node is a relationship between two nodes in which one node is a prerequisite of the other
*/
Node* BST::findRelatedNode(Node* node, const shared_ptr<Course> course) const {

    if (node == nullptr) return nullptr;

    Node* relatedNode = findRelatedNode(node->right, course);

    if (relatedNode != nullptr) return relatedNode;

    if ((isPrerequisiteOf(node->course, course)) || (isPrerequisiteOf(course, node->course))) {
        return node;
    }

    return findRelatedNode(node->left, course);
}

/**
 * Add a course to some node (recursive)
 *  
 * Will attempt to maintain prerequisite constraint
 * @param node Current node in tree
 * @param course course to be added
 */
void BST::addNode(Node* node, const shared_ptr<Course> course) {

    if (this->root == nullptr) this->root = new Node(course);

    Node* relatedNode = findRelatedNode(node, course);

    if (relatedNode == nullptr) {
        if (course->courseNumber.compare(node->course->courseNumber) < 0) {
            if (node->left == nullptr) {
                node->left = new Node(course);
            } else {
                addNode(node->left, course);
            }
        } else if (course->courseNumber.compare(node->course->courseNumber) > 0) {
            if (node->right == nullptr) {
                node->right = new Node(course);
            } else {
                addNode(node->right, course);
            }
        }
    } else if (isPrerequisiteOf(relatedNode->course, course)) {
        if (relatedNode->right == nullptr) {
            relatedNode->right = new Node(course);
        } else {
            addNode(relatedNode->right, course);
        }
    } else if (isPrerequisiteOf(course, relatedNode->course)) {
        if (relatedNode->left == nullptr) {
            relatedNode->left = new Node(course);
        } else {
            addNode(relatedNode->left, course);
        }
    } else {
        cout << "Likely duplicate course detected." << endl;
    }
}

void BST::inOrder(const Node* node) const {      
    if (node != nullptr) {
        inOrder(node->left);

        // output course information
        cout << node->course->courseNumber << ": " << node->course->courseName << endl;
        
        inOrder(node->right);
    }
}

// Default constructor
BST::BST() {
    this->root = nullptr;
}

// Delete a subtree
void BST::deleteSubtree(Node* node) {
    if (node == nullptr) {
        return;
    }

    deleteSubtree(node->left);
    deleteSubtree(node->right);
    delete node;
}

BST::~BST() {
    deleteSubtree(this->root);
}

void BST::InOrder() const {
    inOrder(this->root);
}

// Insert a node
void BST::Insert(const shared_ptr<Course> course) {
    if (root == nullptr) {
        root = new Node(course);
    } else {
        addNode(root, course);
    }
}

// Returns the dependent courses of given course
// In other words, courses that have the given course as a prerequisite
Node* BST::findDependentCourse(Node* node, const shared_ptr<Course> course) const {
    if (node == nullptr) return nullptr;

    Node* relatedNode = findDependentCourse(node->left, course);

    if (relatedNode != nullptr) return relatedNode;

    if (isPrerequisiteOf(course, node->course)) {
        return node;
    }

    return findDependentCourse(node->right, course);
}

/**
 * In order traversal to find nodes because I could not figure out a 
 * log n time search with the prerequisite constraint
*/
Node* BST::Search(Node* node, const string courseNumber) const {

    if (node == nullptr || node->course->courseNumber == courseNumber)
        return node;

    Node* leftResult = Search(node->left, courseNumber);
    if (leftResult != nullptr)
        return leftResult;

    Node* rightResult = Search(node->right, courseNumber);
    if (rightResult != nullptr)
        return rightResult;

    return nullptr;
}

vector<vector<string>> loadFile(const string filename) {
    // Open file 
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Unable to open file." << endl;
        exit(EXIT_FAILURE);
    }

    cout << "File successfully opened. Reading contents." << endl;
    
    // Read file contents
    string currLine;
    vector<vector<string>> fileContents; // 2d vector to store file contents

    // For each line in file
    while (getline(file, currLine)) {
        stringstream ss(currLine);
        string currElement;
        vector<string> line;
        
        // For each element in line
        while (getline(ss, currElement, ',')) {
            line.push_back(currElement);
        }

        fileContents.push_back(line);
    }
    
    return fileContents;
}

vector<string> getValidCourses(const vector<vector<string>>& fileContents) {
    vector<string> validCourses;
    for (const auto& line : fileContents) {
        if (line.size() > 0) {
            validCourses.push_back(line[0]);
        }
    }

    return validCourses;
}

shared_ptr<Course> getCourse(const unique_ptr<vector<shared_ptr<Course>>>& courses, const string courseNumber) {
    for (const auto& course : *courses) {
        if (course->courseNumber == courseNumber) {
            return course;
        }
    }

    return nullptr;
}

void loadPrerequisites(unique_ptr<vector<shared_ptr<Course>>>& courses, const vector<vector<string>>& fileContents) {
    for (const auto& line : fileContents) {
        if (line.size() < 3) continue;

        auto currCourse = getCourse(courses, line[0]);

        for (int i = 2; i < line.size(); ++i) {
            auto currPrereq = getCourse(courses, line[i]);
            auto validCourses = getValidCourses(fileContents);

            if (vectorContains(validCourses, currPrereq->courseNumber)) {
                currCourse->prerequisites->push_back(currPrereq);
            } else {
                cerr << "Invalid prerequisite course detected. Please check file contents." << endl;
                exit(EXIT_FAILURE);
            }            
        }
    }
}   

// Unique ptr to a vector of shared ptrs to courses
unique_ptr<vector<shared_ptr<Course>>> loadCourses(const vector<vector<string>>& fileContents) {
    auto courses = make_unique<vector<shared_ptr<Course>>>();
    auto validCourses = getValidCourses(fileContents);

    for (const auto& line : fileContents) {
        shared_ptr<Course> currCourse = make_shared<Course>();
        if (line.size() > 0) {
            currCourse->courseNumber = line[0];
            currCourse->courseName = line[1];
        }
        courses->push_back(currCourse);
    }
    
    loadPrerequisites(courses, fileContents);

    // Sort by number of prerequisites so that courses with the most prerequisites are added to the BST first
    sort(courses->begin(), courses->end(), [](const shared_ptr<Course>& course1, const shared_ptr<Course>& course2) -> bool {
        return course1->prerequisites->size() > course2->prerequisites->size();
    });

    return courses;
}

unique_ptr<BST> loadBST(const unique_ptr<vector<shared_ptr<Course>>>& courses) {
    unique_ptr<BST> bst = make_unique<BST>();

    // for course in courses, add to bst
    for (const auto& course : *courses) {
        bst->Insert(course);
    }
    return bst;
}

void printMenu() {
    cout << "Welcome to the course planner." << endl << endl;
    cout << "\t1. Load Data Structure" << endl;
    cout << "\t2. Print Course List" << endl;
    cout << "\t3. Print Course" << endl;
    cout << "\t4. Exit" << endl << endl;
}

int main() {
    // Get filename from user
    string filename;
    cout << "Enter the name of the data file: ";
    getline(cin, filename);

    auto fileContents = loadFile(filename);
    auto courses = loadCourses(fileContents);

    string userInput;
    bool userIsDone = false;
    unique_ptr<BST> bst;
    Node* userNode;

    while (!userIsDone) {
        printMenu();
        cout << "What would you like to do? ";
        cin >> userInput;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        string userCourse;

        switch (stoi(userInput)) {
            case 1:
                bst = loadBST(courses);
                cout << "Data structure successfully loaded." << endl;
                break;
            case 2:
                if (bst == nullptr) {
                    cout << "Please load the data structure before attempting to traverse it." << endl;
                    break;
                }
                cout << "Here is a sample schedule:" << endl << endl;
                bst->InOrder();
                cout << endl;
                break;
            case 3:
                if (bst == nullptr) { 
                    cout << "Please load the data structure before attempting to traverse it." << endl;
                    break;
                }
                cout << "What course do you want to know about? " << endl;
                cin >> userCourse;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                userNode = bst->Search(bst->getRoot(), userCourse);
                if (userNode != nullptr) {
                    cout << userNode->course->courseNumber << ", " << userNode->course->courseName << endl;
                    if (userNode->course->prerequisites->size() != 0) {
                        cout << "Prerequisites: ";
                        for (int i = 0; i < userNode->course->prerequisites->size(); ++i) {
                            if (i == userNode->course->prerequisites->size() - 1) {
                                cout << userNode->course->prerequisites->at(i)->courseNumber << endl;
                            } else {
                                cout << userNode->course->prerequisites->at(i)->courseNumber << ", ";
                            }
                        }
                    }
                } else {
                    cout << "Course \"" << userCourse << "\" not found." << endl;
                }
                break;
            case 4:
                userIsDone = true;
                break;
        }

    }

    cout << "Thank you for using the course planner!" << endl;
    return EXIT_SUCCESS;
}