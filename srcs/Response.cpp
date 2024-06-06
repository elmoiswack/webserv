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

void Server::EventsPollout(int fd, int index, std::vector<Server>::iterator it)
{
	logger("POLLOUT");
	if (it->_whatsockvec[index] == "CLIENT")
	{
		logger("sending response to client");
		std::string html_file = it->HtmlToString("./var/www/index.html");
		it->_response = 
		"HTTP/1.1 200 OK\r\n"
		"Content-Type: text/html\r\n"
		"Content-Length: " + std::to_string(html_file.length()) + "\r\n"
		"\r\n"
		+ html_file;
		logger(it->_response);
		write(fd, it->_response.c_str(), it->_response.size());
		logger("response is sent to fd!");
		close(fd);
		logger("fd is closed");
		it->_response.clear();
	}
}