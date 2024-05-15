#pragma once

#include "Parser.hpp"
#include <vector>
#include <poll.h>

class Server
{
private:
	std::string _ip;
	std::string _port;
	std::string _server_name;
	bool		_server_running;
	std::vector<struct pollfd> _sockvec;
	int					_sock_elem;
public:
	Server(Parser &in);
	~Server();

	void SetUpServer();
	void RunPoll();
	void PollEvents();
	void AddSocket(int fd);
	void RmvSocket(int index);
};

void logger(std::string input);