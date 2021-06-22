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

/* Widget variables */
GtkWidget* window;

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

	asprintf(&config_path, "%s/.minecraft-pi/gmcpil.json", getenv("HOME"));
	config = mcpil_config_new(config_path);
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

	mcpil_config_save(config);
	g_object_unref(config);
	return rt;
}
