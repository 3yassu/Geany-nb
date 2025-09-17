/*
 *  geany-nb.c
 *
 *  Copyright 2025 Eyassu Mongalo <3yassu@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <geanyplugin.h>



static gboolean notebook_init(GeanyPlugin *plugin, gpointer pdata){
	GtkWidget *main_menu_item;
 
    main_menu_item = gtk_menu_item_new_with_mnemonic("Geany Notebook");
    gtk_widget_show(main_menu_item);
    gtk_container_add(GTK_CONTAINER(plugin->geany_data->main_widgets->tools_menu), main_menu_item);
    /*g_signal_connect(main_menu_item, "activate", G_CALLBACK(notebook_setting), NULL);
     *Widget will make menu to instantiate code/markup block or to change the order?
     *	Also a setting to allow to edit format
		 *	add make cell, markdown setting.
     */
 
    geany_plugin_set_data(plugin, main_menu_item, NULL);
    return TRUE;
}

void notebook_cleanup(GeanyPlugin *plugin, gpointer pdata){
	/*Also nothing*/
}
/*
 * may add extension plugins to get called with the creation of a new file
 *
 */

G_MODULE_EXPORT
void geany_load_module(GeanyPlugin *plugin){

	/*Metadata*/
	plugin->info->name = "Geany Notebook";
	plugin->info->description = "Extend geany support for python notebook";
	plugin->info->version = "0.0.1";
	plugin->info->author = "Eyassu Mongalo";

	plugin->funcs->init= notebook_init;
	plugin->funcs->cleanup = notebook_cleanup;

	//GEANY_PLUGIN_REGISTER(plugin, 216);
}
