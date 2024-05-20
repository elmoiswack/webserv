#include "../includes/Parser.hpp"

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

/* parsing starts here:
 * OpenConfigFile: checks if file can be opened and isn't empty
 * TokenizeString: Tokenizes the string by splitting it
 * CheckConfigData: checks the data within the config file
*/

void Parser::StartParser(std::string inputfile)
{
	OpenConfigFile(inputfile);
	TokenizeString(inputfile);
	CheckConfigData(inputfile);
	
}

// OpenConfigFile: checks if file can be opened and isn't empty
void Parser::OpenConfigFile(std::string inputfile)
{
	std::ifstream configfile;
	// check if file can be opened 
	configfile.open(inputfile, std::ifstream::in);
	if (configfile.fail())
		throw (FailedOpenFileException());
	// check if file is empty
	 if (inputfile.empty() || inputfile.length() == 0)
	 	throw (FailedOpenFileException());

}

// TokenizeString: tokenizing a string denotes splitting a string with respect to some delimiter(s)
void Parser::TokenizeString(std::string &inputfile)
{
	// read entire file into string
	std::stringstream file(inputfile);
	std::string configFileContent((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	// take out anything from ConfigFile that isn't important
	this->ProcessConfigData(inputfile);
	// Tokenize the given vector
	std::vector<std::string> tokens = this->Tokenizing(inputfile);
	// Print tokens (for demonstration)
    for (const auto& token : tokens)
        std::cout << token << std::endl;

}

// split the string into tokens based on whitespace
std::vector<std::string> Parser::Tokenizing(const std::string &ProcessedString)
{
	std::vector<std::string> tokens;
    std::istringstream stream(ProcessedString);
    std::string token;

    while (stream >> token) {
        tokens.push_back(token);
    }

    return (tokens);
}

// ProcessConfigData: takes out anything from ConfigFile that isn't important
void Parser::ProcessConfigData(std::string &ConfigString) {
    // Regex to match single-line comments (// ...)
    std::regex singleLineCommentRegex("//.*");

    // Regex to match multi-line comments (/* ... */)
    std::regex multiLineCommentRegex("/\\*[^*]*\\*+([^/*][^*]*\\*+)*/");

    // Remove single-line comments
    ConfigString = std::regex_replace(ConfigString, singleLineCommentRegex, " ");

    // Remove multi-line comments
    ConfigString = std::regex_replace(ConfigString, multiLineCommentRegex, " ");

    // Replace newlines with spaces
    ConfigString = std::regex_replace(ConfigString, std::regex("\n+"), " ");

    // Remove excess whitespace, change multiple spaces to one
    ConfigString = std::regex_replace(ConfigString, std::regex("\\s+"), " ");
}

// CheckConfigData: checks the data within the config file
void Parser::CheckConfigData(std::string inputfile)
{
	std::ifstream configfile;
	std::string fileline;

	(void)inputfile;
	std::getline(configfile, fileline);
	// first line must be server {
	if (fileline != "server {")
	{
		std::cout << "ERROR: invalid firstline of config!" << std::endl;
		return ;
	}
	// 
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
		if (fileline.find("client_max_body_size", 0) != fileline.npos)
		{
			while (std::isdigit(fileline[index]) == 0)
				index++;
			int i = index;
			while (fileline[i] && fileline[i] != ';')
				i++;
			if (i == (int)fileline.size())
				throw (InvalidLineConfException(fileline));
			this->_client_max = fileline.substr((size_t)index, (size_t)i - index);
		}
		if (fileline.find("root", 0) != fileline.npos)
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
			this->_root = fileline.substr((size_t)index, (size_t)i - index);
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