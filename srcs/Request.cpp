#include "../includes/Server.hpp"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>

void Server::EventsPollin(int fd, int index)
{
	logger("POLLIN");
	if (this->_whatsockvec[index] == "CLIENT")
	{
		std::cout << this->GetResponse(fd, index) << std::endl;
		return ;
	}

}
std::string Server::GetResponse(int fd, int index)
{
	this->RecieveMessage(fd, index);
	return (this->_response);
}

void Server::RecieveMessage(int fd, int index)
{
	logger("NOT WORKING");
	logger("Ready to recieve...");
	if (index != 0)
		index = 0;

	char buff[10000];
	int nbytes = recv(fd, &buff, sizeof(buff), 0);
	if (nbytes == -1)
	{
		std::cout << "ERROR read" << std::endl;
		exit(EXIT_FAILURE);
	}
	std::string temp(buff);
	this->_response = temp;
	logger("message recieved!");
}