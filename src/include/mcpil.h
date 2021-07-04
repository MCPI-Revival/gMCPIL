/*
 * mcpil.h - gMCPIL main header
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

#ifndef GMCPIL_H
#define GMCPIL_H

#include <gtk/gtk.h>
#include <config.h>

#define STR(str) #str
#define SAFE_ATOI(str) strtol(str ? str : "", NULL, 10)

#define FEAT_BOOL(i) (features[i][0])
#define FEAT_INT(i) ((int)features[i][0])
#define FEAT_PTR(i) (features[i][1])
#define FEAT_STR(i) ((char*)features[i][1])
#define FEAT_CMP(i, str) (strcmp(FEAT_STR(i), str) == 0)

#define TAB(name, button_cb, code) GtkWidget* name ## _tab(GtkWidget* stack) \
{ \
	void* cb_udata = NULL; \
	GtkWidget* tab = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0); \
	GtkWidget* button = gtk_button_new_with_label("Save"); \
	GtkWidget* button_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2); \
	code; \
	gtk_box_pack_end(GTK_BOX(button_hbox), button, FALSE, FALSE, 0); \
	gtk_box_pack_end(GTK_BOX(tab), button_hbox, FALSE, FALSE, 0); \
	gtk_stack_add_titled(GTK_STACK(stack), tab, STR(name), STR(name)); \
	g_signal_connect(button, "clicked", G_CALLBACK(button_cb), cb_udata); \
	return tab; \
}
//gtk_notebook_append_page(GTK_NOTEBOOK(notebook), tab, gtk_label_new(STR(name_str)));

#define GMCPIL_REPO_URL "https://github.com/MCPI-Revival/gMCPIL"
#define GMCPIL_COPYRIGHT "Copyright 2021 MCPI-Revival contributors"
#define GMCPIL_DESCRIPTION "Simple launcher for MCPI-Reborn - GTK+ Edition."
#define DEFAULT_SERVERS "thebrokenrail.com\npbptanarchy.tk\n"
#define SERVERS_LABEL "External multiplayer is now built-in into MCPI-Reborn,\n" \
	"this is a just simple editor for the servers.txt file.\n" \
	"Each line is an ip:port tuple. If the port is omitted,\n" \
	"the default (19132) is used.\n"

typedef void* feature_t[2];

typedef struct settings_box_t
{
	GtkEntry* username_entry;
	GtkEntry* hud_entry;
	GtkComboBoxText* distance_combo;
	GtkComboBoxText* profile_combo;
	GtkCheckButton* hide_check;
	char* buff;
} settings_box_t;

/* Global variables */
extern int featc;
extern char* servers_path;
extern char* features_envs[5];
extern settings_box_t settings_box;
extern feature_t features[32];
extern GMCPILConfig* config;
extern GtkWidget* window;

/* Tabs */
GtkWidget* Play_tab(GtkWidget* notebook);
GtkWidget* Features_tab(GtkWidget* notebook);
GtkWidget* Servers_tab(GtkWidget* notebook);
GtkWidget* Settings_tab(GtkWidget* notebook);

#endif /* MCPIL_H */
