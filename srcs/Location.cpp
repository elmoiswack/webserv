#include "../includes/Server.hpp"
#include "../includes/Parser.hpp"
#include "../includes/Location.hpp"

Location::Location()
{

}

Location::~Location()
{
}

void Location::ParseLocationBlock(std::vector<std::string>& tokens, Parser& parser)
{
	Location locationblock;
	// remove location cause you know it is already there
	// then check if the url is missing
	tokens.erase(tokens.begin());
    if ( tokens[0] == "{" || tokens.size() == 1)
        throw Parser::InvalidLineConfException("Location needs a '{ after it");

	// validate url that comesafter location
	locationblock.ValidateLocationURL(tokens);

	// now we continue looping after the Location URL {
    while (tokens[0] != "}")
    {
            if (tokens.size() < 2)
                throw Parser::InvalidLineConfException("Expected {");
            while (!tokens.empty() && tokens[0] != "}")
            {
				// if (tokens[0] == "root")
				// 	locationblock.ParseLocation(tokens);
				if (tokens[0] == "autoindex")
					locationblock.ValidateAutoIndex(tokens);
                else if (tokens[0] == "allow_methods")
                    locationblock.ValidateAllowMethods(tokens);
                else if (tokens[0] == "index")
                    locationblock.ValidateIndex(tokens);
                else if (tokens[0] == "return")
                    locationblock.ValidateReturn(tokens);
                else if (tokens[0] == "alias")
                    locationblock.ValidateAlias(tokens);
                else if (tokens[0] == "cgi_path")
                    locationblock.Validate_CGIpath(tokens);
				else if (tokens[0] == "cgi_exit")
                    locationblock.Validate_CGIexit(tokens);
                else
                    throw Parser::InvalidLineConfException("Unexpected token: " + tokens[0]);
            }

            if (tokens.empty() || tokens[0] != "}")
                throw Parser::InvalidLineConfException("Unmatched '{' in server block");

            tokens.erase(tokens.begin());

            parser.AddServerBlock(temp_server);
        }
        else
            throw Parser::InvalidLineConfException("Unexpected token: " + tokens[0]);
    }
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
    tokens.erase( tokens.begin() + 1);
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
    tokens.erase( tokens.begin() + 1);
}

void    Location::ValidateAllowMethods(std::vector<std::string> &tokens)
{

    if ( tokens.size() == 1) 
        throw Parser::InvalidLineConfException("The Allow Method is Missing!'");

	// erase allow_methods token
    tokens.erase( tokens.begin());

	if (tokens[0] != ";" || tokens[1] != ";")
		throw Parser::InvalidLineConfException("The Allow Method or ; is Missing!");

	if (tokens[0] == "GET")
		this->allow_methods = GET;
	else if (tokens[0] == "POST")
		this->allow_methods = POST;
	else if (tokens[0] == "DELETE")
		this->allow_methods = DELETE;
	else
		throw Parser::InvalidLineConfException("Incorrect Allow Method, it should be GET, POST or DELETE!");

    // erase allow method and ;
    tokens.erase( tokens.begin() + 1);
}

void    Location::ValidateIndex(std::vector<std::string> &tokens)
{

    if ( tokens.size() == 1) 
        throw Parser::InvalidLineConfException("The Index is Missing!'");

	// erase index token
    tokens.erase( tokens.begin());

	if (tokens[0] != ";" || tokens[1] != ";")
		throw Parser::InvalidLineConfException("The Index or ; is Missing!");
	
	this->index = tokens[0];

    // erase index and ;
    tokens.erase( tokens.begin() + 1);
}

void    Location::ValidateReturn(std::vector<std::string> &tokens)
{

    if ( tokens.size() == 1) 
        throw Parser::InvalidLineConfException("The redirection html is Missing!'");

	// erase return token
    tokens.erase( tokens.begin());

	if (tokens[0] != ";" || tokens[1] != ";")
		throw Parser::InvalidLineConfException("The redirection html or ; is Missing!");
	
	this->returnredirect = tokens[0];

    // erase return and ;
    tokens.erase( tokens.begin() + 1);
}

void    Location::ValidateAlias(std::vector<std::string> &tokens)
{

    if ( tokens.size() == 1) 
        throw Parser::InvalidLineConfException("The Alias is Missing!'");

	// erase alias token
    tokens.erase( tokens.begin());

	if (tokens[0] != ";" || tokens[1] != ";")
		throw Parser::InvalidLineConfException("The Alias or ; is Missing!");
	
	this->alias = tokens[0];

    // erase alias and ;
    tokens.erase(tokens.begin() + 1);
}

void    Location::Validate_CGIpath(std::vector<std::string> &tokens)
{
    if ( tokens.size() == 1) 
        throw Parser::InvalidLineConfException("The CGI Path is Missing!'");

	// erase cgi_path token
    tokens.erase( tokens.begin());

	while (tokens[0] != ";")
	{
		if (tokens[0])
		this->cgi_path.push_back(tokens[0]);
		tokens.erase(tokens.begin());
	}

    // erase ;
    tokens.erase( tokens.begin() + 1);
}

void    Location::Validate_CGIexit(std::vector<std::string> &tokens)
{
    if ( tokens.size() == 1) 
        throw Parser::InvalidLineConfException("The CGI Path is Missing!'");

	// erase cgi_path token
    tokens.erase( tokens.begin());

	while (tokens[0] != ";")
	{
		if (tokens[0])
		this->cgi_path.push_back(tokens[0]);
		tokens.erase(tokens.begin());
	}

    // erase ;
    tokens.erase( tokens.begin() + 1);
}

bool    Location::GetURL(void) const {
    return (this->url);
}

bool    Location::GetAutoIndex(void) const {
    return (this->auto_index);
}

bool    Location::GetAllowMethods( void ) const {
    return (this->allow_methods);
}

std::string Location::GetIndex( void ) const {
    return (this->index);
}

std::string Location::GetAlias( void ) const
{
    return (this->alias);
}

std::string Location::GetReturnRedirect( void ) const {
    return (this->returnredirect);
}

std::vector<std::string> Location::GetCGIPath( void ) const {
    return (this->returnredirect);
}