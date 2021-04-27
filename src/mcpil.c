/*
 * mcpil.c - MCPIL GTK+ PoC/Edition
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
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <fcntl.h>

#include <gtk/gtk.h>
#include <mcpil.h>
#include <splashes.h>

/* Widget variables */
GtkWidget* window;
GtkWidget* tabs[3];

/* Profiles */
char* profile_names[5] = {"Classic MCPI", "Modded MCPI", "Modded MCPE", "Optimized MCPE", "Custom Profile"};
char* profile_descriptions[5] = {
	"Classic Minecraft Pi Edition.\n(Not Recommended)\nAll optional features disabled.",
	"Modded Minecraft Pi Edition.\nDefault MCPI-Reborn optional features without Touch GUI.",
	"Minecraft Pocket Edition.\n(Recommended)\nDefault MCPI-Reborn optional features.",
	"Optimized Minecraft Pocket Edition.\nDefault MCPI-Reborn optional features with lower quality graphics.",
	"Custom Profile.\nModify its settings in the Features tab."
};
char* features_envs[5] = {""};

/* Rendering distances */
char* distances[4] = {"Far", "Normal", "Short", "Tiny"};

/* Helper functions */
char* get_splash_text()
{
	return splashes[rand() % sizeof(splashes) / sizeof(char*)];
}

int get_features()
{
	int i = 0;
	int sz = 0;
	int line = 0;
	int offset = 0;
	size_t buff_sz;
	char* buff;
	FILE* file;

	buff_sz = 48;
	buff = (char*)malloc((int)buff_sz + 1);
	file = fopen("/usr/share/minecraft-pi/client/features", "r");

	if (file == NULL)
	{
		free(buff);
		return -1;
	}
	while ((sz = getline(&buff, &buff_sz, file)) > 0)
	{
		FEAT_BOOL(i) = (void*)(buff[0] == 'T');
		buff[sz - 2] = 0x00;
		offset = ASSERT(FEAT_BOOL(i), 6, 7);
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
			if (FEAT_CMP(feat, "Fancy Graphics") || FEAT_CMP(feat, "Smooth Lighting") || FEAT_CMP(feat, "Animated Water") || FEAT_CMP(feat, "Disable gui_blocks Atlas"))
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
		if (strcmp(FEAT_STR(i), str) == 0)
		{
			return &(features[i]);
		}
		i++;
	}
	return NULL;
}

int get_servers(char** ip, char** port)
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
		return -1;
	}

	fseek(servers_file, 0, SEEK_END);
	sz = ftell(servers_file);
	fseek(servers_file, 0, SEEK_SET);

	multiplayer.buff = (char*)malloc(sz + 1);
	fread((void*)multiplayer.buff, 1, sz, servers_file);

	lf_ptr = strchr(multiplayer.buff, '\n');
	slash_ptr = strchr(multiplayer.buff, '/');
	if (lf_ptr == NULL || slash_ptr == NULL)
	{
		free(servers_path);
		free(multiplayer.buff);
		return -2;
	}
	*lf_ptr = 0x00;
	*slash_ptr = 0x00;

	*ip = multiplayer.buff;
	*port = slash_ptr + 1;

	fclose(servers_file);
	free(servers_path);
	return 0;
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

int check_libmultiplayer(char* path)
{
	struct stat attrs[2];

    stat(path, &attrs[0]);
    stat("/usr/lib/mcpil/libmultiplayer.so", &attrs[1]);

    if (access(path, F_OK) != 0 || attrs[1].st_mtime >= attrs[0].st_mtime)
    {
		return 1;
	}
	return 0;
}

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

/* Tabs */
TAB(Play, "Minecraft Pi Launcher", "Launch", launch_cb, {
	int i = 0;
	char* splash_text;
	GtkWidget* list;
	GtkWidget* rows[5];
	GtkWidget* hbox;
	GtkWidget* tmp_hbox;
	GtkWidget* splash;
	GtkWidget* description;

	asprintf(&splash_text, "<span foreground=\"#ffff00\" size=\"12000\">%s</span>\n", get_splash_text());

	splash = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(splash), splash_text);
	gtk_box_pack_start(GTK_BOX(tab), splash, FALSE, FALSE, 0);

	gtk_box_pack_start(GTK_BOX(tab), gtk_label_new("Select a Minecraft profile to launch"), FALSE, FALSE, 0);
	hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);

	list = gtk_list_box_new();
	gtk_list_box_set_selection_mode(GTK_LIST_BOX(list), GTK_SELECTION_SINGLE);

	description = gtk_label_new("The description of the selected profile\nshould appear here.");
	gtk_label_set_justify(GTK_LABEL(description), GTK_JUSTIFY_CENTER);
	gtk_label_set_line_wrap(GTK_LABEL(description), TRUE);

	while (i < 5)
	{
		rows[i] = gtk_list_box_row_new();
		tmp_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
		gtk_box_pack_start(GTK_BOX(tmp_hbox), gtk_label_new(profile_names[i]), FALSE, FALSE, 8);
		gtk_container_add(GTK_CONTAINER(rows[i]), tmp_hbox);
		gtk_container_add(GTK_CONTAINER(list), rows[i]);
		i++;
	}

	g_signal_connect(list, "row-selected", G_CALLBACK(select_cb), (void*)description);
	gtk_list_box_select_row(GTK_LIST_BOX(list), GTK_LIST_BOX_ROW(rows[0]));
	gtk_box_pack_start(GTK_BOX(hbox), list, FALSE, FALSE, 6);
	gtk_box_pack_start(GTK_BOX(hbox), description, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(tab), hbox, TRUE, FALSE, 0);

	gtk_widget_show_all(list);
	free(splash_text);
});

TAB(Features, "Features", "Save", features_cb, {
	int i = 0;
	int sz = 0;
	char* tmp;
	char* buff;
	char* features_path;
	FILE* features_file;
	GtkWidget* feature_check;
	feature_t* feature;

	featc = 0;
	featc = get_features();

	features_envs[1] = (char*)malloc(1);
	features_envs[2] = (char*)malloc(1);
	features_envs[3] = (char*)malloc(1);
	features_envs[4] = NULL;
	while (i < featc)
	{
		set_feature_envs(i);
		i++;
	}

	asprintf(&features_path, "%s/.minecraft-pi/profile.txt", getenv("HOME"));
	features_file = fopen(features_path, "r");
	if (features_file != NULL)
	{
		fseek(features_file, 0, SEEK_END);
		sz = ftell(features_file);
		fseek(features_file, 0, SEEK_SET);

		buff = (char*)malloc(sz + 1);
		fread((void*)buff, 1, sz, features_file);
		fclose(features_file);
		i = 0;
		while (i < featc)
		{
			FEAT_BOOL(i) = FALSE;
			i++;
		}

		i = 0;
		tmp = strtok(buff, "|");
		while (tmp != NULL && i < featc)
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

	i = 0;
	while (i < featc)
	{
		feature_check = gtk_check_button_new_with_label(FEAT_STR(i));
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(feature_check), FEAT_INT(i));
		g_signal_connect(feature_check, "toggled", G_CALLBACK(toggle_cb), (void*)i);
		gtk_box_pack_start(GTK_BOX(tab), feature_check, FALSE, FALSE, 0);
		i++;
	}
	free(features_path);

	features_cb(NULL, (void*)FALSE);
	cb_udata = (void*)TRUE;
});

TAB(Multiplayer, "Multiplayer", "Save", multiplayer_cb, {
	char* default_port;
	char* default_ip;
	char* servers_path;
	GtkWidget* ip_hbox;
	GtkWidget* ip_label;
	GtkWidget* ip_entry;
	GtkWidget* port_hbox;
	GtkWidget* port_label;
	GtkWidget* port_entry;

	default_ip = NULL;
	default_port = 0x00;
	asprintf(&servers_path, "%s/.minecraft-pi/servers.txt", getenv("HOME"));
	if (access(servers_path, F_OK) == 0)
	{
		get_servers(&default_ip, &default_port);
	}
	free(servers_path);
	if (default_ip == NULL || default_port == 0x00)
	{
		default_ip = "thebrokenrail.com";
		default_port = "19132";
	}

	ip_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	ip_label = gtk_label_new("IP Address:");
	ip_entry = gtk_entry_new_with_buffer(gtk_entry_buffer_new(default_ip, strlen(default_ip)));

	port_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	port_label = gtk_label_new("Port:");
	port_entry = gtk_entry_new_with_buffer(gtk_entry_buffer_new(default_port, strlen(default_port)));

	gtk_box_pack_start(GTK_BOX(ip_hbox), ip_label, FALSE, FALSE, 10);
	gtk_box_pack_end(GTK_BOX(ip_hbox), ip_entry, FALSE, FALSE, 10);

	gtk_box_pack_start(GTK_BOX(port_hbox), port_label, FALSE, FALSE, 10);
	gtk_box_pack_end(GTK_BOX(port_hbox), port_entry, FALSE, FALSE, 10);

	gtk_box_pack_start(GTK_BOX(tab), ip_hbox, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(tab), port_hbox, FALSE, FALSE, 0);

	multiplayer.ip_entry = ip_entry;
	multiplayer.port_entry = port_entry;
});

TAB(Settings, "Settings", "Save", settings_cb, {
	int i = 0;
	int sz = 0;
	int distance_int;
	char* buff = NULL;
	char* lf_ptrs[2];
	char* default_username;
	char* default_distance;
	char* settings_path;
	FILE* settings_file;
	GtkWidget* username_hbox;
	GtkWidget* username_label;
	GtkWidget* username_entry;
	GtkWidget* distance_hbox;
	GtkWidget* distance_label;
	GtkWidget* distance_combo;

	asprintf(&settings_path, "%s/.minecraft-pi/settings.txt", getenv("HOME"));
	settings_file = fopen(settings_path, "r");

	distance_int = 1;
	default_distance = NULL;
	default_username = "StevePi";
	if (settings_file != NULL)
	{
		fseek(settings_file, 0, SEEK_END);
		sz = ftell(settings_file);
		fseek(settings_file, 0, SEEK_SET);

		buff = (char*)malloc(sz + 1);
		fread((void*)buff, 1, sz, settings_file);

		lf_ptrs[0] = strchr(buff, '\n');
		if (lf_ptrs[0] != NULL)
		{
			*lf_ptrs[0] = 0x00;
			default_username = buff;

			lf_ptrs[1] = strchr(lf_ptrs[0] + 1, '\n');
			if (lf_ptrs[1] != NULL)
			{
				*lf_ptrs[1] = 0x00;
				default_distance = lf_ptrs[0] + 1;
				distance_int = get_distance(default_distance);
				if (distance_int < 0)
				{
					distance_int = 1;
				}
			}
		}
		fclose(settings_file);
	}

	username_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	username_label = gtk_label_new("Username:");

	username_entry = gtk_entry_new_with_buffer(gtk_entry_buffer_new(default_username, strlen(default_username)));

	distance_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	distance_label = gtk_label_new("Rendering distance:");

	distance_combo = gtk_combo_box_text_new();

	while (i < 4)
	{
		gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(distance_combo), distances[i]);
		i++;
	}
	gtk_combo_box_set_active(GTK_COMBO_BOX(distance_combo), distance_int);

	gtk_box_pack_start(GTK_BOX(username_hbox), username_label, FALSE, FALSE, 10);
	gtk_box_pack_start(GTK_BOX(username_hbox), username_entry, TRUE, TRUE, 10);

	gtk_box_pack_start(GTK_BOX(distance_hbox), distance_label, FALSE, FALSE, 10);
	gtk_box_pack_start(GTK_BOX(distance_hbox), distance_combo, TRUE, TRUE, 10);

	gtk_box_pack_start(GTK_BOX(tab), username_hbox, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(tab), distance_hbox, FALSE, FALSE, 0);

	settings.username_entry = GTK_ENTRY(username_entry);
	settings.distance_combo = GTK_COMBO_BOX_TEXT(distance_combo);

	setenv("MCPI_USERNAME", default_username, 1);
	setenv("MCPI_RENDER_DISTANCE", distances[distance_int], 1);

	if (buff != NULL)
	{
		free(buff);
	}
	free(settings_path);
});

TAB(About, "Minecraft Pi Launcher", "Help...", about_cb, {
	int sz = 0;
	char* buff;
	FILE* changelog_file;
	GtkWidget* scroll;
	GtkWidget* info_label;
	GtkWidget* changelog_view;
	GtkWidget* link;
	GtkTextBuffer* changelog_buffer;

	buff = NULL;
	changelog_file = fopen("/usr/share/doc/mcpil/CHANGELOG.txt", "r");
	if (changelog_file != NULL)
	{
		fseek(changelog_file, 0, SEEK_END);
		sz = ftell(changelog_file);
		fseek(changelog_file, 0, SEEK_SET);
		buff = (char*)malloc(sz + 1);
		fread((void*)buff, 1, sz, changelog_file);
		buff[sz] = 0x00;
		fclose(changelog_file);
	}

	info_label = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(info_label), "<b>" MCPIL_VERSION "</b>\nby all its contributors");
	gtk_label_set_justify(GTK_LABEL(info_label), GTK_JUSTIFY_CENTER);

	link = gtk_link_button_new_with_label(MCPIL_REPO_URL, MCPIL_REPO_URL);

	scroll = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scroll), GTK_SHADOW_ETCHED_IN);
	gtk_container_set_border_width(GTK_CONTAINER(scroll), 5);

	changelog_buffer = gtk_text_buffer_new(NULL);

	if (buff != NULL)
	{
		gtk_text_buffer_set_text(changelog_buffer, buff, -1);
	} else
	{
		gtk_text_buffer_set_text(changelog_buffer, "ERROR: Could not load changelog.", -1);
	}

	changelog_view = gtk_text_view_new_with_buffer(changelog_buffer);
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(changelog_view), GTK_WRAP_WORD);
	gtk_text_view_set_editable(GTK_TEXT_VIEW(changelog_view), FALSE);
	gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(changelog_view), FALSE);

	gtk_container_add(GTK_CONTAINER(scroll), changelog_view);
	gtk_box_pack_start(GTK_BOX(tab), info_label, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(tab), link, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(tab), scroll, TRUE, TRUE, 0);
});

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

int main(int argc, char* argv[])
{
	int i = 0;
	int sz = 0;
	int status = 0;
	int libmultiplayer_fd[2];
	char* libmultiplayer_path;
	GtkApplication* app;

	/* Initialize */
	srand(time(NULL));

	asprintf(&libmultiplayer_path, "%s/.minecraft-pi/mods/libmultiplayer.so", getenv("HOME"));
	if (check_libmultiplayer(libmultiplayer_path) != 0)
	{
		libmultiplayer_fd[0] = open("/usr/lib/mcpil/libmultiplayer.so", O_RDONLY, S_IRUSR | S_IRGRP);
		sz = (int)lseek(libmultiplayer_fd[0], 0, SEEK_END);
		lseek(libmultiplayer_fd[0], 0, SEEK_SET);
		libmultiplayer_fd[1] = open(libmultiplayer_path, O_WRONLY | O_CREAT, S_IRWXU);
		sendfile(libmultiplayer_fd[1], libmultiplayer_fd[0], NULL, sz);
		close(libmultiplayer_fd[0]);
		close(libmultiplayer_fd[1]);
	}
	free(libmultiplayer_path);

	gtk_init(&argc, &argv);

	app = gtk_application_new("tk.mcpi.gmcpil", G_APPLICATION_FLAGS_NONE);

	/* Signals */
	g_signal_connect(app, "activate", G_CALLBACK(activate_cb), NULL);

	status = g_application_run(G_APPLICATION(app), argc, argv);

	/* Free memory and resources */
	g_object_unref(app);
	free(features_envs[1]);
	free(features_envs[2]);
	free(features_envs[3]);
	free(features_envs[4]);
	while (i < featc)
	{
		free(FEAT_PTR(i));
		i++;
	}
	if (multiplayer.buff != NULL)
	{
		free(multiplayer.buff);
	}
	return 0;
}
