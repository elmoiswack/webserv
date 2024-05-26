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
	ParseServer(tokens);
}

// split the string into tokens based on whitespace, storing each token in a vector
std::vector<std::string> Parser::Tokenizing(std::string &ProcessedString)
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
			// tokens.erase(tokens.begin(), tokens.begin() + i + 1);
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

void Parser::ParseServer(std::vector<std::string>& tokens)
{
    size_t i = 0;

	// Ensure the configuration starts with "server {"
    if (tokens[i] != "server" || tokens[i + 1] != "{")
    {
		std::cout << "tokens[i]: " << tokens[i] << std::endl;
		std::cout << "tokens[i + 1]: " << tokens[i + 1] << std::endl;
        throw InvalidLineConfException("Configuration must start with 'server {'");
    }

	ValidateServerBlock(tokens, i);

	while (i < tokens.size())
    {
        if (tokens[i] == "server")
        {
            ValidateServerBlock(tokens, i);
        }
        else
        {
            throw InvalidLineConfException("Unexpected token: " + tokens[i]);
        }
    }

}

void Parser::ValidateServerBlock(std::vector<std::string>& tokens, size_t i)
{
	Server Server_Block(*this);

	// std::cout << "tokens[0]: " << tokens[0] << std::endl;
	tokens.erase( tokens.begin(), tokens.begin() + 2 );
	// std::cout << "tokens[0]: " << tokens[0] << std::endl;
	std::cout << "tokens.size(): " << tokens.size() << std::endl;
	while (i < tokens.size() && tokens[0] != "}")
    {
        if (tokens[0] == "listen")
            Server_Block.ValidateListen(tokens, i);
        else if (tokens[0] == "port")
            Server_Block.ValidatePort(tokens, i);
        else if (tokens[0] == "server_name")
            Server_Block.ValidateServerName(tokens, i);
        else if (tokens[0] == "client_max_body_size")
            Server_Block.ValidateClientMaxBodySize(tokens, i);
        else if (tokens[0] == "root")
            Server_Block.ValidateRoot(tokens, i);
        else
            throw InvalidLineConfException("Invalid token in server block: " + tokens[i]);
			std::cout << "tokens[0]: " << tokens[0] << std::endl;
    }
    if (tokens[i] != "}")
        throw InvalidLineConfException("Unmatched '{' in server block");
	 std::cout << "passed!" << std::endl;
	tokens.erase(tokens.begin(), tokens.begin() + 1);
	 this->serverblocks.push_back(Server_Block);
	
}

void Server::ValidateListen(std::vector<std::string>& tokens, size_t i)
{
    // Ensure there's a token after 'listen'
    if (i + 1 >= tokens.size() || tokens[i + 1] == ";")
        throw std::runtime_error("Listen token is missing!");

    // Ensure there's a semicolon after the IP
    if (i + 2 >= tokens.size() || tokens[i + 2] != ";")
        throw std::runtime_error("Invalid Listen Token");

	tokens.erase(tokens.begin(), tokens.begin() + 1);
	// check whether it can be converted to int
	try {
		this->_ip.push_back(std::stoi(tokens[0]));
		}
		catch(const std::exception& e) {
            throw ( std::runtime_error( "Invalid IP number!" ) );
    }

    // Add the IP to the list
    this->_ip = (tokens[i + 1]);

    // Erase the processed tokens
    tokens.erase(tokens.begin(), tokens.begin() + 2);
}

void Server::ValidatePort(std::vector<std::string>& tokens, size_t i)
{

    // Ensure there's a token after 'port'
    if (i >= tokens.size() || tokens[0 + 1] == ";")
        throw Parser::InvalidLineConfException("Port token is missing!");

    // Ensure there's a semicolon after the port
    if (0 + 2 >= tokens.size() || tokens[0 + 2] != ";")
        throw Parser::InvalidLineConfException("Invalid Port Token");

    // Add the port to the list
    this->_port = (tokens[i + 1]);

    // Erase the processed tokens
    tokens.erase(tokens.begin(), tokens.begin() + 3);
}

void Server::ValidateServerName(std::vector<std::string>& tokens, size_t i)
{
    // Ensure there's a token after 'server_name'
    if (i + 1 >= tokens.size() || tokens[0 + 1] == ";")
        throw Parser::InvalidLineConfException("Server Name token is missing!");

    // Ensure there's a semicolon after the server name
    if (0 + 2 >= tokens.size() || tokens[0 + 2] != ";")
        throw Parser::InvalidLineConfException("Invalid Server Name Token");

    // Add the server name to the list
    this->_server_name = (tokens[i + 1]);

    // Erase the processed tokens
    tokens.erase(tokens.begin(), tokens.begin() + 3);
}

void Server::ValidateClientMaxBodySize(std::vector<std::string>& tokens, size_t i)
{
    // Ensure there's a token after 'client_max_body_size'
    if (i + 1 >= tokens.size() || tokens[0 + 1] == ";")
        throw Parser::InvalidLineConfException("Client Max Body Size token is missing!");

    std::string value = tokens[0 + 1];
	std::cout << "value: " << value << std::endl;
    // Ensure there's a semicolon after the size
    if (0 + 2 >= tokens.size() || tokens[0 + 2] != ";")
        throw Parser::InvalidLineConfException("Invalid Client Max Body Size Token");

    // Check the validity of the client max body size value
    if (value.length() > 10)
        throw Parser::InvalidLineConfException("Invalid Client Max Body Size Token");

    char unit = value.back();
    if (unit == 'B' || unit == 'K' || unit == 'M')
        value.pop_back();
    else
        unit = '\0';

    if (value.find_first_not_of("1234567890") != std::string::npos)
        throw Parser::InvalidLineConfException("Invalid Client Max Body Size Token");

    uint64_t val = (uint64_t)std::stol(value);
    switch (unit)
    {
    case 'M':
        val *= (1 << 20);
        break;
    case 'K':
        val *= (1 << 10);
        break;
    case 'B':
    default:
        break;
    }

    this->_client_max = std::to_string(val);

    // Erase the processed tokens
    tokens.erase(tokens.begin(), tokens.begin() + 3);
	std::cout << "after client body max tokens[0]: " << tokens[0] << std::endl;
}

void Server::ValidateRoot(std::vector<std::string>& tokens, size_t i)
{
	std::cout << "tokens[0]: " << tokens[0] << std::endl;
    // Ensure there's a token after 'root'
    if (i + 1 >= tokens.size() || tokens[0 + 1] == ";")
        throw Parser::InvalidLineConfException("Root token is missing!");

    // Ensure there's a semicolon after the root
    if (0 + 2 >= tokens.size() || tokens[0 + 2] != ";")
        throw Parser::InvalidLineConfException("Invalid Root Token");

    // Add the root to the list
    this->_root = (tokens[0 + 1]);

    // Erase the processed tokens
    tokens.erase(tokens.begin(), tokens.begin() + 3);
	std::cout << "after root tokens[0]: " << tokens[0] << std::endl;
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

std::string Parser::GetClientMax()
{
	return (this->_client_max);
}

std::string Parser::GetRoot()
{
	return (this->_root);
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