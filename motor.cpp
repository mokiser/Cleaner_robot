#include "motor.h"

void MotorController::moveForward(int time) {
	std::cout << "[Motor] Moving forward for " << time << " ms" << std::endl;
	// Здесь будет код для отправки команды на сервер
}

void MotorController::moveBackward(int time) {
	std::cout << "[Motor] Moving backward for " << time << " ms" << std::endl;
	// Здесь будет код для отправки команды на сервер
}

void MotorController::turnRight(int time) {
	std::cout << "[Motor] Turning right for " << time << " ms" << std::endl;
	// Здесь будет код для отправки команды на сервер
}

void MotorController::turnLeft(int time) {
	std::cout << "[Motor] Turning left for " << time << " ms" << std::endl;
	// Здесь будет код для отправки команды на сервер
}

void MotorController::stop() {
	std::cout << "[Motor] Stopping" << std::endl;
	// Здесь будет код для отправки команды на сервер
}