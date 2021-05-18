//By: Benjamin Herrera Navarro
//5/10/2021
//12:34 AM

#ifndef __GRAPHICS__H__
#define __GRAPHICS__H__

#include <stdint.h>
#include <stdint.h>
#include <iostream>
#include <functional>
#include <SFML/Graphics.hpp>

namespace graphics
{
    class graphics
    {
        public:
            graphics();
            ~graphics();
        private:
    };

    class window
    {
        public:
            window(uint16_t height, uint16_t width, bool resizeable);
            void setTitle(std::string title);
            std::string getTitle();
            sf::Uint8 getFrameBuffer();
            virtual ~window();

            //
            void draw(sf::RenderWindow* renderWindow);
        private:
            /* data */
            bool resizeable;
            bool titlebar;
            bool titlebar_controls;

            uint16_t height;
            uint16_t width;
            std::string title;
            std::string title_size;
            sf::Uint8 frame_buffer;
    };

    class button
    {
        public:
            button();
            ~button();
            std::string text;
            bool resizable;
            void draw(sf::RenderWindow* renderWindow);
    };

    class textbox
    {
        public:
            textbox();
            ~textbox();
            std::string text;
            bool resizable = false;
            bool scrollable = false;
            bool multiline = false;
    };

    enum toolbar_tile_type
    {
        DROPDOWN_MENU,
        TOOLBAR_TILE
    };

    typedef struct __toolbar_tile
    {
        //These need no position given that a toobar function will draw them dinamically
        std::string title;
        int width;
        std::function<void()> call_back(); //Function to call when clicked
        void draw(sf::RenderWindow* renderwindow);
        sf::Texture* texture;
        sf::Sprite* sprite;
        std::vector<__toolbar_tile> entries;
    } toolbar_tile_t;
    
    class toolbar
    {
        public:
            toolbar();
            ~toolbar();
        private:
            std::function<void()> call_back();
    };

    enum STICK_TO 
    {
        NONE,
        FULL_SCREEN,
        LEFT,
        UPPER_LEFT_CORNER,
        UPPER_RIGHT_CORNER,
        DOWN_RIGHT_CORNER,
        DOWN_LEFT_CORNER,
        RIGHT,
        UP,
        DOWN
    };

    enum ARRANGEMENT_E
    {
        VERTICAL,
        HORIZONTAL
    };

    class container;

    typedef struct _panel_s
    {
        STICK_TO stick_to;
        ARRANGEMENT_E arrangement; //Defines how elements are going to be stack to each other
        std::vector<_panel_s> _panel_s; //These are childs of this panel
        unsigned int size;
        typedef struct
        {
            bool adjacent; //Set to true and it will stick to the panel adjacent to it
            bool stick_to; //Tell how to stick to caller
        } adjacent;
        std::vector<window*> windows; 
        std::vector<container*> containers;
        typedef struct
        {
            //It will strech according to the defenition of arrangement of the parent container or pannel
            bool fit_parent;
            int height;
            int lenght;
            bool center;
        } sizenlook;
    } pannel_t;

    class container
    {
        public:
            //When lenght of 0, lenght is discarded
            //Define size of container with window dimenstions
            container(STICK_TO stick_to, ARRANGEMENT_E arrangement, sf::RenderWindow* window);
            //Define size of container manually
            container(STICK_TO stick_to, int height, int width, ARRANGEMENT_E arrangement);
            container(STICK_TO stick_to, int height);
            ~container();
            int pannel_border = 10;
            bool fit_pannels = false;
            void draw(sf::RenderWindow* render_window); //Pannel to draw
            sf::Vector2f position;
            std::vector<pannel_t> pannels;
        private:
            int width;
            int height;
            ARRANGEMENT_E arrangement; //Defines how childs are going to be stack to each other
            STICK_TO stick_to;
    };

}

#endif