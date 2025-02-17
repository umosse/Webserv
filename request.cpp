#include "webserv.hpp"
#include <cstdio>
#include <string>

void	ft_parse_request(parsing *parsing)
{
	int	offset = 0;
	int	pos = 0;
	int len = 0;

	if (parsing->request_body.find("GET") == 0)
	{
		parsing->request_type = "GET";
		offset = parsing->request_body.find("/");
		pos = parsing->request_body.find("HTTP", offset) - 1;
		len = pos - offset;
		parsing->get_request = parsing->request_body.substr(offset, len);
	}
	else if (parsing->request_body.find("POST") == 0)
		parsing->request_type = "POST";
	else if (parsing->request_body.find("DELETE") == 0)
		parsing->request_type = "DELETE";
	else
		parsing->request_type = "ERROR";
}

void	ft_parse_img(parsing *parsing, img *img)
{
	std::string	str;
	int	offset = 0;
	int	pos = 0;
	int	len = 0;

	// find key
	offset = parsing->request_body.find("\r\n\r\n");
	offset = parsing->request_body.find("------", offset);
	pos = parsing->request_body.find("\r\n", offset);
	if (pos == -1 || offset == -1)
		return ;
	len = pos - offset;
	img->key = parsing->request_body.substr(offset, len);

	// find img type
	offset = parsing->request_body.find("filename=", pos);
	offset = parsing->request_body.find("\"", offset) + 1;
	pos = parsing->request_body.find("\"\r\n", offset);
	if (pos == -1 || offset == -1)
		return ;
	len = pos - offset;
	img->type = parsing->request_body.substr(offset, len);
	// if img->type is the same as a preexisting image, stop.

	// get img body
	offset = parsing->request_body.find("\r\n\r\n", pos) + 4;
	pos = parsing->request_body.find(img->key, offset);
	if (pos == -1 || offset == -1)
		return ;
	len = pos - offset;
	img->body = parsing->request_body.substr(offset, len);
	std::string	FileName = "www/";
	FileName.append(img->type);
	std::ofstream ofs(FileName.c_str(), std::ios_base::binary);
	ofs << img->body;

	offset = parsing->request_body.find("\r\n", pos);
}

void	ft_parse_delete(parsing *parsing)
{
	int	offset = 0;
	int	pos = 0;
	int	len = 0;

	offset = parsing->request_body.find("/") + 1;
	pos = parsing->request_body.find(" HTTP", offset);
	len = pos - offset;
	parsing->delURL = parsing->request_body.substr(offset, len);
	std::cout << parsing->delURL << "\n";
}
