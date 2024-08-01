#include "../includes/Server.hpp"

std::string Server::GetHardCPathCode(int code)
{
	logger("GETTING HARDCODED PATH TO ERRORFILE!");
	std::cout << "Getting " << code << " html file!" << std::endl;
	std::unordered_map<int, std::string>::iterator it = this->_hcerr_page.begin();
	while (it != this->_hcerr_page.end())
	{
		if (it->first == code)
			break ;
		it++;
	}
	if (it == this->_hcerr_page.end())
	{
		logger("Code passed isn't valid! Internal server error! Sending 500!");
		for (auto iterr = this->_hcerr_page.begin(); iterr != this->_hcerr_page.end(); iterr++)
		{
			if (iterr->first == 500)
				return (iterr->second);
		}
	}
	return (it->second);
}

std::string Server::GetSatusCodeFile(std::string path, Client *client)
{
	std::string::iterator begin = path.begin();
	while (begin != path.end() && !std::isdigit(*begin))
		begin++;
	if (begin == path.end())
	{
		logger("Invalid path to errorpage! Sending 404!\n");
		return (this->HtmlToString(this->GetHardCPathCode(404), client));
	}
	auto end = begin;
	while (std::isdigit(*end))
		end++;
	std::string strcode(begin, end);
	int code = std::stoi(strcode);
	std::cout << "CODE = " << code << std::endl;

	std::unordered_map<int, std::string>::iterator iterr = client->GetErrorpageBegin();
	while (iterr != client->GetErrorpageEnd() && iterr->first != code)
		iterr++;
	if (iterr == client->GetErrorpageEnd())
	{
		logger("Error page not found! Sending 404!\n");
		return (this->HtmlToString(this->GetHardCPathCode(404), client));
	}
	
	std::string statuscode = client->GetRoot() + iterr->second;
	std::cout << "Statuscode = " << statuscode << std::endl;
	return (this->HtmlToString(statuscode, client));
}

std::string Server::HtmlToString(std::string path, Client *client)
{
	if (access(path.c_str(), F_OK) == -1)
	{
		std::cout << "Path: " << path << " doens't exist, sending 404!" << std::endl;
		return (this->HtmlToString(this->GetHardCPathCode(404), client));
	}
	if (access(path.c_str(), R_OK) == -1)
	{
		std::cout << "Path: " << path << " has no reading rights, sending 403!" << std::endl;
		return (this->HtmlToString(this->GetHardCPathCode(403), client));
	}
	
	std::ifstream file(path, std::ios::binary);
	if (!file.good())
	{
		std::cout << "Failed to read file! Sending 500!\n" << std::endl;
		return (this->HtmlToString(this->GetHardCPathCode(500), client));
	}
	std::stringstream buffer;
	buffer << file.rdbuf();
	return (buffer.str());
}