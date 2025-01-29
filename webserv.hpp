/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: umosse <umosse@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/23 13:17:10 by umosse            #+#    #+#             */
/*   Updated: 2025/01/29 13:35:28 by umosse           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include <stdlib.h>
# include <stdio.h>
# include <iostream>
# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <unistd.h>
# include <sys/epoll.h>
# include <fstream>
# include <string>
# include <sstream>

struct parsing {
	std::string	request_body;
	std::string	request_type;
};

void	ft_parse_request(parsing *parsing);