#include "../includes/Server.hpp"

void Server::EventsPollout(int fd, Client *client)
{
	logger("POLLOUT");
	logger("sending response to client...");
	if (write(fd, this->_response.c_str(), this->_response.size()) == -1)
	{
		logger("ERROR WRITE: failed to send response! Trying to send 500!");
		this->_response.clear();
		std::string errfile = this->HtmlToString(this->GetHardCPathCode(500), client);
		this->_response = 
		"HTTP/1.1 500 OK\r\n"
		"Content-Type: text/html\r\n"
		"Content-Length: " + std::to_string(errfile.length()) + "\r\n"
		"\r\n"
		+ errfile;
		if (write(fd, this->_response.c_str(), this->_response.size()) == -1)
			throw(Server::WriteErrorException());
	}
	logger("response is sent to fd!");
	close(fd);
	logger("fd is closed and removed!");
	this->_response.clear();
	this->_donereading = false;
	this->_iffirstread = true;
	this->_statuscode = 0;
	this->_isstatuscode = false;
}

void Server::BuildResponse(Client *client)
{
	std::string htmlfile = this->ParseRequest(client);
	if (this->_iscgi == false)
	{
		if (this->_isstatuscode == true)
		{
			std::string code = std::to_string(this->_statuscode);
			this->_response = 
			"HTTP/1.1 " + code + " OK\r\n"
			"Content-Type: text/html\r\n"
			"Content-Length: " + std::to_string(htmlfile.length()) + "\r\n"
			"\r\n"
			+ htmlfile;
			this->_request.clear();
		}
		else 
		{
			this->_response = 
			"HTTP/1.1 200 OK\r\n"
			"Content-Type: text/html\r\n"
			"Content-Length: " + std::to_string(htmlfile.length()) + "\r\n"
			"\r\n"
			+ htmlfile;
			this->_request.clear();
		}
	}
	else if (this->_iscgi == true)
	{
		this->_response = htmlfile;
		this->_iscgi = false;
		this->_request.clear();
	}
	logger("response created!");	
}
