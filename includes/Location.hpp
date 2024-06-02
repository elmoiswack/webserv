#ifndef LOCATION_HPP
#define LOCATION_HPP

#include <list>
#include <stdio.h>
#include <string.h>
#include <vector>

#include "../includes/Parser.hpp"
#include "../includes/Server.hpp"


typedef enum e_allowmethods {
	GET,
	POST,
	DELETE,
} t_allowmethods;

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
	std::string				 alias;
	std::string				 locroot;

	std::vector<Location>	 locations_info;

	public:

	Location();
	~Location();

	// void ParseLocationBlock(std::vector<std::string>& tokens);

	void ValidateLocationURL(std::vector<std::string> &tokens);
	void ValidateAutoIndex(std::vector<std::string> &tokens);
	void ValidateAllowMethods(std::vector<std::string> &tokens);
	void ValidateIndex(std::vector<std::string> &tokens);
	void ValidateReturn(std::vector<std::string> &tokens);
	void ValidateAlias(std::vector<std::string> &tokens);
	void Validate_CGIpath(std::vector<std::string> &tokens);
	void Validate_CGIexit(std::vector<std::string> &tokens);
	void ValidateLocRoot(std::vector<std::string>& tokens);

std::vector<std::string>					 GetURL(void) const;
	bool 					 GetAutoIndex(void) const;
	bool 					 GetAllowMethods(void) const;
	std::string 			 GetIndex(void) const;
	std::string 			 GetReturnRedirect(void) const;
	std::string 			 GetAlias(void) const;
	std::vector<std::string> GetCGIPath(void) const;

	std::string GetLocRoot(void) const;

};

#endif