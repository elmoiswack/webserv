#pragma once

#include "Parser.hpp"
#include <vector>
#include <poll.h>

class Server
{
private:
	std::string _ip;
	std::vector<std::string> _ports;
	std::string _server_name;
	bool		_server_running;
	int			_ammount_sock;
	std::vector<struct pollfd> _sockvec;
	std::vector<std::string> _whatsockvec;
	std::string _response;
	bool 		_htmlstartsend;
public:
	Server(Parser &in);
	~Server();

	void SetUpServer();
	void InitSocket();
	void BindSockets();
	void ListenSockets();

	void RunPoll();

	void PollEvents();
	void EventsPollin(int fd, int index);
	void EventsPollout(int fd, int index);
	
	void AcceptClient(int index);
	void RecieveMessage(int fd, int index);

	std::string HtmlToString(std::string path);
	std::string GetResponse();

	void AddSocket(int fd, bool is_client);
	void RmvSocket(int index);
	void CloseAllFds();
};

void logger(std::string input);