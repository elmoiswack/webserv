#ifndef PARSER_HPP
#define PARSER_HPP

#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <regex>
#include <unordered_map>

#include "../includes/Server.hpp"

class Server;

class Parser
{
private:
	std::string _port;
	std::string _ip;
	std::string _server_name;
	std::string _client_max;
	std::string _root;
	std::unordered_map<int, std::string> _error_page;

	std::vector<Server>				serverblocks;

public:
	Parser(std::string inputfile, Parser& parser);
	~Parser();

	const std::vector<Server>& GetServerBlocks() const;

	void 					 OpenConfigFile(std::string inputfile, Parser& parser);
	std::vector<std::string> Tokenizing(std::string &ProcessedString);
	void 					 ProcessConfigData(std::string &ConfigString);
	void					 ParseServer(std::vector<std::string>& tokens, Parser& parser);

	void AddServerBlock(const Server& server_block);

	template <typename T>
	int SkipWhitespaces(T input, int index)
	{
		while ((input[index] == ' ' ) || (input[index] == '\t') \
			|| (input[index] == '\n') || (input[index] == '\r') \
			|| (input[index] == '\f') || (input[index] == '\v'))
			index++;
		return (index);
	}

	class InvalidLineConfException : public std::exception
	{
		public:
			InvalidLineConfException(std::string input);
			const char* what() const throw();
		private:
			std::string _error_str;
	};

	class FailedOpenFileException : public std::exception
	{
		const char* what() const throw();
	};
};

#endif