#ifndef SYSTEM_H
#define SYSTEM_H

#include "motor.h"
#include "reciever.h"
#include <thread>
#include <chrono>
#include <atomic>
#include <iostream>

class System {
public:
	System();
	void run();

private:
	MotorController motorController;
	ServerResponseHandler responseHandler;

	std::atomic<bool> isRunning; // Флаг для остановки команды
	std::atomic<bool> isProgramRunning; // Флаг для управления выполнением программы

	void executeCommand(int time, int direction);
	void stopMovement();
	void displayMenu() const;
};

#endif // SYSTEM_H