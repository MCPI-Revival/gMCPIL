/*
 * multiplayer.c - Proxy-free MCPI multiplayer
 * 
 * Copyright 2021 Alvarito050506 <donfrutosgomez@gmail.com>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */

#define _GNU_SOURCE /* Required for RTLD_NEXT */

/* Lots of standard includes and libraries, for types and function prototypes. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#include <sys/mman.h>
#include <libgen.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>

#include <libreborn.h>
#include <servers.h>
#include <config.h>

char* buff = NULL;

int build_sockaddr(server_t* server)
{
	struct hostent* host;
	struct sockaddr_in* addr_in;

	addr_in = (struct sockaddr_in*)&server->addr;
	if (server == NULL || server->ip == NULL || server->port == 0)
	{
		return -1;
	}

	host = gethostbyname(server->ip);
	if (host == NULL)
	{
		return -2;
	}

	memcpy(&addr_in->sin_addr, host->h_addr_list[0], host->h_length);
	addr_in->sin_family = AF_INET;
	addr_in->sin_port = htons(server->port);
	return 0;
}

/* Extends port search (19132-19139) and address search (servers->ip:servers->port). */
HOOK(sendto, ssize_t, (int sockfd, const void* buf, size_t len, int flags, const struct sockaddr* dest_addr, socklen_t addrlen))
{
	int i = 19136;
	struct sockaddr_in* addr = (struct sockaddr_in*)dest_addr;

	ensure_sendto();
	if (addr->sin_addr.s_addr == -1 && ntohs(addr->sin_port) == 19135)
	{
		while (i < 19139)
		{
			addr->sin_port = htons(i);
			if ((*real_sendto)(sockfd, buf, len, flags, dest_addr, addrlen) < 0)
			{
				fprintf(stderr, "sendto failed with exit errno %i\n", errno);
			}
			i++;
		}
		i = 0;
		while (i < (sizeof(servers) / sizeof(server_t)))
		{
			if ((*real_sendto)(sockfd, buf, len, flags, &servers[i].addr, (socklen_t)(sizeof(servers[i].addr))) < 0)
			{
				fprintf(stderr, "sendto failed with exit errno %i\n", errno);
			}
			i++;
		}
		addr->sin_port = htons(19135);
	}
	return (*real_sendto)(sockfd, buf, len, flags, dest_addr, addrlen);
}

void __attribute__((constructor)) init()
{
	int i = 1;
	char* config_path;
	MCPILConfig* config;

	asprintf(&config_path, "%s/.minecraft-pi/gmcpil.json", getenv("HOME"));
	config = mcpil_config_new(config_path);
	free(config_path);

	while (i < (sizeof(servers) / sizeof(server_t)))
	{
		build_sockaddr(&servers[i]);
		i++;
	}

	servers[0].port = (short)strtol(mcpil_config_get_port(config), NULL, 10);
	servers[0].ip = mcpil_config_get_ip(config);

	build_sockaddr(&servers[0]);
	g_object_unref(config);
	return;
}

void __attribute__((destructor)) destroy()
{
	if (buff != NULL)
	{
		free(buff);
	}
	return;
}
