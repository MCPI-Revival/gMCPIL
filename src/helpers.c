/*
 * helpers.c - MCPIL GTK+ helper functions
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

#define _GNU_SOURCE /* Required for asprintf */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <mcpil.h>
#include <config.h>
#include <splashes.h>

/* Helper functions */
char* get_splash_text()
{
	return splashes[rand() % sizeof(splashes) / sizeof(char*)];
}

int get_features()
{
	int i = 0;
	int sz = 0;
	int offset = 0;
	size_t buff_sz;
	char* buff;
	FILE* file;

	buff_sz = 48;
	buff = (char*)malloc((int)buff_sz + 1);
	buff[0] = 0x00;
	file = fopen("/opt/minecraft-pi-reborn-client/available-feature-flags", "r");

	if (file == NULL)
	{
		free(buff);
		return -1;
	}
	while ((sz = getline(&buff, &buff_sz, file)) > 0)
	{
		FEAT_BOOL(i) = (void*)(intptr_t)(buff[0] == 'T');
		buff[sz - 1] = 0x00;
		offset = FEAT_BOOL(i) == FALSE ? 6 : 5;
		FEAT_PTR(i) = strdup(buff + offset);
		i++;
	}
	fclose(file);
	return i;
}

int set_feature_envs(int feat)
{
	int i = 0;
	static int sz[3] = {1, 1, 1};
	static int len[3] = {0, 0, 0};
	static int tmp[3] = {0, 0, 0};

	while (i < 3)
	{
		if (i == 0 && FEAT_CMP(feat, "Touch GUI"))
		{
			i++;
			continue;
		}
		if (i == 2)
		{
			if (FEAT_CMP(feat, "Fancy Graphics") ||
				FEAT_CMP(feat, "Smooth Lighting") ||
				FEAT_CMP(feat, "Animated Water") ||
				FEAT_CMP(feat, "Disable gui_blocks Atlas"))
			{
				i++;
				continue;
			}
		}
		if (FEAT_INT(feat) == TRUE)
		{
			len[i] = strlen(FEAT_STR(feat));
			tmp[i] = sz[i] + len[i] + 1;
			features_envs[i + 1] = (char*)realloc((void*)features_envs[i + 1], tmp[i]);
			if (sz[i] == 1)
			{
				strcpy(features_envs[i + 1], FEAT_STR(feat));
			} else
			{
				strcat(features_envs[i + 1], FEAT_STR(feat));
			}
			features_envs[i + 1][tmp[i] - 2] = '|';
			features_envs[i + 1][tmp[i] - 1] = 0x00;
			sz[i] = tmp[i];
		}
		i++;
	}
	return 0;
}

feature_t* get_feature(char* str)
{
	int i = 0;

	while (i < featc)
	{
		if (FEAT_CMP(i, str))
		{
			return &(features[i]);
		}
		i++;
	}
	return NULL;
}

int get_distance(char* str)
{
	switch (str[0])
	{
		case 'F':
			return 0;
		break;
		case 'N':
			return 1;
		break;
		case 'S':
			return 2;
		break;
		case 'T':
			return 3;
		break;
	}
	return -1;
}
