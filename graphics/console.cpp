#include <queue>
#include <string>
#include <gtk-3.0/gtk/gtk.h>
#include <gtk-3.0/gtk/gtkbuilder.h>
#include <gdk/gdk.h>
#include "console.h"



console::console(/* args */)
{
    tab_label = gtk_label_new("Console/UART");
    tagtable = gtk_text_tag_table_new();
    buffer = gtk_text_buffer_new(tagtable);
    //Empty the buffer
    gtk_text_buffer_set_text(buffer, "", 0);
    //Create text view
    console_txt_vw = gtk_text_view_new_with_buffer(buffer);
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
        output_chars.push(output.front());
        output.pop();
    }
}

void console::signal_backspace(GtkTextView *text_view, gpointer user_data)
{
    //Push ASCII backspace
    input_chars.push(0x8);    
}

void console::attach_to_notebook(GtkNotebook* notebook)
{
    gtk_notebook_append_page(notebook, console_txt_vw, tab_label);
}
void console::deattach_to_notebook(GtkNotebook* notebook)
{
    gtk_notebook_detach_tab(notebook, console_txt_vw);
}