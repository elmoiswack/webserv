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
	std::vector<std::string> tmp = serverblock->GetAllowMethods();
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

std::vector<Location>::iterator Client::GetLocationblockBegin()
{
	return (this->_locationblocks.begin());
}

std::vector<Location>::iterator Client::GetLocationblockEnd()
{
	return (this->_locationblocks.end());
}

int Client::Getrecvmax()
{
	return (this->_recvmax);
}

std::unordered_map<int, std::string>::iterator Client::GetErrorpageBegin()
{
	return (this->_error_page.begin());
}

std::unordered_map<int, std::string>::iterator Client::GetErrorpageEnd()
{
	return (this->_error_page.end());
}

std::string Client::GetRoot()
{
	return (this->_root);
}

std::vector<std::string>::iterator Client::GetMethodsBegin()
{
	return (this->_allow_methods.begin());
}

std::vector<std::string>::iterator Client::GetMethodsEnd()
{
	return (this->_allow_methods.end());
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
	for (auto it = in->GetErrorpageBegin(); it != in->GetErrorpageEnd(); it++)
	{
		out << it->first << " ";
	}
	out << std::endl;
	for (auto it = in->GetMethodsBegin(); it != in->GetMethodsEnd(); it++)
	{
		out << *it << " ";
	}
	out << std::endl;
	return (out);
}