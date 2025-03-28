#include <string>
#include <vector>

using std::string;
using std::vector;

typedef struct {
    string courseNumber;
    string courseName;
    vector<string> prerequisites;
} Course;
