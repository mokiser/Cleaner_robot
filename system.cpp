#include "system.h"
#include <iostream>

System::System() : isRunning(false), isProgramRunning(true) {}

void System::run() {
	while (isProgramRunning) {
		displayMenu();

		int choice;
		std::cin >> choice;

		if (choice == 6) {
			isProgramRunning = false; // Завершаем программу
			continue;
		}

		int time = 0;
		if (choice >= 1 && choice <= 4) {
			std::cout << "Enter time (in ms): ";
			std::cin >> time;
		}

		switch (choice) {
		case 1:
			executeCommand(time, 1); // 1 - вперед
			break;
		case 2:
			executeCommand(time, 2); // 2 - назад
			break;
		case 3:
			executeCommand(time, 3); // 3 - вправо
			break;
		case 4:
			executeCommand(time, 4); // 4 - влево
			break;
		case 5:
			stopMovement();
			break;
		default:
			std::cout << "Invalid choice. Try again.\n";
			continue;
		}

		// Пример обработки ответа от сервера
		responseHandler.handleResponse("Command executed");
	}

	std::cout << "Exiting program.\n";
}

void System::executeCommand(int time, int direction) {
	isRunning = true;

	// Запускаем команду в отдельном потоке
	std::thread commandThread([this, time, direction]() {
		auto start = std::chrono::steady_clock::now();
		while (isRunning && std::chrono::steady_clock::now() - start < std::chrono::milliseconds(time)) {
			std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Симуляция работы
		}

		if (isRunning) {
			std::cout << "[System] Command completed.\n";
		}
		else {
			std::cout << "[System] Command stopped.\n";
		}
	});

	commandThread.detach(); // Отсоединяем поток, чтобы он работал независимо
}

void System::stopMovement() {
	if (isRunning) {
		isRunning = false;
		std::cout << "[System] Stopping command...\n";
	}
	else {
		std::cout << "[System] No command to stop.\n";
	}
}

void System::displayMenu() const {
	std::cout << "Choose an action:\n";
	std::cout << "1. Move forward\n";
	std::cout << "2. Move backward\n";
	std::cout << "3. Turn right\n";
	std::cout << "4. Turn left\n";
	std::cout << "5. Stop\n";
	std::cout << "6. Exit\n";
	std::cout << "Enter your choice: ";
}	