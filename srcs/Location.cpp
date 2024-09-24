#include "../includes/Server.hpp"
#include "../includes/Parser.hpp"
#include "../includes/Location.hpp"

bool Location::cgi_status = false;

Location::Location() :
    auto_index(false),
    allow_methods(0)
	// cgi_status(false)
{}


Location::~Location() {
	this->url.clear();
	this->index.clear();
	this->returnredirect.clear();
	this->cgi_param.clear();
	this->alias.clear();
	this->locroot.clear();
	this->cgi_index.clear();
	this->cgi_pass.clear();
}

void    Location::ValidateLocationURL(std::vector<std::string> &tokens)
{
    if ( tokens.size() == 1) 
        throw Parser::InvalidLineConfException("URL is Missing!'");
    if (tokens[1] != "{")
		throw Parser::InvalidLineConfException("{ is Missing after Location URL!'");

	// add the URL to the location block after removing any trailing '/'
    this->url = tokens[0];
    // erase the processed URL token and {
     tokens.erase(tokens.begin(), tokens.begin() + 1);
}

void    Location::ValidateAutoIndex(std::vector<std::string> &tokens)
{

    if ( tokens.size() == 1) 
        throw Parser::InvalidLineConfException("on / off is Missing!'");

	// erase autoindex token
    tokens.erase( tokens.begin());

	if (tokens[1] != ";")
		throw Parser::InvalidLineConfException("; is Missing after auto_index!'");

	if (tokens[0] == "on")
		this->auto_index  = true;
	else if (tokens[0] == "off")
		this->auto_index = false;
	else
		throw Parser::InvalidLineConfException("on / off is Missing!'");

    // erase on or off and ;
    tokens.erase(tokens.begin(), tokens.begin() + 2);
}

void Location::Validate_AllowMethods(std::vector<std::string>& tokens) {
    if (tokens.size() < 3 || tokens[1] == ";") {
        throw Parser::InvalidLineConfException("The Allow Method is Missing!");
    }

    // erase allow_methods token
    tokens.erase(tokens.begin(), tokens.begin() + 1);

    // Check if the first token is a semicolon, which means no method is provided
    if (tokens[0] == ";") {
        throw Parser::InvalidLineConfException("The Allow Method is Missing!");
    }
	

    // Loop through the tokens to collect allowed methods
    size_t i = 0;
    while (i < tokens.size()) {
        if (tokens[i] == ";") {
            if (i == 0) {
                throw Parser::InvalidLineConfException("The Allow Method is Missing!");
            }
            break;
        }
        if (tokens[i] == "GET") {
            allow_methods.push_back("GET");
        }
		else if (tokens[i] == "POST") {
            allow_methods.push_back("POST");
        }
		else if (tokens[i] == "DELETE") {
            allow_methods.push_back("DELETE");
        } else {
            throw Parser::InvalidLineConfException("Incorrect Allow Method, it should be GET, POST or DELETE!");
        }

        i++;
    }

    // Check if the last token is not a semicolon
    if (i == tokens.size() || tokens[i] != ";") {
        throw Parser::InvalidLineConfException("; is Missing!");
    }

    // erase allow methods and ;
    tokens.erase(tokens.begin(), tokens.begin() + i + 1);
}

// void    Location::ValidateIndex(std::vector<std::string> &tokens)
// {
//     if ( tokens.size() == 1) 
//         throw Parser::InvalidLineConfException("The Index is Missing!'");

// 	// erase index token
//     tokens.erase(tokens.begin());

//   	if (!((tokens[0] == "/index.html" && tokens[1] == ";") || (tokens[0] == "/" && tokens[1] == ";"))) {
//         throw Parser::InvalidLineConfException("The Index must be '/index.html' or '/' followed by ';'");
// 	}
// 	this->index = tokens[0];

//     // erase index and ;
//      tokens.erase(tokens.begin(), tokens.begin() + 2);
// }

void    Location::ValidateIndex(std::vector<std::string> &tokens)
{
    if ( tokens.size() == 1) 
        throw Parser::InvalidLineConfException("The Index is Missing!'");

	// erase index token
    tokens.erase( tokens.begin());
	if (tokens[0] == ";") {
		this->index = "EMPTY";
		tokens.erase(tokens.begin(), tokens.begin() + 1); // erase ;
		return;
	}
  	// if (!((tokens[0] == "/index.html" && tokens[1] == ";") || (tokens[0] == "/" && tokens[1] == ";"))) {
    //     throw Parser::InvalidLineConfException("The Index must be '/index.html' or '/' followed by ';'");
	// }
	if (tokens[0] == "/" && tokens[1] == ";")
		this->index = "/index.html";
	else
		this->index = tokens[0];

    // erase index and ;
     tokens.erase(tokens.begin(), tokens.begin() + 2);
}

void    Location::ValidateReturn(std::vector<std::string> &tokens) {
    if ( tokens.size() == 1) {
        throw Parser::InvalidLineConfException("The redirection html is Missing!'");
	}
	// erase return token
       tokens.erase(tokens.begin(), tokens.begin());
	if (tokens[0] == ";") {
		throw Parser::InvalidLineConfException("The redirection html is Missing!");
	}

	if (tokens[3] != ";") {
		throw Parser::InvalidLineConfException("The ; is Missing!");
	}
    this->returnredirectcode = tokens[1];
	this->returnredirect = tokens[2];
    // erase return and ;
    tokens.erase(tokens.begin(), tokens.begin() + 4);
}

void    Location::ValidateAlias(std::vector<std::string> &tokens)
{
    if ( tokens.size() == 1) 
        throw Parser::InvalidLineConfException("The Alias is Missing!'");

	// erase alias token
    tokens.erase( tokens.begin());

	if (tokens[0] == ";")
		throw Parser::InvalidLineConfException("The Alias is Missing!");

	if (tokens[1] != ";")
		throw Parser::InvalidLineConfException("; is Missing!");

	this->alias = tokens[0];

    // erase alias and ;
     tokens.erase(tokens.begin(), tokens.begin() + 2);
}

void Location::ValidateLocRoot(std::vector<std::string>& tokens)
{
    // ensure there's a token after 'root'
    if (tokens.size() < 3|| tokens[1] == ";")
        throw Parser::InvalidLineConfException("Root token is missing!");

    // ensure there's a semicolon after the root
    if (tokens[2] != ";")
        throw Parser::InvalidLineConfException("Invalid Root Token");

    // add the root to the list
    this->locroot = (tokens[1]);

    // erase the processed tokens
    tokens.erase(tokens.begin(), tokens.begin() + 3);
}


//  CGIparam sets parameters that are passed to the FastCGI server
void    Location::Validate_CGIparam(std::vector<std::string> &tokens)
{
    if ( tokens.size() == 1) 
        throw Parser::InvalidLineConfException("The CGI Param is Missing!'");

	// erase fastcgi_param token
    tokens.erase(tokens.begin(), tokens.begin() + 1);

	if (tokens.size() <= 1)
		throw Parser::InvalidLineConfException("Invalid CGI Param!'");
	while (tokens[0] != ";")
	{
		if (tokens[0] == "}")
			throw Parser::InvalidLineConfException("Invalid CGI Param!'");
		this->cgi_param.push_back(tokens[0]);
		tokens.erase(tokens.begin());
	}

    // erase ;
     tokens.erase(tokens.begin(), tokens.begin() + 1);
}

//  sets the default file to serve when a directory is requested
void    Location::Validate_CGIindex(std::vector<std::string> &tokens)
{
    if ( tokens.size() == 1) 
        throw Parser::InvalidLineConfException("The CGI Index is Missing!'");

	// erase fastcgi_index token
    tokens.erase(tokens.begin(), tokens.begin() + 1);
	
	if (tokens[0] == ";")
		throw Parser::InvalidLineConfException("The CGI Index is Missing!");

	if (tokens[1] != ";")
		throw Parser::InvalidLineConfException("; is Missing!");

    // add the root to the list
    this->cgi_index = (tokens[0]);

    // erase ;
    tokens.erase(tokens.begin(), tokens.begin() + 2);
}

// specifies the address of the FastCGI server to which requests should be sent.
void    Location::Validate_CGIpass(std::vector<std::string> &tokens)
{
    if ( tokens.size() == 1) 
        throw Parser::InvalidLineConfException("The CGI Index is Missing!'");

	// erase fastcgi_pass token
    tokens.erase(tokens.begin(), tokens.begin() + 1);
	
	if (tokens[0] == ";")
		throw Parser::InvalidLineConfException("The CGI Pass is Missing!");

	if (tokens[1] != ";")
		throw Parser::InvalidLineConfException("; is Missing!");

    // add the root to the list
    this->cgi_pass = (tokens[0]);

    // erase ;
    tokens.erase(tokens.begin(), tokens.begin() + 2);
}

void    Location::Validate_CGI(std::vector<std::string> &tokens)
{
    if ( tokens.size() == 1) 
        throw Parser::InvalidLineConfException("on / off is Missing!'");

	// erase autoindex token
    tokens.erase( tokens.begin());

	if (tokens[1] != ";")
		throw Parser::InvalidLineConfException("; is Missing after cgi!'");

	if (tokens[0] == "on")
		this->cgi_status  = true;
	else if (tokens[0] == "off")
		this->cgi_status = false;
	else
		throw Parser::InvalidLineConfException("on / off is Missing!'");

    // erase on or off and ;
    tokens.erase(tokens.begin(), tokens.begin() + 2);
}

std::string	Location::GetURL(void) const {
    return (this->url);
}

bool    Location::GetAutoIndex(void) const {
    return (this->auto_index);
}

std::vector<std::string> Location::Get_AllowMethods() const {
    // std::vector<std::string> methods_as_string;
    // for (const auto& method : allow_methods) {
    //         if (method == "GET")
    //         {
    //             methods_as_string.push_back("GET");
    //             break;
    //         }
    //         if (method == "POST")
    //         {
    //             methods_as_string.push_back("POST");
    //             break;
    //         }
    //         if (method == "DELETE")
    //         {
    //             methods_as_string.push_back("DELETE");
    //             break;
    //         }
    // }
    // return methods_as_string;
    return (this->allow_methods);
}

std::string Location::GetIndex(void) const {
    return (this->index);
}

std::string Location::GetAlias(void) const {
    return (this->alias);
}

std::string Location::GetReturnRedirect(void) {
    return (this->returnredirect);
}

std::string Location::GetReturnRedirectCode(void)
{
    return (this->returnredirectcode);
}

std::vector<std::string> Location::GetCGIparam(void) const {
    return (this->cgi_param);
}

std::string Location::GetLocRoot(void) const {
    return (this->locroot);
}

std::string Location::GetCGIindex(void) const {
    return (this->cgi_index);
}

std::string Location::GetCGIpass(void) const {
    return (this->cgi_pass);
}

bool Location::GetCGIstatus(void) {
    return (cgi_status);
}
