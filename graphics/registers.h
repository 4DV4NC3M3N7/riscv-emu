#pragma once

#include <gtk-3.0/gtk/gtk.h>
#include <gtk-3.0/gtk/gtkbuilder.h>
#include <vector>
#include <string>
#include <iostream>

typedef struct {
    GtkLabel* label;
    GtkEntry* entry;
} regs_t;


//This is the tab in charge of displaying registers
class registers
{
    public:
        registers(/* args */);
        ~registers();
        GtkWidget* get_widget();
        void attach_to_notebook(GtkNotebook* notebook);
        void deattach_to_notebook(GtkNotebook* notebook);
    private:
        GtkWidget* grid;
        GtkWidget* label;
        std::vector<regs_t*> register_fields;
};