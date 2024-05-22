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
	int			_ammount_sock;
	std::vector<struct pollfd> _sockvec;
	std::vector<std::string> _whatsockvec;
public:
	Server(Parser &in);
	~Server();

	void SetUpServer();
	void RunPoll();
	void PollEvents(int pollammount);
	void EventsPollin(int fd, int index);
	void EventsPollout(int fd);
	void AddSocket(int fd, bool is_client);
	void RmvSocket(int index);
	void CloseAllFds();
};

void logger(std::string input);