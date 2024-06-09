#include "../includes/Server.hpp"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>

void Server::EventsPollin(int fd, std::vector<Server>::iterator it)
{
	logger("POLLIN");
	this->GetResponse(fd, it);
}
void Server::GetResponse(int fd, std::vector<Server>::iterator it)
{
	if (it->_donereading == false)
		this->RecieveMessage(fd, it);
	if (it->_donereading == true)
	{
		std::string htmlfile = this->ParseRequest(it);
		if (htmlfile.size() < 10)
		{
			logger("SOMETHIGN WHGENTEIA RRHWA");
			exit(EXIT_FAILURE);
		}
		it->_response = 
		"HTTP/1.1 200 OK\r\n"
		"Content-Type: text/html\r\n"
		"Content-Length: " + std::to_string(htmlfile.length()) + "\r\n"
		"\r\n"
		+ htmlfile;
	}
}

std::string Server::ParseRequest(std::vector<Server>::iterator it)
{
	std::vector<char>::iterator itfind = std::find(it->_request.begin(), it->_request.end(), " ");
	
	
	if (itfind != this->_request.end())
	{
		it->_method = "GET";
		if (std::isspace(*itfind))
		{
			while (std::isspace(*itfind))
				itfind++;
		}
		std::vector<char>::iterator itend = itfind;
		while (std::isalnum(*itend))
			itend++;
		std::string ja(itfind, itend);
		it->_request.clear();
		this->_donereading = false;
		std::cout << "ja = ";
		logger(ja);
		if (ja == "/")
			return (it->HtmlToString("./var/www/index.html"));
		return (ja);
	}
	// itfind = std::find(it->_request.begin(), it->_request.end(), "POST");
	// if (itfind != this->_request.end())
	// {
		
	// }
	// itfind = std::find(it->_request.begin(), it->_request.end(), "DELETE");
	// if (itfind != this->_request.end())
	// {
		
	// }
	it->_request.clear();
	this->_donereading = false;
	return ("");
}

void Server::RecieveMessage(int fd, std::vector<Server>::iterator it)
{
	logger("Ready to recieve...");
	char buff[it->_recvmax];
	int rbytes = recv(fd, &buff, it->_recvmax, 0);
	if (rbytes == -1)
	{
		std::cout << "ERROR read" << std::endl;
		exit(EXIT_FAILURE);
	}
	std::cout << "buff = " << buff << std::endl;
	for (int i = 0; buff[i]; i++)
	{
		it->_request.push_back(buff[i]);
	}
	if (rbytes < it->_recvmax)
	{
		std::cout << "Bytes recv = " << rbytes << std::endl;
		it->_donereading = true;
		it->_request.push_back('\0');
	}
	logger("message recieved!");
}