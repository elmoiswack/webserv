#include "../includes/Parser.hpp"
#include "../includes/Server.hpp"

/* ValidateServerBlock:
-  validates the server block tokens and assigns values to the member
   variables in Server */
void Server::ValidateServerBlock(std::vector<std::string>& tokens, Parser& parser)
{
	// instance of Server is created will be used to store parsed data
	Server Server_Block(*this);

	// remove server and {
	tokens.erase( tokens.begin(), tokens.begin() + 2 );
	while (!tokens.empty() && tokens[0] != "}")
    {
        if (tokens[0] == "listen")
            Server_Block.ValidateListen(tokens);
        else if (tokens[0] == "port")
            Server_Block.ValidatePort(tokens);
        else if (tokens[0] == "server_name")
            Server_Block.ValidateServerName(tokens);
        else if (tokens[0] == "client_max_body_size")
            Server_Block.ValidateClientMaxBodySize(tokens);
        else if (tokens[0] == "root")
            Server_Block.ValidateRoot(tokens);
        else
            throw Parser::InvalidLineConfException("Invalid token in server block: " + tokens[0]);
    }
    if (tokens.empty() || tokens[0] != "}")
        throw Parser::InvalidLineConfException("Unmatched '{' in server block");
	 std::cout << "passed!" << std::endl;
	tokens.erase(tokens.begin(), tokens.begin() + 1);
	parser.AddServerBlock(Server_Block);
	
}

void Server::ValidateListen(std::vector<std::string>& tokens)
{
    // ensure there's a token after 'listen'
    if (tokens.size() < 3|| tokens[1] == ";")
        throw std::runtime_error("Listen token is missing!");

    // ensure there's a semicolon after the IP
    if (tokens[2] != ";")
        throw std::runtime_error("Invalid Listen Token");


	std::string ip = tokens[1];
    // check that the IP contains only digits and dots
    int dot_count = 0;
    for (size_t i = 0; i < ip.size(); ++i)
    {
		char c = ip[i];
        if (!isdigit(c) && c != '.')
            throw std::runtime_error("Invalid character in IP address");
        if (c == '.')
            dot_count++;
    }

	// ensure there are exactly 3 dots in the IP address
    if (dot_count != 3)
        throw std::runtime_error("Invalid IP address format");

	tokens.erase(tokens.begin(), tokens.begin() + 1);

    // add the IP to the list
    this->_ip = (tokens[1]);

    // erase the processed tokens
    tokens.erase(tokens.begin(), tokens.begin() + 2);
}

void Server::ValidatePort(std::vector<std::string>& tokens)
{

    // ensure there's a token after 'port'
    if (tokens.size() < 3 || tokens[1] == ";")
        throw Parser::InvalidLineConfException("Port token is missing!");

    // ensure there's a semicolon after the port
    if (tokens[2] != ";")
        throw Parser::InvalidLineConfException("Invalid Port Token");

	std::string port = tokens[1];
	std::cout << "port: " << port[0] << std::endl;
    // check that the port contains only digits
    for (size_t i = 0; i < port.size(); ++i)
    {
		char c = port[i];
		std::cout << "port c: " << c << std::endl;
        if (!isdigit(c))
            throw std::runtime_error("Invalid character in Port Token");
    }

    // add the port to the list
    this->_port = (tokens[1]);

    // erase the processed tokens
    tokens.erase(tokens.begin(), tokens.begin() + 3);
}

void Server::ValidateServerName(std::vector<std::string>& tokens)
{
    // ensure there's a token after 'server_name'
    if (tokens.size() < 3 || tokens[1] == ";")
        throw Parser::InvalidLineConfException("Server Name token is missing!");

    // ensure there's a semicolon after the server name
    if (tokens[2] != ";")
        throw Parser::InvalidLineConfException("Invalid Server Name Token");

    // add the server name to the list
    this->_server_name = (tokens[1]);

    // erase the processed tokens
    tokens.erase(tokens.begin(), tokens.begin() + 3);
}

void Server::ValidateClientMaxBodySize(std::vector<std::string>& tokens)
{
    // ensure there's a token after 'client_max_body_size'
    if (tokens.size() < 3 || tokens[1] == ";")
        throw Parser::InvalidLineConfException("Client Max Body Size token is missing!");

    std::string value = tokens[1];
	std::cout << "value: " << value << std::endl;
    // ensure there's a semicolon after the size
    if (tokens[2] != ";")
        throw Parser::InvalidLineConfException("Invalid Client Max Body Size Token");

    // check the validity of the client max body size value
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

    // erase the processed tokens
    tokens.erase(tokens.begin(), tokens.begin() + 3);
	std::cout << "after client body max tokens[0]: " << tokens[0] << std::endl;
}

void Server::ValidateRoot(std::vector<std::string>& tokens)
{
    // ensure there's a token after 'root'
    if (tokens.size() < 3|| tokens[1] == ";")
        throw Parser::InvalidLineConfException("Root token is missing!");

    // ensure there's a semicolon after the root
    if (tokens[2] != ";")
        throw Parser::InvalidLineConfException("Invalid Root Token");

    // add the root to the list
    this->_root = (tokens[1]);

    // erase the processed tokens
    tokens.erase(tokens.begin(), tokens.begin() + 3);
	std::cout << "after root tokens[0]: " << tokens[0] << std::endl;
}