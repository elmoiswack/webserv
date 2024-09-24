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

	std::string url;
	bool 					 auto_index;
	std::vector<std::string> allow_methods;
	std::string				 index;
	std::string				 returnredirect;
	std::string				 returnredirectcode;
	std::vector<std::string> cgi_param;
	std::string				 alias;
	std::string				 locroot;
	std::string				 cgi_index;
	std::string				 cgi_pass;
	static bool				 cgi_status;

	std::vector<Location>	 locations_info;

	public:

	Location();
	~Location();

	// void ParseLocationBlock(std::vector<std::string>& tokens);

	void ValidateLocationURL(std::vector<std::string> &tokens);
	void ValidateAutoIndex(std::vector<std::string> &tokens);
	void Validate_AllowMethods(std::vector<std::string> &tokens);
	void ValidateIndex(std::vector<std::string> &tokens);
	void ValidateReturn(std::vector<std::string> &tokens);
	void ValidateAlias(std::vector<std::string> &tokens);
	void Validate_CGIparam(std::vector<std::string> &tokens);
	void ValidateLocRoot(std::vector<std::string>& tokens);
	void Validate_CGIindex(std::vector<std::string>& tokens);
	void Validate_CGIpass(std::vector<std::string>& tokens);
	void Validate_CGI(std::vector<std::string>& tokens);

	std::string				 GetURL(void) const;
	bool 					 GetAutoIndex(void) const;
	std::vector<std::string> Get_AllowMethods(void) const;
	std::string 			 GetIndex(void) const;
	std::string 			 GetReturnRedirect(void);
	std::string 			 GetReturnRedirectCode(void);
	std::string 			 GetAlias(void) const;
	std::vector<std::string> GetCGIparam(void) const;
	std::string				 GetCGIindex(void) const;
	std::string				 GetCGIpass(void) const;
	std::string GetLocRoot(void) const;
	static bool GetCGIstatus(void);
};

#endif