#include "../includes/Parser.hpp"
#include "../includes/Server.hpp"
#include "../includes/Location.hpp"

Parser::Parser(std::string inputfile, Parser& parser) : serverblocks() {
    OpenConfigFile(inputfile, parser);
}

Parser::~Parser()
{
	this->_ip.clear();
	this->_port.clear();
	this->_server_name.clear();
	this->_client_max.clear();
	this->_root.clear();
	this->_error_page.clear();
	this->_serverindex.clear();
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

	// parse the server block
	ParseServer(tokens, parser);
}

/* Tokenizing:
-  splits the processed string into tokens based on whitespace and stores each token in a vector */
std::vector<std::string> Parser::Tokenizing(std::string &ProcessedString)
{
	std::vector<std::string> tokens;
	std::istringstream stream(ProcessedString);
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

void Parser::ParseServer(std::vector<std::string>& tokens, Parser& parser)
{
    if (tokens[0] != "server" || tokens[1] != "{")
        throw InvalidLineConfException("Configuration must start with 'server {'");

    while (!tokens.empty())
    {
        if (tokens[0] == "server")
        {
            if (tokens.size() < 2 || tokens[1] != "{")
                throw InvalidLineConfException("Expected '{' after 'server'");

            tokens.erase(tokens.begin(), tokens.begin() + 2);

            std::unordered_map<int, std::string> emptyErrorPage;
            Server temp_server("", "", "", "", "", emptyErrorPage, "", 0);

            while (!tokens.empty() && tokens[0] != "}")
            {
				if (tokens[0] == "location")
					temp_server.ParseLocationBlock(tokens);
                else if (tokens[0] == "listen")
                    temp_server.ValidateListen(tokens);
                else if (tokens[0] == "port")
                    temp_server.ValidatePort(tokens);
                else if (tokens[0] == "server_name")
                    temp_server.ValidateServerName(tokens);
                else if (tokens[0] == "client_max_body_size")
                    temp_server.ValidateClientMaxBodySize(tokens);
				else if (tokens[0] == "index")
                    temp_server.ValidateServerIndex(tokens);
                else if (tokens[0] == "root")
                    temp_server.ValidateRoot(tokens);
				else if (tokens[0] == "error_page")
                    temp_server.ValidateErrorPage(tokens);
				else if (tokens[0] == "allow_methods")
                    temp_server.ValidateAllowMethods(tokens);
                else
				{
                    throw InvalidLineConfException("Unexpected token: " + tokens[0]);
				}
            }

            if (tokens.empty() || tokens[0] != "}")
                throw InvalidLineConfException("Unmatched '{' in server block");

            parser.AddServerBlock(temp_server);
        }

		tokens.erase( tokens.begin() );
    }
}

void Server::ParseLocationBlock(std::vector<std::string>& tokens)
{
    Location newLocation;

    // Remove "location" from the vector
    tokens.erase(tokens.begin());

    // Parse the URL in the location block
    if (tokens.size() == 1 || tokens[0] == "}")
        throw std::runtime_error("Invalid Location block in config file");
	
    newLocation.ValidateLocationURL(tokens);

    // Remove the opening bracket from the vector
    if (tokens[0] != "{")
        throw std::runtime_error("Expected '{' after Location URL");
   tokens.erase(tokens.begin(), tokens.begin() + 1);

    // Loop through the tokens vector and parse the Location block
    while (tokens[0] != "}")
    {
        if (tokens.size() == 1)
            throw std::runtime_error("Invalid Location block in config file");

        // Parse the instruction
        if (tokens[0] == "allow_methods")
            newLocation.ValidateAllowMethods(tokens);
        else if (tokens[0] == "autoindex")
            newLocation.ValidateAutoIndex(tokens);
        else if (tokens[0] == "index")
            newLocation.ValidateIndex(tokens);
        else if (tokens[0] == "return")
            newLocation.ValidateReturn(tokens);
        else if (tokens[0] == "alias")
            newLocation.ValidateAlias(tokens);
        else if (tokens[0] == "root")
            newLocation.ValidateLocRoot(tokens);
        else if (tokens[0] == "fastcgi_param")
            newLocation.Validate_CGIparam(tokens);
		else if (tokens[0] == "fastcgi_pass")
            newLocation.Validate_CGIpass(tokens);
		else if (tokens[0] == "fastcgi_index")
            newLocation.Validate_CGIindex(tokens);
        else {
            throw std::runtime_error("Invalid instruction in Location block");
		}
    }

    // Remove the closing bracket from the tokens vector
    tokens.erase(tokens.begin(), tokens.begin() + 1);

    // Add the location block to the server block
    this->_locationblocks.push_back(newLocation);
}

Server& Parser::getServer(const std::string& serverName, int port)
{
    std::vector<Server>::iterator defaultServer = this->serverblocks.end();
    std::vector<Server>::iterator serverMatchPort = this->serverblocks.end();
    bool portMatchFound = false;
	
	// iterate through each server block in the serverblocks vector
    for (auto it = this->serverblocks.begin(); it != this->serverblocks.end(); ++it)
    {
		// get the server names and port for the current server block
        std::vector<std::string> serverNames = it->GetServerNames();
        int serverPort = std::stoi(it->GetPort());

		// iterate through each server name for the current server block
        for (const auto& name : serverNames)
        {
			// if the server name and port both match, return the current server block
            if (name == serverName && serverPort == port)
                return *it;

			// if the port matches and no previous port match was found, set serverMatchPort to the current server block
            if (serverPort == port && !portMatchFound)
            {
                serverMatchPort = it;
                portMatchFound = true;
            }

			// if the server name is "_" (default server) and the port matches, set defaultServer to the current server block
            if (name == "_" && serverPort == port)
                defaultServer = it;
        }
    }

	// if a server with the matching port was found, return it
    if (serverMatchPort != this->serverblocks.end())
        return *serverMatchPort;
	
	// if a default server with the matching port was found, return it
    if (defaultServer != this->serverblocks.end())
        return *defaultServer;
	
	// if no matching server or default server was found, throw an exception
    throw InvalidLineConfException("There is no matching server or default server found!");
}

const std::vector<Server>& Parser::GetServerBlocks() const
{
    return serverblocks;
}

void Parser::AddServerBlock(const Server& servers)
{
    this->serverblocks.push_back(servers);
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