#include "../includes/Parser.hpp"
#include "../includes/Server.hpp"

void Server::ValidateListen(std::vector<std::string>& tokens)
{
    // ensure there's a token after 'listen'
    if (tokens.size() < 3|| tokens[1] == ";") {
        throw std::runtime_error("Listen token is missing!");
	}

    // ensure there's a semicolon after the IP
    if (tokens[2] != ";") {
        throw std::runtime_error("Invalid Listen Token");
	}


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
    if (dot_count != 3) {
        throw std::runtime_error("Invalid IP address format");
	}

	tokens.erase(tokens.begin(), tokens.begin() + 1);

    // add the IP to the list
    this->_ip = (tokens[0]);

    // erase the processed tokens
    tokens.erase(tokens.begin(), tokens.begin() + 2);
}

void Server::ValidatePort(std::vector<std::string>& tokens)
{
    // ensure there's a token after 'port'
    if (tokens.size() < 3 || tokens[1] == ";") {
        throw Parser::InvalidLineConfException("Port token is missing!");
	}

    // ensure there's a semicolon after the port
    if (tokens[2] != ";") {
        throw Parser::InvalidLineConfException("Invalid Port Token");
	}

	std::string port = tokens[1];
    // check that the port contains only digits
    for (size_t i = 0; i < port.size(); ++i)
    {
		char c = port[i];
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
}

void Server::ValidateErrorPage(std::vector<std::string>& tokens)
{
    // Check if there are enough tokens to process
    if (tokens.size() < 3)
        throw Parser::InvalidLineConfException("Invalid Error Page Token: Not enough tokens");

    // Erase the 'error_page' token
    tokens.erase(tokens.begin());

    // Temporary vector to hold tokens until ';' is found
    std::vector<std::string> tmp;
    while (!tokens.empty() && tokens[0] != ";")
    {
        tmp.push_back(tokens[0]);
        tokens.erase(tokens.begin());
    }

    // Ensure that the ';' token is found
    if (tokens.empty() || tokens[0] != ";")
        throw Parser::InvalidLineConfException("Invalid Error Page Token: Missing ';'");

    // Erase the ';' token
    tokens.erase(tokens.begin());

    // If no tokens were copied to tmp, it's an invalid error page directive
    if (tmp.empty())
        throw Parser::InvalidLineConfException("Invalid Error Page Token: No error codes or URL");

    // Take the error page URL and store it in errorURL, then remove it from tmp
    std::string errorURL = tmp.back();
    tmp.pop_back();

    // Loop through each tmp token to process error codes
    for (const auto& token : tmp)
    {
        try
        {
            size_t i = 0;
            int error_code = std::stoi(token, &i, 10);
            if (i != token.size())
            {
                std::cerr << "Invalid character in error code: " << token.substr(i) << std::endl;
                throw std::runtime_error("Invalid error code in config file");
            }

            // Insert error code and URL into the error page map
            _error_page[error_code] = errorURL;
        }
        catch (const std::exception& e)
        {
            std::cerr << "Error processing error code '" << token << "': " << e.what() << std::endl;
            throw std::runtime_error("Invalid error code in config file");
        }
    }
}

void    Server::ValidateServerIndex(std::vector<std::string> &tokens)
{
    if ( tokens.size() == 1) 
        throw Parser::InvalidLineConfException("The Index is Missing!'");

	// erase index token
    tokens.erase( tokens.begin());
    if (tokens[0] != "/index.html" || (tokens[0] != "/" && tokens[1] != ";")) {
        throw Parser::InvalidLineConfException("The Index must be '/index.html' or '/' followed by ';'");
	}
	
	this->_serverindex = tokens[0];

    // erase index and ;
     tokens.erase(tokens.begin(), tokens.begin() + 2);
}

void Server::ValidateAllowMethods(std::vector<std::string>& tokens) {
	if (tokens.size() < 3 || tokens[1] == ";") {
        throw Parser::InvalidLineConfException("The Allow Method is Missing!");
	}

    // erase allow_methods token
    tokens.erase(tokens.begin(), tokens.begin() + 1);

    if (tokens[0] == ";") {
        throw Parser::InvalidLineConfException("The Allow Method is Missing!");
	}

    if (tokens[1] != ";") {
        throw Parser::InvalidLineConfException("; is Missing!");
    }

    if (tokens[0] == "GET")
        this->_allow_methods = GET;
    else if (tokens[0] == "POST")
        this->_allow_methods = POST;
    else if (tokens[0] == "DELETE")
        this->_allow_methods = DELETE;
    else
        throw Parser::InvalidLineConfException("Incorrect Allow Method, it should be GET, POST or DELETE!");

    // erase allow method and ;
    tokens.erase(tokens.begin(), tokens.begin() + 2);
}

std::string Server::GetIp() const
{
	return (this->_ip);
}

std::string Server::GetPort() const
{
	return (this->_port);
}

std::string Server::GetServName() const
{
	return (this->_server_name);
}

std::string Server::GetClientMax() const
{
	return (this->_client_max);
}

std::string Server::GetRoot() const
{
	return (this->_root);
}

std::unordered_map<int, std::string> Server::GetErrorPage() const
{
	return (this->_error_page);
}

std::string Server::GetServerIndex() const
{
	return (this->_serverindex);
}

std::vector<Location> Server::GetLocations() const {
    return (_locationblocks);
}

std::vector<std::string> Server::GetServerNames() const
{
    std::vector<std::string> names;
    names.push_back(this->_server_name);
    return names;
}

int Server::GetAllowMethods() const {
    return _allow_methods;
}