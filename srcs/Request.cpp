#include "../includes/Server.hpp"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>

void Server::EventsPollin(int fd, std::vector<Server>::iterator it)
{
	logger("POLLIN");
	std::cout << this->GetResponse(fd, it) << std::endl;
}
std::string Server::GetResponse(int fd, std::vector<Server>::iterator it)
{
	if (it->_donereading == false)
		this->RecieveMessage(fd, it);
	
	return (this->_response);
}

void Server::RecieveMessage(int fd, std::vector<Server>::iterator it)
{
	logger("NOT WORKING");
	logger("Ready to recieve...");
	
	std::string max = it->GetClientMax();
	int recvmax = std::atoi(max.c_str());
	char buff[10000];
	int rbytes = recv(fd, &buff, it->_recvmax, 0);
	if (rbytes == -1)
	{
		std::cout << "ERROR read" << std::endl;
		exit(EXIT_FAILURE);
	}
	it->_request.push_back(buff);
	if (rbytes < recvmax)
	{
		it->_donereading = true;
	}
	logger("message recieved!");
}