#pragma once

#include "Parser.hpp"

class Server
{
private:
	std::string _ip;
	std::string _port;
	std::string _server_name;
	int			_websock;
public:
	Server(Parser &in);
	~Server();

	void StartServer();
	//void StartClient(int fd);
};
