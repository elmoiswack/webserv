#include "../includes/Server.hpp"
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

Server::Server(Parser &in)
{
	this->_ip = in.GetIp();
	this->_port = in.GetPort();
	this->_server_name = in.GetServName();
	this->_ammount_sock = 0;
}

Server::~Server()
{
	this->_ip.clear();
	this->_port.clear();
	this->_server_name.clear();
	this->_sockvec.clear();
}

void Server::AddSocket(int fd, bool is_client)
{
	pollfd temp;
	temp.fd = fd;
	temp.events = POLLIN | POLLOUT;
	temp.revents = 0;
	this->_sockvec.push_back(temp);
	if (is_client == true)
		this->_whatsockvec.push_back("client");
	else
		this->_whatsockvec.push_back("server");
	this->_ammount_sock += 1;
}

void Server::RmvSocket(int index)
{
	std::vector<struct pollfd>::iterator it = this->_sockvec.begin();
	for (int i = 0; i != index; i++)
	{
		it++;
	}
	close(this->_sockvec[index].fd);
	this->_sockvec.erase(it);
	std::vector<std::string>::iterator jt = this->_whatsockvec.begin();
	for (int j = 0; j != index; j++)
	{
		jt++;
	}
	this->_whatsockvec.erase(jt);
}

//https://localhost:8080/ our address
void Server::SetUpServer()
{
	int websock = socket(AF_INET, SOCK_STREAM, 0);
	if (websock < 0)
	{
		std::cout << "ERROR" << std::endl;
		exit(EXIT_FAILURE);
	}
	this->AddSocket(websock, false);
	struct sockaddr_in infoaddr;
	memset(&infoaddr, '\0', sizeof(infoaddr));
	infoaddr.sin_family = AF_INET;
	infoaddr.sin_addr.s_addr = INADDR_ANY;
	infoaddr.sin_port = htons(std::atoi(this->_port.c_str()));
	if (bind(this->_sockvec[0].fd, (struct sockaddr *)&infoaddr, sizeof(infoaddr)) == -1)
	{
		std::cout << "ERROR BIND" << std::endl;
		exit(EXIT_FAILURE);
	}
	logger("Server is running!");
	if (listen(this->_sockvec[0].fd, 5) == -1)
	{
		std::cout << "ERROR LISTEN" << std::endl;
		exit(EXIT_FAILURE);		
	}
	if (fcntl(this->_sockvec[0].fd, F_SETFL, O_NONBLOCK) == -1)
	{
		std::cout << "ERROR FCNTL" << std::endl;
		exit(EXIT_FAILURE);	
	}
	this->_server_running = true;
	this->RunPoll();
	this->CloseAllFds();
}	

void Server::PollEvents()
{
	for (pollfd &tmp : this->_sockvec)
	{
		if (tmp.revents == POLLIN)
		{
			int newsock = accept(this->_sockvec[0].fd, NULL, NULL);
       		if (newsock == -1)
       		{
           		std::cout << "ERROR ACCEPT" << std::endl;
           		exit(EXIT_FAILURE);
       		}
			this->AddSocket(newsock, true);
			logger("Connection is accepted!");
		}
		if (tmp.revents == POLLOUT)
		{
			std::string response =
			"HTTP/1.1 200 OK\r\n"
			"Content-Type: text/html\r\n"
			"Content-Length: 25\r\n"
			"\r\n"
			"<html>"
			"HELLO WORLD!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1"
			"</html>";
			write(tmp.fd, response.c_str(), response.size());
		}
	}	
}

void Server::RunPoll()
{
	while (this->_server_running == true)
	{
		int ret = poll(this->_sockvec.data(), this->_sockvec.size(), -1);
		if (ret < 0)
		{
			std::cout << "ERROR POLL" << std::endl;
			exit(EXIT_FAILURE);
		}
		this->PollEvents();
	}	
}

void Server::CloseAllFds()
{
	for (int i = 0;  i != this->_ammount_sock; i++)
	{
		close(this->_sockvec[i].fd);
	}
}

void logger(std::string input)
{
	std::cout << input << std::endl;
}

// char buffer[1024] = { 0 }; 
// recv(newsock, buffer, sizeof(buffer), 0); 
// std::cout << "Message from client: " << buffer << std::endl; 
// void Server::StartClient(int fd)
// {

// 	struct sockaddr_in infoaddr;
// 	memset(&infoaddr, '\0', sizeof(infoaddr));
// 	infoaddr.sin_family = AF_INET;
// 	infoaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
// 	infoaddr.sin_port = htons(std::atoi(this->_port.c_str()));	
// 	if (connect(fd, (struct sockaddr *)&infoaddr, sizeof(infoaddr)) == -1)
// 	{
// 		std::cout << "ERROR CONNECT" << std::endl;
// 		exit(EXIT_FAILURE);
// 	}
// 	fcntl(fd, F_SETFL, O_NONBLOCK);
// 	std::string lmao = "hello world";
// 	int bret = send(fd, lmao.c_str(), lmao.size(), 0);
// 	if (bret == -1)
// 	{
// 		std::cout << "ERROR SEND" << std::endl;
// 		exit(EXIT_FAILURE);			
// 	}
// }
