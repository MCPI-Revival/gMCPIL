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

struct MCPILConfigPrivate
{
	gchar* ip;
	gchar* port;
	gchar* username;
	gchar* features;
	gchar* distance;
	gchar* filename;
};

enum
{
	PROP_NULL,
	PROP_IP,
	PROP_PORT,
	PROP_USERNAME,
	PROP_FEATURES,
	PROP_DISTANCE
};

static void mcpil_config_finalize(GObject* obj);
static void mcpil_config_set_property(GObject* obj, guint prop_id, const GValue* value, GParamSpec* pspec);
static void mcpil_config_get_property(GObject* obj, guint prop_id, GValue* value, GParamSpec* pspec);
 
G_DEFINE_TYPE_WITH_PRIVATE(MCPILConfig, mcpil_config, G_TYPE_OBJECT);

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
	return;
}
 
static void mcpil_config_init(MCPILConfig* self)
{
	MCPILConfigPrivate* private = MCPIL_CONFIG_PRIVATE(self);

	private->ip = NULL;
	private->port = 0;
	private->username = NULL;
	private->features = NULL;
	private->distance = NULL;
	return;
}
   
static void mcpil_config_finalize(GObject* obj)
{
	MCPILConfig* self = MCPIL_CONFIG(obj);
	MCPILConfigPrivate* private = MCPIL_CONFIG_PRIVATE(self);
	GObjectClass* parent_class = G_OBJECT_CLASS(mcpil_config_parent_class);

	if (private->ip != NULL)
	{
		g_free(private->ip);
	}
	if (private->port != NULL)
	{
		g_free(private->port);
	}
	if (private->username != NULL)
	{
		g_free(private->username);
	}
	if (private->features != NULL)
	{
		g_free(private->features);
	}
	if (private->distance != NULL)
	{
		g_free(private->distance);
	}
	if (private->filename != NULL)
	{
		g_free(private->filename);
	}

	(*parent_class->finalize)(obj);
	return;
}

GETTER_SETTER(gchar*, ip);
GETTER_SETTER(gchar*, port);
GETTER_SETTER(gchar*, username);
GETTER_SETTER(gchar*, features);
GETTER_SETTER(gchar*, distance);

static void mcpil_config_set_property(GObject* obj, guint prop_id, const GValue* value, GParamSpec* pspec)
{
	MCPILConfig* self = MCPIL_CONFIG(obj);

	switch (prop_id)
	{
		case PROP_IP:
			mcpil_config_set_ip(self, g_value_get_string(value));
		break;
		case PROP_PORT:
			mcpil_config_set_port(self, g_value_get_string(value));
		break;
		case PROP_USERNAME:
			mcpil_config_set_username(self, g_value_get_string(value));
		break;
		case PROP_FEATURES:
			mcpil_config_set_features(self, g_value_get_string(value));
		break;
		case PROP_DISTANCE:
			mcpil_config_set_distance(self, g_value_get_string(value));
		break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, prop_id, pspec);
		break;
	}
	return;
}

static void mcpil_config_get_property(GObject* obj, guint prop_id, GValue* value, GParamSpec* pspec)
{
	MCPILConfig* self = MCPIL_CONFIG(obj);

	switch (prop_id)
	{
		case PROP_IP:
			g_value_set_string(value, mcpil_config_get_ip(self));
		break;
		case PROP_PORT:
			g_value_set_string(value, mcpil_config_get_port(self));
		break;
		case PROP_USERNAME:
			g_value_set_string(value, mcpil_config_get_username(self));
		break;
		case PROP_FEATURES:
			g_value_set_string(value, mcpil_config_get_features(self));
		break;
		case PROP_DISTANCE:
			g_value_set_string(value, mcpil_config_get_distance(self));
		break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, prop_id, pspec);
		break;
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
		free(buff);
		buff = (char*)malloc(2);
		buff[0] = '{';
		buff[1] = '}';
		err = NULL;
		obj = json_gobject_from_data(MCPIL_TYPE_CONFIG, buff, sz, &err);
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

#if 0
int main(int argc, char* argv[])
{
	int sz;
	char* buff;
	FILE* fd;
	GError* err = NULL;
	MCPILConfig* config = mcpil_config_new_from_file("config.json", &err);

	printf("Username: %s\n", mcpil_config_get_username(config));
	printf("IP: %s\n", mcpil_config_get_ip(config));
	printf("Port: %i\n", mcpil_config_get_port(config));
	printf("Features: %s\n", mcpil_config_get_features(config));
	printf("Render distance: %s\n", mcpil_config_get_distance(config));
	mcpil_config_set_username(config, "RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRelmao");
	mcpil_config_save_to_file(config, "config.json");
	g_object_unref(config);
	return 0;
}
#endif