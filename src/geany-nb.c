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
#include "json-parser.h"

static void activate_notebook(GtkMenuItem *menuitem, gpointer user_data){
	GeanyDocument *doc = document_get_current();

	if(!doc){
		dialogs_show_msgbox(GTK_MESSAGE_INFO, "Doc not obtained </3");
		return;
	}
	
	Notebook *notebook = notebook_create(doc->editor->sci,1);
	
	if(!notebook){
		dialogs_show_msgbox(GTK_MESSAGE_INFO, "Notbook not made....");
		return;
	}
	if(!notebook_load_cells(notebook)){
		dialogs_show_msgbox(GTK_MESSAGE_INFO, "Notbook not loaded fully...");
	}
	notebook_set_text(notebook, doc->editor->sci);
	dialogs_show_msgbox(GTK_MESSAGE_INFO, "NOTEBOOK SUCCESS....");
	notebook_drop(notebook);
	return;
}

static gboolean notebook_init(GeanyPlugin *plugin, gpointer pdata){
	GtkWidget *main_menu_item;
	
    main_menu_item = gtk_menu_item_new_with_mnemonic("Format Notebook!");
    gtk_widget_show(main_menu_item);
    
    gtk_container_add(GTK_CONTAINER(plugin->geany_data->main_widgets->tools_menu), main_menu_item);
    g_signal_connect(main_menu_item, "activate", G_CALLBACK(activate_notebook), NULL);
     
 
    geany_plugin_set_data(plugin, main_menu_item, NULL);
    return TRUE;
    
}

static void notebook_cleanup(GeanyPlugin *plugin, gpointer pdata){
    printf("Bye World :-(\n");
    GtkWidget *main_menu_item = (GtkWidget *) pdata;
 
    gtk_widget_destroy(main_menu_item);
}

G_MODULE_EXPORT
void geany_load_module(GeanyPlugin *plugin){

	/*Metadata*/
	plugin->info->name = "Geany Notebook";
	plugin->info->description = "Python Notebook support";
	plugin->info->version = "0.0.1";
	plugin->info->author = "Eyassu Mongalo";

	plugin->funcs->init= notebook_init;
	plugin->funcs->cleanup = notebook_cleanup;

	GEANY_PLUGIN_REGISTER(plugin, 225);
}
