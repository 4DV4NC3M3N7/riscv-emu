#pragma once
#include <queue>
#include <string>
#include <gtk-3.0/gtk/gtk.h>
#include <gtk-3.0/gtk/gtkbuilder.h>
#include <gdk/gdk.h>

//This class is for the tabs on the notebook and windows
class console
{
    public:
        console(/* args */);
        ~console();

        //This function will update the input and output buffer from the terminal
        //Can be called from the emulator or the main thread. Once certain theshhold.
        void update(std::queue<char>& input, std::queue<char>& output);
        //This function should be called to add a backspace to the input buffer
        void signal_backspace (GtkTextView *text_view, gpointer user_data);
        //Add or remove tab from notebook
        void attach_to_notebook(GtkNotebook* notebook);
        void deattach_to_notebook(GtkNotebook* notebook);
    private:
        //Input from the user
        std::queue<char> input_chars;
        //Output from the emulator
        std::queue<char> output_chars;

        GtkWidget* tab_label;
        GtkTextTagTable* tagtable;// = gtk_text_tag_table_new();
        GtkTextBuffer* buffer;// = gtk_text_buffer_new(tagtable);(
        GtkWidget* console_txt_vw;// = gtk_text_view_new_with_buffer(buffer);
        /* data */
};