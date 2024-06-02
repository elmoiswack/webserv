#include "../includes/Parser.hpp"

Parser::Parser(std::string inputfile, Parser& parser) :
	serverblocks()
	{
	OpenConfigFile(inputfile, parser);
}

Parser::~Parser()
{
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
            Server temp_server("", "", "", "", "", emptyErrorPage);

            while (!tokens.empty() && tokens[0] != "}")
            {
				// if (tokens[0] == "location")
				// 	temp_server.ParseLocation(tokens);
                if (tokens[0] == "listen")
                    temp_server.ValidateListen(tokens);
                else if (tokens[0] == "port")
                    temp_server.ValidatePort(tokens);
                else if (tokens[0] == "server_name")
                    temp_server.ValidateServerName(tokens);
				else if (tokens[0] == "error_page")
                    temp_server.ValidateErrorPage(tokens);
                else if (tokens[0] == "client_max_body_size")
                    temp_server.ValidateClientMaxBodySize(tokens);
                else if (tokens[0] == "root")
                    temp_server.ValidateRoot(tokens);
                else
                    throw InvalidLineConfException("Unexpected token: " + tokens[0]);
            }

            if (tokens.empty() || tokens[0] != "}")
                throw InvalidLineConfException("Unmatched '{' in server block");

            tokens.erase(tokens.begin());

            parser.AddServerBlock(temp_server);
        }
        else
            throw InvalidLineConfException("Unexpected token: " + tokens[0]);
    }
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