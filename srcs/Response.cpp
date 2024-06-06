#include "../includes/Server.hpp"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include<fstream>
#include<string>
#include <sstream>

std::string Server::HtmlToString(std::string path)
{
	std::ifstream file(path, std::ios::binary);
	if (!file.good())
	{
		std::cout << "Failed to read file!\n";
		std::exit(EXIT_FAILURE);
	}
	std::stringstream buffer;
	buffer << file.rdbuf();
	return (buffer.str());
}

void Server::EventsPollout(int fd, int index)
{
	logger("POLLOUT");
	if (this->_whatsockvec[index] == "CLIENT")
	{
		logger("sending response to client");
		if (this->_htmlstartsend == false)
		{
			std::string html_file = this->HtmlToString("./var/www/index.html");
			this->_response = 
			"HTTP/1.1 200 OK\r\n"
			"Content-Type: text/html\r\n"
			"Content-Length: " + std::to_string(html_file.length()) + "\r\n"
			"\r\n"
			+ html_file;
			this->_htmlstartsend = true;
		}
		logger(this->_response);
		write(fd, this->_response.c_str(), this->_response.size());
		logger("response is sent to fd!");
		close(fd);
		logger("fd is closed");
		this->_response.clear();
	}
}