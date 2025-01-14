/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: umosse <umosse@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/08 13:35:03 by umosse            #+#    #+#             */
/*   Updated: 2025/01/14 17:51:12 by umosse           ###   ########.fr       */
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

#define MAX_EVENTS 42

int	main()
{
	int	socket_fd;
	int	client_fd;
	int	epoll_fd;
	//fill structure for address to bind
	struct sockaddr_in addr; {};
	addr.sin_family = AF_INET;
	addr.sin_port = htons(9997);
	addr.sin_addr.s_addr = inet_addr("0.0.0.0");
	sockaddr_in client_addr; {};
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
			if (events[n].data.fd == socket_fd)
			{
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
				if (events[n].events & EPOLLRDHUP)
				{
					if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, events[n].data.fd, &ev) == -1)
					{
						perror("Error deleting the current connection");
						continue ;
					}
					close(events[n].data.fd);
					continue ;
				}
				if (events[n].events & EPOLLIN)
				{
					std::cout << "test\n";
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
					std::cout << "oui\n";

					//print on terminal what the server receives
					int	i = 0;
					char	buffer[1025];
					do
					{
						i = read(events[n].data.fd, buffer, 1024);
						buffer[i] = '\0';
						printf("%s", buffer);
					} while (i == 1024);

					//simulate response from server
					write(events[n].data.fd, "HTTP/1.1 200 OK\r\n", 17);
					// Content-Type: text/html
					write(events[n].data.fd, "Content-Type: text/html\r\n", 25);
					write(events[n].data.fd, "\r\n", 2);
					write(events[n].data.fd, "<head>\n\t<title>Hello world</title>\n</head>", 43);
					write(events[n].data.fd, "<h1>Hello world</h1>\n", 22);
					write(events[n].data.fd, "<p style='color: red;'>This is a paragraph</p>\n", 48);
					write(events[n].data.fd, "<a href=\"https://www.youtube.com/watch?v=MtN1YnoL46Q&pp=ygUNdGhlIGR1Y2sgc29uZw%3D%3D\">very cool link you should click</a>\n", 123);
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

	/*
		int epoll_fd = epoll_create
		add socket_fd to epoll_fd
		std::vector<int> clients;

		while (1)
		{
			EPOLL_EVENT events[32];
			int event_count = epoll_wait(epoll_fd, events, 32);

			for (int i = 0; i < event_count; i++)
			{
				// handle errors

				if (events[i].fd == socket_fd)
				{
					int client_fd = accept(...);
					clients.push_back(client_fd);

					// add client_fd to epoll_fd
				}
				else
				{
					if (events[i].flags & EPOLL_IN) {
						// read the request
					} else if (events[i].flags & EPOLL_OUT) {
						// send the response
					}
				}
			}
		}
	*/


	

	close(socket_fd);
}
