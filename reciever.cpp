#include "reciever.h"

void ServerResponseHandler::handleResponse(const std::string& response) {
	std::cout << "[Receiver] Received response from server: " << response << std::endl;
}