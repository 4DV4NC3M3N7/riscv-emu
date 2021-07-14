
#include "log.h"

log::log()
{
    tab_label = gtk_label_new("log");
    tagtable = gtk_text_tag_table_new();
    buffer = gtk_text_buffer_new(tagtable);
    text_view = gtk_text_view_new_with_buffer(buffer);
}
log::~log()
{

}
//Display warning message on log tab
void log::warning(std::string message)
{
    //Push in into the message history, so there is a log of all events
    msg_buffer.push_back(message_t(message, WARNING_MSG));
    //Update into the GtkTextBuffer
}
//Display info message on log tab
void log::info(std::string message)
{
    //Push in into the message history, so there is a log of all events
    msg_buffer.push_back(message_t(message, INFO_MSG));
    //Update into the GtkTextBuffer
}
//Display error message on log tab
void log::error(std::string message)
{
    //Push in into the message history, so there is a log of all events
    msg_buffer.push_back(message_t(message, ERROR_MSG));
    //Update into the GtkTextBuffer
}
//Save logged messages
int log::save(std::string file)
{
    std::fstream log;
    log.open("log_file.log", std::fstream::in | std::fstream::out | std::fstream::binary);
    if(log.is_open())
    {
        log.seekg(log.beg);
        for(auto entry : msg_buffer)
        {
            switch (entry.type)
            {
                case INFO_MSG: { log << "Info: " << entry.message << "\n"; }; break;
                case WARNING_MSG: { log << "Warning: " << entry.message << "\n"; }; break;
                case ERROR_MSG: { log << "Error: " << entry.message << "\n"; }; break;
                default: { log << "(Unrecognized Type): " << entry.message << "\n"; }; break;
            }
        }
        log.close();
    }
    else
    {
        printf("Error: Couldn't save log file %s\n", file.c_str());
        return -1;
    }
}
//Attach tab to notebook
void log::attack_to_notebook(GtkNotebook* notebook)
{
    gtk_notebook_append_page(notebook, text_view, tab_label);
}
//Deattach tab to notebook
void log::deattack_to_notebook(GtkNotebook* notebook)
{
    gtk_notebook_detach_tab(notebook, text_view);
}