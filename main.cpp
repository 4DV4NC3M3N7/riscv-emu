#include <fstream>
#include <iostream>
#include "core/core.h"
#include "bus/bus.h"
#include "terminal/terminal.h"
#include <memory.h>
#include <limits>
#include <unistd.h>
#include "memory_map.h"
#include "display/display.h"
#include <SFML/Graphics.hpp>
#include <thread>
#include <unistd.h>
#include <chrono>
#include <atomic>
#include <future>
#include <signal.h>
#include <termios.h>
#include "debug.h"
#include "timer/timer.h"
#include "elf/elf.h"
#include "elf/elf_tables.h"
#include "ext_mem/ext_mem.h"
#include <argp.h>
#include <math.h>
#include "graphics/graphics.h"
#include <gtk-3.0/gtk/gtk.h>
#include <gtk-3.0/gtk/gtkbuilder.h>
#include <gdk/gdk.h>
#include "graphics/registers.h"
#include "graphics/console.h"
#include "graphics/log_console.h"


char getch(void)
{
    char buf = 0;
    struct termios old = {0};
    fflush(stdout);
    if(tcgetattr(0, &old) < 0)
        perror("tcsetattr()");
    old.c_lflag &= ~ICANON;
    old.c_lflag &= ~ECHO;
    old.c_cc[VMIN] = 1;
    old.c_cc[VTIME] = 0;
    if(tcsetattr(0, TCSANOW, &old) < 0)
        perror("tcsetattr ICANON");
    if(read(0, &buf, 1) < 0)
        perror("read()");
    old.c_lflag |= ICANON;
    old.c_lflag |= ECHO;
    if(tcsetattr(0, TCSADRAIN, &old) < 0)
        perror("tcsetattr ~ICANON");
    //printf("%c\n", buf);
    return buf;
 }

extern "C" 
{
    int riscv_initialise();
    int memory_initialise();
    void riscv_run();
    void riscv_reset();
    void riscv_dump();
    void memory_run();
    void riscv_finish();
    void memory_finish();
    int riscv_reg(int);
    int riscv_pc();
    void run_cycle();
    int get_cycle();   
}

using namespace std::chrono_literals;
std::atomic<bool> stop;

uint64_t nframes = 0;

void display_handler(DISPLAY* display)
{
    std::cout << "Creating Window\n";
    sf::RenderWindow window(sf::VideoMode(display->width+400, display->height+400), "Riscv Emu!");
    std::cout << "Finish Creating Window\n";
    sf::Texture* buffer_texture;
    buffer_texture = new sf::Texture();
    buffer_texture->create(display->width, display->height);
    uint8_t* second_frame_buff = (uint8_t*)malloc(sizeof(uint8_t) * display->height * display->width * 4);
    sf::Sprite sprite(*buffer_texture); // needed to draw the texture on screen
    std::cout << "Entering thread loop\n";
    //memset(display->frame_buffer, 0xff, (DISPLAY_HEIGHT*DISPLAY_WIDTH*4));
    bool pressed = true;
    sf::Vector2i position;
    sf::Vector2f mouse_drag_position;
    sf::Vector2f sprite_drag_position;
    sf::Vector2f sprite_scale;
    sf::VertexArray line(sf::Lines, 2);

    while(!stop.load(std::memory_order_relaxed))
    {
        while (window.isOpen())
        {
            //std::cout << "Running main window thread\n";
            sf::Event event;
            while (window.pollEvent(event))
            {
                //std::cout << "Pull Event >:(\n";
                if (event.type == sf::Event::Closed)
                {
                    std::cout << "Closing Window >:(\n";
                    window.close();
                    stop.store(true, std::memory_order_relaxed);
                }
                if (sf::Mouse::isButtonPressed(sf::Mouse::Left) | sf::Mouse::isButtonPressed(sf::Mouse::Right))
                {
                    if(pressed)
                    {
                        
                        if(sf::Vector2f((sf::Mouse::getPosition(window)-position).x, (sf::Mouse::getPosition(window)-position).y) != mouse_drag_position)
                        {
                            line[0].position = sf::Vector2f(position.x, position.y);
                            line[0].color  = sf::Color::Red;
                            mouse_drag_position = sf::Vector2f((sf::Mouse::getPosition(window)-position).x, (sf::Mouse::getPosition(window)-position).y)+(sf::Vector2f)position;
                            line[1].position = mouse_drag_position;
                            line[1].color = sf::Color::Red;

                            window.draw(line);
                            printf("Dragged x: %d y: %d mag: %f\n", sf::Mouse::getPosition(window)-position, sqrt(((sf::Mouse::getPosition(window).x - position.x)*(sf::Mouse::getPosition(window).x - position.x))+((sf::Mouse::getPosition(window).y - position.y)*(sf::Mouse::getPosition(window).y - position.y))));
                            
                            if(sf::Mouse::isButtonPressed(sf::Mouse::Right))
                            {
                                if(sprite.getGlobalBounds().contains(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y))
                                {
                                    sprite.setPosition(sprite_drag_position + mouse_drag_position-(sf::Vector2f)position);
                                }
                            }
                            if(sf::Mouse::isButtonPressed(sf::Mouse::Left))
                            {
                                sprite.setScale(sprite_scale+(mouse_drag_position-(sf::Vector2f)position)*0.01f);
                            }
                        }
                    }
                    else
                    {
                        pressed = true;
                        position = sf::Mouse::getPosition(window);
                        sprite_drag_position = sprite.getPosition();
                        sprite_scale = sprite.getScale();
                    }
                }
                else
                {
                    pressed = false;
                }

            }
            //printf("Pixel 1 %x, %x, %x, %x\n", *(buffer_texture), *(buffer_texture + 1), *(buffer_texture + 2), *(buffer_texture + 3));
            //window.clear();
            //sf::CircleShape shape(500.f);
            //shape.setFillColor(sf::Color::Green);
            //window.draw(shape);
            //Draw frame buffer, and update.
            memcpy(second_frame_buff, display->frame_buffer, 4*display->height*display->height);
            buffer_texture->update(second_frame_buff);
            //buffer_texture->update(window);
            //test_container.draw(&window);
            window.draw(sprite);
            window.display();
            //window.setFramerateLimit(1000);
            nframes++;
            window.clear();
            //std::this_thread::sleep_for(10ms);
        }
    }
    std::cout << "Exiting while loop\n\n\n\n\n";
    std::cout << "Closing Window\n\n\n\n\n";
    window.close();
    std::cout << "Fisnish Closing Window\n\n\n\n\n";
}

void terminal_input_handler(std::queue<char>* input_buffer)
{
    while(!stop.load(std::memory_order_relaxed))
    {
        char input = getch();
        if(input != 0)
        {
            if(input_buffer->size() < 0xff)
            {
                input_buffer->push(input);
                //std::cout << "Data Recieved: " << input << " Buffer Size: " << input_buffer->size() << std::endl;
            }
            else
            {
                //std::cout << "Error: Input Buffer Full" << std::endl;
            }
            
        }
        std::this_thread::sleep_for(10ms);
    }
}

uint64_t instruction_counter = 0;
#ifdef __INS_COUNT__
void sigalrm_handler(int sig)
{
    static uint64_t old;
    static uint64_t oldnf;
    std::cout << "Instructions per second: " << (instruction_counter - old) ;//<< std::endl;
    std::cout << "\tFPS: " << (nframes - oldnf) << std::endl;
    old = instruction_counter;
    oldnf = nframes;
    if(!stop.load(std::memory_order_relaxed)) alarm(1); 
}
#endif

struct termios old_terminal_state = {0};

void sig_int_handler(int sig)
{
    std::cout << "Restoring terminal defaults\n";
    if(tcsetattr(0, TCSADRAIN, &old_terminal_state) < 0)
        perror("tcsetattr ~ICANON");
    stop.store(true, std::memory_order_relaxed);
    exit(0);
}

void generate_steady_clock(TIMER* timer)
{
    using namespace std::chrono_literals;
    std::chrono::steady_clock::time_point last;
    while(!stop.load(std::memory_order_relaxed))
    {
        std::chrono::steady_clock::time_point elapsed = std::chrono::steady_clock::now();
        if(std::chrono::duration_cast<std::chrono::microseconds>(elapsed-last).count() >= 100)
        {
            timer->timer_handle();
            last = elapsed;
        }
    }
}


const char *argp_program_version = "riscv-emu";
const char *argp_program_bug_address = "<bitglitcher@github.com>";
static char doc[] = "RISCV Emulator.";
static char args_doc[] = "[FILENAME]...";

static struct argp_option options[] = { 
    { "input file", 'f', "FILE", 0, "Place input executable file as <file>."},
    { "graphical",  'g', 0, 0,      "graphical mode."},
    { "debug",      'd', 0, 0,      "Debug mode."},
    { "call trace", 'c', 0, 0,      "Print Call Trace."},
    { "stack trace",'s', 0, 0,      "Print Stack Trace."},
    { 0 } 
};

struct arguments {
    std::string input_file;
    std::string debug_file;
    bool debug;
    bool graphical;
    bool call_trace;
    bool stack_trace;
    bool exec_trace;
    bool device_access;
    bool fast_exec_trace;
    bool step;
};

static error_t parse_opt(int key, char *arg, struct argp_state *state) {
    struct arguments *argument = (arguments*)state->input;
    switch (key) {
        case 'f': argument->input_file = arg; break;
        case 'd': argument->debug_file = arg; break;
        case 'g': argument->graphical = true; break;
        case 'c': argument->call_trace = true; break;
        case 's': argument->stack_trace = true; break;
        case 'e': argument->exec_trace = true; break;
        case 'D': argument->device_access = true; break;
        case 'F': argument->fast_exec_trace = true; break;
        //case 'r': argument->raw = true; break;
        case ARGP_KEY_ARG: argument->input_file = arg; return 0;
        default: return ARGP_ERR_UNKNOWN;
    }   
    return 0;
}

//This function will contain the main core thread. This will call the gui_update function,
//if fast mode is not enabled
std::atomic<bool> fast_mode;

//Different tabs that must be created at runtime
registers* register_tab;
console* console_tab;
log_console* log_tab;

BUS bus;
MEMORY main_memory(MEMORY_H_ADDR, MEMORY_L_ADDR, MEMORY_H_ADDR);
DISPLAY display(DISPLAY_L_ADDR, DISPLAY_H_ADDR, DISPLAY_HEIGHT, DISPLAY_WIDTH);
TERMINAL terminal(TERMINAL_ADDR_L, TERMINAL_ADDR_H);
TIMER timer(TIMER_L_ADDR, TIMER_H_ADDR);
EXT_MEM ext_mem(EXT_L_ADDR, EXT_H_ADDR);
elf_reader* reader;
CORE* core;

int core_setup(std::string input_file)
{
    bus.add(&main_memory);
    bus.add(&terminal);
    bus.add(&display);
    bus.add(&timer);
    bus.add(&ext_mem);
    std::ifstream binary;
    binary.open(input_file, std::ifstream::binary);

    if(!binary.is_open())
    {
        printf("Error: Couldn't open file %s\n", input_file.c_str());
        return -1;
    }
    else
    {
        binary.ignore( std::numeric_limits<std::streamsize>::max() );
        std::streamsize size = binary.gcount();
        binary.clear();   //  Since ignore will have set eof.
        binary.seekg( 0, std::ios_base::beg );

        //Allocate buffer
        char* buffer = (char*)malloc(size * sizeof(size));
        //Initialize buffer to 0
        memset(buffer, 0x00, size);

        //Read data into buffer
        binary.read(buffer, size);

        //Create new elf reader
        reader = new elf_reader(buffer, size);
        
        //load buffer sections into memory
        std::cout << "Loading ROM..." << std::endl;

        //Read program headers and initialize into memory
        for(int i = 0;i < reader->elf32_ehdr.e_phnum;i++)
        {
            if(reader->elf32_phdr[i].p_type == PT_LOAD)
            {
                //In this case it would be the physical address into memory, because the emulator doesnt support virtual memory yet
                if(main_memory.initalize(reader->elf32_phdr[i].p_paddr, reader->get_ph_buffer(i), reader->elf32_phdr[i].p_filesz) == false)
                {
                    std::cout << "Couln't initialize Section at Address: " << std::hex << reader->elf32_phdr[i].p_paddr << "\n";
                }
            }
        }
        core = new CORE(reader->elf32_ehdr.e_entry, &bus, &timer);
        std::vector<symbol32_t>* dumped = reader->dump_symbols();
        core->attach_debug_symbols(dumped);
        stop.store(false, std::memory_order_relaxed);
    }
    return 0;
}

void core_delete()
{
    if(reader)
    {
       delete reader; 
    }
}

gboolean update_gui(gpointer data)
{
    int regs[32];
    for(int i = 0;i < 32;i++) regs[i] = core->get_reg(i);
    register_tab->update((uint32_t*)regs, core->get_pc(), 0);
    return true;
}

gboolean update_terminal_gui(gpointer data)
{
    //Update terminal
    while(terminal.lock.try_lock() != -1)
    {
        //Update gui   
        console_tab->update(terminal.input_buffer, terminal.output_buffer);
        terminal.lock.unlock();
        return true;
    }
}
void core_thread()
{

    std::this_thread::sleep_for(2s);
    while(core->running && (!stop.load(std::memory_order_relaxed)))
    {
        core->execute();
        instruction_counter++;
        //gtk_main_iteration();
    }
    stop.store(true, std::memory_order_relaxed);
    //update_display.wait();
    //terminal_handler.wait();
    core->print_tracing();
    printf("Execution terminated: %d instructions executed\nExiting Main\n", instruction_counter); 
}

static void
activate (GtkApplication *app,
          gpointer        user_data)
{



  GtkWidget *window;
  GtkWidget *button;
  GtkWidget *button_box;
  GtkBuilder      *builder; 

    
    builder = gtk_builder_new();
    gtk_builder_add_from_file (builder, "layout.glade", NULL);

    window = GTK_WIDGET(gtk_builder_get_object(builder, "window_main"));
    gtk_builder_connect_signals(builder, NULL);
    gtk_application_add_window(GTK_APPLICATION(app), GTK_WINDOW(window));


    GtkWidget* main_context = GTK_WIDGET(gtk_builder_get_object(builder, "main_context"));
    GtkWidget* context1 = GTK_WIDGET(gtk_builder_get_object(builder, "context1"));
    GtkWidget* context2 = GTK_WIDGET(gtk_builder_get_object(builder, "context2"));
    
    register_tab = new registers;
    console_tab = new console;
    log_tab = new log_console;

    //Attack tabs
    register_tab->attach_to_notebook(GTK_NOTEBOOK(main_context));
    console_tab->attach_to_notebook(GTK_NOTEBOOK(context1));
    log_tab->attack_to_notebook(GTK_NOTEBOOK(context2));
    //g_object_unref(builder);

    //gtk_widget_show(window);                
    //gtk_main();
  //window = gtk_application_window_new (app);
  //gtk_window_set_title (GTK_WINDOW (window), "Window");
  //gtk_window_fullscreen(GTK_WINDOW (window));

  //Set window to full screen

  //button_box = gtk_button_box_new (GTK_ORIENTATION_HORIZONTAL);
  //gtk_container_add (GTK_CONTAINER (window), button_box);

  //button = gtk_button_new_with_label ("Hello World");
  ///g_signal_connect (button, "clicked", G_CALLBACK (print_hello), NULL);
  //g_signal_connect_swapped (button, "clicked", G_CALLBACK (gtk_widget_destroy), window);
  //gtk_container_add (GTK_CONTAINER (button_box), button);
    
    //Initialize Emulator Core
    gdk_threads_add_idle(update_gui, nullptr);
    gdk_threads_add_timeout(500, update_terminal_gui, nullptr);
    gtk_widget_show_all (window);
}

static struct argp argp = { options, parse_opt, args_doc, doc, 0, 0, 0 };

void setup_cmd_options(GtkApplication *app)
{
    g_application_add_main_option (G_APPLICATION (app),
                           "tracing",
                           't',
                           G_OPTION_FLAG_NO_ARG,
                           G_OPTION_ARG_NONE,
                           "Enable CPU tracing",
                           nullptr);
    g_application_add_main_option (G_APPLICATION (app),
                           "no-graphics",
                           'g',
                           G_OPTION_FLAG_NO_ARG,
                           G_OPTION_ARG_NONE,
                           "Disable graphics",
                           nullptr);
    g_application_add_main_option (G_APPLICATION (app),
                           "stepping",
                           's',
                           G_OPTION_FLAG_NO_ARG,
                           G_OPTION_ARG_NONE,
                           "Instruction stepping",
                           nullptr);
    g_application_add_main_option (G_APPLICATION (app),
                           "log",
                           'l',
                           G_OPTION_FLAG_FILENAME,
                           G_OPTION_ARG_FILENAME,
                           "Log file",
                           "[filename]");
    g_application_add_main_option (G_APPLICATION (app),
                           "log-options",
                           'o',
                           G_OPTION_FLAG_OPTIONAL_ARG,
                           G_OPTION_ARG_STRING_ARRAY,
                           "Log options",
                           "int,exec,call,csr\n");
}


int main(int argc, char *argv[])
{
    //So the emulator waits until the GUI is ready
    stop = true;
    
    GtkApplication *app;
    int status;
    
    //Set graphical mode for the terminal
    terminal.graphical = true;
    
    app = gtk_application_new ("git.bitglitcher.riscv_emu", G_APPLICATION_FLAGS_NONE);
    g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
    setup_cmd_options(app);
    //std::future<void> emu_thread = std::async(fake_emulator);
    //std::future<void> emu_thread = std::async(fake_emulator);
    std::future<void> clock = std::async(&generate_steady_clock, &timer);
    std::future<void> update_display = std::async(&display_handler, &display); 
    std::future<void> terminal_handler = std::async(&terminal_input_handler, &terminal.input_buffer); 
    core_setup("test_linked.o");
    std::future<void> core_handler = std::async(&core_thread); 
    status = g_application_run (G_APPLICATION (app), argc, argv);
    g_object_unref (app);
    return 0;


    
//
    //    return status;
    //struct arguments arguments;
    //arguments.graphical = false;
    //arguments.call_trace = false;
    //arguments.stack_trace = false;
    //argp_parse(&argp, argc, argv, 0, 0, &arguments);


    std::cout << "Starting Main\n\n\n\n\n";
    
    //Get terminal current state before doing anything weird
    if(tcgetattr(0, &old_terminal_state) < 0)
        perror("tcsetattr()");
//
    #ifdef __INS_COUNT__
        signal(SIGALRM, &sigalrm_handler);  // set a signal handler
        signal(SIGINT, &sig_int_handler);  // set a signal handler for interrupt request
        alarm(1);  // set an alarm for 10 seconds from now
    #endif
    std::cout << "Restoring terminal defaults\n";
    if(tcsetattr(0, TCSADRAIN, &old_terminal_state) < 0)
        perror("tcsetattr ~ICANON");

    return 0;
}