#include <glib.h>
#include <gtk/gtk.h> 
#include <stdbool.h>

#define VERSION "0.0.1"

/*
gui_data g_data;

typedef struct myma_data
{
} gui_data;


typedef struct gui
{
    GtkWidget *entry_;
    GtkWidget *entry_location;
    GtkWidget *entry_description;

} gui;
*/

GdkPixbuf *create_pixbuf(const gchar * filename)
{
   GdkPixbuf *pixbuf;
   GError *error = NULL;
   pixbuf = gdk_pixbuf_new_from_file(filename, &error);
   if(!pixbuf) {
      fprintf(stderr, "%s\n", error->message);
      g_error_free(error);
   }

   return pixbuf;
}

void show_about(GtkWidget *widget, gpointer data)
{
  //GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file("data/myma.png", NULL);

  GtkWidget *dialog = gtk_about_dialog_new();
  gtk_about_dialog_set_name(GTK_ABOUT_DIALOG(dialog), "psc");
  gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(dialog), VERSION); 
  gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(dialog),"(c) Alessandro Rosetti");
  gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(dialog), 
     "project secure chat");
  gtk_about_dialog_set_website(GTK_ABOUT_DIALOG(dialog), 
      "http://code.google.com/p/project-secure-chat/");
  //gtk_about_dialog_set_logo(GTK_ABOUT_DIALOG(dialog), pixbuf);
  //g_object_unref(pixbuf), pixbuf = NULL;
  gtk_dialog_run(GTK_DIALOG (dialog));
  gtk_widget_destroy(dialog);
}

/*
void show_message (gchar *message)
{
   GtkWidget *dialog, *label;
   
   dialog = gtk_dialog_new_with_buttons ("pschat",
                                         0,
                                         GTK_DIALOG_DESTROY_WITH_PARENT,
                                         GTK_STOCK_OK,
                                         GTK_RESPONSE_NONE,
                                         NULL);
   label = gtk_label_new (message);
  
   g_signal_connect_swapped (dialog,
                             "response", 
                             G_CALLBACK (gtk_widget_destroy),dialog);
   gtk_container_add (GTK_CONTAINER (GTK_DIALOG(dialog)->vbox),label);
   gtk_widget_show_all (dialog);
}
*/

void destroy (GtkObject *object, gpointer user_data) 
{ 
    gtk_main_quit(); 
}

int main(int argc, char** argv)
{
    GtkWidget *window; 
    GtkWidget *vbox_main; 

    GtkWidget *menubar;
    GtkWidget *filemenu,*helpmenu;
    GtkWidget *file;
    GtkWidget *new,*open,*sep,*quit;
    GtkWidget *help;
    GtkWidget *about;
    GtkAccelGroup *accel_group = NULL;
    
    GtkWidget *toolbar;
    GtkToolItem *toolbar_connect;
    GtkToolItem *toolbar_separator;
    GtkToolItem *toolbar_exit;

    GtkWidget *hbox_chat; 
    GtkWidget *view_chat;
    GtkTextBuffer *buffer;
    GtkTextIter start, end;
    GtkTextIter iter;


    GtkWidget *hbox_inputs;
    GtkWidget *vbox_inputs;
    GtkWidget *entry_command;
    GtkWidget *button_send;

    GtkWidget *status_bar;
    
    GtkWidget *dialog;

    GdkColor color;
    gdk_color_parse ("red", &color);
    
    /* inits */
    gtk_init (&argc, &argv); 

    //psc_init();

    /* window */
    window = gtk_window_new (GTK_WINDOW_TOPLEVEL); 
    gtk_container_set_border_width(GTK_CONTAINER(window),0); 
    gtk_window_set_urgency_hint (GTK_WINDOW(window), TRUE); 
    gtk_window_set_title (GTK_WINDOW (window), "pschat");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_window_set_resizable(GTK_WINDOW(window), TRUE);
    //gtk_window_set_icon(GTK_WINDOW(window), create_pixbuf("data/psc.png"));

    gtk_widget_show(window); 
    
    g_signal_connect(G_OBJECT(window), "delete_event", G_CALLBACK(destroy), NULL); 
    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(destroy), NULL); 
    
    /* vbox */

    vbox_main = gtk_vbox_new (FALSE, 3); 
    gtk_container_add(GTK_CONTAINER(window), vbox_main); 
    gtk_container_set_border_width(GTK_CONTAINER(vbox_main),0);

    /* accellgroup */
    
    accel_group = gtk_accel_group_new();
    gtk_window_add_accel_group(GTK_WINDOW(window), accel_group);

    /* menubar */

    menubar = gtk_menu_bar_new();
    filemenu = gtk_menu_new();
    helpmenu = gtk_menu_new();

    file = gtk_menu_item_new_with_label("File");
    //new = gtk_image_menu_item_new_from_stock(GTK_STOCK_NEW, NULL);
    //open = gtk_image_menu_item_new_from_stock(GTK_STOCK_OPEN, NULL);
    sep = gtk_separator_menu_item_new();
    quit = gtk_image_menu_item_new_from_stock(GTK_STOCK_QUIT, accel_group);
    help = gtk_image_menu_item_new_from_stock(GTK_STOCK_HELP, NULL);
    about = gtk_image_menu_item_new_from_stock(GTK_STOCK_ABOUT, NULL);

    gtk_menu_item_set_submenu(GTK_MENU_ITEM(file), filemenu);
    //gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), connect);
    gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), sep);
    gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), quit);
    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), file);

    gtk_menu_item_set_submenu(GTK_MENU_ITEM(help), helpmenu);
    gtk_menu_shell_append(GTK_MENU_SHELL(helpmenu), about);
    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), help);


    gtk_box_pack_start(GTK_BOX(vbox_main), menubar, FALSE, FALSE, 3);

    g_signal_connect(G_OBJECT(quit), "activate", G_CALLBACK(gtk_main_quit), NULL); 
    g_signal_connect(G_OBJECT(about), "activate", G_CALLBACK(show_about), NULL); 

    /* toolbar */
    
    toolbar = gtk_toolbar_new();
    gtk_toolbar_set_style(GTK_TOOLBAR(toolbar), GTK_TOOLBAR_ICONS);

    gtk_container_set_border_width(GTK_CONTAINER(toolbar), 2);

    toolbar_connect = gtk_tool_button_new_from_stock(GTK_STOCK_NEW);
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), toolbar_connect, -1);

    toolbar_separator = gtk_separator_tool_item_new();
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), toolbar_separator, -1); 

    toolbar_exit = gtk_tool_button_new_from_stock(GTK_STOCK_QUIT);
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), toolbar_exit, -1);

    gtk_box_pack_start(GTK_BOX(vbox_main), toolbar, FALSE, FALSE, 0);
    g_signal_connect(G_OBJECT(toolbar_exit), "clicked", G_CALLBACK(gtk_main_quit), NULL);
    

    /* CHAT */
    hbox_chat = gtk_hbox_new (FALSE, 2);
    gtk_container_add(GTK_CONTAINER(vbox_main), hbox_chat);
    view_chat = gtk_text_view_new();
    gtk_box_pack_start(GTK_BOX(vbox_main), view_chat, TRUE, TRUE, 0);

/*##############################################*/
  buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(view_chat));

  gtk_text_buffer_create_tag(buffer, "gap", "pixels_above_lines", 30, NULL);

  gtk_text_buffer_create_tag(buffer, "lmarg", "left_margin", 5, NULL);
  gtk_text_buffer_create_tag(buffer, "blue_fg", "foreground", "blue", NULL); 
  gtk_text_buffer_create_tag(buffer, "gray_bg", "background", "gray", NULL); 
  gtk_text_buffer_create_tag(buffer, "italic", "style", PANGO_STYLE_ITALIC, NULL);
  gtk_text_buffer_create_tag(buffer, "bold", "weight", PANGO_WEIGHT_BOLD, NULL);

  gtk_text_buffer_get_iter_at_offset(buffer, &iter, 0);

  gtk_text_buffer_insert(buffer, &iter, "Plain text\n", -1);
  gtk_text_buffer_insert_with_tags_by_name(buffer, &iter, "Colored Text\n", -1, "blue_fg", "lmarg",  NULL);
  gtk_text_buffer_insert_with_tags_by_name (buffer, &iter, "Text with colored background\n", -1, "lmarg", "gray_bg", NULL);

  gtk_text_buffer_insert_with_tags_by_name (buffer, &iter, "Text in italics\n", -1, "italic", "lmarg",  NULL);

  gtk_text_buffer_insert_with_tags_by_name (buffer, &iter, "Bold text\n", -1, "bold", "lmarg",  NULL);
/*##############################################*/


    /* INPUTS */
    hbox_inputs = gtk_hbox_new (FALSE, 0);
    gtk_box_pack_start(GTK_BOX (vbox_main), hbox_inputs, FALSE, FALSE, 0);

    vbox_inputs = gtk_vbox_new (FALSE, 0); 
    gtk_container_add(GTK_CONTAINER(hbox_inputs), vbox_inputs); 
    entry_command = gtk_entry_new();
    gtk_container_add(GTK_CONTAINER(vbox_inputs), entry_command);
    button_send = gtk_button_new_with_label("Send");
    gtk_widget_set_size_request (GTK_WIDGET (button_send), 70, 30);
    gtk_box_pack_start(GTK_BOX (hbox_inputs), button_send, FALSE, FALSE, 0);
    //g_signal_connect(G_OBJECT(button_add), "clicked", G_CALLBACK(add_item), &m_input);

    /* status_bar */
    status_bar = gtk_statusbar_new();      
    gtk_box_pack_start(GTK_BOX (vbox_main), status_bar, FALSE, FALSE, 0);

    g_object_set_data(G_OBJECT(status_bar), "info", (gpointer)"1");
    g_object_set_data(G_OBJECT(status_bar), "info", (gpointer) "2");
    g_object_set_data(G_OBJECT(status_bar), "info", (gpointer) "3");

    g_object_set_data(G_OBJECT(status_bar), "warning", (gpointer) "A");
    g_object_set_data(G_OBJECT(status_bar), "warning", (gpointer) "B");
    g_object_set_data(G_OBJECT(status_bar), "warning", (gpointer) "C");

    guint id = gtk_statusbar_get_context_id(GTK_STATUSBAR(status_bar), "info");
    gtk_statusbar_push(GTK_STATUSBAR(status_bar), id, "* uninitialized");

    /* end_widgets */
    gtk_widget_show_all(window);
    gtk_main (); 

    return 0;
}
