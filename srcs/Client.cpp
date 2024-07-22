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
	std::cout << "CLIENT CREATED AND VARS SET!" << std::endl;
}

Client::~Client()
{
	this->_allow_methods.clear();
	this->_error_page.clear();
	this->_locationblocks.clear();
	this->_recvmax = 0;
	this->_listensocket = 0;
	this->_root.clear();
	std::cout << "client is about to get deleted!" << std::endl;
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

std::ostream& operator<<(std::ostream &out, Client *in)
{
	out << "Client listen sock = " << in->GetListensock() << std::endl;
	out << "Client recvmax = " << in->Getrecvmax() << std::endl;
	out << "Client root = " << in->GetRoot() << std::endl;
	return (out);
}