#include "../includes/Server.hpp"

void Server::WriteToClient(int fd, Client *client)
{
	logger("sending response to client...");
	std::string response = client->GetResponse();
	if (write(fd, response.c_str(), response.size()) == -1)
	{
		logger("ERROR WRITE: failed to send response! Trying to send 500!");
		response.clear();
		std::string errfile = this->HtmlToString(this->GetHardCPathCode(500, client), client);
		response = 
		"HTTP/1.1 500 Internal Server Error\r\n"
		"Content-Type: text/html\r\n"
		"Content-Length: " + std::to_string(errfile.length()) + "\r\n"
		"\r\n"
		+ errfile;
		client->ClearResponse();
		client->SetResponse(response);
		return ;
	}
	logger("response is sent to fd!");
	client->ClearResponse();
	client->SetResponse("EMPTY");
	client->SetDonereading(false);
	client->SetCurrentMethod("EMPTY");
	client->SetStatusCode(0);
	client->SetStatusCodeState(false);
	this->_cgi_running = false;
	this->_totalread = 0;
}

void Server::BuildResponse(Client *client)
{
	std::string htmlfile = this->ParseRequest(client);
	std::string response;
	if (this->_iscgi == false)
	{
		if (client->GetStatusCodeState() == true)
		{
			this->BuildResponseCode(client, htmlfile);
		}
		else if (client->GetReturnstate() == true)
		{
			std::cout << "NANI DA FUQ" << std::endl;
			this->BuildResponseRedirect(client, htmlfile);
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

void Server::BuildResponseCode(Client *client, std::string htmlfile)
{
	std::string code = std::to_string(client->GetStatusCode());
	std::string message = this->WhichMessageCode(std::stoi(code));
	std::string response = 
	"HTTP/1.1 " + code + " " + message + "\r\n"
	"Content-Type: text/html\r\n"
	"Content-Length: " + std::to_string(htmlfile.length()) + "\r\n"
	"\r\n"
	+ htmlfile;
	
	client->ClearRequest();
	client->SetResponse(response);
}

void Server::BuildResponseRedirect(Client *client, std::string htmlfile)
{
	std::string code = client->GetReturnCode();
	std::string message = this->WhichMessageCode(std::stoi(code));
	std::string response = 
	"HTTP/1.1 " + code + " " + message + "\r\n"
	"Location: " + client->GetReturn() + "\r\n"
	"Content-Type: text/html\r\n"
	"Content-Length: " + std::to_string(htmlfile.length()) + "\r\n"
	"\r\n"
	+ htmlfile;

	client->ClearRequest();
	client->SetResponse(response);
}