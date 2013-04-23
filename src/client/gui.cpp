#include "gui.h"
#include "revision.h"

enum
{
  COLUMN_STRING1,
  COLUMN_STRING2,
  COLUMNS
};

pthread_mutex_t  mutex_guichange;

struct gui_res
{
    GtkTextBuffer *chat_buffer;
    GtkWidget *text_entry;
    GtkWidget *status_bar;

    GtkWidget *scrolledwindow_chat;
    GtkWidget *scrolledwindow_user_list;

    GtkToolItem *toolbar_connect;
    
    /* Lista Utenti */
    GtkListStore *model_user_list;
    GtkWidget *view_user_list;
    GtkCellRenderer *renderer_user_list;
    GtkTreeSelection *selection_user_list;
} gres;


void push_status_bar(const gchar*);
void add_message_to_chat(gpointer data, gchar *str, gchar type);
void add_user_to_list(gpointer data, gchar *str, gchar *lev);
void remove_user_from_list(gpointer data, const gchar *str);

void* GuiThread(void* arg)
{
    gui_res* gres = (gui_res*) arg;

    bool oldstatus = false;
    while(1)
    {
        gdk_threads_enter();
        if(c_core->IsConnected() && !oldstatus)
        {
            gtk_tool_button_set_label(
                GTK_TOOL_BUTTON(gres->toolbar_connect),
                "Disconnect");
            push_status_bar("Connected with server!");
            add_message_to_chat(gres->chat_buffer,
                                (gchar*) "<local> Connected!\n", 'e');
            add_user_to_list(gres->view_user_list,
                             (gchar*) CFG_GET_STRING("nickname").c_str(),
                             (gchar*) "*");
        }
        if(!c_core->IsConnected() && oldstatus)
        {
            gtk_tool_button_set_label(
                GTK_TOOL_BUTTON(gres->toolbar_connect),
                "Connect");
            push_status_bar("Disconnected from server!");
            add_message_to_chat(gres->chat_buffer,
                                (gchar*) "<local> Disconnected!\n", 'e');
            remove_user_from_list(gres->view_user_list,
                 (gchar*) CFG_GET_STRING("nickname").c_str());
        }
        oldstatus = c_core->IsConnected();

        if(c_core->IsConnected())
        {
            string msg = c_core->GetEvent();
            if(msg.length() > 0)
            {
                if (msg[msg.length()-1] != '\n')
                    msg.append("\n");
                add_message_to_chat(gres->chat_buffer,
                                    (gchar*) msg.c_str(), 'm');
            }
        }
        
        if(!c_core->EmptyEvents())
        {
            INFO("debug","GUI: There's another event to be handled\n");
            continue;
        }
        
        gdk_threads_leave();
        c_core->WaitEvent();
    }

    pthread_exit(NULL);
}

GdkPixbuf *create_pixbuf(const gchar * filename)
{
   GdkPixbuf *pixbuf;
   GError *error = NULL;
   pixbuf = gdk_pixbuf_new_from_file(filename, &error);

   if(!pixbuf)
   {
      fprintf(stderr, "%s\n", error->message);
      g_error_free(error);
   }

   return pixbuf;
}

void show_about()
{
  GdkPixbuf *pixbuf = create_pixbuf("data/psc.png");

  GtkWidget *dialog = gtk_about_dialog_new();
  //gtk_about_dialog_set_name(GTK_ABOUT_DIALOG(dialog), "psc");
  gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(dialog), _REVISION);
  gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(dialog),
      "(c) 2012-2013 Alessandro Rosetti Daniele Lazzarini");
  gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(dialog),
      "project secure chat");
  gtk_about_dialog_set_website(GTK_ABOUT_DIALOG(dialog),
      "http://code.google.com/p/project-secure-chat/");
  gtk_about_dialog_set_logo(GTK_ABOUT_DIALOG(dialog), pixbuf);
  g_object_unref(pixbuf), pixbuf = NULL;
  gtk_dialog_run(GTK_DIALOG (dialog));
  gtk_widget_destroy(dialog);
}

void show_message(gchar *message)
{
   GtkWidget *dialog, *label;

   dialog = gtk_dialog_new_with_buttons (_REVISION,
                                         0,
                                         GTK_DIALOG_DESTROY_WITH_PARENT,
                                         GTK_STOCK_OK,
                                         GTK_RESPONSE_NONE,
                                         NULL);
   label = gtk_label_new (message);

   g_signal_connect_swapped (dialog,
                             "response",
                             G_CALLBACK (gtk_widget_destroy),
                             dialog);
   //gtk_container_add (GTK_CONTAINER (GTK_DIALOG(dialog)->vbox),
   //                   label);
   gtk_widget_show_all (dialog);
}

void on_destroy(gpointer user_data)
{
    gtk_main_quit();
}

void add_user_to_list(gpointer data, gchar *str, gchar *lev)
{
  GtkTreeView *view = GTK_TREE_VIEW(data);
  GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(view));
  GtkTreeIter iter;
  //pthread_mutex_lock(&mutex_guichange);
  gtk_list_store_append(GTK_LIST_STORE(model), &iter);
  gtk_list_store_set(GTK_LIST_STORE(model),
                     &iter,
                     COLUMN_STRING1,
                     str,
                     COLUMN_STRING2,
                     lev,
                     -1);
  //pthread_mutex_unlock(&mutex_guichange);
}


void remove_user_from_list(gpointer data, const gchar *str)
{
    GtkTreeModel            *model;
    GtkTreeIter             iter;
    gboolean                r;
    gchar                   *value;
    
    model = gtk_tree_view_get_model (GTK_TREE_VIEW (data));
    
    if (gtk_tree_model_get_iter_first (model, &iter) == FALSE)
        return;
        
    for (r = gtk_tree_model_get_iter_first (model, &iter); 
         r == TRUE; 
         r = gtk_tree_model_iter_next (model, &iter))
    {                
        gtk_tree_model_get (model, &iter, COLUMN_STRING1, &value,  -1);
        
        if (g_ascii_strcasecmp(value, str) == 0) 
        {
            gtk_list_store_remove (GTK_LIST_STORE (model), &iter);
            break;
        }

        if (value) 
            g_free (value);        
    }
}

void scroll_down(GtkWidget *scrolled)
{
    assert(scrolled);
    GtkAdjustment* adjustment;
    adjustment = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(scrolled));
    gtk_adjustment_set_value(adjustment, gtk_adjustment_get_upper(adjustment));
}

void add_message_to_chat(gpointer data, gchar *msg, gchar type)
{
    //pthread_mutex_lock(&mutex_guichange);
    GtkTextBuffer *text_view_buffer = GTK_TEXT_BUFFER(data);
    GtkTextIter textiter;

    INFO("debug","GUI: Adding Message: \"%s\" to chat\n", (char*) msg);
    
    std::stringstream sstr;  // Starting Horror
    sstr << "[" << get_timestamp(':') << "] " << (char*)msg;

    const std::string& tmp = sstr.str();
    const char* cstr = tmp.c_str();

    gchar *str = (gchar*)cstr;

    gtk_text_buffer_get_end_iter(text_view_buffer, &textiter);
    switch(type)
    {
        case 'j': //user join
            gtk_text_buffer_insert_with_tags_by_name (text_view_buffer,
                &textiter, str, -1, "lmarg", "green_bg", "bold", NULL);
        break;
        case 'l': //user leave
            gtk_text_buffer_insert_with_tags_by_name (text_view_buffer,
                &textiter, str, -1, "lmarg", "red_bg", "white_fg", "bold", NULL);
        break;
        case 'm': //message received
            gtk_text_buffer_insert_with_tags_by_name (text_view_buffer,
                &textiter, str, -1, "lmarg", "black_fg", NULL);
        break;
        case 'M': //message sent
            gtk_text_buffer_insert_with_tags_by_name (text_view_buffer,
                &textiter, str, -1, "lmarg", "black_fg", "bold", NULL);
        break;
        case 'w': //whisp received
            gtk_text_buffer_insert_with_tags_by_name (text_view_buffer,
                &textiter, str, -1, "lmarg", "blue_bg", "white_fg", NULL);
        break;
        case 'W': //whisp sent
            gtk_text_buffer_insert_with_tags_by_name (text_view_buffer,
                &textiter, str, -1, "lmarg", "blue_bg", "white_fg", "bold", NULL);
        break;
        case 'e': //local event
            gtk_text_buffer_insert_with_tags_by_name (text_view_buffer,
                &textiter, str, -1, "lmarg", "magenta_bg", "white_fg", "bold", NULL);
        break;
        case 's': //server communication
            gtk_text_buffer_insert_with_tags_by_name (text_view_buffer,
                &textiter, str, -1, "lmarg", "yellow_bg", "bold", NULL);
        break;
        default:
        break;
    }

    scroll_down(gres.scrolledwindow_chat);
    //pthread_mutex_unlock(&mutex_guichange);
}

void button_send_click(gpointer data, gchar *str, gchar type)
{
    stringstream ss;
    gchar *text = (gchar*) gtk_entry_get_text(GTK_ENTRY(gres.text_entry));

    if (strlen(text) == 0) // TODO check max length
        return;

    ss << CFG_GET_STRING("nickname") << ": " << text << endl;
    if ((text[0] != '\\') || (strncmp(text, "\\send", 5) == 0))
    {
        add_message_to_chat(gres.chat_buffer, (gchar*) ss.str().c_str(), 'M');
    }
    else if (strncmp(text, "\\whisp", 6) == 0)
    {
        add_message_to_chat(gres.chat_buffer, (gchar*) ss.str().c_str(), 'W');
    }
    else
    {
        ss.str("");
        ss << text << endl;
        add_message_to_chat(gres.chat_buffer, (gchar*) ss.str().c_str(), 'e');
    }

    ss.str("");
    ss << text;
    if (!c_core->HandleSend((char*)ss.str().c_str()))
    {
        add_message_to_chat(gres.chat_buffer, (gchar*) "<local> send failed!\n", 'e');
        return;
    }

    gtk_entry_set_text (GTK_ENTRY(gres.text_entry), "");
}

void push_status_bar(const gchar *str)
{
    //pthread_mutex_lock(&mutex_guichange);
    guint id = gtk_statusbar_get_context_id(GTK_STATUSBAR(gres.status_bar), "info");
    gtk_statusbar_pop(GTK_STATUSBAR(gres.status_bar), id);
    gtk_statusbar_push(GTK_STATUSBAR(gres.status_bar), id, str);
    //pthread_mutex_unlock(&mutex_guichange);
}

void toolbar_reset_click(gpointer data)
{
    //pthread_mutex_lock(&mutex_guichange);
    GtkTextBuffer *text_view_buffer = GTK_TEXT_BUFFER(gres.chat_buffer);
    GtkTextIter textiter;
    gtk_text_buffer_get_end_iter(text_view_buffer, &textiter);
    gtk_text_buffer_set_text(text_view_buffer, "", 0);
    //pthread_mutex_unlock(&mutex_guichange);
}

void toolbar_connect_click(gpointer data, gchar *str, gchar type)
{
    GtkToolButton *toolbar_connect = GTK_TOOL_BUTTON(data);
    if (!c_core->IsConnected())
    {
        INFO("debug", "connect button pressed\n");
        if(!c_core->Connect())
            push_status_bar("Connection failed!");

        return;
    }

    if(c_core->IsConnected())
    {
        INFO("debug", "disconnect button pressed\n");
        if(!c_core->Disconnect())
            push_status_bar("Disconnection failed!?");
    }
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
    GtkToolItem *toolbar_refresh;
    GtkToolItem *toolbar_reset;
    GtkToolItem *toolbar_separator;
    GtkToolItem *toolbar_exit;

    /* paned */
    GtkWidget *paned_main;

    /* chat */
    GtkWidget *hbox_chat;
    GtkWidget *view_chat;
    GtkTextBuffer *view_chat_buffer;

    /* input della chat */
    GtkWidget *hbox_inputs;
    GtkWidget *vbox_inputs;
    GtkWidget *label_nick;
    GtkWidget *entry_command;
    GtkWidget *button_send;

    GtkWidget *dialog;

    /* inits */
    gdk_threads_init();
    gdk_threads_enter();
    gtk_init (&argc, &argv);
    pthread_mutex_init(&mutex_guichange, NULL);

    /* window */
    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_container_set_border_width(GTK_CONTAINER(window), 0);
    gtk_window_set_urgency_hint (GTK_WINDOW(window), TRUE);
    gtk_window_set_title (GTK_WINDOW (window), _PROJECTNAME);
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_window_set_resizable(GTK_WINDOW(window), TRUE);

    /* setting window icon */
    gtk_window_set_icon(GTK_WINDOW(window), create_pixbuf("data/psc.png"));

    gtk_widget_show(window);

    g_signal_connect(G_OBJECT(window), "delete_event", G_CALLBACK(on_destroy), NULL);
    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(on_destroy), NULL);

    /* vbox principale */
    vbox_main = gtk_box_new (GTK_ORIENTATION_VERTICAL, 1);
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
    open = gtk_image_menu_item_new_from_stock(GTK_STOCK_OPEN, NULL);
    sep = gtk_separator_menu_item_new();
    quit = gtk_image_menu_item_new_from_stock(GTK_STOCK_QUIT, accel_group);
    help = gtk_image_menu_item_new_from_stock(GTK_STOCK_HELP, NULL);
    about = gtk_image_menu_item_new_from_stock(GTK_STOCK_ABOUT, NULL);

    gtk_menu_item_set_submenu(GTK_MENU_ITEM(file), filemenu);
    gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), connect);
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
    gtk_toolbar_set_style(GTK_TOOLBAR(toolbar), GTK_TOOLBAR_BOTH);

    gtk_container_set_border_width(GTK_CONTAINER(toolbar), 2);

    toolbar_connect = gtk_tool_button_new_from_stock(GTK_STOCK_NETWORK);
    if (!c_core->IsConnected())
        gtk_tool_button_set_label(GTK_TOOL_BUTTON(toolbar_connect), "Connect");
    else
        gtk_tool_button_set_label(GTK_TOOL_BUTTON(toolbar_connect), "Disconnect");
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), toolbar_connect, -1);
    g_signal_connect(G_OBJECT(toolbar_connect), "clicked", G_CALLBACK(toolbar_connect_click), NULL);

    toolbar_refresh = gtk_tool_button_new_from_stock(GTK_STOCK_REFRESH);
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), toolbar_refresh, -1);
    //g_signal_connect(G_OBJECT(toolbar_refresh), "clicked", G_CALLBACK(toolbar_refresh_click), NULL);

    toolbar_reset = gtk_tool_button_new_from_stock(GTK_STOCK_CLEAR);
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), toolbar_reset, -1);
    g_signal_connect(G_OBJECT(toolbar_reset), "clicked", G_CALLBACK(toolbar_reset_click), NULL);

    toolbar_separator = gtk_separator_tool_item_new();
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), toolbar_separator, -1);

    toolbar_exit = gtk_tool_button_new_from_stock(GTK_STOCK_QUIT);
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), toolbar_exit, -1);
    g_signal_connect(G_OBJECT(toolbar_exit), "clicked", G_CALLBACK(gtk_main_quit), NULL);

    gtk_box_pack_start(GTK_BOX(vbox_main), toolbar, FALSE, FALSE, 0);

    /* Paned */
    paned_main = gtk_paned_new (GTK_ORIENTATION_HORIZONTAL);
    gtk_box_pack_start(GTK_BOX(vbox_main), paned_main, TRUE, TRUE, 0);

    gres.scrolledwindow_chat = gtk_scrolled_window_new (NULL, NULL);
    gtk_paned_pack1 (GTK_PANED(paned_main), gres.scrolledwindow_chat, true, true);

    gtk_container_set_border_width (GTK_CONTAINER (gres.scrolledwindow_chat), 2);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (gres.scrolledwindow_chat),
                                    GTK_POLICY_NEVER,
                                    GTK_POLICY_AUTOMATIC);

    view_chat = gtk_text_view_new();

    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(view_chat), GTK_WRAP_WORD_CHAR);
    gtk_text_view_set_editable(GTK_TEXT_VIEW(view_chat), false);
    gtk_container_add (GTK_CONTAINER (gres.scrolledwindow_chat), view_chat);
    view_chat_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(view_chat));
    gtk_text_buffer_create_tag(view_chat_buffer, "gap", "pixels_above_lines", 30, NULL);
    gtk_text_buffer_create_tag(view_chat_buffer, "lmarg", "left_margin", 5, NULL);
    gtk_text_buffer_create_tag(view_chat_buffer, "black_fg", "foreground", "#000000", NULL);
    gtk_text_buffer_create_tag(view_chat_buffer, "white_fg", "foreground", "#ffffff", NULL);
    gtk_text_buffer_create_tag(view_chat_buffer, "blue_fg", "foreground", "#3200ff", NULL);
    gtk_text_buffer_create_tag(view_chat_buffer, "green_bg", "background", "#55ff00", NULL);
    gtk_text_buffer_create_tag(view_chat_buffer, "blue_bg", "background", "#3200ff", NULL);
    gtk_text_buffer_create_tag(view_chat_buffer, "red_bg", "background", "#ff3200", NULL);
    gtk_text_buffer_create_tag(view_chat_buffer, "yellow_bg", "background", "#f7f732", NULL);
    gtk_text_buffer_create_tag(view_chat_buffer, "magenta_bg", "background", "#ff32ff", NULL);
    gtk_text_buffer_create_tag(view_chat_buffer, "italic", "style", PANGO_STYLE_ITALIC, NULL);
    gtk_text_buffer_create_tag(view_chat_buffer, "bold", "weight", PANGO_WEIGHT_BOLD, NULL);

    /*############################################## test messages 
    add_message_to_chat(view_chat_buffer, (gchar*) "<server> MOTD: welcome to project secure chat!\n", (gchar)'s');
    add_message_to_chat(view_chat_buffer, (gchar*) "<lazzalf> has joined the chat\n", (gchar)'j');
    add_message_to_chat(view_chat_buffer, (gchar*) "<lazzalf> salve buonuomo\n", (gchar)'m');
    add_message_to_chat(view_chat_buffer, (gchar*) "<paradox> ave!\n", (gchar)'m');
    add_message_to_chat(view_chat_buffer, (gchar*) "<furla> ...\n", (gchar)'m');
    add_message_to_chat(view_chat_buffer, (gchar*) "<paradox> problem solved!\n", (gchar)'w');
    add_message_to_chat(view_chat_buffer, (gchar*) "<lazzalf> wooot!\n", (gchar)'W');
    add_message_to_chat(view_chat_buffer, (gchar*) "<paradox> \"furla\" has been kicked out!\n", (gchar)'l');*/


    gres.scrolledwindow_user_list = gtk_scrolled_window_new (NULL, NULL);
    gtk_paned_pack2 (GTK_PANED(paned_main), gres.scrolledwindow_user_list, false, false);
    gtk_container_set_border_width (GTK_CONTAINER (gres.scrolledwindow_user_list), 2);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (gres.scrolledwindow_user_list),
                                    GTK_POLICY_NEVER,
                                    GTK_POLICY_AUTOMATIC);
    gtk_widget_show (gres.scrolledwindow_user_list);

    gres.model_user_list     = gtk_list_store_new(COLUMNS, G_TYPE_STRING, G_TYPE_STRING);
    gres.view_user_list      = gtk_tree_view_new_with_model (GTK_TREE_MODEL(gres.model_user_list));
    gres.selection_user_list = gtk_tree_view_get_selection(GTK_TREE_VIEW(gres.view_user_list));

    gtk_tree_selection_set_mode(gres.selection_user_list, GTK_SELECTION_SINGLE);
    gtk_tree_view_set_rules_hint(GTK_TREE_VIEW(gres.view_user_list), TRUE);

    gres.renderer_user_list = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(gres.view_user_list), /* vista */
                        -1,                  /* posizione della colonna */
                        "Name",  /* titolo della colonna */
                        gres.renderer_user_list,            /* cella inserita nella colonna */
                        "text",              /* attributo colonna */
                        COLUMN_STRING1,    /* colonna inserita  */
                        NULL);               /* fine ;-) */

    gres.renderer_user_list = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(gres.view_user_list),
                        -1,
                        "Level",
                        gres.renderer_user_list,
                        "text",
                        COLUMN_STRING2,
                        NULL);

    gtk_widget_show (gres.view_user_list);
    g_object_unref(gres.model_user_list);

    gtk_container_add (GTK_CONTAINER (gres.scrolledwindow_user_list), gres.view_user_list);
    gtk_container_set_border_width (GTK_CONTAINER (gres.view_user_list), 0);
    gtk_tree_view_set_rules_hint (GTK_TREE_VIEW (gres.view_user_list), TRUE);

    /* INPUTS */
    hbox_inputs = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_pack_start(GTK_BOX (vbox_main), hbox_inputs, FALSE, FALSE, 0);
    
    label_nick = gtk_label_new((gchar *) CFG_GET_STRING("nickname").c_str()); 
    gtk_misc_set_alignment(GTK_MISC(label_nick),0.0,0.5);
    gtk_box_pack_start(GTK_BOX (hbox_inputs), label_nick, FALSE, FALSE, 2 );

    entry_command = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX (hbox_inputs), entry_command, TRUE, TRUE, 5);
    
    button_send = gtk_button_new_with_label("Send");
    gtk_widget_set_size_request (GTK_WIDGET (button_send), 70, 30);
    gtk_box_pack_start(GTK_BOX (hbox_inputs), button_send, FALSE, FALSE, 0);

    gres.text_entry = entry_command;
    gres.chat_buffer = view_chat_buffer;
    g_signal_connect(G_OBJECT(entry_command), "activate", G_CALLBACK(button_send_click), NULL);
    g_signal_connect(G_OBJECT(button_send), "clicked", G_CALLBACK(button_send_click), NULL);

    /* status_bar */
    gres.status_bar = gtk_statusbar_new();
    gtk_box_pack_start(GTK_BOX (vbox_main), gres.status_bar, FALSE, FALSE, 0);

    g_object_set_data(G_OBJECT(gres.status_bar), "info", (gpointer)"1");
    g_object_set_data(G_OBJECT(gres.status_bar), "info", (gpointer) "2");
    g_object_set_data(G_OBJECT(gres.status_bar), "info", (gpointer) "3");

    g_object_set_data(G_OBJECT(gres.status_bar), "warning", (gpointer) "A");
    g_object_set_data(G_OBJECT(gres.status_bar), "warning", (gpointer) "B");
    g_object_set_data(G_OBJECT(gres.status_bar), "warning", (gpointer) "C");

    //guint id = gtk_statusbar_get_context_id(GTK_STATUSBAR(status_bar), "info");
    //gtk_statusbar_push(GTK_STATUSBAR(status_bar), id, "* uninitialized");

    /* end_widgets */
    gtk_widget_show_all(window);
    gres.toolbar_connect = toolbar_connect;

    /* default focus on command entry */
    gtk_widget_grab_focus (GTK_WIDGET(gres.text_entry));

    // TODO avviare il thread decentemente
    pthread_t tid;
    pthread_attr_t tattr;
    pthread_attr_init(&tattr);
    pthread_attr_setdetachstate(&tattr, PTHREAD_CREATE_DETACHED);
    pthread_create(&tid, &tattr, GuiThread, (void*)&gres);
    pthread_attr_destroy(&tattr);

    INFO ("debug", "* starting GUI (GTK+3)\n");
    gtk_main();
    gdk_threads_leave();
    pthread_mutex_destroy(&mutex_guichange);

    return;
}
