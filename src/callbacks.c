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
 * 
 */

#define _GNU_SOURCE /* Required for asprintf */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <mcpil.h>
#include <splashes.h>

/* Callbacks */
void features_cb(GtkWidget* button, void* udata)
{
	int i = 0;
	int sz = 1;
	int len = 0;
	int tmp = 0;
	char* features_path;
	FILE* features_file;

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
		asprintf(&features_path, "%s/.minecraft-pi/profile.txt", getenv("HOME"));
		features_file = fopen(features_path, "w");
		if (features_file != NULL)
		{
			fwrite((const void*)features_envs[4], 1, sz - 2, features_file);
			fclose(features_file);
		}
		free(features_path);
	}
	return;
}

void toggle_cb(GtkWidget* check, void* udata)
{
	int i = (int)udata;

	FEAT_BOOL(i) = (void*)(!FEAT_INT(i));
	return;
}

void multiplayer_cb(GtkWidget* button, void* udata)
{
	int pid;
	char* ip_port;
	char* servers_path;
	const char* ip;
	const char* port;
	FILE* servers_file;
	GtkEntryBuffer* ip_buff;
	GtkEntryBuffer* port_buff;

	ip_buff = gtk_entry_get_buffer(GTK_ENTRY(multiplayer.ip_entry));
	port_buff = gtk_entry_get_buffer(GTK_ENTRY(multiplayer.port_entry));
	ip = gtk_entry_buffer_get_text(ip_buff);
	port = gtk_entry_buffer_get_text(port_buff);

	if (strlen(port) < 2 || strlen(ip) < 3)
	{
		return;
	}

	asprintf(&ip_port, "%s/%s\n", ip, port);
	asprintf(&servers_path, "%s/.minecraft-pi/servers.txt", getenv("HOME"));

	servers_file = fopen(servers_path, "w");
	if (servers_file == NULL)
	{
		free(servers_path);
		return;
	}

	fwrite((const void*)ip_port, 1, strlen(ip_port), servers_file);
	fclose(servers_file);
	free(ip_port);
	return;
}

void watch_cb(GPid pid, int status, void* udata)
{
	g_spawn_close_pid(pid);
	return;
}

void launch_cb(GtkWidget* button, void* udata)
{
	char* argv[] = {"/usr/bin/minecraft-pi", NULL};
	GPid pid;
	GError* err = NULL;

	g_spawn_async(NULL, argv, NULL, G_SPAWN_DO_NOT_REAP_CHILD, NULL, NULL, &pid, &err);

	if (err != NULL)
	{
		return;
	}

	g_child_watch_add(pid, watch_cb, NULL);
	return;
}

void select_cb(GtkWidget* list, GtkListBoxRow* row, void* udata)
{
	int i = 0;

	i = gtk_list_box_row_get_index(row);
	gtk_label_set_text(GTK_LABEL(udata), profile_descriptions[i]);

	setenv("MCPI_FEATURES", features_envs[i], 1);
	return;
}

void settings_cb(GtkWidget* button, void* udata)
{
	char* buff;
	const char* username;
	const char* distance;
	char* settings_path;
	FILE* settings_file;
	GtkEntryBuffer* gtk_buff;

	gtk_buff = gtk_entry_get_buffer(GTK_ENTRY(settings.username_entry));

	username = gtk_entry_buffer_get_text(gtk_buff);
	distance = gtk_combo_box_text_get_active_text(settings.distance_combo);

	asprintf(&settings_path, "%s/.minecraft-pi/settings.txt", getenv("HOME"));
	asprintf(&buff, "%s\n%s\n", username, distance);

	settings_file = fopen(settings_path, "w");
	fwrite((const void*)buff, 1, strlen(buff), settings_file);

	setenv("MCPI_USERNAME", username, 1);
	setenv("MCPI_RENDER_DISTANCE", distance, 1);
	fclose(settings_file);
	free(settings_path);
	free(buff);
	return;
}

void about_cb(GtkWidget* button, void* udata)
{
	GError* err;
	gtk_show_uri_on_window(GTK_WINDOW(window), "https://discord.gg/jEXwEdx", GDK_CURRENT_TIME, &err);
	return;
}

void activate_cb(GtkApplication* app, void* udata)
{
	GtkWidget* notebook;
	GtkCssProvider* provider;

	window = gtk_application_window_new(app);
	gtk_window_set_title(GTK_WINDOW(window), "gMCPIL - MCPIL GTK+ Edition");
	gtk_window_set_default_size(GTK_WINDOW(window), 480, 320);

	notebook = gtk_notebook_new();

	Play_tab(notebook);
	Features_tab(notebook);
	Multiplayer_tab(notebook);
	Settings_tab(notebook);
	About_tab(notebook);

	provider = gtk_css_provider_new();
	gtk_css_provider_load_from_path(provider, "/usr/share/mcpil/Adwaita-dark.css", NULL);
	gtk_style_context_add_provider_for_screen(gdk_screen_get_default(), GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);

	gtk_container_add(GTK_CONTAINER(window), notebook);
	gtk_widget_show_all(window);
	return;
}
