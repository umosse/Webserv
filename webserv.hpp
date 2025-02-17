/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: umosse <umosse@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/23 13:17:10 by umosse            #+#    #+#             */
/*   Updated: 2025/02/17 13:29:49 by umosse           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include <cstddef>
# include <stdlib.h>
# include <stdio.h>
# include <iostream>
# include <sys/types.h>
# include <sys/stat.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <unistd.h>
# include <sys/epoll.h>
# include <fstream>
# include <string>
# include <string.h>
# include <sstream>
# include <dirent.h>

struct parsing {
	std::string	request_body;
	std::string	request_type;
	std::string	get_request;

	std::string	delURL;
};

struct img {
	std::string	key;
	std::string	body;
	std::string	type;



	std::string	oldimage;
};

void	ft_parse_request(parsing *parsing);
void	ft_parse_img(parsing *parsing, img *img);
void	ft_parse_delete(parsing *parsing);