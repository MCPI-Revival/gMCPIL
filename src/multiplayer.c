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

int mp_port;
char* buff;
char* mp_ip;

int build_sockaddr(struct sockaddr_in* dst_addr, char* ip, int port)
{
	struct hostent* host = gethostbyname(ip);
	if (host == NULL)
	{
		return -1;
	}
	memcpy(&dst_addr->sin_addr, host->h_addr_list[0], host->h_length);
	dst_addr->sin_family = AF_INET;
	dst_addr->sin_port = htons(port);
	return 0;
}

/* Extends port search (19132-19139) and address search (mp_ip:mp_port). */
HOOK(sendto, ssize_t, (int sockfd, const void* buf, size_t len, int flags, const struct sockaddr* dest_addr, socklen_t addrlen))
{
	int i = 19136;
	struct sockaddr_in* addr = (struct sockaddr_in*)dest_addr;
	struct sockaddr tmp_addr;

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
		if (build_sockaddr((struct sockaddr_in*)&tmp_addr, mp_ip, mp_port) == 0)
		{
			if ((*real_sendto)(sockfd, buf, len, flags, &tmp_addr, addrlen) < 0)
			{
				fprintf(stderr, "sendto failed with exit errno %i\n", errno);
			}
		}
		addr->sin_port = htons(19135);
	}
	return (*real_sendto)(sockfd, buf, len, flags, dest_addr, addrlen);
}

void __attribute__((constructor)) init()
{
	int sz = 0;
	char* lf_ptr;
	char* slash_ptr;
	char* servers_path;
	FILE* servers_file;

	asprintf(&servers_path, "%s/.minecraft-pi/servers.txt", getenv("HOME"));

	servers_file = fopen(servers_path, "r");

	if (servers_file == NULL)
	{
		free(servers_path);
		return;
	}

	fseek(servers_file, 0, SEEK_END);
	sz = ftell(servers_file);
	fseek(servers_file, 0, SEEK_SET);

	buff = (char*)malloc(sz);
	fread((void*)buff, 1, sz, servers_file);

	lf_ptr = strchr(buff, '\n');
	slash_ptr = strchr(buff, '/');
	if (lf_ptr == NULL || slash_ptr == NULL)
	{
		free(servers_path);
		free(buff);
		return;
	}
	*lf_ptr = 0x00;
	*slash_ptr = 0x00;

	mp_port = atoi(slash_ptr + 1);
	mp_ip = buff;

	free(servers_path);
	return;
}

void __attribute__((destructor)) destroy()
{
	free(buff);
	return;
}
