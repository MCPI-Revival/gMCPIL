/*
 * config.c - GMCPILConfig class
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

#include <stdio.h>
#include <string.h>

#include <json-glib/json-glib.h>
#include <json-glib/json-gobject.h>
#include <config.h>

struct __attribute__((packed)) GMCPILConfigPrivate
{
	gchar* username;
	gchar* features;
	gchar* distance;
	gchar* last_profile;
	gchar* hud;
	gchar* hide;
	gchar* filename;
};

enum
{
	PROP_NULL,
	PROP_USERNAME,
	PROP_FEATURES,
	PROP_DISTANCE,
	PROP_LAST_PROFILE,
	PROP_HUD,
	PROP_HIDE,
	PROP_LAST
};

G_DEFINE_TYPE_WITH_CODE(GMCPILConfig, gmcpil_config, G_TYPE_OBJECT, G_ADD_PRIVATE(GMCPILConfig))

static void gmcpil_config_finalize(GObject* obj);
static void gmcpil_config_set_property(GObject* obj, guint prop_id, const GValue* value, GParamSpec* pspec);
static void gmcpil_config_get_property(GObject* obj, guint prop_id, GValue* value, GParamSpec* pspec);

static void gmcpil_config_class_init(GMCPILConfigClass* klass)
{
	GObjectClass* gobject_class = G_OBJECT_CLASS(klass);
	GParamSpec* pspec;

	gobject_class->finalize = gmcpil_config_finalize;

	gobject_class->get_property = gmcpil_config_get_property;
	gobject_class->set_property = gmcpil_config_set_property;

	GMCPIL_GLIB_PROPERTY("username", "Username", "Player name", PROP_USERNAME);
	GMCPIL_GLIB_PROPERTY("features", "Features", "MCPI-Reborn features", PROP_FEATURES);
	GMCPIL_GLIB_PROPERTY("distance", "Distance", "Render distance", PROP_DISTANCE);
	GMCPIL_GLIB_PROPERTY("last_profile", "Last profile", "Last selected profile", PROP_LAST_PROFILE);
	GMCPIL_GLIB_PROPERTY("hud", "Gallium HUD", "Gallium HUD options", PROP_HUD);
	GMCPIL_GLIB_PROPERTY("hide", "Hide launcher", "Hide launcher on launch", PROP_HIDE);
	return;
}

static void gmcpil_config_init(GMCPILConfig* self)
{
	int i = 0;
	GMCPILConfigPrivate* private = GMCPIL_CONFIG_PRIVATE(self);
	gchar** private_gchar = (gchar**)(private);

	while (i < PROP_LAST - 1)
	{
		private_gchar[i] = NULL;
		i++;
	}
	return;
}

static void gmcpil_config_finalize(GObject* obj)
{
	int i = 0;
	GMCPILConfig* self = GMCPIL_CONFIG(obj);
	GMCPILConfigPrivate* private = GMCPIL_CONFIG_PRIVATE(self);
	GObjectClass* parent_class = G_OBJECT_CLASS(gmcpil_config_parent_class);
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

GMCPIL_GETTER_SETTER(username);
GMCPIL_GETTER_SETTER(features);
GMCPIL_GETTER_SETTER(distance);
GMCPIL_GETTER_SETTER(last_profile);
GMCPIL_GETTER_SETTER(hud);
GMCPIL_GETTER_SETTER(hide);

static void gmcpil_config_set_property(GObject* obj, guint prop_id, const GValue* value, GParamSpec* pspec)
{
	GMCPILConfig* self = GMCPIL_CONFIG(obj);
	GMCPILConfigPrivate* private = GMCPIL_CONFIG_PRIVATE(self);
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

static void gmcpil_config_get_property(GObject* obj, guint prop_id, GValue* value, GParamSpec* pspec)
{
	GMCPILConfig* self = GMCPIL_CONFIG(obj);
	GMCPILConfigPrivate* private = GMCPIL_CONFIG_PRIVATE(self);
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

GMCPILConfig* gmcpil_config_new(gchar* filename)
{
	int sz;
	char* buff;
	FILE* fd;
	GMCPILConfig* self;
	GMCPILConfigPrivate* private;
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

	if (sz <= 13) /* {"ip":"a.co"} */
	{
		buff = (char*)malloc(2);
		buff[0] = '{';
		buff[1] = '}';
		sz = 2;
	} else
	{
		buff = (char*)malloc(sz + 1);
		fread(buff, 1, sz, fd);
	}

	obj = json_gobject_from_data(GMCPIL_TYPE_CONFIG, buff, sz, &err);
	if (err != NULL)
	{
		err = NULL;
		obj = g_object_new(GMCPIL_TYPE_CONFIG, 0);
	}
	self = GMCPIL_CONFIG(obj);
	private = GMCPIL_CONFIG_PRIVATE(self);
	private->filename = g_strdup(filename);

	free(buff);
	fclose(fd);
	return self;
}

int gmcpil_config_save(GMCPILConfig* self)
{
	char* buff;
	FILE* fd;
	GMCPILConfigPrivate* private = GMCPIL_CONFIG_PRIVATE(self);

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
