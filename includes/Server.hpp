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
	bool isCgi(const std::string &url);
	std::string extractCgiPath(const std::string &url);
	//void StartClient(int fd);
};
