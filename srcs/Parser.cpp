#include "../includes/Parser.hpp"
#include "fstream"
#include <string>

Parser::Parser(std::string inputfile)
{
	this->StartParser(inputfile);
}

Parser::~Parser()
{
	this->_ip.clear();
	this->_port.clear();
	this->_server_name.clear();
}

void Parser::StartParser(std::string inputfile)
{
	std::ifstream configfile;
	std::string fileline;

	configfile.open(inputfile, std::ifstream::in);
	if (configfile.fail())
		throw (FailedOpenFileException());
	std::getline(configfile, fileline);
	if (fileline != "server {")
	{
		std::cout << "ERROR: invalid firstline of config!" << std::endl;
		return ;
	}
	while (std::getline(configfile, fileline))
	{
		int index = 0;
		if (fileline.find("listen", 0) != fileline.npos)
		{
			while (std::isdigit(fileline[index]) == 0)
				index++;
			int i = index;
			while (fileline[i] && fileline[i] != ';')
				i++;
			if (i == (int)fileline.size())
				throw (InvalidLineConfException(fileline));
			this->_ip = fileline.substr((size_t)index, (size_t)i - index);
		}
		if (fileline.find("port", 0) != fileline.npos)
		{
			while (std::isdigit(fileline[index]) == 0)
				index++;
			int i = index;
			while (fileline[i] && fileline[i] != ';')
				i++;
			if (i == (int)fileline.size())
				throw (InvalidLineConfException(fileline));
			this->_port = fileline.substr((size_t)index, (size_t)i - index);
		}
		if (fileline.find("server_name", 0) != fileline.npos)
		{
			index = this->SkipWhitespaces(fileline, index);
			while (std::isalpha(fileline[index]) || fileline[index] == '_')
				index++;
			index = this->SkipWhitespaces(fileline, index);
			int i = index;
			while (fileline[i] && fileline[i] != ';')
				i++;
			if (i == (int)fileline.size())
				throw (InvalidLineConfException(fileline));
			this->_server_name = fileline.substr((size_t)index, (size_t)i - index);
		}
	}
}

std::string Parser::GetIp()
{
	return (this->_ip);
}

std::string Parser::GetPort()
{
	return (this->_port);
}

std::string Parser::GetServName()
{
	return (this->_server_name);
}

Parser::InvalidLineConfException::InvalidLineConfException(std::string input)
{
	this->_error_str = "bad input -> " + input;
}

const char* Parser::InvalidLineConfException::what() const throw()
{
	return (this->_error_str.c_str());
}

const char* Parser::FailedOpenFileException::what() const throw()
{
	return ("failed to open file!");
}