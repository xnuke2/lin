// robot_main.cpp
#include "robot.h"
#include <iostream>

int main() {
    try {
        Robot robot;
        robot.run();
    }
    catch (const std::exception& e) {
        std::cerr << "Ошибка робота: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}