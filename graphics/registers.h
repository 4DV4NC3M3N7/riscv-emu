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
        //This function should only be called by the main app thread, and not the emulator thread.
        void update(uint32_t* regs, __uint32_t PC, __uint32_t IR);
    private:
        GtkWidget* grid;
        GtkWidget* label;
        std::vector<regs_t*> register_fields;
};