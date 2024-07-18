#include "../includes/Server.hpp"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include<fstream>
#include<string>
#include <sstream>

void Server::EventsPollout(int fd, int index)
{
	logger("POLLOUT");
	if (this->_response.size() > 0)
	{
		logger("sending response to client...");
		write(fd, this->_response.c_str(), this->_response.size());
		logger("response is sent to fd!");
		close(fd);
		logger("fd is closed and removed!");
		this->_response.clear();
		this->_donereading = false;
		this->RmvSocket(index); //if removed, i believe the connection is there and so the the client doesn't have to reload, need to test it tho...
	}
}