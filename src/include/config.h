/*
 * config.h
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

#ifndef CONFIG_H
#define CONFIG_H

#include <glib-object.h>
#include <json-glib/json-glib.h>

G_BEGIN_DECLS

#define GMCPIL_TYPE_CONFIG gmcpil_config_get_type()
#define GMCPIL_CONFIG(obj) G_TYPE_CHECK_INSTANCE_CAST(obj, GMCPIL_TYPE_CONFIG, GMCPILConfig)
#define GMCPIL_IS_CONFIG(obj) G_TYPE_CHECK_INSTANCE_TYPE(obj, GMCPIL_TYPE_CONFIG)
#define GMCPIL_CONFIG_PRIVATE(obj) gmcpil_config_get_instance_private(obj)

#define VALID_JSON_ARGS(dst, name, parent) \
	(parent.node != NULL || dst != NULL || name != NULL || JSON_NODE_HOLDS_OBJECT(parent.node) || parent.obj == NULL)

#define GMCPIL_GETTER(name) gchar* gmcpil_config_get_ ## name (GMCPILConfig* self) \
{ \
	GMCPILConfigPrivate* private; \
	private = GMCPIL_CONFIG_PRIVATE(self); \
	return private->name; \
}

#define GMCPIL_SETTER(name) void gmcpil_config_set_ ## name (GMCPILConfig* self, const gchar* name) \
{ \
	GMCPILConfigPrivate* private; \
	private = GMCPIL_CONFIG_PRIVATE(self); \
	private->name = g_strdup((gchar*)name); \
	return; \
}

#define GMCPIL_GETTER_SETTER(name) GMCPIL_GETTER(name); GMCPIL_SETTER(name);

#define GMCPIL_GLIB_PROPERTY(id, name, description, prop) \
	pspec = g_param_spec_string(id, name, description, NULL, G_PARAM_READWRITE); \
	g_object_class_install_property(gobject_class, prop, pspec);

#define GMCPIL_SET_DEFAULT(name, value) \
	default_##name = gmcpil_config_get_##name(config); \
	if (default_##name == NULL) \
	{ \
		default_##name = value; \
		gmcpil_config_set_##name(config, default_##name); \
	}

typedef struct GMCPILConfigPrivate GMCPILConfigPrivate;

typedef struct GMCPILConfig
{
	GObject parent;
} GMCPILConfig;

typedef struct GMCPILConfigClass
{
	GObjectClass parent_class;
} GMCPILConfigClass;

GType gmcpil_config_get_type() G_GNUC_CONST;

GMCPILConfig* gmcpil_config_new(gchar* filename);

void gmcpil_config_set_username(GMCPILConfig* self, const gchar* username);
void gmcpil_config_set_features(GMCPILConfig* self, const gchar* features);
void gmcpil_config_set_distance(GMCPILConfig* self, const gchar* distance);
void gmcpil_config_set_last_profile(GMCPILConfig* self, const gchar* last_profile);
void gmcpil_config_set_hud(GMCPILConfig* self, const gchar* hud);
void gmcpil_config_set_hide(GMCPILConfig* self, const gchar* hide);

gchar* gmcpil_config_get_username(GMCPILConfig* self);
gchar* gmcpil_config_get_features(GMCPILConfig* self);
gchar* gmcpil_config_get_distance(GMCPILConfig* self);
gchar* gmcpil_config_get_last_profile(GMCPILConfig* self);
gchar* gmcpil_config_get_hud(GMCPILConfig* self);
gchar* gmcpil_config_get_hide(GMCPILConfig* self);

int gmcpil_config_save(GMCPILConfig* self);

G_END_DECLS

#endif /* CONFIG_H */
