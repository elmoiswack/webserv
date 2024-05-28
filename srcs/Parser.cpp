#include "../includes/Parser.hpp"

// initalise Parser and call OpenConfigFile to read and process config file
Parser::Parser(std::string inputfile, Parser& parser)
{
	OpenConfigFile(inputfile, parser);
}

// destructor that clears the private member strings
Parser::~Parser()
{
	this->_ip.clear();
	this->_port.clear();
	this->_server_name.clear();
	this->_client_max.clear();
	this->_root.clear();
}

/* OpenConfigFile:
- opens and reads the config file, processes it to remove comments and
  unnecessary spaces, tokenizes the content, and checks and parses the data */
void Parser::OpenConfigFile(std::string inputfile, Parser& parser)
{
	// to open config file
    std::ifstream configfile(inputfile);
    if (!configfile.is_open())
        throw (FailedOpenFileException());

	// read file into string
    std::stringstream buffer;
    buffer << configfile.rdbuf();
    std::string configFileContent = buffer.str();

    // process the string to remove comments and newlines
    ProcessConfigData(configFileContent);

    // tokenize the processed string
    std::vector<std::string> tokens = Tokenizing(configFileContent);
	if (tokens.empty())
        throw(std::runtime_error("ERROR: Tokens are empty!"));

    // print tokens (for demonstration)
    for (const auto& token : tokens) {
        std::cout << "token: " << token << std::endl;
    }
	// parse the server block
	ParseServer(tokens, parser);
}

/* Tokenizing:
-  splits the processed string into tokens based on whitespace and stores each token in a vector */
std::vector<std::string> Parser::Tokenizing(std::string &ProcessedString)
{
	// empty vector of strings to store the tokens from input string
	std::vector<std::string> tokens;
	// use stream extraction operator to read from string as if it was a stream (file or standard input)
    std::istringstream stream(ProcessedString);
	// empty string to store each token temporarily
    std::string token;

	// >> operator reads non whitespace characters from stream and stores them in token variable
	// push_back is used to add an element to end of vector
    while (stream >> token)
        tokens.push_back(token);

    return (tokens);
}

/* ProcessConfigData:
-  processes the config string to remove comments, unnecessary spaces, and
   formats it properly for tokenization */
void Parser::ProcessConfigData(std::string &configString)
{
    // replace all tabs and returns with spaces
    configString = std::regex_replace(configString, std::regex("[\\t\\r]+"), " ");

    // remove all non important spaces
    configString = std::regex_replace(configString, std::regex(" +\\n|\\n +"), "\n");

    // delete all single-line comments, also inline (# and //)
    configString = std::regex_replace(configString, std::regex(R"((//.*|#.*))"), "");

    // delete all multi-line comments (/* ... */)
    configString = std::regex_replace(configString, std::regex(R"(/\*[\s\S]*?\*/)"), "");

    // delete all newlines
    configString = std::regex_replace(configString, std::regex("\\n"), " ");

    // add a space before and after '{' and '}'
    configString = std::regex_replace(configString, std::regex("\\{"), " { ");
    configString = std::regex_replace(configString, std::regex("\\}"), " } ");

    // spaces before and after the ';'
    configString = std::regex_replace(configString, std::regex("([^\\s;])\\s*;"), "$1 ; ");

    // replace multiple spaces with a space
    configString = std::regex_replace(configString, std::regex(" +"), " ");
}

/* ParseServer:
-  validates and parses the server block configuration */
void Parser::ParseServer(std::vector<std::string>& tokens, Parser& parser)
{
    // Ensure the configuration starts with "server {"
    if (tokens[0] != "server" || tokens[1] != "{")
        throw InvalidLineConfException("Configuration must start with 'server {'");

    while (!tokens.empty())
    {
        if (tokens[0] == "server")
        {
            // check if there is a '{' after 'server'
            if (tokens.size() < 2 || tokens[1] != "{")
                throw InvalidLineConfException("Expected '{' after 'server'");
            
			// temp Server object to call ValidateServerBlock (can perhaps be done better?)
			Server temp_server(*this);
			// validate the server block tokens and assigns value to variables
            temp_server.ValidateServerBlock(tokens, parser);
        }
        else
            throw InvalidLineConfException("Unexpected token: " + tokens[0]);
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

std::string Parser::GetClientMax()
{
	return (this->_client_max);
}

std::string Parser::GetRoot()
{
	return (this->_root);
}

void Parser::AddServerBlock(const Server& server_block)
{
    serverblocks.push_back(server_block);
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