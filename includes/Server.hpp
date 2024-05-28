#ifndef SERVER_HPP
#define SERVER_HPP

#include <list>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

#include "../includes/Parser.hpp"

class Parser;

class Server
{
private:

	int		_websock;

	std::string _port;
	std::string _ip;
	std::string _server_name;
	std::string _client_max;
	std::string _root;
public:
	Server(Parser &parser);
	~Server();

	void StartServer();

	void   ValidateServerBlock(std::vector<std::string>& tokens, Parser& parser);

	void   ValidateListen(std::vector<std::string>& tokens);
	void   ValidatePort(std::vector<std::string>& tokens);
	void   ValidateServerName(std::vector<std::string>& tokens);
	void   ValidateClientMaxBodySize(std::vector<std::string>& tokens);
	void   ValidateRoot(std::vector<std::string>& tokens);
};

#endif