#ifndef LOCATION_HPP
#define LOCATION_HPP

#include <list>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

#include "../includes/Parser.hpp"
#include "../includes/Server.hpp"


typedef enum ALLOWMETHODS {
	GET = 1,
	POST,
	DELETE,
};

class Parser;

class Server;

class Location {
	private:

	std::vector<std::string> url;
	bool 					 auto_index;
	int						 allow_methods;
	std::string				 index;
	std::string				 returnredirect;
	std::vector<std::string> cgi_path;



	public:

	Location();
	~Location();

	void ParseLocationBlock(std::vector<std::string>& tokens, Parser& parser);

	void ValidateLocationURL(std::vector<std::string> &tokens);
	void ValidateAutoIndex(std::vector<std::string> &tokens);
	void ValidateAllowMethods(std::vector<std::string> &tokens);
	void ValidateIndex(std::vector<std::string> &tokens);
	void ValidateReturn(std::vector<std::string> &tokens);
	void ValidateAlias(std::vector<std::string> &tokens);
	void Validate_CGIpath(std::vector<std::string> &tokens);
	void Validate_CGIexit(std::vector<std::string> &tokens);

	bool					 GetURL(void) const;
	bool 					 GetAutoIndex(void) const;
	bool 					 GetAllowMethods(void) const;
	std::string 			 GetIndex(void) const;
	std::string 			 GetReturnRedirect(void) const;
	std::string 			 GetAlias(void) const;
	std::vector<std::string> GetCGIPath(void) const;



};

#endif