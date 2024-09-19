#include "../includes/Server.hpp"

void Server::WriteToClient(int fd, Client *client)
{
	logger("sending response to client...");
	std::string response = client->GetResponse();
	if (write(fd, response.c_str(), response.size()) == -1)
	{
		logger("ERROR WRITE: failed to send response! Trying to send 500!");
		response.clear();
		std::string errfile = this->HtmlToString(this->GetHardCPathCode(500), client);
		response = 
		"HTTP/1.1 500 Internal Server Error\r\n"
		"Content-Type: text/html\r\n"
		"Content-Length: " + std::to_string(errfile.length()) + "\r\n"
		"\r\n"
		+ errfile;
		if (write(fd, response.c_str(), response.size()) == -1)
			throw(Server::WriteErrorException());
	}
	logger("response is sent to fd, closing fd!");
	close(fd);
	client->ClearResponse();
	client->SetDonereading(false);
	client->SetCurrentMethod("EMPTY");
	this->_statuscode = 0;
	this->_isstatuscode = false;
	this->_totalread = 0;
}

void Server::BuildResponse(Client *client)
{
	std::string htmlfile = this->ParseRequest(client);
	std::string response;
	if (this->_iscgi == false)
	{
		if (this->_isstatuscode == true)
		{
			std::string code = std::to_string(this->_statuscode);
			std::string message = this->WhichMessageCode(std::stoi(code));
			response = 
			"HTTP/1.1 " + code + " " + message + "\r\n"
			"Content-Type: text/html\r\n"
			"Content-Length: " + std::to_string(htmlfile.length()) + "\r\n"
			"\r\n"
			+ htmlfile;

			client->ClearRequest();
			client->SetResponse(response);
		}
		else 
		{
			response = 
			"HTTP/1.1 200 OK\r\n"
			"Content-Type: text/html\r\n"
			"Content-Length: " + std::to_string(htmlfile.length()) + "\r\n"
			"\r\n"
			+ htmlfile;

			client->ClearRequest();
			client->SetResponse(response);
		}
	}
	else if (this->_iscgi == true)
	{
		response = htmlfile;
		this->_iscgi = false;
		client->ClearRequest();
		client->SetResponse(response);
	}
	logger("response created!");	
}
