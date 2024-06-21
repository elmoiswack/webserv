#include "../includes/Server.hpp"
#include "../includes/Parser.hpp"
#include "../includes/Location.hpp"

Location::Location() :
    auto_index(false),
    allow_methods(0)
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
    this->url.push_back( std::regex_replace( tokens[0], std::regex("/+$"), "") );

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

void    Location::ValidateAllowMethods(std::vector<std::string> &tokens)
{
    if ( tokens.size() == 1) 
        throw Parser::InvalidLineConfException("The Allow Method is Missing!'");

	// erase allow_methods token
    tokens.erase(tokens.begin(), tokens.begin() + 1);

	if (tokens[0] == ";")
		throw Parser::InvalidLineConfException("The Allow Method is Missing!");
	if (tokens[1] != ";") {
		throw Parser::InvalidLineConfException("; is Missing!");
	}
		if (tokens[0] == "GET")
		this->allow_methods = GET;
	else if (tokens[0] == "POST")
		this->allow_methods = POST;
	else if (tokens[0] == "DELETE")
		this->allow_methods = DELETE;
	else
		throw Parser::InvalidLineConfException("Incorrect Allow Method, it should be GET, POST or DELETE!");
	
	// erase allow method and ;
    tokens.erase(tokens.begin(), tokens.begin() + 2);
}

void    Location::ValidateIndex(std::vector<std::string> &tokens)
{
    if ( tokens.size() == 1) 
        throw Parser::InvalidLineConfException("The Index is Missing!'");

	// erase index token
    tokens.erase(tokens.begin());

    if (tokens[0] != "/index.html" || tokens[0] != "/" || tokens[1] != ";") {
        throw Parser::InvalidLineConfException("The Index must be '/index.html' or "/" followed by ';'");
	}
	
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

	if (tokens[2] != ";") {
		throw Parser::InvalidLineConfException("The ; is Missing!");
	}

	this->returnredirect = tokens[0];

    // erase return and ;
    tokens.erase(tokens.begin(), tokens.begin() + 3);
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

std::vector<std::string>	Location::GetURL(void) const {
    return (this->url);
}

bool    Location::GetAutoIndex(void) const {
    return (this->auto_index);
}

bool    Location::GetAllowMethods(void) const {
    return (this->allow_methods);
}

std::string Location::GetIndex(void) const {
    return (this->index);
}

std::string Location::GetAlias(void) const {
    return (this->alias);
}

std::string Location::GetReturnRedirect(void) const {
    return (this->returnredirect);
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