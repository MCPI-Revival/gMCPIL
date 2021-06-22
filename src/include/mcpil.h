/*
 * mcpil.h - MCPIL GTK+ PoC/Edition
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

#ifndef MCPIL_H
#define MCPIL_H

#include <gtk/gtk.h>
#include <config.h>

#define STR(str) #str
#define SAFE_ATOI(str) strtol(str ? str : "", NULL, 10)

#define FEAT_BOOL(i) (features[i][0])
#define FEAT_INT(i) ((int)features[i][0])
#define FEAT_PTR(i) (features[i][1])
#define FEAT_STR(i) ((char*)features[i][1])
#define FEAT_CMP(i, str) (strcmp(FEAT_STR(i), str) == 0)

#define TAB(name_str, title_str, button_str, button_cb, code) GtkWidget* name_str ## _tab(GtkWidget* notebook) \
{ \
	void* cb_udata = NULL; \
	GtkWidget* tab = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0); \
	GtkWidget* title = gtk_label_new(NULL); \
	GtkWidget* button = gtk_button_new_with_label(button_str); \
	GtkWidget* title_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0); \
	GtkWidget* button_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0); \
	gtk_label_set_markup(GTK_LABEL(title), "<span size=\"24000\">" title_str "</span>"); \
	gtk_box_pack_start(GTK_BOX(title_hbox), title, TRUE, FALSE, 0); \
	gtk_box_pack_end(GTK_BOX(button_hbox), button, FALSE, FALSE, 0); \
	gtk_box_pack_start(GTK_BOX(tab), title_hbox, FALSE, FALSE, 8); \
	gtk_box_pack_end(GTK_BOX(tab), button_hbox, FALSE, FALSE, 0); \
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), tab, gtk_label_new(STR(name_str))); \
	code; \
	g_signal_connect(button, "clicked", G_CALLBACK(button_cb), cb_udata); \
	return tab; \
}

#define MCPIL_REPO_URL "https://github.com/MCPI-Revival/gMCPIL"
#define MCPIL_FEATURED_NOTICE "<b>Note:</b> TBR's and PBPT servers are now featured servers,\n" \
	"this means you don't need to add them here to be able to join."

typedef void* feature_t[2];

typedef struct settings_box_t
{
	GtkEntry* ip_entry;
	GtkEntry* port_entry;
	GtkEntry* username_entry;
	GtkEntry* hud_entry;
	GtkComboBoxText* distance_combo;
	GtkCheckButton* hide_check;
	char* buff;
} settings_box_t;

feature_t features[24];
settings_box_t settings_box;
MCPILConfig* config;

int featc;

/* Play */
char* get_splash_text();
void launch_cb(GtkWidget* button, void* udata);
void select_cb(GtkWidget* list, GtkListBoxRow* row, void* udata);

/* Features */
int get_features();
feature_t* get_feature(char* str);
int set_feature_envs(int feat);
void features_cb(GtkWidget* button, void* udata);
void toggle_cb(GtkWidget* check, void* udata);

/* Multiplayer */
int get_servers(char** ip, char** port);
void multiplayer_cb(GtkWidget* button, void* udata);

/* Settings */
int get_distance(char* str);
void settings_cb(GtkWidget* button, void* udata);

/* About */
void about_cb(GtkWidget* button, void* udata);

/* Misc/App */
void activate_cb(GtkApplication* app, void* udata);

/* Tabs */
GtkWidget* Play_tab(GtkWidget* notebook);
GtkWidget* Features_tab(GtkWidget* notebook);
GtkWidget* Multiplayer_tab(GtkWidget* notebook);
GtkWidget* Settings_tab(GtkWidget* notebook);
GtkWidget* About_tab(GtkWidget* notebook);

/* Variable declarations */
GtkWidget* window;
char* profile_names[5];
char* profile_descriptions[5];
char* features_envs[5];
char* distances[4];

#endif /* MCPIL_H */
