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
#include "json/notebook.h"
/* #include "interpreter/" */

/* Function Declatarions (Might be moved to geany-nb.h) */
static void activate_notebook(GtkMenuItem *menu_item, gpointer menu_data);
static void deactivate_notebook(GtkMenuItem *menu_item, gpointer menu_data);
static void open_notebook_cell(GtkMenuItem *menu_item, gpointer user_data);
static gboolean create_gtk_spin(size_t *value, GtkWidget *parent, size_t max);
static void save_notebook(GObject *obj, GeanyDocument *doc, gpointer user_data);

static Notebook *global_notebook = NULL;
static GeanyDocument *nb_doc= NULL;

static gboolean create_gtk_spin(size_t *value, GtkWidget *parent, size_t max){
	GtkWidget *window, *spin;
	GtkAdjustment *adj;
	
	adj = gtk_adjustment_new(0, 0, max, 1, 10, 0);
	spin = gtk_spin_button_new(adj, 1, 0);
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	
	gtk_spin_button_set_numeric(GTK_SPIN_BUTTON(spin), TRUE);
	gtk_spin_button_set_snap_to_ticks(GTK_SPIN_BUTTON(spin), TRUE);
	GtkWidget *dialog = gtk_dialog_new_with_buttons(
		"Enter a number",
		GTK_WINDOW(window),
		GTK_DIALOG_MODAL,
		"_OK", GTK_RESPONSE_OK,
		"_Cancel", GTK_RESPONSE_CANCEL,
		NULL
	);

	GtkWidget *content = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
	gtk_container_add(GTK_CONTAINER(content), spin);
	
	gtk_widget_show_all(dialog);
	if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_OK){
		*value = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spin));
		gtk_widget_destroy(dialog);
		return TRUE;
	}else{
		gtk_widget_destroy(dialog);
		return FALSE;
	}
}

static void open_notebook_cell(GtkMenuItem *menu_item, gpointer user_data){
	size_t cell_num = 0;
	GeanyDocument *doc = document_get_current();

	if(doc != nb_doc){
		dialogs_show_msgbox(GTK_MESSAGE_INFO, "Doc not obtained </3");
		return;
	}

	ScintillaObject *sci = doc->editor->sci;
	gboolean err = create_gtk_spin(&cell_num, GTK_WIDGET(menu_item), notebook_len(global_notebook) - 1);
	
	if(!err)
		return;

	const char **str_list = notebook_get_text(global_notebook, cell_num);
	if(!str_list){
		dialogs_show_msgbox(GTK_MESSAGE_INFO, "Failed to Obtain cell source");
		return;
	}
	sci_set_text(sci, "\n");
	const char *elem;
	for(size_t i = 0; (elem = str_list[i++]) != NULL;)
		sci_insert_text(sci, -1, elem);
	free(str_list);
}

static void deactivate_notebook(GtkMenuItem *menu_item, gpointer menu_data){
	notebook_free(global_notebook);
	global_notebook = NULL;
	nb_doc = NULL;
	
	GtkWidget *nb_initialize;
	GtkWidget *notebook_menu = (GtkWidget *) menu_data;
	GList *children, *l;

	children = gtk_container_get_children(GTK_CONTAINER(notebook_menu));

	for (l = children; l != NULL; l = l->next)
		gtk_widget_destroy(GTK_WIDGET(l->data));

	g_list_free(children);

	/* Submenu items */
	nb_initialize = gtk_menu_item_new_with_mnemonic("_Initialize Notebook");
	gtk_widget_show(nb_initialize);
	gtk_menu_shell_append(GTK_MENU_SHELL(notebook_menu), nb_initialize);
	g_signal_connect(nb_initialize, "activate",
	G_CALLBACK(activate_notebook), notebook_menu);
}

static void activate_notebook(GtkMenuItem *menu_item, gpointer menu_data){
	GeanyDocument *doc = document_get_current();

	if(!doc){
		dialogs_show_msgbox(GTK_MESSAGE_INFO, "Doc not obtained </3");
		return;
	}

	GtkWidget *nb_open_cell;
	GtkWidget *nb_deinitialize;
	GtkWidget *notebook_menu = (GtkWidget *) menu_data;

	ScintillaObject *sci = doc->editor->sci;
	Notebook *notebook = notebook_from_str(sci_get_contents(sci, -1), sci_get_length(sci));

	if(!notebook){
		dialogs_show_msgbox(GTK_MESSAGE_INFO, "Notbook not made....");
		return;
	}

	dialogs_show_msgbox(GTK_MESSAGE_INFO, "NOTEBOOK SUCCESS....");
	global_notebook = notebook;
	gtk_widget_destroy(GTK_WIDGET(menu_item));

	nb_open_cell = gtk_menu_item_new_with_mnemonic("_Open Cell");
	gtk_widget_show(nb_open_cell);
	gtk_menu_shell_append(GTK_MENU_SHELL(notebook_menu), nb_open_cell);
	g_signal_connect(nb_open_cell, "activate",
	  G_CALLBACK(open_notebook_cell), NULL);

	nb_deinitialize = gtk_menu_item_new_with_mnemonic("_Deinitialize Notebook");
	gtk_widget_show(nb_deinitialize);
	gtk_menu_shell_append(GTK_MENU_SHELL(notebook_menu), nb_deinitialize);
	g_signal_connect(nb_deinitialize, "activate",
	  G_CALLBACK(deactivate_notebook), notebook_menu);
	nb_doc = doc;
}

static void before_save_notebook(GObject *obj, GeanyDocument *doc, gpointer user_data){
	if(doc != nb_doc)
		return;
	ScintillaObject *sci = doc->editor->sci;
	notebook_set_text(global_notebook, sci_get_contents(sci, -1), sci_get_length(sci));
	const char *doc_str = notebook_get_doc(global_notebook);
	sci_set_text(sci, doc_str);
	free(doc_str);
}

static void save_notebook(GObject *obj, GeanyDocument *doc, gpointer user_data){
	if(doc != nb_doc)
		return;
	ScintillaObject *sci = doc->editor->sci;
	const char **str_list = notebook_get_text(global_notebook, notebook_get_cur_ind(global_notebook));
	if(!str_list){
		dialogs_show_msgbox(GTK_MESSAGE_INFO, "Failed to Obtain cell source");
		return;
	}
	sci_set_text(sci, "\n");
	const char *elem;
	for(size_t i = 0; (elem = str_list[i++]) != NULL;)
		sci_insert_text(sci, -1, elem);
	free(str_list);
}


static gboolean notebook_init(GeanyPlugin *plugin, gpointer pdata)
{
	GtkWidget *notebook_menu_item;
	GtkWidget *notebook_menu;
	GtkWidget *nb_initialize;

	/* Parent item: Tools -> Notebook */
	notebook_menu_item = gtk_menu_item_new_with_mnemonic("_Notebook");
	gtk_widget_show(notebook_menu_item);

	/* Submenu */
	notebook_menu = gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(notebook_menu_item), notebook_menu);

	/* Submenu items */
	nb_initialize = gtk_menu_item_new_with_mnemonic("_Initialize Notebook");
	gtk_widget_show(nb_initialize);
	gtk_menu_shell_append(GTK_MENU_SHELL(notebook_menu), nb_initialize);
	g_signal_connect(nb_initialize, "activate",
					 G_CALLBACK(activate_notebook), notebook_menu);
	g_signal_connect(nb_initialize, "activate",
					 G_CALLBACK(activate_notebook), notebook_menu);
	/* Add parent to Tools menu */
	gtk_menu_shell_append(
		GTK_MENU_SHELL(plugin->geany_data->main_widgets->tools_menu),
		notebook_menu_item
	);
	
	geany_plugin_set_data(plugin, notebook_menu_item, NULL);
	plugin_signal_connect(
		plugin,
		NULL,
		"document-before-save",
		TRUE,
		G_CALLBACK(before_save_notebook),
		NULL
	);

	plugin_signal_connect(
		plugin,
		NULL,
		"document-save",
		TRUE,
		G_CALLBACK(save_notebook),
		NULL
	);
	return TRUE;
}

static void notebook_cleanup(GeanyPlugin *plugin, gpointer pdata){
	printf("Bye World :-(\n");
		notebook_free(global_notebook);
	GtkWidget *notebook_menu_item = (GtkWidget *) pdata;
 
	gtk_widget_destroy(notebook_menu_item);
}

G_MODULE_EXPORT
void geany_load_module(GeanyPlugin *plugin){

	/* Metadata */
	plugin->info->name = "Geany Notebook";
	plugin->info->description = "Extend .ipynb support to Geany";
	plugin->info->version = "0.0.1";
	plugin->info->author = "Eyassu Mongalo";

	plugin->funcs->init= notebook_init;
	plugin->funcs->cleanup = notebook_cleanup;

	GEANY_PLUGIN_REGISTER(plugin, 225);
}
