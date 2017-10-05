#include <string>
#include <sstream>
#include "Robot.h"
#include "serialib.h"

using namespace std;

Robot::Robot()
{
    port.Open(SERIAL_PORT, SERIAL_BAUDRATE);
}

void Robot::sendOrder(float leftMotor, float rightMotor)
{
    int left = (int)(leftMotor*255);
    int right = (int)(rightMotor*255);

    ostringstream oss;
    oss << "L" << left << "\n" << "R" << right << "\n";
	port.WriteString(oss.str().c_str());
}
