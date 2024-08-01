#include "../includes/Server.hpp"

void Server::EventsPollout(int fd, Client *client)
{
	logger("POLLOUT");
	if (this->_response.size() > 0)
	{
		logger("sending response to client...");
		if (write(fd, this->_response.c_str(), this->_response.size()) == -1)
		{
			logger("ERROR WRITE: failed to send response! Trying to send 500!");
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
		this->_donereading = false;
		this->_iffirstread = true;
	}
}

void Server::BuildResponse(Client *client)
{
	std::string htmlfile = this->ParseRequest(client);
	if (this->_iscgi == false)
	{
		this->_response = 
		"HTTP/1.1 200 OK\r\n"
		"Content-Type: text/html\r\n"
		"Content-Length: " + std::to_string(htmlfile.length()) + "\r\n"
		"\r\n"
		+ htmlfile;
		this->_request.clear();
	}
	else if (this->_iscgi == true)
	{
		this->_response = htmlfile;
		this->_iscgi = false;
		this->_request.clear();
	}
	logger("response created!");	
}

std::string Server::ParseRequest(Client *client)
{
	std::vector<char>::iterator itfirst = this->_request.begin();
	logger("\n\nRequest after reading is done =");
	for (std::vector<char>::iterator print = this->_request.begin(); print != this->_request.end(); print++)
	{
		std::cout << *print;
	}
	std::cout << std::endl;
	logger("\n\n");
	char arr[7];
	int index = 0;
	if (std::isspace(*itfirst))
	{
		while (std::isspace(*itfirst))
			itfirst++;
	}
	if (itfirst == this->_request.end())
		return (this->HtmlToString(this->GetHardCPathCode(400), client));
	while (!std::isspace(*itfirst) && index < 7)
	{
		arr[index] = *itfirst;
		index++;
		itfirst++;
	}
	arr[index] = '\0';
	std::string method(arr);
	
	return (this->WhichMethod(client, method, itfirst));
}