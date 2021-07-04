/*
 * servers.c - gMCPIL servers tab
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
#include <errno.h>

#include <mcpil.h>
#include <config.h>

char* servers_path;

static void servers_cb(__attribute__((unused)) GtkWidget* button, void* udata)
{
	char* buff;
	FILE* fd;
	GtkTextView* view;
	GtkTextBuffer* gtk_buff;
	GtkTextIter start;
	GtkTextIter end;

	fd = fopen(servers_path, "w+");
	if (fd == NULL)
	{
		fprintf(stderr, "Could not save servers file: %s.", strerror(errno));
		return;
	}
	view = (GtkTextView*)udata;

	gtk_buff = gtk_text_view_get_buffer(view);
	gtk_text_buffer_get_start_iter(gtk_buff, &start);
	gtk_text_buffer_get_end_iter(gtk_buff, &end);
	buff = gtk_text_buffer_get_text(gtk_buff, &start, &end, FALSE);

	fwrite(buff, 1, strlen(buff), fd);
	fclose(fd);
	return;
}

TAB(Servers, servers_cb, {
	int sz = 0;
	char* buff;
	FILE* fd;
	GtkWidget* label;
	GtkWidget* scroll;
	GtkWidget* view;
	GtkTextBuffer* gtk_buff;

	buff = NULL;
	asprintf(&servers_path, "%s/.minecraft-pi/servers.txt", getenv("HOME"));

	fd = fopen(servers_path, "r+");
	if (fd != NULL)
	{
		fseek(fd, 0, SEEK_END);
		sz = ftell(fd);
		fseek(fd, 0, SEEK_SET);
		buff = (char*)malloc(sz + 1);
		fread((void*)buff, 1, sz, fd);
		buff[sz] = 0x00;
		fclose(fd);
	}

	label = gtk_label_new(SERVERS_LABEL);
	gtk_label_set_line_wrap(GTK_LABEL(label), TRUE);
	gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_CENTER);
	gtk_widget_set_margin_top(label, 6);

	scroll = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scroll), GTK_SHADOW_ETCHED_IN);
	gtk_container_set_border_width(GTK_CONTAINER(scroll), 5);

	gtk_buff = gtk_text_buffer_new(NULL);

	if (buff != NULL)
	{
		gtk_text_buffer_set_text(gtk_buff, buff, -1);
	} else
	{
		gtk_text_buffer_set_text(gtk_buff, DEFAULT_SERVERS, -1);
	}

	view = gtk_text_view_new_with_buffer(gtk_buff);
	gtk_text_view_set_editable(GTK_TEXT_VIEW(view), TRUE);
	gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(view), TRUE);
	gtk_container_add(GTK_CONTAINER(scroll), view);

	gtk_box_pack_start(GTK_BOX(tab), label, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(tab), scroll, TRUE, TRUE, 0);
	cb_udata = (void*)view;
	free(buff);
});
