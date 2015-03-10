//
// breakout.c
//
// Computer Science 50
// Problem Set 4
//

// standard libraries
#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Stanford Portable Library
#include "gevents.h"
#include "gobjects.h"
#include "gwindow.h"

// height and width of game's window in pixels
#define HEIGHT 600
#define WIDTH 400

// number of rows of bricks
#define ROWS 5

// number of columns of bricks
#define COLS 10

// radius of ball in pixels
#define RADIUS 10

// lives
#define LIVES 3

//ball speed
#define BDX .04
#define BDY .04

// prototypes
void initBricks(GWindow window);
GOval initBall(GWindow window);
GRect initPaddle(GWindow window);
GLabel initScoreboard(GWindow window);
void updateScoreboard(GWindow window, GLabel label, int points);
GObject detectCollision(GWindow window, GOval ball);

int main(void)
{
    // seed pseudorandom number generator
    srand48(time(NULL));

    // instantiate window
    GWindow window = newGWindow(WIDTH, HEIGHT);

    // instantiate bricks
    initBricks(window);

    // instantiate ball, centered in middle of window
    GOval ball = initBall(window);

    // instantiate paddle, centered at bottom of window
    GRect paddle = initPaddle(window);

    // instantiate scoreboard, centered in middle of window, just above ball
    GLabel label = initScoreboard(window);

    // number of bricks initially
    int bricks = COLS * ROWS;

    // number of lives initially
    int lives = LIVES;

    // number of points initially
    int points = 0;
    
    // ball speed initially
    double random = drand48();
    double bdx = BDX * random;
    double bdy = BDY;
    
    //wait for click to start game
    waitForClick();
    
    // keep playing until game over
    while (lives > 0 && bricks > 0)
    {
        // TODO
        //move paddle
        GEvent event = getNextEvent(MOUSE_EVENT);
        if (event != NULL)
        {
            if (getEventType(event) == MOUSE_MOVED)
            {
                double x = getX(event) - getWidth(paddle) / 2;
                setLocation(paddle, x, (HEIGHT - 30));
            }
        }
        
        //move ball
        move(ball,bdx,bdy);
        
        // bounce off edges
        // left or right
        if ((getX(ball) + getWidth(ball) >= WIDTH) || getX(ball) <= 0)
        {
            bdx *= -1;
        }
        
        // bounce off bricks and paddle
        GObject object = detectCollision(window, ball);
        if (object != NULL)
        {
            if (strcmp(getType(object), "GRect") == 0)
            {
                bdy *= -1;
            }
            
        // if brick is hit, remove it
            if (strcmp(getType(object), "GRect") == 0 && object != paddle) 
            {
                removeGWindow(window, detectCollision(window, object));
                points++;
                updateScoreboard(window, label, points);
            }
        }
        
        // detect if ball hits bottom of screen
        if (getY(ball) + getWidth(ball) >= HEIGHT)
        {
            lives--;
            pause(500);
            setLocation(ball, (WIDTH/2 - RADIUS), (HEIGHT/2 - RADIUS));
            waitForClick();
        }  
    }
        
    // wait for click before exiting
    waitForClick();

    // game over
    closeGWindow(window);
    return 0;
}

/**
 * Initializes window with a grid of bricks.
 */
void initBricks(GWindow window)
{
    // TODO
    string color[COLS] = {"RED", "ORANGE", "GREEN", "CYAN", "BLUE"};
    for (int i = 0; i < COLS; i++) {
        for (int j = 0; j < ROWS; j++) {
            int brickHeight = 10;
            int brickWidth = 36;
            int x = ((brickWidth + 4) * i) + 1.5;
            int y = ((brickHeight + 4) * j) + 20;
            GRect brick = newGRect(x, y, brickWidth, brickHeight);
            setFilled(brick, true);
            setColor(brick, color[j]);
            add(window, brick);
        } 
    }
}

/**
 * Instantiates ball in center of window.  Returns ball.
 */
GOval initBall(GWindow window)
{
    // TODO
    int ballDiameter = 2*RADIUS;
    GOval ball = newGOval((WIDTH/2 - ballDiameter/2), (HEIGHT/2 - ballDiameter/2), ballDiameter, ballDiameter); 
    setFilled(ball, true);
    setColor(ball, "BLACK");
    add(window, ball);
    return ball;
}

/**
 * Instantiates paddle in bottom-middle of window.
 */
GRect initPaddle(GWindow window)
{
    // TODO
    int x = (WIDTH/2 - 40);
    int y = (HEIGHT - 30);
    GRect paddle = newGRect(x, y, 80, 20);
    setFilled(paddle, true);
    setColor(paddle, "BLACK");
    add(window, paddle);
    return paddle;
}

/**
 * Instantiates, configures, and returns label for scoreboard.
 */
GLabel initScoreboard(GWindow window)
{
    // TODO
    GLabel label;    
    label = newGLabel("0");
    double x = (getWidth(window) - getWidth(label)) / 2;
    double y = (getHeight(window) - getHeight(label)) / 2 - 50;
    setLocation(label, x, y);
    setFont (label, "SanSerif-24");
    
    add(window, label);
    return label;
}

/**
 * Updates scoreboard's label, keeping it centered in window.
 */
void updateScoreboard(GWindow window, GLabel label, int points)
{
    // update label
    char s[12];
    sprintf(s, "%i", points);
    setLabel(label, s);

    // center label in window
    double x = (getWidth(window) - getWidth(label)) / 2;
    double y = (getHeight(window) - getHeight(label)) / 2 - 50;
    setLocation(label, x, y);
}

/**
 * Detects whether ball has collided with some object in window
 * by checking the four corners of its bounding box (which are
 * outside the ball's GOval, and so the ball can't collide with
 * itself).  Returns object if so, else NULL.
 */
GObject detectCollision(GWindow window, GOval ball)
{
    // ball's location
    double x = getX(ball);
    double y = getY(ball);

    // for checking for collisions
    GObject object;

    // check for collision at ball's top-left corner
    object = getGObjectAt(window, x, y);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's top-right corner
    object = getGObjectAt(window, x + 2 * RADIUS, y);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's bottom-left corner
    object = getGObjectAt(window, x, y + 2 * RADIUS);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's bottom-right corner
    object = getGObjectAt(window, x + 2 * RADIUS, y + 2 * RADIUS);
    if (object != NULL)
    {
        return object;
    }

    // no collision
    return NULL;
}
