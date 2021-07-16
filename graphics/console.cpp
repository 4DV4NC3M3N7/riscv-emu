#include <queue>
#include <string>
#include <gtk-3.0/gtk/gtk.h>
#include <gtk-3.0/gtk/gtkbuilder.h>
#include <gdk/gdk.h>
#include "console.h"
#include <iostream>



console::console(/* args */)
{
    tab_label = gtk_label_new("Console/UART");
    tagtable = gtk_text_tag_table_new();
    buffer = gtk_text_buffer_new(tagtable);
    //Empty the buffer
    gtk_text_buffer_set_text(buffer, "", 0);
    //Create text view
    console_txt_vw = gtk_text_view_new_with_buffer(buffer);
    gtk_text_view_set_editable(GTK_TEXT_VIEW(console_txt_vw), true);
    //Create scrolled window
    scrolled_window = gtk_scrolled_window_new (NULL, NULL);
    gtk_container_add(GTK_CONTAINER(scrolled_window), console_txt_vw);
}

console::~console()
{
}

void console::update(std::queue<char>& input, std::queue<char>& output)
{
    //Push new chars into input
    for(int i = 0;i < input_chars.size();i++)
    {
        input.push(input_chars.front());
        input_chars.pop();
    }

    //Push output characters into output buffer
    for(int i = 0;i < output.size();i++)
    {   
        output_chars.push_back(output.front());
        gtk_text_buffer_insert_at_cursor (buffer, std::string(1, output.front()).c_str(), 1);
        output.pop();
    }
    //gtk_text_buffer_set_text(buffer, output_chars.c_str(), output_chars.size());
}

void console::signal_backspace(GtkTextView *text_view, gpointer user_data)
{
    //Push ASCII backspace
    input_chars.push(0x8);    
}

void console::attach_to_notebook(GtkNotebook* notebook)
{
    gtk_notebook_append_page(notebook, scrolled_window, tab_label);
}
void console::deattach_to_notebook(GtkNotebook* notebook)
{
    gtk_notebook_detach_tab(notebook, scrolled_window);
}