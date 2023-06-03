#include <C:/Users/18472/Studio C Files/SnakeCurses/classes.h>
#include <unistd.h>

using namespace std;

Game *newGame() { //function to initialize game, useful in case user wants to play again
    char mode; //variable for users desired difficulty
    int player_input; //variable for player input (arrow keys)
    int max_x = 0, max_y = 0; //maximum width, height of screen

    srand(time(0)); //set random seed so we have different snake and apple spawns every time
    initscr(); //initialize terminal in curses mode
    keypad(stdscr, TRUE); //allows us to accept arrow keys as inputs
    getmaxyx(stdscr, max_y, max_x); //get maximum width and height of screen, pass into variables created earlier
    
    start_color(); //allows us to use color functions
    init_pair(1, COLOR_GREEN, COLOR_BLACK); //make pair with green text, black background
    attron(COLOR_PAIR(1)); //apply this color configuration to the terminal
    mvprintw((max_y/2 - 1), (max_x/2)-2, "SNAKE");
    mvprintw((max_y/2), (max_x/2) - 11, "Use arrow keys to move");
    mvprintw((max_y)/2 + 1, (max_x/2) - 41, "Enter your desired game mode: [E] for easy, [I] for intermediate, [H] for hard: ");
    refresh();
    mode = getch();
    while (mode != 'E' && mode != 'e' && mode != 'I' && mode != 'i' && mode != 'H' && mode != 'h') { //if user input is invalid, don't advance
        mvprintw((max_y/2) + 1, (max_x/2) + 39, "invalid input!");
        refresh();
        sleep(1);
        clear();
        mvprintw((max_y/2 - 1), (max_x/2)-2, "SNAKE");
        mvprintw((max_y/2), (max_x/2) - 11, "Use arrow keys to move");
        mvprintw((max_y)/2 + 1, (max_x/2) - 41, "Enter your desired game mode: [E] for easy, [I] for intermediate, [H] for hard: ");
        refresh();
        mode = getch();
    }

    cbreak(); //accept inputs without needing to press enter
    curs_set(FALSE); //turn cursor off
    noecho(); //don't echo user inputs to screen

    Game *game = new Game(max_y, max_x); //max_y first since height is passed to game constructor first
    if (mode == 'E' || mode == 'e') { //set refresh_rate of game according to difficulty user chose
        game->setRate(100);
    } else if (mode == 'I' || mode == 'i') {
        game->setRate(50);
    } else {
        game->setRate(26);
    }
    game->render();
    refresh();
    Snake *snake = game->getSnake();
    while (player_input != KEY_DOWN && player_input != KEY_UP && player_input != KEY_LEFT && player_input != KEY_RIGHT) { //don't start game until player presses an arrow key
        player_input = wgetch(stdscr);
        switch(player_input) {
            case KEY_UP:
                snake->set_direction(UP);
                break;
            case KEY_DOWN:
                snake->set_direction(DOWN);
                break;
            case KEY_LEFT:
                snake->set_direction(LEFT);
                break;
            case KEY_RIGHT:
                snake->set_direction(RIGHT);
                break;
            default:
                break;
        }
    }
    return game;
}

int main() {
    int game_over = 0; //variable to determine whether we should exit the game loop
    int player_input; //variable for player's input (arrow keys)
    int high_score = 0;
    int max_x = 0, max_y = 0;
    FILE* in_file = fopen("high_score.txt", "r");
    fscanf(in_file, "%d", &high_score);
    fclose(in_file);
    Game *game = newGame(); //create new game
    getmaxyx(stdscr, max_y, max_x);
    while (!game_over) {
        game->render();
        mvprintw(max_y - 1, max_x - 15, "High Score: %d", high_score);
        refresh();
        Snake *snake = game->getSnake();
        if (snake->getDirection() == UP || snake->getDirection() == DOWN) { //change snake's movement speed based on direction (vertical moves much faster)
            timeout(game->getRate()); //timeout will break out of getch() without user input, allowing the snake to move without the user pressing a button
        }
        else {
            timeout((game->getRate())/2);
        }
        player_input = getch();
        switch(player_input) { //change snake direction if player presses an arrow key
            case KEY_UP:
                snake->set_direction(UP);
                break;
            case KEY_DOWN:
                snake->set_direction(DOWN);
                break;
            case KEY_LEFT:
                snake->set_direction(LEFT);
                break;
            case KEY_RIGHT:
                snake->set_direction(RIGHT);
                break;
            default:
                break;
        }
        tuple<int, int> new_position(get<0>(snake->getHead()) + get<0>(snake->getDirection()), get<1>(snake->getHead()) + get<1>(snake->getDirection())); //determine new position for snake's head
        if ((get<0>(new_position) < 0 || get<1>(new_position) < 0 || get<0>(new_position) >= game->getHeight() || get<1>(new_position) >= game->getWidth()) || snake->getDead()) { //if new position is out of bounds or snake has collided with itself, game over
            if (game->getLength() > high_score) {
                high_score = game->getLength();
                FILE* out_file = fopen("high_score.txt", "w");
                fprintf(out_file, "%d", high_score);
                fclose(out_file);
            }
            char playAgain; //variable to determine whether player wants to play again or quit
            curs_set(TRUE); //
            echo();         //--->turn cursor and echo back on for user input, turn timeout off so user input is needed to advance
            timeout(-1);    //
            clear(); //clear terminal
            mvprintw((max_y/2) - 1, (max_x/2) - 5, "Game Over");
            mvprintw((max_y/2), (max_x/2) - 7, "High Score: %d", high_score);
            mvprintw((max_y/2) + 1, (max_x/2) - 19, "Play again? [Y] for yes, [N] for no: ");
            refresh();
            playAgain = getch();
            while (playAgain != 'y' && playAgain != 'Y' && playAgain != 'n' && playAgain != 'N') { //if player input is invalid, don't advance
                mvprintw((max_y/2) + 1, (max_x/2) + 18, "invalid input!");
                refresh();
                sleep(1);
                clear();
                mvprintw((max_y/2) - 1, (max_x/2) - 5, "Game Over");
                mvprintw((max_y/2), (max_x/2) - 7, "High Score: %d", high_score);
                mvprintw((max_y/2) + 1, (max_x/2) - 19, "Play again? [Y] for yes, [N] for no: ");
                refresh();
                playAgain = getch();
            }
            if (playAgain == 'y' || playAgain == 'Y') { //if player wants to play again, delete old game and start a new one
                clear();
                delete game;
                game = newGame();
            } else { //if player wants to quit, set game_over to 1 so main game loop is exited
                delete game;
                game_over = 1;
            }
        } else {
            snake->take_step(new_position); //if new position is valid and snake hasn't collided with itself, advance
        }
    }
    endwin();
    return 0;
}