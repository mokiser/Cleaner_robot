#ifndef RECIEVER_H
#define RECIEVER_H

#include <string>
#include <iostream>

class ServerResponseHandler {
public:
	virtual void handleResponse(const std::string& response);
};

#endif // RECIEVER_H