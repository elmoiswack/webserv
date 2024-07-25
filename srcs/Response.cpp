#include "../includes/Server.hpp"

void Server::EventsPollout(int fd, Client *client)
{
	logger("POLLOUT");
	if (this->_response.size() > 0)
	{
		logger("sending response to client...");
		if (write(fd, this->_response.c_str(), this->_response.size()) == -1)
		{
			logger("ERROR WRITE: failed to send response!");
			this->_response.clear();
			std::string errfile = this->HtmlToString(this->GetHardCPathCode(500), client);
			this->_response = 
			"HTTP/1.1 200 OK\r\n"
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
	}
}