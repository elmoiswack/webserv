#ifndef CLIENT_HPP
# define CLIENT_HPP

#include "../includes/Location.hpp"
#include "../includes/Server.hpp"

class Client
{
private:
	int		_listensocket;
	std::vector<Location> _locationblocks;
	int		_recvmax;
	std::unordered_map<int, std::string> _error_page;
	std::string		_root;
	std::vector<std::string> _allow_methods;
	long		_contentlenght;

public:
	Client();
	Client(int socket, std::vector<Server>::iterator serverblock);
	~Client();

	int GetListensock();
	std::vector<Location>::iterator GetLocationblockBegin();
	std::vector<Location>::iterator GetLocationblockEnd();
	int Getrecvmax();
	std::unordered_map<int, std::string>::iterator GetErrorpageBegin();
	std::unordered_map<int, std::string>::iterator GetErrorpageEnd();
	std::string GetRoot();
	std::vector<std::string>::iterator GetMethodsBegin();
	std::vector<std::string>::iterator GetMethodsEnd();
	std::vector<std::string> GetMethods();
	void SetContentLenght(long contentlen);
	long GetContentLenght();

};

std::ostream& operator<<(std::ostream &out, Client *in);

#endif

