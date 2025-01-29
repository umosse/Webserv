#include "webserv.hpp"

void	ft_parse_request(parsing *parsing)
{
	if (parsing->request_body.find("GET") == 0)
		parsing->request_type = "GET";
	else if (parsing->request_body.find("POST") == 0)
		parsing->request_type = "POST";
	else if (parsing->request_body.find("DELETE") == 0)
		parsing->request_type = "DELETE";
	else
		parsing->request_type = "ERROR";
}
