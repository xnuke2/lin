// robot.h

#include <string>
#include <ctime>

class Robot {
public:
    struct Point {
        double x;
        double y;
    };

    enum class CommandType {
        MOVE,
        EMERGENCY_STOP,
        CHANGE_SPEED,
        STOP
    };

    struct Command {
        CommandType type;
        double x;
        double y;
        double speed;
        time_t timestamp;
    };

    Robot();
    ~Robot();

    void run();
    void stop();

private:
    void processCommand(const std::string& cmd);
    void sendStatus();
    void moveTo(double x, double y);
    void changeSpeed(double newSpeed);
    void emergencyStop();

    Point position;
    double speed;
    int battery;
    time_t lastUpdate;
    bool running;
    int fd_robot;
    int fd_control;
    static const std::string FIFO_ROBOT;
    static const std::string FIFO_CONTROL;
};

