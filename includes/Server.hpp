#ifndef SERVER_HPP
#define SERVER_HPP

#include <list>

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
	Server(Parser &in);
	~Server();

	void StartServer();

	void   ValidateListen(std::vector<std::string>& tokens, size_t i);
	void   ValidatePort(std::vector<std::string>& tokens, size_t i);
	void   ValidateServerName(std::vector<std::string>& tokens, size_t i);
	void   ValidateClientMaxBodySize(std::vector<std::string>& tokens, size_t i);
	void   ValidateRoot(std::vector<std::string>& tokens, size_t i);
};

#endif