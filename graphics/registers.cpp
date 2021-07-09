#include <gtk-3.0/gtk/gtk.h>
#include <gtk-3.0/gtk/gtkbuilder.h>
#include "registers.h"

registers::registers(/* args */)
{
    grid = gtk_grid_new();
    label = gtk_label_new("Registers");
    //Generate Register names
    std::vector<std::string> register_names;
    for(int i = 0;i < 32;i++)
    {
        register_names.push_back("R" + std::to_string(i));
    }
    //Add at the end PC and IR
    register_names.push_back("PC");
    register_names.push_back("IR");

    //Add registers to the vector
    for(int i = 0;i < 32;i++)
    {
        regs_t* register_field = new regs_t;
        register_field->entry = GTK_ENTRY(gtk_entry_new());
        gtk_entry_set_max_width_chars(register_field->entry, strlen("0x00000000"));
        gtk_entry_set_width_chars(register_field->entry, strlen("0x00000000"));
        register_field->label = GTK_LABEL(gtk_label_new(register_names[i].c_str()));
        register_fields.push_back(register_field);
    }
    //Add PC and IR
    for(int i = 32;i < 34;i++)
    {
        regs_t* register_field = new regs_t;
        register_field->entry = GTK_ENTRY(gtk_entry_new());
        gtk_entry_set_max_width_chars(register_field->entry, strlen("0x00000000"));
        gtk_entry_set_width_chars(register_field->entry, strlen("0x00000000"));
        register_field->label = GTK_LABEL(gtk_label_new(register_names[i].c_str()));
        register_fields.push_back(register_field);
    }

    //Arrange registers into the grid
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
    for(int i = 0; i < 8;i++)
    {
        gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(register_fields[i]->label), 0, i, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(register_fields[i]->entry), 1, i, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(register_fields[i+8]->label), 2, i, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(register_fields[i+8]->entry), 3, i, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(register_fields[i+16]->label), 4, i, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(register_fields[i+16]->entry), 5, i, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(register_fields[i+24]->label), 6, i, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(register_fields[i+24]->entry), 7, i, 1, 1);
    }
    gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(register_fields[32]->label), 0, 8, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(register_fields[32]->entry), 1, 8, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(register_fields[33]->label), 2, 8, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(register_fields[33]->entry), 3, 8, 1, 1);
}


void registers::attach_to_notebook(GtkNotebook* notebook)
{
    gtk_notebook_append_page(notebook, grid, label);
}

void registers::deattach_to_notebook(GtkNotebook* notebook)
{
    gtk_notebook_detach_tab(notebook, grid);
}

registers::~registers()
{
    for(auto it : register_fields)
    {
        if(it)
        {
            if(it->entry)
            {
                delete it->entry;
            }
            if(it->label)
            {
                delete it->label;
            }
            delete it;
        }
    }
}

GtkWidget* registers::get_widget()
{
    return GTK_WIDGET(grid);
}