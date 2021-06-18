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
 * 
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <glib-object.h>
#include <json-glib/json-glib.h>

G_BEGIN_DECLS

#define MCPIL_TYPE_CONFIG mcpil_config_get_type()
#define MCPIL_CONFIG(obj) G_TYPE_CHECK_INSTANCE_CAST(obj, MCPIL_TYPE_CONFIG, MCPILConfig)
#define MCPIL_IS_CONFIG(obj) G_TYPE_CHECK_INSTANCE_TYPE(obj, MCPIL_TYPE_CONFIG)
#define MCPIL_CONFIG_PRIVATE(obj) mcpil_config_get_instance_private(obj)

#define VALID_JSON_ARGS(dst, name, parent) \
	(parent.node != NULL || dst != NULL || name != NULL || JSON_NODE_HOLDS_OBJECT(parent.node) || parent.obj == NULL)

#define GETTER(type, name) type mcpil_config_get_ ## name (MCPILConfig* self) \
{ \
	MCPILConfigPrivate* private; \
	private = MCPIL_CONFIG_PRIVATE(self); \
	return private->name; \
}

#define SETTER(type, name) void mcpil_config_set_ ## name (MCPILConfig* self, const type name) \
{ \
	MCPILConfigPrivate* private; \
	private = MCPIL_CONFIG_PRIVATE(self); \
	private->name = g_strdup((type)name); \
	return; \
}

#define GETTER_SETTER(type, name) GETTER(type, name); SETTER(type, name);

typedef struct MCPILConfigPrivate MCPILConfigPrivate;

typedef struct MCPILConfig
{
	GObject parent;
} MCPILConfig;

typedef struct MCPILConfigClass
{
	GObjectClass parent_class;
} MCPILConfigClass;

GType mcpil_config_get_type() G_GNUC_CONST;

MCPILConfig* mcpil_config_new(gchar* filename);

void mcpil_config_set_ip(MCPILConfig* self, const gchar* ip);
void mcpil_config_set_port(MCPILConfig* self, const gchar* port);
void mcpil_config_set_username(MCPILConfig* self, const gchar* username);
void mcpil_config_set_features(MCPILConfig* self, const gchar* features);
void mcpil_config_set_distance(MCPILConfig* self, const gchar* distance);
void mcpil_config_set_last_profile(MCPILConfig* self, const gchar* profile);
void mcpil_config_set_hud(MCPILConfig* self, const gchar* hud);

gchar* mcpil_config_get_ip(MCPILConfig* self);
gchar* mcpil_config_get_port(MCPILConfig* self);
gchar* mcpil_config_get_username(MCPILConfig* self);
gchar* mcpil_config_get_features(MCPILConfig* self);
gchar* mcpil_config_get_distance(MCPILConfig* self);
gchar* mcpil_config_get_last_profile(MCPILConfig* self);
gchar* mcpil_config_get_hud(MCPILConfig* self);

int mcpil_config_save(MCPILConfig* self);

G_END_DECLS

#endif /* CONFIG_H */
