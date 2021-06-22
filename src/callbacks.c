/*
 * callbacks.c - MCPIL GTK+ callbacks
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

#define _GNU_SOURCE /* Required for asprintf */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <mcpil.h>
#include <config.h>
#include <splashes.h>

/* Callbacks */
void features_cb(__attribute__((unused)) GtkWidget* button, void* udata)
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

	if ((int)udata == TRUE)
	{
		mcpil_config_set_features(config, features_envs[4]);
		mcpil_config_save(config);
	}
	return;
}

void toggle_cb(__attribute__((unused)) GtkWidget* check, void* udata)
{
	int i = (int)udata;

	FEAT_BOOL(i) = (void*)(intptr_t)(!FEAT_INT(i));
	return;
}

void multiplayer_cb(__attribute__((unused)) GtkWidget* button, __attribute__((unused)) void* udata)
{
	const char* ip;
	const char* port;
	GtkEntryBuffer* ip_buff;
	GtkEntryBuffer* port_buff;

	ip_buff = gtk_entry_get_buffer(GTK_ENTRY(settings_box.ip_entry));
	port_buff = gtk_entry_get_buffer(GTK_ENTRY(settings_box.port_entry));
	ip = gtk_entry_buffer_get_text(ip_buff);
	port = gtk_entry_buffer_get_text(port_buff);

	if (strlen(port) < 2 || strlen(ip) < 3)
	{
		return;
	}

	mcpil_config_set_ip(config, ip);
	mcpil_config_set_port(config, port);
	mcpil_config_save(config);
	setenv("GMCPIL_SERVER_IP", ip, 1);
	setenv("GMCPIL_SERVER_PORT", port, 1);
	return;
}

void watch_cb(GPid pid, __attribute__((unused)) int status, __attribute__((unused)) void* udata)
{
	gtk_widget_show_all(window);
	g_spawn_close_pid(pid);
	return;
}

void launch_cb(__attribute__((unused)) GtkWidget* button, __attribute__((unused)) void* udata)
{
	char* argv[] = {"minecraft-pi-reborn-client", NULL};
	GPid pid;
	GError* err = NULL;

	g_spawn_async(NULL, argv, NULL, G_SPAWN_DO_NOT_REAP_CHILD | G_SPAWN_SEARCH_PATH, NULL, NULL, &pid, &err);

	if (err != NULL)
	{
		g_error("Spawning child failed: %s", err->message);
		return;
	}
	if (mcpil_config_get_hide(config)[0] == 'T')
	{
		gtk_widget_hide(window);
	}
	g_child_watch_add(pid, watch_cb, NULL);
	return;
}

void select_cb(__attribute__((unused)) GtkWidget* list, GtkListBoxRow* row, void* udata)
{
	int i = 0;
	char tmp[2] = {0x00, 0x00};

	i = gtk_list_box_row_get_index(row);
	gtk_label_set_text(GTK_LABEL(udata), profile_descriptions[i]);

	setenv("MCPI_FEATURE_FLAGS", features_envs[i], 1);
	tmp[0] = i + 0x30;
	mcpil_config_set_last_profile(config, tmp);
	return;
}

void settings_cb(__attribute__((unused)) GtkWidget* button, __attribute__((unused)) void* udata)
{
	const char* username;
	const char* distance;
	const char* hud;
	gboolean hide;
	GtkEntryBuffer* username_buff;
	GtkEntryBuffer* hud_buff;

	username_buff = gtk_entry_get_buffer(GTK_ENTRY(settings_box.username_entry));
	hud_buff = gtk_entry_get_buffer(GTK_ENTRY(settings_box.hud_entry));

	username = gtk_entry_buffer_get_text(username_buff);
	distance = gtk_combo_box_text_get_active_text(settings_box.distance_combo);
	hud = gtk_entry_buffer_get_text(hud_buff);
	hide = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(settings_box.hide_check));

	setenv("MCPI_USERNAME", username, 1);
	setenv("MCPI_RENDER_DISTANCE", distance, 1);
	setenv("GALLIUM_HUD", hud, 1);

	mcpil_config_set_username(config, username);
	mcpil_config_set_distance(config, distance);
	mcpil_config_set_hud(config, hud);
	if (hide)
	{
		mcpil_config_set_hide(config, "TRUE");
	} else
	{
		mcpil_config_set_hide(config, "FALSE");
	}
	mcpil_config_save(config);
	return;
}

void about_cb(__attribute__((unused)) GtkWidget* button, __attribute__((unused)) void* udata)
{
	GError* err;
	gtk_show_uri_on_window(GTK_WINDOW(window), "https://discord.gg/jEXwEdx", GDK_CURRENT_TIME, &err);
	return;
}

void activate_cb(GtkApplication* app, __attribute__((unused)) void* udata)
{
	GdkPixbuf* icon;
	GtkWidget* notebook;

	window = gtk_application_window_new(app);
	gtk_window_set_title(GTK_WINDOW(window), "gMCPIL - MCPIL GTK+ Edition");
	gtk_window_set_default_size(GTK_WINDOW(window), 458, 375); // I don't even know

	icon = gtk_icon_theme_load_icon(gtk_icon_theme_get_default(), "gmcpil", 32, GTK_ICON_LOOKUP_NO_SVG, NULL);
	gtk_window_set_icon(GTK_WINDOW(window), icon);

	notebook = gtk_notebook_new();

	Play_tab(notebook);
	Features_tab(notebook);
	Multiplayer_tab(notebook);
	Settings_tab(notebook);
	About_tab(notebook);

	gtk_container_add(GTK_CONTAINER(window), notebook);
	gtk_widget_show_all(window);
	return;
}
