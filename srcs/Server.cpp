#include "../includes/Server.hpp"
#include "../includes/Parser.hpp"
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <vector>

Server::Server(const std::string& ip, const std::string& port, const std::string& server_name,
               const std::string& client_max, const std::string& root, const std::unordered_map<int, std::string>& error_page, const std::string& serverindex)
    : _port(port), _ip(ip),_server_name(server_name), _client_max(client_max), _root(root), _serverindex(serverindex), _error_page(error_page), _websock(-1) {}



Server::Server(Parser &in)
{
	this->_serverblocks = in.GetServerBlocks();
	for (std::vector<Server>::iterator it = this->_serverblocks.begin(); it != this->_serverblocks.end(); it++)
	{
		it->_locationblocks = it->GetLocations();
		it->_ammount_sock = 0;
	}
}



Server::~Server()
{
	// this->_ip.clear();
	// this->_server_name.clear();
	// this->_client_max.clear();
	// this->_root.clear();
	// this->_error_page.clear();
	// this->_serverindex.clear();
	// close(this->_websock);
	// this->_sockvec.clear();
	// this->_whatsockvec.clear();
	// this->_ports.clear();

}

void Server::AddSocket(int fd, bool is_client)
{
	pollfd temp;
	if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1)
	{
		std::cout << "ERROR FCNTL" << std::endl;
		exit(EXIT_FAILURE);	
	}
	temp.fd = fd;
	temp.events = POLLIN | POLLOUT;
	temp.revents = 0;
	this->_sockvec.push_back(temp);
	if (is_client == true)
	{
		this->_whatsockvec.push_back("CLIENT");
	}
	else
		this->_whatsockvec.push_back("SERVER");
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
	for (unsigned long i = 0; i < this->_serverblocks.size(); i++)
	{
		std::vector<Server>::iterator it = this->_serverblocks.begin();
		this->InitSocket(it);
		this->BindSockets(it);
		this->ListenSockets(it);
	}
	logger("Server is initialized!");
	this->RunPoll();
	for (unsigned long i = 0; i < this->_serverblocks.size(); i++)
	{
		std::vector<Server>::iterator it = this->_serverblocks.begin();
		this->CloseAllFds(it);
	}
}

void Server::InitSocket(std::vector<Server>::iterator it)
{
	// std::vector<std::string>::iterator it;
	// for (it = this->_ports.begin(); it != this->_ports.end(); it++)
	// {
	// 	int websock = socket(AF_INET, SOCK_STREAM, 0);
	// 	if (websock < 0)
	// 	{
	// 		std::cout << "ERROR SOCKET" << std::endl;
	// 		exit(EXIT_FAILURE);
	// 	}
	// 	if (fcntl(websock, F_SETFL, O_NONBLOCK) == -1)
	// 	{
	// 		std::cout << "ERROR FCNTL" << std::endl;
	// 		exit(EXIT_FAILURE);	
	// 	}
	// 	this->AddSocket(websock, false);
	// }

	int websock = socket(AF_INET, SOCK_STREAM, 0);
	if (websock < 0)
	{
		std::cout << "ERROR SOCKET" << std::endl;
		exit(EXIT_FAILURE);
	}
	it->AddSocket(websock, false);
}

void Server::BindSockets(std::vector<Server>::iterator it)
{
	struct sockaddr_in infoaddr;
	std::string port = it->GetPort();
	infoaddr.sin_family = AF_INET;
	infoaddr.sin_addr.s_addr = INADDR_ANY;
	infoaddr.sin_port = htons(std::atoi(port.c_str()));
	std::cout << "port= " << port << std::endl;
	if (bind(it->_sockvec[0].fd, reinterpret_cast<struct sockaddr *>(&infoaddr), sizeof(infoaddr)) == -1)
	{
		std::cout << "ERROR bruh" << std::endl;
		exit(EXIT_FAILURE);
	}
}

void Server::ListenSockets(std::vector<Server>::iterator it)
{
	if (listen(it->_sockvec[0].fd, 5) == -1)
	{
		std::cout << "ERROR LISTEN" << std::endl;
		exit(EXIT_FAILURE);		
	}
}

void Server::RunPoll()
{
	while (1)
	{
		for (std::vector<Server>::iterator it = this->_serverblocks.begin(); it != this->_serverblocks.end(); it++)
		{
			int ret = poll(it->_sockvec.data(), it->_sockvec.size(), -1);
			std::cout << ret << std::endl;
			if (ret < 0)
			{
				std::cout << "ERROR POLL" << std::endl;
				exit(EXIT_FAILURE);
			}
			if (ret > 0)
				this->PollEvents(it);
		}
	}	
}

void Server::PollEvents(std::vector<Server>::iterator it)
{
	std::cout << "da" << it->_ammount_sock << std::endl;
	for (int index = 0; index != it->_ammount_sock; index++)
	{
		pollfd temp;
		temp.fd = it->_sockvec[index].fd;
		temp.events = it->_sockvec[index].events;
		temp.revents = it->_sockvec[index].revents;
		if (temp.revents & POLLIN)
		{
			if (it->_whatsockvec[index] == "SERVER")
			{
				it->AcceptClient(index, it);
			}
			else
			{
				it->EventsPollin(temp.fd, index);
			}
		}
		if (temp.revents & POLLOUT)
		{
			
			it->EventsPollout(temp.fd, index, it);
		}
		if (temp.revents & POLLHUP)
		{
			logger("Connection hung up!");
			close(temp.fd);
			it->RmvSocket(index);
		}
		if (temp.revents & POLLERR)
		{
			logger("Fuck error poll");
			exit(EXIT_FAILURE);
		}
	}	
}

void Server::AcceptClient(int index, std::vector<Server>::iterator it)
{
	logger("attempting connection...");
	int newsock = accept(it->_sockvec[index].fd, NULL, NULL);
	if (newsock == -1)
	{
		std::cout << "ERROR ACCEPT" << std::endl;
		exit(EXIT_FAILURE);
	}
	it->AddSocket(newsock, true);
	logger("Connection is accepted!");
}

void Server::CloseAllFds(std::vector<Server>::iterator it)
{
	for (int i = 0;  i != it->_ammount_sock; i++)
	{
		close(it->_sockvec[i].fd);
	}
}

void logger(std::string input)
{
	std::cout << input << std::endl;
}