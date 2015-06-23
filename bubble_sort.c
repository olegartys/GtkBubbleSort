/*
 * bubble_sort.c
 * 
 * Copyright 2014 olegartys
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
#include <gtk/gtk.h>
#include <stdlib.h>

#define GLADE_FILE_NAME "ui.glade"
#define CSS_FILE_NAME "style.css"
#define BUTTON_K 25


struct MainWindow {
	GtkWindow *win;
	GtkButton *b_start,
			  *b_continue,
			  *b_pause,
			  *b_stop,
			  *b_refresh,
			  *b_elemenet[BUTTON_K];
	GtkWidget *combobox,
			  *grid,
			  *bar[3];
	GtkLabel *label;
} window;

gint rand_array[BUTTON_K];
gint sort_i = 0,
	 sort_j = 0,
	 kol = 0,
	 right_border = 0,
	 left_border = 0;
gint k_exchange = 0,
	 k_comparison = 0,
	 k_iteration = 0;
GtkBuilder *builder = NULL;
gboolean is_sorting = 0,
		 is_paused = 0;

void start_clicked(GtkWidget *wg, gpointer data);
void status_bar_update(GtkWidget *bar);
GtkWidget *get_wg(gchar *name);
GtkBuilder *gtk_builder_init(void);
gboolean sort_update(gpointer data);
void css_style(GtkWidget *widget, gint mode);
enum mode {
	DEFAULT,
	PREPARE,
	ACTIVE
};
void stop_clicked(GtkWidget *wg, gpointer data);
void about_open(GtkMenu *item, gpointer data);
void continue_clicked(GtkWidget *wg, gpointer data);
void pause_clicked(GtkWidget *wg, gpointer data);
void refresh_clicked(GtkWidget *wg, gpointer data);

gint 
main(gint argc, gchar **argv)
{
	gtk_init(&argc, &argv);
	builder = gtk_builder_init(); 
	
	// получение виджетов
	{
		window.win = GTK_WINDOW(get_wg("window1"));
		window.b_start = GTK_BUTTON(get_wg("b_start"));
		window.b_continue = GTK_BUTTON(get_wg("b_continue"));
		window.b_pause = GTK_BUTTON(get_wg("b_pause"));
		window.b_stop = GTK_BUTTON(get_wg("b_stop"));
		window.b_refresh = GTK_BUTTON(get_wg("b_refresh"));
		window.combobox = get_wg("combobox");
		window.label = GTK_LABEL(get_wg("label"));
		window.grid = get_wg("grid");
		window.bar[0] = get_wg("statusbar1");
		window.bar[1] = get_wg("statusbar2");
		window.bar[2] = get_wg("statusbar3");
		status_bar_update(window.bar[0]);
		status_bar_update(window.bar[1]);
		status_bar_update(window.bar[2]);
		
		if (!is_sorting) {
			gtk_widget_set_sensitive(GTK_WIDGET(window.b_continue), FALSE);
			gtk_widget_set_sensitive(GTK_WIDGET(window.b_pause), FALSE);
			gtk_widget_set_sensitive(GTK_WIDGET(window.b_stop), FALSE);
		}
		gtk_combo_box_set_active(GTK_COMBO_BOX(window.combobox), 0);
		css_style(GTK_WIDGET(window.label), ACTIVE);
		
		
		srand(time(NULL));
		gint i;
		const gchar *t1 = "button";
		for (i = 0; i < BUTTON_K; i++) {
			const gchar *t2 = g_strdup_printf("%d", i+1);
			gchar *s = g_strconcat(t1, t2, NULL);
			window.b_elemenet[i] = GTK_BUTTON(get_wg(s));
			g_object_set(window.b_elemenet[i], "can-focus", FALSE, NULL);
			gtk_widget_set_sensitive(GTK_WIDGET(window.b_elemenet[i]), FALSE);
			rand_array[i] = random()%1001;

			gchar *s1 = g_strdup_printf("%d", rand_array[i]);
			gtk_button_set_label(window.b_elemenet[i], s1);
			css_style(GTK_WIDGET(window.b_elemenet[i]), DEFAULT);
			g_free(s);
			g_free(s1);
		}
	}
	gtk_builder_connect_signals(builder, NULL);
	gtk_widget_show_all(GTK_WIDGET(window.win));
	gtk_main();
	g_object_unref(builder);
	return 0;
}

//сигналы
/**********************************************************************/
void
start_clicked(GtkWidget *wg, gpointer data)
{
	k_comparison = 0;
	k_exchange = 0;
	k_iteration = 0;
	status_bar_update(window.bar[0]);
	status_bar_update(window.bar[1]);
	status_bar_update(window.bar[2]);
	
	is_sorting = 1;
	gtk_widget_set_sensitive(GTK_WIDGET(window.b_start), FALSE);
	gtk_widget_set_sensitive(GTK_WIDGET(window.b_stop), TRUE);
	gtk_widget_set_sensitive(GTK_WIDGET(window.b_pause), TRUE);
	gtk_widget_set_sensitive(GTK_WIDGET(window.b_refresh), FALSE);
	gtk_widget_set_sensitive(GTK_WIDGET(window.combobox), FALSE);
	gchar *text = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(window.combobox));
	sscanf(text, "%d", &kol);
	gint i;
	for (i = 0; i < BUTTON_K; i++)
		css_style(GTK_WIDGET(window.b_elemenet[i]), DEFAULT);
	switch(kol) {
		case 5: 
			for (i = 10; i < 15; i++) {
				gtk_widget_set_sensitive(GTK_WIDGET(window.b_elemenet[i]), TRUE);
				css_style(GTK_WIDGET(window.b_elemenet[i]), PREPARE);
			}
			sort_i = 10;
			sort_j = 10;
			left_border = 10;
			right_border = 15;
			g_timeout_add_seconds(4, sort_update, NULL);
			break;
		case 10:
			for (i = 10; i < 20; i++) {
				gtk_widget_set_sensitive(GTK_WIDGET(window.b_elemenet[i]), TRUE);
				css_style(GTK_WIDGET(window.b_elemenet[i]), PREPARE);
			}
			sort_i = 10;
			sort_j = 10;
			left_border = 10;
			right_border = 20;
			g_timeout_add_seconds(4, sort_update, NULL);
			break;
		case 15:
			for (i = 0; i < 15; i++) {
				gtk_widget_set_sensitive(GTK_WIDGET(window.b_elemenet[i]), TRUE);
				css_style(GTK_WIDGET(window.b_elemenet[i]), PREPARE);
			}
			sort_i = 0;
			sort_j = 0;
			left_border = 0;
			right_border = 15;
			g_timeout_add_seconds(4, sort_update, NULL);
			break;
		case 20:
			for (i = 0; i < 20; i++) {
				gtk_widget_set_sensitive(GTK_WIDGET(window.b_elemenet[i]), TRUE);
				css_style(GTK_WIDGET(window.b_elemenet[i]), PREPARE);
			}
			sort_i = 0;
			sort_j = 0;
			left_border = 0;
			right_border = 20;
			g_timeout_add_seconds(4, sort_update, NULL);
			break;
		case 25:
			for (i = 0; i < BUTTON_K; i++) {
				gtk_widget_set_sensitive(GTK_WIDGET(window.b_elemenet[i]), TRUE);
				css_style(GTK_WIDGET(window.b_elemenet[i]), PREPARE);
			}
			sort_i = 0;
			sort_j = 0;
			left_border = 0;
			right_border = BUTTON_K;
			g_timeout_add_seconds(4, sort_update, NULL);
			break;
	}
}

void 
stop_clicked(GtkWidget *wg, gpointer data)
{
	gint i;
	is_sorting = 0;
	
	for (i = 0; i < BUTTON_K; i++) {
		gtk_widget_set_sensitive(GTK_WIDGET(window.b_elemenet[i]), FALSE);
	}
	gtk_widget_set_sensitive(GTK_WIDGET(window.b_start), TRUE);
	gtk_widget_set_sensitive(GTK_WIDGET(window.combobox), TRUE);
	gtk_widget_set_sensitive(GTK_WIDGET(window.b_refresh), TRUE);
	gtk_widget_set_sensitive(GTK_WIDGET(window.b_continue), FALSE);
	gtk_widget_set_sensitive(GTK_WIDGET(window.b_pause), FALSE);
	gtk_widget_set_sensitive(GTK_WIDGET(window.b_stop), FALSE);
}

void
refresh_clicked(GtkWidget *wg, gpointer data)
{
	gint i;
	srand(time(NULL));
	for (i = 0; i < BUTTON_K; i++) {
		rand_array[i] = rand()%1001;
		gchar *s = g_strdup_printf("%d", rand_array[i]);
		gtk_button_set_label(window.b_elemenet[i], s);
		g_free(s);
	}
	sort_i = 0;
	sort_j = 0;	
}

void
continue_clicked(GtkWidget *wg, gpointer data)
{
	gtk_widget_set_sensitive(GTK_WIDGET(window.b_continue), FALSE);
	gtk_widget_set_sensitive(GTK_WIDGET(window.b_pause), TRUE);
	is_paused = 0;
}

void
pause_clicked(GtkWidget *wg, gpointer data)
{
	gtk_widget_set_sensitive(GTK_WIDGET(window.b_continue), TRUE);
	gtk_widget_set_sensitive(GTK_WIDGET(window.b_pause), FALSE);
	is_paused = 1;
}

void
about_open(GtkMenu *item, gpointer data)
{
	#define ICON_NAME "bubble.gif"
	const gchar *path = g_build_filename(ICON_NAME, NULL);
	GdkPixbuf *pic =  gdk_pixbuf_new_from_file(path, NULL);
	const gchar *authors[30] = {"<olegartys@gmail.com>"};
	gtk_show_about_dialog (GTK_WINDOW(window.win), 
					  "program-name", "Graphic demonstration of \"BubbleSort\" algorithm.",
					  "logo", pic,
					  "copyright", "Copyright (c) 2014",
					  //"comments", "Программа предназначена для наглядного рассмотрения работы алгоритма сортировки \"пузырьком\".",
					  "license-type", GTK_LICENSE_GPL_3_0,
					  "authors", authors,
					  NULL);
}

//функции
/**********************************************************************/
gboolean sort_update(gpointer data)
{
	if (is_paused) return TRUE;
	if (!is_sorting) return FALSE;
	if (sort_i < right_border) {
		if (sort_j < right_border-1) {
			css_style(GTK_WIDGET(window.b_elemenet[sort_j]), ACTIVE);
			css_style(GTK_WIDGET(window.b_elemenet[sort_j+1]), ACTIVE);
			//g_print("HERE COLOR!\n");
			gboolean update(gpointer data) 
			{
				k_comparison++;
				status_bar_update(window.bar[2]);
				if(rand_array[sort_j] > rand_array[sort_j + 1]) {
					k_exchange++;
					status_bar_update(window.bar[1]);
					const gchar *s1 = gtk_button_get_label(window.b_elemenet[sort_j]);
					const gchar *s2 = gtk_button_get_label(window.b_elemenet[sort_j+1]);
					gchar *tmp = g_strdup(s1);
					s1 = g_strdup(s2);
					s2 = g_strdup(tmp);
					gtk_button_set_label(window.b_elemenet[sort_j], s1);
					gtk_button_set_label(window.b_elemenet[sort_j+1], s2);
						 
					gint t;
					t = rand_array[sort_j];
					rand_array[sort_j] = rand_array[sort_j+1];
					rand_array[sort_j+1] = t;  
				}
				sort_j++;
				//g_print("HERE COMPARE\n");
				return FALSE;
			}
			g_timeout_add_seconds(2, update, NULL);
			
			
			gboolean reset(gpointer dat)
			{
				gint i = left_border;
				for (;i < right_border; i++) {
					GtkStyleContext *style_context = gtk_widget_get_style_context(GTK_WIDGET(window.b_elemenet[i]));
					const gchar *data = "* {box-shadow: 0 0 5px #333 inset;	margin: 10px; background:#AAAAAA;}";
					GtkCssProvider *css_provider = gtk_css_provider_new();
					gtk_css_provider_load_from_data(css_provider, data, -1, NULL);
					gtk_style_context_add_provider(style_context, 
													GTK_STYLE_PROVIDER(css_provider),
													GTK_STYLE_PROVIDER_PRIORITY_USER);
				}
				//g_print("HERE UNCOLOR\n");
				return FALSE;
			}
			g_timeout_add_seconds(3, reset, NULL);
		} else {
			sort_j = left_border; 
			sort_i++;
		}
		k_iteration++;
		status_bar_update(window.bar[0]);
	} else {
		stop_clicked(NULL, NULL);
		return FALSE;
	}	
	return TRUE;
}

void
status_bar_update(GtkWidget *bar)
{
	if (bar == window.bar[0]) {
		// обновление значения первого статус бара
		guint t = gtk_statusbar_get_context_id(GTK_STATUSBAR(window.bar[0]), "ITERATION");
		gchar *s = g_strdup_printf("Количество итераций: %d", k_iteration);
		gtk_statusbar_push(GTK_STATUSBAR(window.bar[0]), t, s);
		g_free(s);
	}
	if (bar == window.bar[1]) {
		// обновление значений второго статус бара
		guint t = gtk_statusbar_get_context_id(GTK_STATUSBAR(window.bar[1]), "EXCHANGE");
		gchar *s = g_strdup_printf("Количество обменов: %d", k_exchange);
		gtk_statusbar_push(GTK_STATUSBAR(window.bar[1]), t, s);
		g_free(s);
	}
	if (bar == window.bar[2]) {
		// обновление значений третьего статус бара
		guint t = gtk_statusbar_get_context_id(GTK_STATUSBAR(window.bar[2]), "COMPARISON");
		gchar *s = g_strdup_printf("Количество сравнений: %d", k_comparison);
		gtk_statusbar_push(GTK_STATUSBAR(window.bar[2]), t, s);
		g_free(s);
	}
}

GtkWidget *
get_wg(gchar *name)
{
	GtkWidget *wg;
	if (!( wg = GTK_WIDGET(gtk_builder_get_object(builder, name)) )) {
		g_critical("Can't find widget %s!", name);
        return NULL;
    }
    return wg;
}

GtkBuilder *
gtk_builder_init(void)
{
	GtkBuilder *builder = gtk_builder_new();
	GError *error = NULL;
	gchar *path = g_build_filename(GLADE_FILE_NAME, NULL);
	if (!gtk_builder_add_from_file(builder, path, &error))
	{
        g_critical("%s", error->message);
        g_error_free(error);
        exit(EXIT_FAILURE);
    }
	g_free(path);
	return builder;
}

void 
css_style(GtkWidget *widget, gint mode) 
{
	const gchar *data_default = "* {box-shadow: 0 0 5px #333 inset;	margin: 10px; background:#CCCCCC;}";
	const gchar *data_prepare = "* {box-shadow: 0 0 5px #333 inset;	margin: 10px; background:#AAAAAA;}";
	const gchar *data_active = "* {box-shadow: 0 0 5px #333 inset;	margin: 10px; background:#DAE0E8;}";
	const gchar *data;
	if (mode == DEFAULT) data = data_default;
	if (mode == PREPARE) data = data_prepare; 
	if (mode == ACTIVE) data = data_active;
	GtkCssProvider *css_provider = gtk_css_provider_new();
	GtkStyleContext *style_context = gtk_widget_get_style_context(GTK_WIDGET(widget));
	gtk_css_provider_load_from_data(css_provider, data, -1, NULL);
	gtk_style_context_add_provider(style_context, 
									GTK_STYLE_PROVIDER(css_provider),
								    GTK_STYLE_PROVIDER_PRIORITY_USER);
}
