#pragma once

#include <iostream>

class Parser
{
private:
	std::string _port;
	std::string _ip;
	std::string _server_name;
public:
	Parser(std::string inputfile);
	~Parser();

	void StartParser(std::string inputfile);

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
