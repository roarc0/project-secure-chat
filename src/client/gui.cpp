
#include "gui.h"

enum {
  COLUMN_STRING,
  COLUMN_INT,
  COLUMNS
};

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
  gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(dialog), PACKAGE_VERSION); 
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

void add_list(gpointer data, gchar *str, gint num)
{
  GtkTreeView *view = GTK_TREE_VIEW(data);
  GtkTreeModel *model;
  GtkTreeIter iter;

  model = gtk_tree_view_get_model(GTK_TREE_VIEW(view));

  gtk_list_store_append(GTK_LIST_STORE(model), &iter);
  gtk_list_store_set(GTK_LIST_STORE(model), &iter, 
		     COLUMN_STRING, str, 
		     COLUMN_INT, num, -1);
}


void main_gui(int argc, char **argv)
{
    GtkWidget *window; 
    GtkWidget *vbox_main; 

    /* menubar */
    GtkWidget *menubar;
    GtkWidget *filemenu,*helpmenu;
    GtkWidget *file;
    GtkWidget *connect,*open,*sep,*quit;
    GtkWidget *help;
    GtkWidget *about;
    GtkAccelGroup *accel_group = NULL;
    
    /* toolbar */
    GtkWidget *toolbar;
    GtkToolItem *toolbar_connect;
    GtkToolItem *toolbar_separator;
    GtkToolItem *toolbar_exit;

    /* chat e userlist */
    GtkWidget *hbox_chat; 
    GtkWidget *scrolledwindow_chat;
    GtkWidget *view_chat;
    GtkTextBuffer *view_chat_buffer;
    GtkTextIter start, end;
    GtkTextIter textiter;

    GtkWidget *scrolledwindow;
    GtkListStore *model;              /* oggetto model       */
    GtkWidget *view;                  /* -\                  */ 
    GtkCellRenderer *renderer;        /* ---> l'oggetto view */
    GtkTreeSelection *selection;      /* -/                  */

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

    vbox_main = gtk_vbox_new (FALSE, 1); 
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
    connect = gtk_image_menu_item_new_from_stock(GTK_STOCK_NEW, NULL);
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


    gtk_box_pack_start(GTK_BOX(vbox_main), menubar, FALSE, FALSE, 0);

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
    hbox_chat = gtk_hbox_new (FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox_main), hbox_chat, TRUE, TRUE, 0);

    scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
    gtk_box_pack_start (GTK_BOX (hbox_chat), scrolledwindow, TRUE, TRUE, 0);
    gtk_container_set_border_width (GTK_CONTAINER (scrolledwindow), 2);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow), 
			      GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_widget_show (scrolledwindow);

    view_chat = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(view_chat), false);

    gtk_container_add (GTK_CONTAINER (scrolledwindow), view_chat);
    //gtk_box_pack_start(GTK_BOX(hbox_chat), view_chat, TRUE, TRUE, 0);

    /*##############################################*/
    view_chat_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(view_chat));

    gtk_text_buffer_create_tag(view_chat_buffer, "gap", "pixels_above_lines", 30, NULL);

    gtk_text_buffer_create_tag(view_chat_buffer, "lmarg", "left_margin", 5, NULL);

    gtk_text_buffer_create_tag(view_chat_buffer, "black_fg", "foreground", "black", NULL);
    gtk_text_buffer_create_tag(view_chat_buffer, "green_fg", "background", "green", NULL);
    gtk_text_buffer_create_tag(view_chat_buffer, "red_fg", "background", "red", NULL);

    /*gtk_text_buffer_create_tag(view_chat_buffer, "green_bg", "background", "green", NULL); 
    gtk_text_buffer_create_tag(view_chat_buffer, "italic", "style", PANGO_STYLE_ITALIC, NULL);*/
    gtk_text_buffer_create_tag(view_chat_buffer, "bold", "weight", PANGO_WEIGHT_BOLD, NULL);
    gtk_text_buffer_get_iter_at_offset(view_chat_buffer, &textiter, 0);

    gtk_text_buffer_insert_with_tags_by_name (view_chat_buffer, &textiter, "\"gufo\" has joined the chat\n", -1, "lmarg", "green_fg", "bold", NULL);
    gtk_text_buffer_insert_with_tags_by_name(view_chat_buffer, &textiter, "<gufo> salve buonuomo\n", -1, "black_fg", "lmarg", NULL);
    gtk_text_buffer_insert_with_tags_by_name (view_chat_buffer, &textiter, "<alec> ave!\n", -1, "black_fg", "lmarg",  NULL);
    gtk_text_buffer_insert_with_tags_by_name (view_chat_buffer, &textiter, "<furla> ciao!\n", -1, "black_fg", "lmarg",  NULL);
    gtk_text_buffer_insert_with_tags_by_name (view_chat_buffer, &textiter, "\"gufo\" has been kicked out by \"alec\"!\n", -1, "red_fg", "lmarg", "bold",  NULL);
    /*##############################################*/

    scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
    gtk_box_pack_start (GTK_BOX (hbox_chat), scrolledwindow, TRUE, TRUE, 0);
    gtk_container_set_border_width (GTK_CONTAINER (scrolledwindow), 2);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow), 
			      GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_widget_show (scrolledwindow);

    model     = gtk_list_store_new(COLUMNS, G_TYPE_STRING, G_TYPE_INT);
    view      = gtk_tree_view_new_with_model (GTK_TREE_MODEL(model));
    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(view));

    gtk_tree_selection_set_mode(selection, GTK_SELECTION_SINGLE);
    gtk_tree_view_set_rules_hint(GTK_TREE_VIEW(view), TRUE);

    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(view), /* vista */
                       -1,                  /* posizione della colonna */
                       "Name",  /* titolo della colonna */
                       renderer,            /* cella inserita nella colonna */
                       "text",              /* attributo colonna */
                       COLUMN_STRING,    /* colonna inserita  */
                       NULL);               /* fine ;-) */

    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(view), -1, 
				          "?", renderer, 
				          "text", COLUMN_INT, 
				          NULL);          

    gtk_widget_show (view);
    g_object_unref(model);

    gtk_container_add (GTK_CONTAINER (scrolledwindow), view);
    gtk_container_set_border_width (GTK_CONTAINER (view), 0);
    gtk_tree_view_set_rules_hint (GTK_TREE_VIEW (view), TRUE);

    add_list(view, (gchar*) "alec", 0);
    add_list(view, (gchar*) "furla", 1);
    add_list(view, (gchar*) "gufo", 2);

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
    g_print ("starting gtk\n");   
    gtk_main(); 

    return;
}
