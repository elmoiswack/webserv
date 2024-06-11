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
		it->_request.clear();
		this->_donereading = false;
	}
}

std::string Server::ParseRequest(std::vector<Server>::iterator it)
{
	std::vector<char>::iterator itfirst = it->_request.begin();
	char arr[6];
	int index = 0;
	while (!std::isspace(*itfirst))
	{
		arr[index] = *itfirst;
		index++;
		itfirst++;
	}
	arr[index] = '\0';
	std::string method(arr);

	if (method == "GET")
	{
		it->_method = "GET";
		if (std::isspace(*itfirst))
		{
			while (std::isspace(*itfirst))
				itfirst++;
		}
		std::vector<char>::iterator itend = itfirst;
		while (!std::isspace(*itend))
			itend++;
		std::string ja;
		ja.assign(itfirst, itend);
		logger(ja);
		if (ja == "/")
			return (it->HtmlToString("./var/www/index.html"));
	}
	if (method == "POST")
	{
		it->_method = "POST";
	}
	if (method == "DELETE")
	{
		it->_method = "DELETE";
	}
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