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
	this->_ports.push_back(in.GetPort());
	this->_server_name = in.GetServName();
	this->_ammount_sock = 0;
}

Server::~Server()
{
	this->_ip.clear();
	this->_server_name.clear();
	this->_sockvec.clear();
	this->_whatsockvec.clear();
	this->_ports.clear();
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

void Server::InitSocket()
{
	std::vector<std::string>::iterator it;
	for (it = this->_ports.begin(); it != this->_ports.end(); it++)
	{
		int websock = socket(AF_INET, SOCK_STREAM, 0);
		if (websock < 0)
		{
			std::cout << "ERROR SOCKET" << std::endl;
			exit(EXIT_FAILURE);
		}
		if (fcntl(websock, F_SETFL, O_NONBLOCK) == -1)
		{
			std::cout << "ERROR FCNTL" << std::endl;
			exit(EXIT_FAILURE);	
		}
		this->AddSocket(websock, false);
	}
}

void Server::BindSockets()
{
	std::vector<std::string>::iterator it;
	struct sockaddr_in infoaddr;
	memset(&infoaddr, '\0', sizeof(infoaddr));
	int index = 0;
	for (it = this->_ports.begin(); it != this->_ports.end(); it++)
	{
		std::string tmp = *it;
		infoaddr.sin_family = AF_INET;
		infoaddr.sin_addr.s_addr = INADDR_ANY;
		infoaddr.sin_port = htons(std::atoi(tmp.c_str()));
		if (bind(this->_sockvec[index].fd, (struct sockaddr *)&infoaddr, sizeof(infoaddr)) == -1)
		{
			std::cout << "ERROR bruh" << std::endl;
			exit(EXIT_FAILURE);
		}
		index++;
	}
}

void Server::ListenSockets()
{
	std::vector<std::string>::iterator it;
	int index = 0;
	for (it = this->_ports.begin(); it != this->_ports.end(); it++)
	{
		if (listen(this->_sockvec[index].fd, 5) == -1)
		{
			std::cout << "ERROR LISTEN" << std::endl;
			exit(EXIT_FAILURE);		
		}
		index++;
	}	
}
//https://localhost:8080/ our address
void Server::SetUpServer()
{
	this->_ports.push_back("8081");
	this->InitSocket();
	this->BindSockets();
	logger("Server is running!");
	this->_server_running = true;
	this->ListenSockets();
	this->RunPoll();
	this->CloseAllFds();
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
		if (ret > 0)
			this->PollEvents(ret);
	}	
}

void Server::PollEvents(int pollammount)
{
	pollammount = 0;
	for (int index = 0; index != this->_ammount_sock; index++)
	{
		pollfd temp;
		temp.fd = this->_sockvec[index].fd;
		temp.events = this->_sockvec[index].events;
		temp.revents = this->_sockvec[index].revents;
		if (temp.revents == POLLIN)
		{
			this->EventsPollin(temp.fd, index);
		}
		if (temp.revents == POLLOUT)
		{
			this->EventsPollout();
		}
		if (temp.revents == POLLHUP)
		{
			logger("Connection hung up!");
			close(temp.fd);
			this->RmvSocket(index);
		}
		if (temp.revents == POLLERR)
		{
			logger("Fuck error poll");
			exit(EXIT_FAILURE);
		}
	}	
}

void Server::EventsPollout()
{
	logger("POLLOUT");
	std::string response =
	"HTTP/1.1 200 OK\r\n"
	"Content-Type: text/html\r\n"
	"Content-Length: 25\r\n"
	"\r\n"
	"<html>"
	"HELLO WORLD!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1"
	"</html>";
	write(temp.fd, response.c_str(), response.size());
	logger("HTMLpage is sent to fd!");
	close(temp.fd);
}

void Server::EventsPollin(int fd, int index)
{
	logger("POLLIN");
	logger("action pending...");
	if ((unsigned long)index < this->_ports.size())
	{
		logger("attempting connection...");
		int newsock = accept(this->_sockvec[index].fd, NULL, NULL);
		if (newsock == -1)
		{
			std::cout << "ERROR ACCEPT" << std::endl;
			exit(EXIT_FAILURE);
		}
		this->AddSocket(newsock, true);
		logger("Connection is accepted!");
		return ;
	}
	else
	{
		logger("Ready to recieve...");
		char buf[1024];
		int nbytes = recv(fd, buf, sizeof(buf), 0);
		if (nbytes == 0)
		{
			logger("Connection closed!");
			close(fd);
			RmvSocket(index);
			return ;
		}
		else if (nbytes < 0)
		{
			std::cout << "ERROR RECV" << std::endl;
			exit(EXIT_FAILURE);
		}
		logger("message recieved!");
		for (int j = 0; j < this->_ammount_sock; j++)
		{
			int destfd = this->_sockvec[j].fd;
			if (j != 0 && j != index)
			{
				if (send(destfd, buf, nbytes, 0) == -1)
				{
					std::cout << "ERROR SEND" << std::endl;
					exit(EXIT_FAILURE);
				}
			}
		}
		logger("message send!");
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
