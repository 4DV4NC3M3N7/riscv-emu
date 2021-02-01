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
#include "ext_mem/ext_mem.h"

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
    sf::RenderWindow window(sf::VideoMode(display->width, display->height), "Riscv Emu!");
    std::cout << "Finish Creating Window\n";
    sf::Texture* buffer_texture;
    buffer_texture = new sf::Texture();
    buffer_texture->create(display->width, display->height);
    uint8_t* second_frame_buff = (uint8_t*)malloc(sizeof(uint8_t) * display->height * display->width * 4);
    sf::Sprite sprite(*buffer_texture); // needed to draw the texture on screen
    std::cout << "Entering thread loop\n";
    //memset(display->frame_buffer, 0xff, (DISPLAY_HEIGHT*DISPLAY_WIDTH*4));
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

int main()
{

    std::cout << "Starting Main\n\n\n\n\n";
    
    //Get terminal current state before doing anything weird
    if(tcgetattr(0, &old_terminal_state) < 0)
        perror("tcsetattr()");

    std::ifstream binary;
    binary.open("ROM.bin", std::ifstream::binary);


    binary.ignore( std::numeric_limits<std::streamsize>::max() );
    std::streamsize size = binary.gcount();
    binary.clear();   //  Since ignore will have set eof.
    binary.seekg( 0, std::ios_base::beg );

    char* buffer = (char*)malloc(size * sizeof(size));

    memset(buffer, 0x00, size);

    binary.read(buffer, size);
    
//
    BUS bus;
    MEMORY main_memory(MEMORY_H_ADDR, MEMORY_L_ADDR, MEMORY_H_ADDR);
    DISPLAY display(DISPLAY_L_ADDR, DISPLAY_H_ADDR, DISPLAY_HEIGHT, DISPLAY_WIDTH);
    TERMINAL terminal(TERMINAL_ADDR_L, TERMINAL_ADDR_H);
    TIMER timer(TIMER_L_ADDR, TIMER_H_ADDR);
    EXT_MEM ext_mem(EXT_L_ADDR, EXT_H_ADDR);
    bus.add(&main_memory);
    bus.add(&terminal);
    bus.add(&display);
    bus.add(&timer);
    bus.add(&ext_mem);
    std::cout << "Loading ROM..." << std::endl;
    for(int i = 0; i < size;i++)
    {
        //std::cout << i << ": " << std::hex << (int)buffer[i] << std::endl;
        main_memory.write(buffer[i], i);
        printf("\r%d of %d", i, size);
    }
    std::cout << "\nROM Loaded" << std::endl;  
    
    CORE core(0x00, &bus, &timer);
    stop.store(false, std::memory_order_relaxed);

    std::future<void> clock = std::async(&generate_steady_clock, &timer);
    std::future<void> update_display = std::async(&display_handler, &display); 
    std::future<void> terminal_handler = std::async(&terminal_input_handler, &terminal.input_buffer); 

    #ifdef __INS_COUNT__
        signal(SIGALRM, &sigalrm_handler);  // set a signal handler
        signal(SIGINT, &sig_int_handler);  // set a signal handler for interrupt request
        alarm(1);  // set an alarm for 10 seconds from now
    #endif

    while(core.running && (!stop.load(std::memory_order_relaxed)))
    {
        core.execute();
        instruction_counter++;
    }
    stop.store(true, std::memory_order_relaxed);
    update_display.wait();
    terminal_handler.wait();
    core.print_tracing();
    printf("Execution terminated: %d instructions executed\nExiting Main\n", instruction_counter); 
    std::cout << "Restoring terminal defaults\n";
    if(tcsetattr(0, TCSADRAIN, &old_terminal_state) < 0)
        perror("tcsetattr ~ICANON");

    return 0;
}