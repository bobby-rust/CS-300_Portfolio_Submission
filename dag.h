#include <iostream>
#include <queue>
#include <string>
#include <unordered_map>
#include <vector>

#include "course.h"

using std::cerr;
using std::cout;
using std::endl;
using std::queue;
using std::runtime_error;
using std::string;
using std::unordered_map;
using std::vector;

class CourseNode {
  public:
    string courseNumber;
    string courseName;
    // The number of dependents, or prerequisite courses that must be taken
    // before this course can be taken
    int inDegree;

    // Prerequisite courses that must be completed before taking this course
    vector<CourseNode *> dependencies;

    // Parameterized constructor
    CourseNode(const Course *course)
        : courseNumber(course->courseNumber), courseName(course->courseName),
          inDegree(0) {};
};

class DAG {
  private:
    // String courseNumber, CourseNode *course
    unordered_map<string, CourseNode *> graph;

    bool graphContainsCourse(string courseNumber) {
        return graph.find(courseNumber) != graph.end();
    }

  public:
    void printGraph() {
        for (const auto &pair : graph) {
            for (const auto &node : pair.second->dependencies) {
                cout << node->courseNumber << " ";
            }
            cout << endl;
        }
    }
    void addCourse(const Course course) {

        if (graphContainsCourse(course.courseNumber))
            return;

        graph[course.courseNumber] = new CourseNode(&course);
    }

    void addPrerequisite(string prerequisiteCourseNumber, string courseNumber) {
        if (!graphContainsCourse(prerequisiteCourseNumber) ||
            !graphContainsCourse(courseNumber)) {
            cerr << "Error: Course(s) do not exist" << endl;
            return;
        };

        CourseNode *prerequisiteCourseNode = graph[prerequisiteCourseNumber];
        CourseNode *courseCourseNode = graph[courseNumber];

        // The courseCourseNode is dependent on the prerequisiteCourseNode
        // because the prerequisite course must be taken first
        courseCourseNode->dependencies.push_back(prerequisiteCourseNode);
        courseCourseNode->inDegree++;
    }

    vector<string> getCourseSchedule() {
        queue<CourseNode *> q;
        vector<string> courseSchedule;
        for (auto &pair : graph) {
            if (pair.second->inDegree == 0) {
                q.push(pair.second);
            }
        }

        while (!q.empty()) {
            CourseNode *current = q.front();
            q.pop();
            courseSchedule.push_back(current->courseNumber);

            for (auto &dep : graph) {
                for (auto it = dep.second->dependencies.begin();
                     it != dep.second->dependencies.end(); ++it) {
                    if (*it == current) {
                        dep.second->inDegree--;
                        dep.second->dependencies.erase(it);
                        if (dep.second->inDegree == 0) {
                            q.push(dep.second);
                        }
                        break;
                    }
                }
            }
        }

        if (courseSchedule.size() != graph.size()) {
            throw runtime_error("Cycle detected. Invalid course schedule.");
        }

        return courseSchedule;
    }

    void printSchedule() {
        try {
            vector<string> schedule = getCourseSchedule();
            cout << "Suggested course schedule:\n";
            for (const string &course : schedule) {
                cout << course << " ";
            }
            cout << endl;
        } catch (const runtime_error &e) {
            cerr << e.what() << endl;
        }
    }

    ~DAG() {
        for (auto &pair : graph) {
            delete pair.second;
        }
    }
};
