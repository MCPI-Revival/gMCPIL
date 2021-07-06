/*
 * mcpil.c - MCPIL GTK+ Edition
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
#define DEFINE_SPLASHES

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <fcntl.h>

#include <gtk/gtk.h>
#include <mcpil.h>
#include <config.h>
#include <splashes.h>

/* Global variables */
settings_box_t settings_box;
GMCPILConfig* config;
GtkWidget* window;

void about_cb(__attribute__((unused)) GtkWidget* button, __attribute__((unused)) void* udata)
{
	GdkPixbuf* logo;
	GtkWidget* about_dialog;

	logo = gtk_icon_theme_load_icon(gtk_icon_theme_get_default(), "gmcpil", 64, GTK_ICON_LOOKUP_NO_SVG, NULL);
	about_dialog = gtk_about_dialog_new();
	gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG(about_dialog), "gMCPIL");
	gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(about_dialog), GMCPIL_VERSION);
	gtk_about_dialog_set_logo(GTK_ABOUT_DIALOG(about_dialog), logo);
	gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(about_dialog), GMCPIL_COPYRIGHT);
	gtk_about_dialog_set_website(GTK_ABOUT_DIALOG(about_dialog), GMCPIL_REPO_URL);
	gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(about_dialog), GMCPIL_DESCRIPTION);
	gtk_dialog_run(GTK_DIALOG(about_dialog));
	gtk_widget_destroy(about_dialog);
	return;
}

void activate_cb(GtkApplication* app, __attribute__((unused)) void* udata)
{
	GdkPixbuf* icon;
	GtkWidget* stack;
	GtkWidget* switcher;
	GtkWidget* header;
	GtkWidget* switcher_box;
	GtkWidget* about_button;

	window = gtk_application_window_new(app);
	gtk_window_set_title(GTK_WINDOW(window), "gMCPIL");
	gtk_window_set_default_size(GTK_WINDOW(window), -1, 375);

	icon = gtk_icon_theme_load_icon(gtk_icon_theme_get_default(), "gmcpil", 32, GTK_ICON_LOOKUP_NO_SVG, NULL);
	gtk_window_set_icon(GTK_WINDOW(window), icon);

	stack = gtk_stack_new();

	switcher_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);

	switcher = gtk_stack_switcher_new();
	gtk_stack_switcher_set_stack(GTK_STACK_SWITCHER(switcher), GTK_STACK(stack));

	about_button = gtk_button_new_from_icon_name("help-about-symbolic", GTK_ICON_SIZE_LARGE_TOOLBAR);

	header = gtk_header_bar_new();
	gtk_header_bar_set_show_close_button(GTK_HEADER_BAR(header), TRUE);
	gtk_header_bar_set_custom_title(GTK_HEADER_BAR(header), switcher_box);

	Play_tab(stack);
	Features_tab(stack);
	Servers_tab(stack);

	gtk_box_pack_start(GTK_BOX(switcher_box), switcher, TRUE, FALSE, 0);
	gtk_box_pack_end(GTK_BOX(switcher_box), about_button, TRUE, FALSE, 2);

	g_signal_connect(about_button, "clicked", G_CALLBACK(about_cb), NULL);

	gtk_window_set_titlebar(GTK_WINDOW(window), header);
	gtk_container_add(GTK_CONTAINER(window), stack);
	gtk_widget_show_all(window);
	return;
}

int main(int argc, char* argv[])
{
	int i = 0;
	int rt = 0;
	char* config_path;
	GtkApplication* app;

	/* Initialize */
	srand(time(NULL));
	/* This is cursed, but it works:tm: */
	setenv("GTK_THEME", "Adwaita:dark", 1);

	asprintf(&config_path, "%s/.gmcpil.json", getenv("HOME"));
	config = gmcpil_config_new(config_path);
	free(config_path);

	gtk_init(&argc, &argv);

	app = gtk_application_new("tk.mcpirevival.gmcpil", G_APPLICATION_FLAGS_NONE);

	/* Signals */
	g_signal_connect(app, "activate", G_CALLBACK(activate_cb), NULL);

	rt = g_application_run(G_APPLICATION(app), argc, argv);

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
	if (settings_box.buff != NULL)
	{
		free(settings_box.buff);
	}
	free(servers_path);

	gmcpil_config_save(config);
	g_object_unref(config);
	return rt;
}
