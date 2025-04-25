// main.cpp
#include "server.h"
#include <iostream>

int main() {
    try {
        ControlServer server;
        server.run("trajectory.txt");
    }
    catch (const std::exception& e) {
        std::cerr << "Îøèáêà: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}