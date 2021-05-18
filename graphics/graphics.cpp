//By: Benjamin Herrera Navarro
//5/10/2021
//12:34

#include "graphics.h"

using namespace graphics;

window::window(uint16_t height, uint16_t width, bool resizeable)
{

}

window::~window()
{

}

//Define size of container manually
container::container(STICK_TO stick_to, int height, int width, ARRANGEMENT_E arrangement)
{
    this->stick_to = stick_to;
    this->height = height; //Not in use if arrangement is FULL_SCREEN
    this->width = width; //Not in use if arrangement is FULL_SCREEN
    this->arrangement = arrangement;
}

//Define size of container with window dimenstions
container::container(STICK_TO stick_to, ARRANGEMENT_E arrangement, sf::RenderWindow* window)
{
    this->stick_to = stick_to;
    this->height = window->getSize().y;
    this->width = window->getSize().x;
    this->arrangement = arrangement;
}


container::~container()
{

}

void container::draw(sf::RenderWindow* render_window)
{
    switch (stick_to)
    {
        case NONE: {

        } break;
        case FULL_SCREEN: {
            //Get screem dimensions
            sf::Vector2u dimensions = render_window->getSize();
            dimensions.x -= 200;
            sf::RectangleShape rect((sf::Vector2f)dimensions);
            rect.setOrigin(position);
            rect.setFillColor(sf::Color::Blue);
            //render_window->draw(rect);
        } break;
        case LEFT:

        break;
        case UPPER_LEFT_CORNER:

        break;
        case UPPER_RIGHT_CORNER:

        break;
        case DOWN_RIGHT_CORNER:

        break;
        case DOWN_LEFT_CORNER:

        break;
        case RIGHT:

        break;
        case UP:

        break;
        case DOWN:

        break;
    
    default:
        break;
    }
    
    //Draw pannels red
    int offset = 0;
    for(std::vector<pannel_t>::iterator it = pannels.begin();it < pannels.end();it++)
    {
        printf("Value of Arrangement %d\n", arrangement);
        if(arrangement == HORIZONTAL)
        {
            std::cout << "its full screen\n";
            sf::Vector2u dimensions;
            //Get screem dimension
            if(fit_pannels)
            {
                dimensions = sf::Vector2u(width/pannels.size() - (pannel_border*2), height - (pannel_border*2));            
                sf::RectangleShape rect((sf::Vector2f)dimensions);
                rect.setPosition(position.x+pannel_border + offset, position.y + pannel_border);
                rect.setFillColor(sf::Color::Green);
                render_window->draw(rect);
                offset += width/pannels.size(); 
            }
            else
            {
                dimensions = sf::Vector2u(width - (pannel_border*2), it->size - pannel_border);            
                sf::RectangleShape rect((sf::Vector2f)dimensions);
                rect.setPosition(position.x+pannel_border, position.y + offset);
                rect.setFillColor(sf::Color::Green);
                render_window->draw(rect);
                offset += it->size;
            }
        }
        else if(arrangement == VERTICAL) //Stack element in a vertical manner. From top to bottom
        {
            sf::Vector2u dimensions;
            //Get screem dimension
            if(fit_pannels)
            {
                dimensions = sf::Vector2u(width - (pannel_border*2), height/pannels.size());            
                sf::RectangleShape rect((sf::Vector2f)dimensions);
                rect.setPosition(position.x+pannel_border, position.y + offset);
                rect.setFillColor(sf::Color::Green);
                render_window->draw(rect);
                offset += height/pannels.size() + pannel_border;
            }
            else
            {
                dimensions = sf::Vector2u(width - (pannel_border*2), it->size - pannel_border);            
                sf::RectangleShape rect((sf::Vector2f)dimensions);
                rect.setPosition(position.x+pannel_border, position.y + offset);
                rect.setFillColor(sf::Color::Green);
                render_window->draw(rect);
                offset += it->size;
            }
        }
        else
        {
            std::cout << "Error: arrangement code %d not recognized\n", arrangement;
        }
    }
}
