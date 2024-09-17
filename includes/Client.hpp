#ifndef CLIENT_HPP
# define CLIENT_HPP

#include "../includes/Location.hpp"
#include "../includes/Server.hpp"

class Client
{
private:
	int		_id;
	int		_listensocket;
	std::vector<Location> _locationblocks;
	int		_recvmax;
	std::unordered_map<int, std::string> _error_page;
	std::string		_root;
	std::vector<std::string> _allow_methods;
	long		_contentlenght;
	std::string _servername;
	std::string _port;
	std::vector<char> _request;
	std::string _response;
	std::string _method;
	bool 		_donereading;

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
	std::string GetServerName();
	std::string GetPort();
	int GetId();
	void SetId(int id);

	void PushToRequest(char buff);
	void ClearRequest();
	std::vector<char>::iterator GetBeginRequest();
	std::vector<char>::iterator GetEndRequest();
	ssize_t GetRequestSize();
	void SetCurrentMethod(std::string method);
	std::string GetCurrentMethod();
	void ClearResponse();
	void SetResponse(std::string tmp);
	std::string GetResponse();
	ssize_t GetResponseSize();
	void SetDonereading(bool state);
	bool GetDonereading();
};

std::ostream& operator<<(std::ostream &out, Client *in);

#endif

