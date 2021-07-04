/*
 * settings.c - gMCPIL settings tab
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

char* distances[4] = {"Far", "Normal", "Short", "Tiny"};
char* profile_names[5] = {"Classic MCPI", "Modded MCPI", "Modded MCPE", "Optimized MCPE", "Custom Profile"};
char* profile_descriptions[5] = {
	"Classic Minecraft Pi Edition (Not Recommended):\nAll optional features disabled.",
	"Modded Minecraft Pi Edition:\nDefault MCPI-Reborn optional features without Touch GUI.",
	"Modded Minecraft Pocket Edition (Recommended):\nDefault MCPI-Reborn optional features.",
	"Optimized Minecraft Pocket Edition:\nDefault MCPI-Reborn features with lower quality graphics.",
	"Custom Profile: Modify its settings in the Features tab."
};

static char* get_splash_text()
{
	return splashes[rand() % sizeof(splashes) / sizeof(char*)];
}

static int get_distance(char* str)
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

static void settings_cb(__attribute__((unused)) GtkWidget* button, void* udata)
{
	int profile;
	char tmp[2] = {0x00, 0x00};
	const char* username;
	const char* distance;
	const char* hud;
	gboolean hide;
	GtkEntryBuffer* username_buff;
	GtkEntryBuffer* hud_buff;

	if ((int)udata == TRUE)
	{
		goto profile;
	}

	username_buff = gtk_entry_get_buffer(GTK_ENTRY(settings_box.username_entry));
	hud_buff = gtk_entry_get_buffer(GTK_ENTRY(settings_box.hud_entry));

	username = gtk_entry_buffer_get_text(username_buff);
	hud = gtk_entry_buffer_get_text(hud_buff);
	distance = gtk_combo_box_text_get_active_text(settings_box.distance_combo);
	hide = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(settings_box.hide_check));

	setenv("MCPI_USERNAME", username, 1);
	setenv("MCPI_RENDER_DISTANCE", distance, 1);
	setenv("GALLIUM_HUD", hud, 1);

	gmcpil_config_set_username(config, username);
	gmcpil_config_set_hud(config, hud);
	gmcpil_config_set_distance(config, distance);
	if (hide)
	{
		gmcpil_config_set_hide(config, "TRUE");
	} else
	{
		gmcpil_config_set_hide(config, "FALSE");
	}

profile:
	profile = gtk_combo_box_get_active(GTK_COMBO_BOX(settings_box.profile_combo));
	setenv("MCPI_FEATURE_FLAGS", features_envs[profile], 1);
	tmp[0] = profile + 0x30;

	gmcpil_config_set_last_profile(config, tmp);
	gmcpil_config_save(config);
	return;
}

static void watch_cb(GPid pid, __attribute__((unused)) int status, __attribute__((unused)) void* udata)
{
	gtk_widget_show_all(window);
	g_spawn_close_pid(pid);
	return;
}

static void launch_cb(__attribute__((unused)) GtkWidget* button, __attribute__((unused)) void* udata)
{
	char* argv[] = {"minecraft-pi-reborn-client", NULL};
	GPid pid;
	GError* err = NULL;

	settings_cb(NULL, (void*)(intptr_t)TRUE);
	g_spawn_async(NULL, argv, NULL, G_SPAWN_DO_NOT_REAP_CHILD | G_SPAWN_SEARCH_PATH, NULL, NULL, &pid, &err);

	if (err != NULL)
	{
		g_error("Spawning child failed: %s", err->message);
		return;
	}
	if (gmcpil_config_get_hide(config)[0] == 'T')
	{
		gtk_widget_hide(window);
	}
	g_child_watch_add(pid, watch_cb, NULL);
	return;
}

static void select_cb(GtkComboBox* combo, void* udata)
{
	int profile;
	GtkLabel* description;

	description = GTK_LABEL(udata);
	profile = gtk_combo_box_get_active(combo);
	gtk_label_set_text(description, profile_descriptions[profile]);
	return;
}

TAB(Play, settings_cb, {
	int i = 0;
	int distance_int;
	int last_profile;
	char* default_username;
	char* default_distance;
	char* default_hud;
	char* default_hide;
	char* splash_text;
	GtkWidget* username_hbox;
	GtkWidget* username_label;
	GtkWidget* username_entry;
	GtkWidget* distance_hbox;
	GtkWidget* distance_label;
	GtkWidget* distance_combo;
	GtkWidget* profile_hbox;
	GtkWidget* profile_label;
	GtkWidget* profile_combo;
	GtkWidget* hud_hbox;
	GtkWidget* hud_label;
	GtkWidget* hud_entry;
	GtkWidget* hide_hbox;
	GtkWidget* hide_check;
	GtkWidget* launch_button;
	GtkWidget* splash;
	GtkWidget* title;
	GtkWidget* title_hbox;
	GtkWidget* description;

	title_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);

	title = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(title), "<span size=\"24000\">Minecraft Pi Launcher</span>");

	splash = gtk_label_new(NULL);
	asprintf(&splash_text, "<span foreground=\"#ffff00\" size=\"12000\">%s</span>\n", get_splash_text());
	gtk_label_set_markup(GTK_LABEL(splash), splash_text);

	GMCPIL_SET_DEFAULT(distance, "Short");
	GMCPIL_SET_DEFAULT(username, "StevePi");
	GMCPIL_SET_DEFAULT(hud, "simple,fps");
	GMCPIL_SET_DEFAULT(hide, "TRUE");

	distance_int = get_distance(default_distance);

	username_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	username_label = gtk_label_new("Username");
	username_entry = gtk_entry_new_with_buffer(gtk_entry_buffer_new(default_username, strlen(default_username)));
	gtk_widget_set_size_request(username_entry, 25 * 10, -1);

	hud_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	hud_label = gtk_label_new("Gallium HUD options");
	hud_entry = gtk_entry_new_with_buffer(gtk_entry_buffer_new(default_hud, strlen(default_hud)));
	gtk_widget_set_size_request(hud_entry, 25 * 10, -1);

	distance_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	distance_label = gtk_label_new("Rendering distance");
	distance_combo = gtk_combo_box_text_new();
	while (i < 4)
	{
		gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(distance_combo), distances[i]);
		i++;
	}
	gtk_combo_box_set_active(GTK_COMBO_BOX(distance_combo), distance_int);
	gtk_widget_set_size_request(distance_combo, 25 * 10, -1);

	i = 0;
	profile_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	profile_label = gtk_label_new("Profile");
	profile_combo = gtk_combo_box_text_new();
	last_profile = SAFE_ATOI(gmcpil_config_get_last_profile(config));
	while (i < 5)
	{
		gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(profile_combo), profile_names[i]);
		i++;
	}
	gtk_combo_box_set_active(GTK_COMBO_BOX(profile_combo), last_profile);
	gtk_widget_set_size_request(profile_combo, 25 * 10, -1);

	hide_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	hide_check = gtk_check_button_new_with_label("Hide launcher");
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(hide_check), default_hide[0] == 'T');
	gtk_widget_set_size_request(hide_check, -1, 32); // To match rest of input widgets

	description = gtk_label_new(profile_descriptions[last_profile]);
	gtk_label_set_justify(GTK_LABEL(description), GTK_JUSTIFY_CENTER);
	gtk_label_set_line_wrap(GTK_LABEL(description), TRUE);

	launch_button = gtk_button_new_with_label("Launch");

	gtk_box_pack_start(GTK_BOX(title_hbox), title, TRUE, FALSE, 0);

	gtk_box_pack_start(GTK_BOX(tab), title_hbox, FALSE, FALSE, 4);
	gtk_box_pack_start(GTK_BOX(tab), splash, FALSE, FALSE, 0);

	gtk_box_pack_start(GTK_BOX(username_hbox), username_label, FALSE, FALSE, 10);
	gtk_box_pack_end(GTK_BOX(username_hbox), username_entry, FALSE, FALSE, 10);

	gtk_box_pack_start(GTK_BOX(hud_hbox), hud_label, FALSE, FALSE, 10);
	gtk_box_pack_end(GTK_BOX(hud_hbox), hud_entry, FALSE, FALSE, 10);

	gtk_box_pack_start(GTK_BOX(distance_hbox), distance_label, FALSE, FALSE, 10);
	gtk_box_pack_end(GTK_BOX(distance_hbox), distance_combo, FALSE, FALSE, 10);

	gtk_box_pack_start(GTK_BOX(profile_hbox), profile_label, FALSE, FALSE, 10);
	gtk_box_pack_end(GTK_BOX(profile_hbox), profile_combo, FALSE, FALSE, 10);

	gtk_box_pack_end(GTK_BOX(hide_hbox), hide_check, FALSE, FALSE, 10);

	gtk_box_pack_start(GTK_BOX(tab), username_hbox, FALSE, FALSE, 2);
	gtk_box_pack_start(GTK_BOX(tab), hud_hbox, FALSE, FALSE, 2);
	gtk_box_pack_start(GTK_BOX(tab), distance_hbox, FALSE, FALSE, 2);
	gtk_box_pack_start(GTK_BOX(tab), profile_hbox, FALSE, FALSE, 2);
	gtk_box_pack_start(GTK_BOX(tab), hide_hbox, FALSE, FALSE, 2);
	gtk_box_pack_start(GTK_BOX(tab), description, FALSE, FALSE, 2);

	gtk_box_pack_end(GTK_BOX(button_hbox), launch_button, FALSE, FALSE, 0);

	settings_box.username_entry = GTK_ENTRY(username_entry);
	settings_box.hud_entry = GTK_ENTRY(hud_entry);
	settings_box.distance_combo = GTK_COMBO_BOX_TEXT(distance_combo);
	settings_box.profile_combo = GTK_COMBO_BOX_TEXT(profile_combo);
	settings_box.hide_check = GTK_CHECK_BUTTON(hide_check);

	g_signal_connect(launch_button, "clicked", G_CALLBACK(launch_cb), NULL);
	g_signal_connect(profile_combo, "changed", G_CALLBACK(select_cb), (void*)description);

	setenv("MCPI_USERNAME", default_username, 1);
	setenv("MCPI_RENDER_DISTANCE", distances[distance_int], 1);
	setenv("GALLIUM_HUD", default_hud, 1);
	free(splash_text);
});
