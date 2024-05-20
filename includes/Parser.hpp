#pragma once

#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <regex>

class Parser
{
private:
	std::string _port;
	std::string _ip;
	std::string _server_name;
	std::string _client_max;
	std::string _root;
public:
	Parser(std::string inputfile);
	~Parser();

	// void StartParser(std::string inputfile);
	void StartParser(std::string inputfile);
	void CheckConfigData(std::string inputfile);
	void OpenConfigFile(std::string inputfile);
	void TokenizeString(std::string &inputfile);
	void ProcessConfigData(std::string &ConfigString);
	std::vector<std::string> Tokenizing(const std::string &ProcessedString);

	std::string GetPort();
	std::string GetIp();
	std::string GetServName();

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
