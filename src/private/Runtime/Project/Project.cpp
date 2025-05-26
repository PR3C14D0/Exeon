#include "Runtime/Project/Project.h"

#include <string>

void Project::OpenProject(std::string filename) {
    std::ifstream file;
    file.open(filename);
    std::string line;
    while (std::getline(file, line)) {
        std::cout << line << std::endl;
    }
}
