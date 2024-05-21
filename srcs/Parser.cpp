#include "../includes/Parser.hpp"

Parser::Parser(std::string inputfile)
{
	OpenConfigFile(inputfile);
}

Parser::~Parser()
{
	this->_ip.clear();
	this->_port.clear();
	this->_server_name.clear();
	this->_client_max.clear();
	this->_root.clear();
}

// OpenConfigFile: checks if file can be opened and isn't empty
void Parser::OpenConfigFile(std::string inputfile)
{
	// to open config file
    std::ifstream configfile(inputfile);
    if (!configfile.is_open())
        throw (FailedOpenFileException());

	// convert to string
    std::stringstream buffer;
    buffer << configfile.rdbuf();
    std::string configFileContent = buffer.str();

    // Process the string to remove comments and newlines
    ProcessConfigData(configFileContent);

    // Tokenize the processed string
    std::vector<std::string> tokens = Tokenizing(configFileContent);
	if (tokens.empty())
        throw(std::runtime_error("ERROR: Tokens are empty!"));

    // Print tokens (for demonstration)
    for (const auto& token : tokens) {
        std::cout << "token: " << token << std::endl;
    }
    // Check the processed and tokenized data
    CheckConfigData(tokens);
}

// split the string into tokens based on whitespace, storing each token in a vector
std::vector<std::string> Parser::Tokenizing(const std::string &ProcessedString)
{
	std::vector<std::string> tokens;
    std::istringstream stream(ProcessedString);
    std::string token;

	// repeatedly extracts tokens from the stream
	// >> operator is used  to extract a sequence of non-whitespace characters
	// from the stream and store it in the variable token
    while (stream >> token) {
        tokens.push_back(token);
    }

    return (tokens);
}

void Parser::ProcessConfigData(std::string &configString)
{
    // Replace all tabs and returns with spaces
    configString = std::regex_replace(configString, std::regex("[\\t\\r]+"), " ");

    // Remove all non important spaces
    configString = std::regex_replace(configString, std::regex(" +\\n|\\n +"), "\n");

    // Delete all single-line comments, also inline (# and //)
    configString = std::regex_replace(configString, std::regex(R"((//.*|#.*))"), "");

    // Delete all multi-line comments (/* ... */)
    configString = std::regex_replace(configString, std::regex(R"(/\*[\s\S]*?\*/)"), "");

    // Delete all newlines
    configString = std::regex_replace(configString, std::regex("\\n"), " ");

    // Add a space before and after '{' and '}'
    configString = std::regex_replace(configString, std::regex("\\{"), " { ");
    configString = std::regex_replace(configString, std::regex("\\}"), " } ");

    // spaces before and after the ';'
    configString = std::regex_replace(configString, std::regex("([^\\s;])\\s*;"), "$1 ; ");

    // Replace multiple spaces with a space
    configString = std::regex_replace(configString, std::regex(" +"), " ");
}

// CheckConfigData: checks the data within the config file
void Parser::CheckConfigData(std::vector<std::string>& tokens)
{
	// checks if the tokens vector has at least two elements
	// and starts with "server {". If not, it prints an error and returns
    if (tokens.size() < 2 || tokens[0] != "server" || tokens[1] != "{")
    {
        std::cerr << "ERROR: invalid first line of config!" << std::endl;
        return;
    }
	// starts after "server {"
	size_t i = 2;
	while (i < tokens.size())
	{
		if (tokens[i] == "}")
		{
			// End of the server block
			tokens.erase(tokens.begin(), tokens.begin() + i + 1);
			std::cout << "End of server block found." << std::endl;
			return;
		}

		if (tokens[i] == "listen")
		{
			if (i + 1 < tokens.size())
				this->_ip = tokens[i + 1];
			i += 2;
		}
		else if (tokens[i] == "port")
		{
			if (i + 1 < tokens.size())
				this->_port = tokens[i + 1];
			i += 2;
		}
		else if (tokens[i] == "server_name")
		{
			if (i + 1 < tokens.size())
				this->_server_name = tokens[i + 1];
			i += 2;
		}
		else if (tokens[i] == "client_max_body_size")
		{
			if (i + 1 < tokens.size())
				this->_client_max = tokens[i + 1];
			i += 2;
		}
		else if (tokens[i] == "root")
		{
			if (i + 1 < tokens.size())
				this->_root = tokens[i + 1];
			i += 2;
		}
		else
			i++;
	}
    // If we reach here, there was an unmatched opening brace
    throw(std::runtime_error("Unmatched '{' in server block"));
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