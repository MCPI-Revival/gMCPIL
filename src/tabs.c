/*
 * tabs.c - MCPIL GTK+ tabs
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
#include <config.h>
#include <splashes.h>

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
	char* tmp;
	char* features_buff;
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

	features_buff = mcpil_config_get_features(config);
	if (features_buff != NULL)
	{
		i = 0;
		while (i < featc)
		{
			FEAT_BOOL(i) = FALSE;
			i++;
		}

		i = 0;
		tmp = strtok(features_buff, "|");
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

	features_cb(NULL, (void*)FALSE);
	cb_udata = (void*)TRUE;
});

TAB(Multiplayer, "Multiplayer", "Save", multiplayer_cb, {
	char* default_port;
	char* default_ip;
	GtkWidget* ip_hbox;
	GtkWidget* ip_label;
	GtkWidget* ip_entry;
	GtkWidget* port_hbox;
	GtkWidget* port_label;
	GtkWidget* port_entry;
	GtkWidget* notice_label;

	default_ip = mcpil_config_get_ip(config);
	default_port = mcpil_config_get_port(config);
	if (default_ip == NULL || default_port == NULL)
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

	notice_label = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(notice_label), MCPIL_FEATURED_NOTICE);
	gtk_label_set_justify(GTK_LABEL(notice_label), GTK_JUSTIFY_CENTER);
	gtk_label_set_line_wrap(GTK_LABEL(notice_label), TRUE);

	gtk_box_pack_start(GTK_BOX(tab), ip_hbox, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(tab), port_hbox, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(tab), notice_label, FALSE, FALSE, 10);

	settings_box.ip_entry = ip_entry;
	settings_box.port_entry = port_entry;
});

TAB(Settings, "Settings", "Save", settings_cb, {
	int i = 0;
	int distance_int;
	char* default_username;
	char* default_distance;
	GtkWidget* username_hbox;
	GtkWidget* username_label;
	GtkWidget* username_entry;
	GtkWidget* distance_hbox;
	GtkWidget* distance_label;
	GtkWidget* distance_combo;

	default_distance = mcpil_config_get_distance(config);
	default_username = mcpil_config_get_username(config);
	if (default_distance == NULL)
	{
		default_distance = "Normal";
		mcpil_config_set_distance(config, default_distance);
	}
	if (default_username == NULL)
	{
		default_username = "StevePi";
		mcpil_config_set_username(config, default_username);
	}
	printf("%s\n", default_distance);
	distance_int = get_distance(default_distance);

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

	settings_box.username_entry = GTK_ENTRY(username_entry);
	settings_box.distance_combo = GTK_COMBO_BOX_TEXT(distance_combo);

	setenv("MCPI_USERNAME", default_username, 1);
	setenv("MCPI_RENDER_DISTANCE", distances[distance_int], 1);
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
