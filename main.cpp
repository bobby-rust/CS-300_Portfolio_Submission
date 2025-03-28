/**
 * Author: Bobby Rust
 */

#include <fstream>
#include <iostream>
#include <string>

#include "dag.h"

using std::endl;
using std::ifstream;
using std::string;

/**
 * Splits a string by a delimiter and returns the resulting vector
 *
 * https://stackoverflow.com/a/13172514
 */
std::vector<std::string> splitString(const std::string &str,
                                     const std::string &delimiter) {
    std::vector<std::string> strings;

    std::string::size_type pos = 0;
    std::string::size_type prev = 0;
    while ((pos = str.find(delimiter, prev)) != std::string::npos) {
        strings.push_back(str.substr(prev, pos - prev));
        prev = pos + delimiter.size();
    }

    // To get the last substring (or only, if delimiter is not found)
    strings.push_back(str.substr(prev));

    return strings;
}

/**
 * Returns the contents of the file located at path
 *
 * https://stackoverflow.com/a/2912614
 */
string loadFile(const string &path) {
    ifstream ifs(path);
    string content((std::istreambuf_iterator<char>(ifs)),
                   (std::istreambuf_iterator<char>()));

    return content;
}

/**
 *
 * The courses file is a CSV file where each row represents a course.
 * the first column is the course number, the second column is the course name,
 * and all following columns are its prerequisites.
 */
vector<Course> loadCourses(const string &fileContents) {
    vector<string> lines = splitString(fileContents, "\n");
    vector<Course> courses;

    for (const auto &line : lines) {
        vector<string> courseInfo = splitString(line, ",");

        // There may be blank lines, so let's not segfault by indexing past the
        // size of the vector
        if (courseInfo.size() < 2)
            continue;

        // The first column is the course number.
        string courseNumber = courseInfo[0];
        // The second column is the course name.
        string courseName = courseInfo[1];
        // The course prerequisits start in column 3 and go to the end.
        // hence the loop beginning at index 2.

        vector<string> prerequisites;
        for (size_t i = 2; i < courseInfo.size(); ++i) {
            prerequisites.push_back(courseInfo[i]);
        }
        Course course = {courseNumber, courseName, prerequisites};
        courses.push_back(course);
    }

    return courses;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        cerr << "Please supply a file containing the courses" << endl;
        return 0;
    }

    string filename = argv[1];

    string contents = loadFile(filename);
    vector<Course> courses = loadCourses(contents);

    DAG courseGraph;

    for (const auto &course : courses) {
        courseGraph.addCourse(course);
        for (const auto &prereq : course.prerequisites) {
            courseGraph.addPrerequisite(prereq, course.courseNumber);
        }
    }

    courseGraph.printSchedule();

    return 0;
}
