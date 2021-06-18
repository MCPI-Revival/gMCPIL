/*
 * config.c
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

#include <stdio.h>
#include <string.h>

#include <json-glib/json-glib.h>
#include <json-glib/json-gobject.h>
#include <config.h>

struct __attribute__((packed)) MCPILConfigPrivate
{
	gchar* ip;
	gchar* port;
	gchar* username;
	gchar* features;
	gchar* distance;
	gchar* last_profile;
	gchar* hud;
	gchar* filename;
};

enum
{
	PROP_NULL,
	PROP_IP,
	PROP_PORT,
	PROP_USERNAME,
	PROP_FEATURES,
	PROP_DISTANCE,
	PROP_LAST_PROFILE,
	PROP_HUD,
	PROP_LAST
};

G_DEFINE_TYPE_WITH_CODE(MCPILConfig, mcpil_config, G_TYPE_OBJECT, G_ADD_PRIVATE(MCPILConfig))

static void mcpil_config_finalize(GObject* obj);
static void mcpil_config_set_property(GObject* obj, guint prop_id, const GValue* value, GParamSpec* pspec);
static void mcpil_config_get_property(GObject* obj, guint prop_id, GValue* value, GParamSpec* pspec);

static void mcpil_config_class_init(MCPILConfigClass* klass)
{
	GObjectClass* gobject_class = G_OBJECT_CLASS(klass);
	GParamSpec* pspec;

	gobject_class->finalize = mcpil_config_finalize;

	gobject_class->get_property = mcpil_config_get_property;
	gobject_class->set_property = mcpil_config_set_property;

	pspec = g_param_spec_string("ip", "IP", "Server IP", NULL, G_PARAM_READWRITE);
	g_object_class_install_property(gobject_class, PROP_IP, pspec);

	pspec = g_param_spec_string("port", "Port", "Server Port", NULL, G_PARAM_READWRITE);
	g_object_class_install_property(gobject_class, PROP_PORT, pspec);

	pspec = g_param_spec_string("username", "Username", "Player name", NULL, G_PARAM_READWRITE);
	g_object_class_install_property(gobject_class, PROP_USERNAME, pspec);

	pspec = g_param_spec_string("features", "Features", "MCPI-Reborn features", NULL, G_PARAM_READWRITE);
	g_object_class_install_property(gobject_class, PROP_FEATURES, pspec);

	pspec = g_param_spec_string("distance", "Distance", "Render distance", NULL, G_PARAM_READWRITE);
	g_object_class_install_property(gobject_class, PROP_DISTANCE, pspec);

	pspec = g_param_spec_string("last_profile", "Last profile", "Last selected profile", NULL, G_PARAM_READWRITE);
	g_object_class_install_property(gobject_class, PROP_LAST_PROFILE, pspec);

	pspec = g_param_spec_string("hud", "Gallium HUD", "Gallium HUD options", NULL, G_PARAM_READWRITE);
	g_object_class_install_property(gobject_class, PROP_HUD, pspec);
	return;
}

static void mcpil_config_init(MCPILConfig* self)
{
	int i = 0;
	MCPILConfigPrivate* private = MCPIL_CONFIG_PRIVATE(self);
	gchar** private_gchar = (gchar**)(private);

	while (i < PROP_LAST - 1)
	{
		private_gchar[i] = NULL;
		i++;
	}
	return;
}

static void mcpil_config_finalize(GObject* obj)
{
	int i = 0;
	MCPILConfig* self = MCPIL_CONFIG(obj);
	MCPILConfigPrivate* private = MCPIL_CONFIG_PRIVATE(self);
	GObjectClass* parent_class = G_OBJECT_CLASS(mcpil_config_parent_class);
	gchar** private_gchar = (gchar**)(private);

	while (i < PROP_LAST - 1)
	{
		if (private_gchar[i] != NULL)
		{
			g_free(private_gchar[i]);
		}
		i++;
	}
	(*parent_class->finalize)(obj);
	return;
}

GETTER_SETTER(gchar*, ip);
GETTER_SETTER(gchar*, port);
GETTER_SETTER(gchar*, username);
GETTER_SETTER(gchar*, features);
GETTER_SETTER(gchar*, distance);
GETTER_SETTER(gchar*, last_profile);
GETTER_SETTER(gchar*, hud);

static void mcpil_config_set_property(GObject* obj, guint prop_id, const GValue* value, GParamSpec* pspec)
{
	MCPILConfig* self = MCPIL_CONFIG(obj);
	MCPILConfigPrivate* private = MCPIL_CONFIG_PRIVATE(self);
	gchar** private_gchar = (gchar**)(private);

	if (prop_id <= PROP_LAST)
	{
		private_gchar[prop_id - 1] = g_strdup((gchar*)g_value_get_string(value));
	} else
	{
		G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, prop_id, pspec);
	}
	return;
}

static void mcpil_config_get_property(GObject* obj, guint prop_id, GValue* value, GParamSpec* pspec)
{
	MCPILConfig* self = MCPIL_CONFIG(obj);

	MCPILConfigPrivate* private = MCPIL_CONFIG_PRIVATE(self);
	gchar** private_gchar = (gchar**)(private);

	if (prop_id <= PROP_LAST)
	{
		g_value_set_string(value, private_gchar[prop_id - 1]);
	} else
	{
		G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, prop_id, pspec);
	}
	return;
}

MCPILConfig* mcpil_config_new_from_file(gchar* filename, GError** err)
{
	int sz;
	char* buff;
	FILE* fd;
	MCPILConfig* self;

	fd = fopen(filename, "a+");
	if (fd == NULL)
	{
		return NULL;
	}

	fseek(fd, 0, SEEK_END);
	sz = ftell(fd);
	fseek(fd, 0, SEEK_SET);

	buff = (char*)malloc(sz);
	fread(buff, 1, sz, fd);

	self = MCPIL_CONFIG(json_gobject_from_data(MCPIL_TYPE_CONFIG, buff, sz, err));
	free(buff);
	fclose(fd);
	return self;
}

MCPILConfig* mcpil_config_new(gchar* filename)
{
	int sz;
	char* buff;
	FILE* fd;
	MCPILConfig* self;
	MCPILConfigPrivate* private;
	GObject* obj;
	GError* err = NULL;

	fd = fopen(filename, "a+");
	if (fd == NULL)
	{
		return NULL;
	}

	fseek(fd, 0, SEEK_END);
	sz = ftell(fd);
	fseek(fd, 0, SEEK_SET);

	if (sz <= 2)
	{
		buff = (char*)malloc(2);
		buff[0] = '{';
		buff[1] = '}';
		sz = 2;
	} else
	{
		buff = (char*)malloc(sz);
		fread(buff, 1, sz, fd);
	}

	obj = json_gobject_from_data(MCPIL_TYPE_CONFIG, buff, sz, &err);
	if (err != NULL)
	{
		err = NULL;
		obj = g_object_new(MCPIL_TYPE_CONFIG, 0);
	}
	self = MCPIL_CONFIG(obj);
	private = MCPIL_CONFIG_PRIVATE(self);
	private->filename = g_strdup(filename);

	free(buff);
	fclose(fd);
	return self;
}

int mcpil_config_save(MCPILConfig* self)
{
	char* buff;
	FILE* fd;
	MCPILConfigPrivate* private = MCPIL_CONFIG_PRIVATE(self);

	fd = fopen(private->filename, "w");
	if (fd == NULL)
	{
		return -1;
	}

	buff = json_gobject_to_data(G_OBJECT(self), NULL);
	fwrite(buff, 1, strlen(buff), fd);

	free(buff);
	fclose(fd);
	return 0;
}
