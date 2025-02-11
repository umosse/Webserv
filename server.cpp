/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: umosse <umosse@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/08 13:35:03 by umosse            #+#    #+#             */
/*   Updated: 2025/02/10 15:40:19 by umosse           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"
#include <cstddef>
#include <cstdio>
#include <fstream>
#include <ios>
#include <iterator>
#include <string>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_EVENTS 42

int	main()
{
	parsing	parsing;
	img	img;
	int	socket_fd;
	int	client_fd;
	int	epoll_fd;
	//fill structure for address to bind
	sockaddr_in addr; {};
	addr.sin_family = AF_INET;
	addr.sin_port = htons(9999);
	addr.sin_addr.s_addr = inet_addr("0.0.0.0");
	// fill struct for epoll
	struct epoll_event ev;
	struct epoll_event events[MAX_EVENTS];

	//create socket
	socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_fd == -1)
	{
		perror("socket_fd");
		exit(1);
	}
	int	b = true;
	setsockopt(socket_fd, SOL_SOCKET, SO_REUSEPORT, &b, sizeof(int));
	//bind the socket to correct address (structure created above)
	if (bind(socket_fd, (sockaddr*)&addr, sizeof(addr)))
	{
		perror("bind");
		close(socket_fd);
		exit(1);
	}
	//tell socket to ready itself to accept a connection
	if (listen(socket_fd, 42))
	{
		perror("listen");
		close(socket_fd);
		exit(1);
	}
	//setup epoll to 
	epoll_fd = epoll_create(1);
	if (epoll_fd == -1)
	{
		perror("epoll_create");
		close(socket_fd);
		exit(1);
	}
	ev.events = EPOLLIN;
	ev.data.fd = socket_fd;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, socket_fd, &ev) == -1)
	{
		perror("epoll_ctl : socket_fd");
		close(socket_fd);
		exit(1);
	}
	while (1)
	{
		int	event_count = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
		if (event_count == -1)
		{
			perror("event_count");
			close(socket_fd);
			continue ;
		}
		for (int n = 0; n < event_count; n++)
		{
			if (events[n].data.fd == socket_fd) //if (new connection)
			{
				sockaddr_in client_addr; {};
				std::cout << "waiting for epoll\n";
				socklen_t client_addr_len = sizeof(client_addr);
				//accept a connection (stock the socket into an fd)
				client_fd = accept(socket_fd, (sockaddr*)&client_addr, &client_addr_len);
				if (client_fd == -1)
				{
					perror("accept");
					close(socket_fd);
					continue ;
				}
				// fill struct for epoll
				struct epoll_event ev;
				ev.events = EPOLLIN | EPOLLRDHUP;
				ev.data.fd = client_fd;
				if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &ev) == -1)
				{
					perror("epoll_ctl : client_fd 1");
					exit(1);
				}
			}
			else
			{
				if (events[n].events & EPOLLRDHUP) // if (a client leaves)
				{
					if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, events[n].data.fd, &ev) == -1)
					{
						perror("Error deleting the current connection");
						continue ;
					}
					std::cout << "fd " << events[n].data.fd << " has been closed\n";
					close(events[n].data.fd);
					continue ;
				}
				if (events[n].events & EPOLLIN)
				{
					int	i = 0;
					char	buffer[1025];
					parsing.request_body.clear();
					do
					{
						i = recv(events[n].data.fd, buffer, 1024, 0);
						parsing.request_body.append(buffer, i);
					} while (i == 1024);
					ft_parse_request(&parsing);
					std::cout << parsing.get_request << "\n";
					std::cout << parsing.request_type << "\n";
					std::cout << parsing.request_body << "\n";
					std::cout << img.type << " <- this is the request_body\n";
					if (parsing.request_type == "GET" && (parsing.get_request == "/200.gif" || parsing.get_request == "/vstineau.jpg"))
					{
						if (parsing.get_request == "/200.gif")
						{
							std::fstream	imgFile;
							std::string		imgName;
							imgName = "www/";
							imgName.append(img.type);
							imgFile.open("www/200.gif", std::ios::in);
							if (!imgFile)
								std::cout << "gif could not be opened\n";
							else
							{
								std::cout << "gif was opened\n";
								std::string	imgStr;
								std::istreambuf_iterator<char> begin(imgFile), end;
								imgStr.assign(begin, end);
								std::string FileName2 = "oui2";
								std::ofstream ofs(FileName2.c_str(), std::ios_base::binary);  // Open output file in binary mode
								ofs.write(imgStr.c_str(), imgStr.size());
								img.oldimage = imgStr;
							}
						}
						if (parsing.get_request== "/vstineau.jpg")
						{
							std::fstream	imgFile;
							std::string		imgName;
							imgName = "www/";
							imgName.append(img.type);
							imgFile.open("www/vstineau.jpg", std::ios::in);
							if (!imgFile)
								std::cout << "vincent could not be opened\n";
							else
							{
								std::cout << "vincent was opened\n";
								std::string	imgStr;
								std::istreambuf_iterator<char> begin(imgFile), end;
								imgStr.assign(begin, end);
								std::string FileName3 = "oui3";
								std::ofstream ofs(FileName3.c_str(), std::ios_base::binary);  // Open output file in binary mode
								ofs.write(imgStr.c_str(), imgStr.size());
								img.oldimage = imgStr;
							}
						}
					}
					if (parsing.request_type == "POST")
					{
						std::string	FileName = "oui";
						std::ofstream ofs(FileName.c_str(), std::ios_base::binary);
						ft_parse_img(&parsing, &img);
						ofs << parsing.request_body;
					}
					if (parsing.request_type == "DELETE")
					{
						ft_parse_delete(&parsing);
						unlink(parsing.delURL.c_str());
					}
					struct epoll_event ev;
					ev.data.fd = events[n].data.fd;
					ev.events = EPOLLOUT;
					if (epoll_ctl(epoll_fd, EPOLL_CTL_MOD, ev.data.fd, &ev) == -1)
					{
						perror("epoll_ctl : client_fd 2");
						continue ;
					}
				}
				if (events[n].events & EPOLLOUT)
				{
					if (parsing.get_request == "/200.gif")
					{
						std::stringstream header;
						std::string page;
						page = img.oldimage;
						std::size_t content_length = img.oldimage.length();
						std::cout << "content length = " << content_length << "\n";
						header << "HTTP/1.1 200 OK\r\n";
						header << "Content-Type: image/gif\r\n";
						header << "Content-Length: " << content_length << "\r\n\r\n";
						send(events[n].data.fd, header.str().c_str(), header.str().length(), 0);
						send(events[n].data.fd, page.c_str(), page.length(), 0);
						img.oldimage.clear();
					}
					if (parsing.get_request == "/vstineau.jpg")
					{
						std::stringstream header;
						std::string page;
						page = img.oldimage;
						std::size_t content_length = img.oldimage.length();
						std::cout << "content length = " << content_length << "\n";
						header << "HTTP/1.1 200 OK\r\n";
						header << "Content-Type: image/gif\r\n";
						header << "Content-Length: " << content_length << "\r\n\r\n";
						send(events[n].data.fd, header.str().c_str(), header.str().length(), 0);
						send(events[n].data.fd, page.c_str(), page.length(), 0);
						img.oldimage.clear();
					}
					else
					{
						std::stringstream header;
						std::string page;
						page = "<!DOCTYPE html>"
						"<html lang=\"en\">"
						"<head>"
						"	<meta charset=\"UTF-8\">"
						"	<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"
						"	<title>Webserv</title>"
						"</head>"
						"	<body>"
						"		<h1>Hello world</h1>"
						"		<p style='color: red;'>This is a paragraph</p>"
						"		<a href=\"https://www.youtube.com/watch?v=MtN1YnoL46Q&pp=ygUNdGhlIGR1Y2sgc29uZw%3D%3D\" target=\"_blank\">DUCK</a>"
						"		<p></p>"
						"		<a href=\"https://www.youtube.com/watch?v=zg00AYUEU9s\" target=\"_blank\"><img src=\"https://imgs.search.brave.com/hfDqCMllFIoY-5uuVLRPZ7I-Rfm2vOt6qK0tDt5z9cs/rs:fit:860:0:0:0/g:ce/aHR0cHM6Ly9pLmlt/Z2ZsaXAuY29tLzIv/MWVsYWlmLmpwZw\" alt=\"FlexingPenguin\"/></a>"
						"		<img src=\"/200.gif\"/>"
						"		<img src=\"/vstineau.jpg\"/>"
						"		<form method=\"POST\" enctype=\"multipart/form-data\">"
						"			<input type=\"file\" id=\"actual-btn\" name=\"file\"/>"
						"			<input type=\"submit\"/>"
						"		</form>"
						"	</body>"
						"</html>";
						std::size_t content_length = page.length();
						header << "HTTP/1.1 200 OK\r\n"
						"Content-Type: text/html\r\n";
						header << "Content-Length: " << content_length << "\r\n\r\n";
						send(events[n].data.fd, header.str().c_str(), header.str().length(), 0);
						send(events[n].data.fd, page.c_str(), page.length(), 0);
					}
					struct epoll_event ev;
					ev.data.fd = events[n].data.fd;
					ev.events = EPOLLIN | EPOLLRDHUP;
					if (epoll_ctl(epoll_fd, EPOLL_CTL_MOD, events[n].data.fd, &ev) == -1)
					{
						std::cout << ev.data.fd << "\n";
						perror("epoll_ctl : client_fd 3");
						continue ;
					}
				}
			}
		}
	}
	close(socket_fd);
}
