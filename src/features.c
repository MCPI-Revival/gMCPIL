/*
 * features.c - gMCPIL features tab
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
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <mcpil.h>
#include <config.h>

int featc;
char* features_envs[5] = {""};
feature_t features[32];

static int get_features()
{
	int i = 0;
	int sz = 0;
	int offset = 0;
	size_t buff_sz;
	char* buff;
	FILE* file;

	file = fopen("/opt/minecraft-pi-reborn-client/available-feature-flags", "r");
	if (file == NULL)
	{
		return -1;
	}

	buff_sz = 48;
	buff = (char*)malloc((int)buff_sz + 1);
	buff[0] = 0x00;
	while ((sz = getline(&buff, &buff_sz, file)) > 0)
	{
		FEAT_BOOL(i) = (void*)(intptr_t)(buff[0] == 'T');
		buff[sz - 1] = 0x00;
		offset = FEAT_BOOL(i) == FALSE ? 6 : 5;
		FEAT_PTR(i) = strdup(buff + offset);
		i++;
	}
	free(buff);
	fclose(file);
	return i;
}

static int set_feature_envs(int feat)
{
	int i = 0;
	int sz[3] = {1, 1, 1};
	int len[3] = {0, 0, 0};
	int tmp[3] = {0, 0, 0};

	while (i < 3)
	{
		if (i == 0 && FEAT_CMP(feat, "Touch GUI"))
		{
			i++;
			continue;
		}
		if (i == 2 && (FEAT_CMP(feat, "Fancy Graphics")
			|| FEAT_CMP(feat, "Smooth Lighting")
			|| FEAT_CMP(feat, "Animated Water")
			|| FEAT_CMP(feat, "Disable gui_blocks Atlas")))
		{
			i++;
			continue;
		}
		if (FEAT_INT(feat) == TRUE || (i >= 2 && i <= 3 && FEAT_CMP(feat, "Force Mob Spawning")))
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

static feature_t* get_feature(char* str)
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

static void features_cb(__attribute__((unused)) GtkWidget* button, void* udata)
{
	int i = 0;
	int sz = 1;
	int len = 0;
	int tmp = 0;

	if (features_envs[4] != NULL)
	{
		free(features_envs[4]);
	}
	features_envs[4] = (char*)malloc(1);

	while (i < featc)
	{
		if (FEAT_INT(i) == TRUE)
		{
			len = strlen(FEAT_STR(i));
			tmp = sz + len + 1;
			features_envs[4] = (char*)realloc((void*)features_envs[4], tmp);
			if (sz == 1)
			{
				strcpy(features_envs[4], FEAT_STR(i));
			} else
			{
				strcat(features_envs[4], FEAT_STR(i));
			}
			features_envs[4][tmp - 2] = '|';
			features_envs[4][tmp - 1] = 0x00;
			sz = tmp;
		}
		i++;
	}

	if (udata == NULL)
	{
		gmcpil_config_set_features(config, features_envs[4]);
		gmcpil_config_save(config);
	}
	return;
}

static void toggle_cb(__attribute__((unused)) GtkWidget* check, void* udata)
{
	int i = (int)udata;

	FEAT_BOOL(i) = (void*)(intptr_t)(!FEAT_INT(i));
	return;
}

TAB(Features, features_cb, {
	int i = 0;
	int last_profile;
	int last_featc;
	char* tmp;
	char* features_buff;
	GtkWidget* feature_check;
	GtkWidget* scroll;
	GtkWidget* vbox;
	feature_t* feature;

	featc = 0;
	featc = get_features();
	last_featc = gmcpil_config_get_last_featc(config);
	last_profile = gmcpil_config_get_last_profile(config);

	features_envs[1] = (char*)malloc(1);
	features_envs[2] = (char*)malloc(1);
	features_envs[3] = (char*)malloc(1);
	features_envs[4] = NULL;
	while (i < featc)
	{
		set_feature_envs(i);
		i++;
	}

	features_buff = gmcpil_config_get_features(config);
	if (features_buff != NULL)
	{
		i = 0;
		while (i < last_featc)
		{
			FEAT_BOOL(i) = FALSE;
			i++;
		}

		i = 0;
		tmp = strtok(features_buff, "|");
		while (tmp != NULL && i < last_featc)
		{
			feature = get_feature(tmp);
			if (feature != NULL)
			{
				*feature[0] = (void*)TRUE;
			}
			tmp = strtok(NULL, "|");
			i++;
		}
	} else
	{
		printf("Yeah, no.\n");
	}

	scroll = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scroll), GTK_SHADOW_NONE);
	gtk_container_set_border_width(GTK_CONTAINER(scroll), 0);

	vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_widget_set_margin_top(vbox, 6);

	i = 0;
	while (i < featc)
	{
		feature_check = gtk_check_button_new_with_label(FEAT_STR(i));
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(feature_check), FEAT_INT(i));
		g_signal_connect(feature_check, "toggled", G_CALLBACK(toggle_cb), (void*)(intptr_t)i);
		gtk_box_pack_start(GTK_BOX(vbox), feature_check, FALSE, FALSE, 0);
		i++;
	}

	features_cb(NULL, (void*)TRUE);
	gmcpil_config_set_last_featc(config, featc);
	gmcpil_config_set_features(config, features_envs[4]);
	setenv("MCPI_FEATURE_FLAGS", features_envs[last_profile], 1);
	gtk_container_add(GTK_CONTAINER(scroll), vbox);
	gtk_box_pack_start(GTK_BOX(tab), scroll, TRUE, TRUE, 0);
});
