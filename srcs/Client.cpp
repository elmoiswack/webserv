#include "../includes/Client.hpp"

Client::Client()
{

}

Client::Client(int socket, std::vector<Server>::iterator serverblock)
{
	this->_listensocket = socket;
	this->_locationblocks = serverblock->GetLocations();
	this->_recvmax = std::stoi(serverblock->GetClientMax());
	this->_root = serverblock->GetRoot();
	this->_error_page = serverblock->GetErrorPage();
	this->_allow_methods = serverblock->GetAllowMethods();
}

Client::~Client()
{

}

int Client::GetListensock()
{
	return (this->_listensocket);
}

std::vector<Location> Client::GetLocationblock()
{
	return (this->_locationblocks);
}

int Client::Getrecvmax()
{
	return (this->_recvmax);
}

std::unordered_map<int, std::string> Client::GetErrorpage()
{
	return (this->_error_page);
}

std::string Client::GetRoot()
{
	return (this->_root);
}

std::vector<std::string> Client::GetMethods()
{
	return (this->_allow_methods);
}