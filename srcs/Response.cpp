#include "../includes/Server.hpp"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include<fstream>
#include<string>
#include <sstream>

void Server::EventsPollout(int fd, int index, std::vector<Server>::iterator it)
{
	logger("POLLOUT");
	if (it->_whatsockvec[index] == "CLIENT")
	{
		logger("sending response to client...");
		//logger(it->_response);
		write(fd, it->_response.c_str(), it->_response.size());
		logger("response is sent to fd!");
		close(fd);
		logger("fd is closed and removed!");
		it->_response.clear();
		RmvSocket(index, it);
	}
}