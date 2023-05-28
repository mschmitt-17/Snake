#include <iostream>
#include <random>
#include <vector>
#include <tuple>
#include <ncurses/ncurses.h>
using namespace std;
#define UP tuple<int, int>(-1, 0)   //
#define DOWN tuple<int, int>(1, 0)  //--->define directions for clearer direction changes later
#define LEFT tuple<int, int>(0, -1) //
#define RIGHT tuple<int, int>(0, 1) //

class Snake {
    public:
        Snake(tuple<int, int> head, tuple<int, int> direction_) { //initialize private attributes of snake
            body.push_back(head);
            direction = direction_;
            new_links = 0;
            dead = 0;
        }
        void take_step(tuple<int, int> position) { //make head node new position, delete tail node of snake 
            tuple<int, int> next_link = body[0]; //save old head position, this will be the position for our next link in the snake's body
            tuple<int, int> old_coord; //variable for old coordinate of link, as this will become the new position for the next link
            body[0] = position; //set head of snake to new position passed to function
            for (int i = 1; i < body.size(); i++) { //loop through all links of snake after head, setting new position to previous position of next link
                old_coord = body[i];
                body[i] = next_link;
                next_link = old_coord;
            }
            if (new_links) { //if snake has eaten apple and we want to add new links, add one new link to body and decrement number of links we must add
                body.push_back(next_link);
                new_links--;
            }
        }
        void add_links(int additions) { //add new links to snake's body from outside of class
            new_links += additions;
        }
        void set_direction(tuple<int, int> direction_) { //function to change direction of snake
            direction = direction_;
        }
        tuple<int, int> getDirection() {
            return direction;
        }
        void set_dead(int is_dead) {
            dead = is_dead;
        }
        int getDead() {
            return dead;
        }
        vector<tuple<int, int>> getBody() {
            return body;
        }
        tuple<int, int> getHead() {
            return body[0];
        }
    private:
        vector<tuple<int, int>> body; //vector that holds coordinates of snake's body
        tuple<int, int> direction; //tuple representing direction snake is currently traveling
        int new_links; //number representing new links that must be added to snake following apple being eaten
        int dead; //number representing whether or not snake is dead (for if snake overlaps itself)
};
class Apple{
    public:
        Apple(tuple<int, int> coord_) {
            coord = coord_;
        }
        void set_coords(tuple<int, int> coord_) {
            coord = coord_;
        }
        tuple<int, int> get_coord() {
            return coord;
        }
    private:
        tuple<int, int> coord; //position of apple on game board
};
class Game{
    public:
        Game(int height, int width) {
            height_ = height - 3;   //--->subtract 2 from width so we can add borders to our game board, 3 from height so we can add score to bottom of screen and borders
            width_ = width - 2;     //
            length = 1; //snake starts off at length 1
            board = new char[height_ * width_];
            tuple<int, int> head_coord(abs(rand()) % height_, abs(rand()) % width_);    //--->generate random starting positions for head of snake and apple
            tuple<int, int> apple_coord(abs(rand()) % height_, abs(rand()) % width_);   //
            while (apple_coord == head_coord) { //generate a new starting position for the apple on the off chance it's the same as the snake's head initially
                apple_coord = tuple<int, int>(abs(rand()) % height_, abs(rand()) % width_);
            }
            snake = new Snake(head_coord, UP);
            apple = new Apple(apple_coord);
        }
        void render() {
            tuple<int, int> head = snake->getHead();
            vector<tuple<int, int>> body = snake->getBody();
            for (int row = 0; row < height_; row++) {
                for (int col = 0; col < width_; col++) {
                    board[row * width_ + col] = ' '; //set every position on board to empty by default
                }
            }
            for (int i = 0; i < body.size(); i++) { //set board positions where snake is
                if (i > 0 && body[i] == head) { //if head has collided with body, snake is dead
                    board[get<0>(body[i]) * width_ + get<1>(body[i])] = 'X';
                    snake->set_dead(1);
                } else {
                    board[get<0>(body[i]) * width_ + get<1>(body[i])] = 'O';
                }
            }
            if (apple->get_coord() == head) { //if snake has eaten apple, add 4 links to it and add new apple in different position
                snake->add_links(4);
                length += 4;
                tuple<int, int> apple_coord(abs(rand()) % height_, abs(rand()) % width_);
                int overlap = 1; //variable to make sure new apple position doesn't overlap with snake's body
                while (overlap) {
                    for (vector<tuple<int, int>>::iterator i = body.begin(); i != body.end(); i++) {
                        if (*i == apple_coord) { //if any part of body overlaps with apple coordinates
                            apple_coord = tuple<int, int>(abs(rand()) % height_, abs(rand()) % width_); //change apple coordinates
                            i = body.begin(); //start the loop again so we don't miss overlap with an earlier link
                        }
                    }
                    overlap = 0; //if we've made it through the entire for loop, there is no overlap and the new apple position is good
                }
                apple->set_coords(apple_coord);
            } else {
                board[get<0>(apple->get_coord()) * width_ + get<1>(apple->get_coord())] = '*'; //if snake has not collided with apple, add apple to board
            }

            int board_index = 0;
            for (int i = 0; i < height_ + 2; i++) {
                for (int j = 0; j < width_ + 2; j++) {
                    if (j == 0 || j == width_ + 1) {
                        if (i == 0 || i == height_ + 1) {
                            mvprintw(i, j, "%c", '+'); //for border of board
                        }
                        else {
                            mvprintw(i, j, "%c", '|'); //for border of board
                        }
                    } else if (i == 0 || i == height_ + 1) {
                        mvprintw(i, j, "%c", '-');; //for border of board
                    } else {
                        mvprintw(i, j, "%c", board[board_index]); //if not printing out border, print out game board
                        board_index++;
                    }
                }
            }
            mvprintw(height_ + 2, 0, "Length: %d", length);
        }
        int getHeight() {
            return height_;
        }
        int getWidth() {
            return width_;
        }
        int getLength() {
            return length;
        }
        void setRate(int refresh) {
            refresh_rate = refresh;
        }
        int getRate() {
            return refresh_rate;
        }
        Snake *getSnake() {
            return snake;
        }
        char *getBoard() {
            return board;
        }
        ~Game() {
            delete[] board;
            delete snake;
            delete apple;
        }
    private:
        int height_;    //height of game board
        int width_;     //width of game board
        int length;     //length of snake
        char *board;    //array representing board
        int refresh_rate;   //refresh rate of game board (changes depending on difficulty)
        Snake *snake;   //snake object in game
        Apple *apple;   //apple object in game
};