// main.cpp
#include "server.h"
#include <iostream>

int main() {
    try {
        ControlServer* server = new ControlServer();
        server->run("trajectory.txt");
    }
    catch (const std::exception& e) {
        std::cerr << "������: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}