/*
 * servers.h - MCPI featured servers
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

#ifndef SERVERS_H
#define SERVERS_H

#include <stdlib.h>
#include <sys/socket.h>

typedef struct server_t
{
	char* ip;
	unsigned short port;
	struct sockaddr addr;
} server_t;

server_t servers[] = {
	/* gMCPIL "custom" server */
	{NULL, 0, {}},
	/* Featured servers */
	{"pbptanarchy.tk", 19132, {}},
	{"thebrokenrail.com", 19132, {}}
};

#endif /* SERVERS_H */
