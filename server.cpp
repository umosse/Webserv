/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: umosse <umosse@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/08 13:35:03 by umosse            #+#    #+#             */
/*   Updated: 2025/01/08 14:46:42 by umosse           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

int	main()
{
	int	socket_fd;
	int	client_fd;
	//fill structure for address to bind
	struct sockaddr_in addr {};
	addr.sin_family = AF_INET;
	addr.sin_port = htons(9999);
	addr.sin_addr.s_addr = inet_addr("0.0.0.0");
	sockaddr_in client_addr {};

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
	

	socklen_t client_addr_len = sizeof(client_addr);
	//accept a connection (stock the socket into an fd)
	client_fd = accept(socket_fd, (sockaddr*)&client_addr, &client_addr_len);
	if (client_fd == -1)
	{
		perror("accept");
		close(socket_fd);
		exit(1);
	}
	std::cout << "oui\n";

	//print on terminal what the server receives
	int	n;
	char	buffer[1025];
	do
	{
		n = read(client_fd, buffer, 1024);
		buffer[n] = '\0';
		printf("%s", buffer);
	} while (n == 1024);

	//simulate response from server
	write(client_fd, "HTTP/1.1 200 OK\r\n", 17);
	// Content-Type: text/html
	write(client_fd, "Content-Type: text/html\r\n", 25);
	write(client_fd, "\r\n", 2);
	write(client_fd, "<p style='color: red;'>Hello world</p>", 39);

	close(socket_fd);
}
