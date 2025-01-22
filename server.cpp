/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: umosse <umosse@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/08 13:35:03 by umosse            #+#    #+#             */
/*   Updated: 2025/01/22 14:03:03 by umosse           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <fstream>

#define MAX_EVENTS 42

int	main()
{
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
					//print on terminal what the server receives
					int	i = 0;
					char	buffer[1025];
					do
					{
						i = read(events[n].data.fd, buffer, 1024);
						buffer[i] = '\0';
						printf("%s", buffer);
					} while (i == 1024);
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
					//simulate response from server
					std::string header;
					header = "HTTP/1.1 200 OK\r\n"
					"Content-Type: text/html\r\n"
					"Content-Length: 604\r\n"
					"\r\n";
					send(events[n].data.fd, header.c_str(), header.length(), 0);
					std::string page;
					page = "<!DOCTYPE html>"
					"<html lang=\"en\">"
					"<head>"
					"	<meta charset=\"UTF-8\">"
					"	<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"
					"	<title>Document</title>"
					"</head>"
					"<body>"
					"	<h1>Hello world</h1>"
					"	<p style='color: red;'>This is a paragraph</p>"
					"	<a href=\"https://www.youtube.com/watch?v=MtN1YnoL46Q&pp=ygUNdGhlIGR1Y2sgc29uZw%3D%3D\" target=\"_blank\">DUCK</a>"
					"	<p></p>"
					"	<a href=\"https://www.youtube.com/watch?v=zg00AYUEU9s\" target=\"_blank\"><img src=\"https://yt3.googleusercontent.com/OuwCgGAdNqyeyJ8i3JocIXzjHdSBtZzMpnltZqtQBjRE5Xx7RCXO3XTzRgZ_JmKPFxkakbb6Ng=s900-c-k-c0x00ffffff-no-rj\" alt=\"FlexingPenguin\"</a>"
					"</body>"
					"</html>";
					send(events[n].data.fd, page.c_str(), page.length(), 0);
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
