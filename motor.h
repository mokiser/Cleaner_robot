#ifndef MOTOR_H
#define MOTOR_H

#include <string>
#include <iostream>

class MotorController {
public:
	virtual void moveForward(int time);
	virtual void moveBackward(int time);
	virtual void turnRight(int time);
	virtual void turnLeft(int time);
	virtual void stop();
};

#endif // MOTOR_H