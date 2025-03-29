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

    // Returns true if the graph contains a course with the given course number
    bool graphContainsCourse(string courseNumber) {
        return graph.find(courseNumber) != graph.end();
    }

  public:
    // Prints the graph in no specific order
    void printGraph() {
        for (const auto &pair : graph) {
            for (const auto &node : pair.second->dependencies) {
                cout << node->courseNumber << " ";
            }
            cout << endl;
        }
    }

    // Add a course to the graph
    void addCourse(const Course course) {
        if (graphContainsCourse(course.courseNumber))
            return;

        graph[course.courseNumber] = new CourseNode(&course);
    }

    // Add a prerequisite to a course that's already in the graph
    void addPrerequisite(string prerequisiteCourseNumber, string courseNumber) {
        if (!graphContainsCourse(prerequisiteCourseNumber) ||
            !graphContainsCourse(courseNumber)) {
            cerr << "Error: Course(s) do not exist" << endl;
            cerr << "Both courses must exist before adding a prerequisite"
                 << endl;
            return;
        };

        // Get the existing prerequisite node
        CourseNode *prerequisiteCourseNode = graph[prerequisiteCourseNumber];
        // Get the existing course node
        CourseNode *courseCourseNode = graph[courseNumber];

        // The courseCourseNode is dependent on the prerequisiteCourseNode
        // because the prerequisite course must be taken first
        courseCourseNode->dependencies.push_back(prerequisiteCourseNode);
        courseCourseNode->inDegree++;
    }

    // Gets a valid course schedule such that no course is taken
    // before all of its prerequisites are met
    vector<string> getCourseSchedule() {
        queue<CourseNode *> q;
        vector<string> courseSchedule;

        // Add courses that have no prerequisites to the queue first
        for (auto &pair : graph) {
            // An inDegree of 0 means no prerequisites
            if (pair.second->inDegree == 0) {
                // pair is [courseNumber, courseNode]
                q.push(pair.second);
            }
        }

        // When starting, the queue only contains courses with no prerequisites
        while (!q.empty()) {
            // Next course to take
            CourseNode *current = q.front();
            q.pop(); // pop does not return a value
            courseSchedule.push_back(current->courseNumber);

            // Check if the current course has any dependencies
            // For each course in the graph
            for (auto &courseMap : graph) {
                // courseMap is [courseNumber, courseNode]
                CourseNode *course = courseMap.second;
                // For each dependency of the current course
                for (auto it = course->dependencies.begin();
                     it != course->dependencies.end(); ++it) {
                    // If the current course is dependent on this course
                    if (*it == current) {
                        // Remove the dependency from the current course
                        // As we are processing it
                        course->inDegree--;
                        course->dependencies.erase(it);
                        // If all dependencies have been met, add the current
                        // course to the queue
                        if (course->inDegree == 0) {
                            q.push(course);
                        }
                        break;
                    }
                }
            }
        }

        // The number of courses in the schedule should be equal to the number
        // of courses in the graph.
        if (courseSchedule.size() != graph.size()) {
            throw runtime_error("Cycle detected. Invalid course schedule.");
        }

        return courseSchedule;
    }

    // Prints a valid course schedule
    void printSchedule() {
        try {
            vector<string> schedule = getCourseSchedule();
            cout << "Suggested course schedule:\n";
            for (const string &course : schedule) {
                cout << course << '\n';
            }
            cout << endl; // Flush buffer
        } catch (const runtime_error &e) {
            cerr << e.what() << endl;
        }
    }

    // Destructor
    ~DAG() {
        for (auto &pair : graph) {
            delete pair.second;
        }
    }
};
