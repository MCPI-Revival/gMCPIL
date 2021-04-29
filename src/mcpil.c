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
