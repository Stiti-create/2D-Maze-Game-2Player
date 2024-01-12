/*This source code copyrighted by Lazy Foo' Productions (2004-2022)
and may not be redistributed without written permission.*/

// Using SDL, SDL_image, standard IO, vectors, and strings
#include "header.h"

#include <SDL2/SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <stdio.h>
#include <string>
#include <fstream>
#include <iostream>

using namespace std;

// The dimensions of the levelvoid LButton::render()

const int LEVEL_WIDTH = 3600;
const int LEVEL_HEIGHT = 2000;

// Screen dimension constants
const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 800;

// button dimensions
const int BUTTON_WIDTH = 500;
const int BUTTON_HEIGHT = 200;

// sprite dimensions
const int SPRITE_WIDTH = 20;
const int SPRITE_HEIGHT = 20;

// task button dimension
const int TB_WIDTH = 60;
const int TB_HEIGHT = 40;

const int LB_WIDTH = 180;
const int LB_HEIGHT = 80;

const int SCREEN_FPS = 60;
const int SCREEN_TICKS_PER_FRAME = 1000 / SCREEN_FPS;

// points, myscore, coins
int myscore = 0;
int opscore = 0;

int tasknum = -1;

int dottype = 0;
int op_dottype = 0;

int DOT_VEL = 5;
int disp = 0;
bool click = true;
int coins = 50;
int lives = 50;
bool byulu = false;
int time1 = 0;

bool newgame; // true for create, false for join
bool claimtask[13];
bool taskreg[13];      // to decide whether player reachers the task spot or not
bool food[6];          // food stall
int onbank = 0;        // to check ATM, 1 when visit atm for first time, 2 when card claimed from main building
                       //  3 when card brought to atm and coind added after onbank = 3
bool mainbuil = false; // to claim atm
bool cardclaimed = false;
bool carddebit = false;
bool wanttoearn = false;
int direction = 4;
int op_direction = 4;
int onoat = 0;
int idclaimed = false;
bool idshown = false;
bool gameend = false;

// bool mouse_check = false;

// int arr[100][180];

// Texture wrapper class
class LTexture
{
public:
    // Initializes variables
    LTexture();

    // Deallocates memory
    ~LTexture();

    // Loads image at specified path
    bool loadFromFile(std::string path);

    // Creates image from font string
    bool loadFromRenderedText(std::string textureText, SDL_Color textColor, TTF_Font *gfont3);

    // Deallocates texture
    void free();

    // Set color modulation
    void setColor(Uint8 red, Uint8 green, Uint8 blue);

    // Set blending
    void setBlendMode(SDL_BlendMode blending);

    // Set alpha modulation
    void setAlpha(Uint8 alpha);

    // Renders texture at given point
    void render(int x, int y, SDL_Rect *clip = NULL, double angle = 0.0, SDL_Point *center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);

    // Gets image dimensions
    int getWidth();
    int getHeight();

private:
    // The actual hardware texture
    SDL_Texture *mTexture;

    // Image dimensions
    int mWidth;
    int mHeight;
};

// The dot that will move around on the screen
class Dot
{
public:
    // The dimensions of the dot
    static const int DOT_WIDTH = 20;
    static const int DOT_HEIGHT = 20;

    // Maximum axis velocity of the dot

    // Initializes the variables
    Dot();

    // Takes key presses and adjusts the dot's velocity
    void handleEvent(SDL_Event &e);

    // Moves the dot
    void move();

    // Shows the dot on the screen relative to the camera
    void render(int camX, int camY);

    // Position accessors
    int getPosX();
    int getPosY();

private:
    // The X and Y offsets of the dot
    int mPosX, mPosY;
    int op_x, op_y;

    // The velocity of the dot
    int mVelX, mVelY;
};

// The mouse button
class LButton
{
public:
    // Initializes internal variables
    LButton();

    // Sets top left position
    void setPosition(int x, int y);

    // Handles mouse event for create
    void handleEvent1(SDL_Event *e);

    // handle mouse event for join game
    void handleEvent2(SDL_Event *e);

    // handle button tasks
    void handleEventtask(SDL_Event *e, int tnum);

    // claim lives
    void handleEventLife(SDL_Event *e);

    // for atm
    void handleEventcardclaim(SDL_Event *e);
    void handleEventcarddebit(SDL_Event *e);
    void handleearncoin(SDL_Event *e);

    // for oat id card
    void handleEventidclaim(SDL_Event *e);
    void handleEventidshow(SDL_Event *e);

    // Shows button of create
    void render1();

    // shows button of play
    void render2();

    void rendertask(int taskno);

    void renderlife();

    void rendercarddebit();

    void rendercardclaim();
    void renderidclaim();

    void renderidshow();
    void renderearncoin();

private:
    // Top left position
    SDL_Point mPosition;
};

// The application time based timer
class LTimer
{
public:
    // Initializes variables
    LTimer();

    // The various clock actions
    void start();
    void stop();
    void pause();
    void unpause();

    // Gets the timer's time
    Uint32 getTicks();

    // Checks the status of the timer
    bool isStarted();
    bool isPaused();

private:
    // The clock time when the timer started
    Uint32 mStartTicks;

    // The ticks stored when the timer was paused
    Uint32 mPausedTicks;

    // The timer status
    bool mPaused;
    bool mStarted;
};

// Starts up SDL and creates window
bool init();

// Loads media
bool loadMedia();

// Frees media and shuts down SDL
void close();

// The window we'll be rendering to
SDL_Window *gWindow = NULL;

// The window renderer
SDL_Renderer *gRenderer = NULL;

// Font global
TTF_Font *gFont = NULL;
TTF_Font *gFont2 = NULL;

// to check if the sprite is walking on the road
bool onroad(int posx, int posy)
{
    // boolean values that decide which region the sprite is moving on

    bool b1 = posx > 20 && posx <= 24 && posy <= 80;                                 // kumaon jwala road
    bool b2 = posx >= 16 && posx <= 20 && posy >= 12 && posy < 15;                   // jwala yulu
    bool b3 = posx >= 18 && posx <= 20 && posy >= 20 && posy < 23;                   // holistic
    bool b4 = posx <= 24 && posy >= 64 && posy <= 67;                                // nilgiri road
    bool b5 = posx >= 24 && posx <= 42 && posy <= 14 && posy >= 12;                  // kumaon vindy road
    bool b6 = posx >= 24 && posx <= 121 && posy > 39 && posy <= 42;                  // sac main road
    bool b7 = posx >= 40 && posx <= 45 && posy >= 54 && posy <= 60;                  // sac diagonal
    bool b8 = posx >= 40 && posx < 43 && posy >= 11 && posy <= 54;                   // sac circle road
    bool b9 = posx >= 56 && posx <= 60 && posy >= 19 && posy <= 74;                  // rajdhani road
    bool b10 = posx >= 40 && posx <= 83 && posy >= 15 && posy <= 17;                 // satpura udai girnar road
    bool b11 = posx + posy >= 125 && posx + posy <= 129 && posy >= 24 && posy <= 40; // bharti corner road
    bool b12 = posx + posy >= 138 && posx + posy <= 140 && posy >= 13 && posy <= 38; // windT road
    bool b13 = posx + posy >= 161 && posx + posy <= 165 && posy <= 42;               // main roadudai
    bool b14 = posx - posy >= 74 && posx - posy <= 77 && posy <= 38 && posy >= 11;   // blocks road
    bool b15 = posx - posy >= 89 && posx - posy <= 92 && posy <= 35 && posy >= 24;   // nescafe road
    bool b16 = posx >= 141 && posx <= 177 && posy >= 21 && posy <= 24;               // kailash road
    bool b17 = posx >= 173 && posx <= 177;                                           // himadri road
    bool b18 = posx - posy >= 134 && posx - posy <= 137 && posy <= 21 && posy >= 14; // him circle road
    bool b19 = posx + posy >= 96 && posx + posy <= 100 && posy <= 11 && posy >= 1;   // blocks
    bool b20 = posx >= 82 && posx <= 83 && posy <= 17;                               // girnar udai road
    bool b21 = posx >= 1 && posx <= 4 && posy >= 94 && posy <= 144;                  // girnar udai road
    bool b22 = posx - posy >= 140 && posx - posy <= 143 && posy <= 10 && posy >= 1;  // bloack-main bool b22 = posx - posy >= 140 && posx - posy <= 143 && posy <= 10 && posy >= 1; /
    bool b23 = posx - posy >= 98 && posx - posy <= 100 && posy <= 41 && posy >= 32;  // sbi-lhc road
    bool b24 = posx >= 116 && posx <= 130 && posy >= 43 && posy <= 45;               // ich road 1
    bool b25 = posx >= 116 && posx <= 119 && posy >= 42 && posy <= 45;               // ich road2
    bool b26 = posx >= 58 && posx <= 62 && posy <= 17;                               // satpura nightmess road
    bool b27 = posx >= 41 && posx <= 51 && posy >= 29 && posy <= 32;                 // shiva zanskar road
    bool b28 = posx >= 25 && posx <= 39 && posy >= 54 && posy <= 56;                 // nalanda gulmohar road
    bool b29 = posx >= 4 && posx <= 5 && posy >= 66;                                 // vaishali road
    bool b30 = posx >= 4 && posx <= 58 && posy >= 81 && posy <= 83;                  // taxila
    bool b31 = posx >= 5 && posx <= 24 && posy >= 90 && posy <= 92;                  // taxila
    bool b32 = posx >= 21 && posx < 58 && posy >= 70 && posy <= 74;                  // taxila
    bool b33 = posx - posy >= 108 && posx - posy <= 110 && posy <= 26 && posy >= 16; // iitd-biotech road
    bool b34 = posx >= 95 && posx <= 145 && posy >= 1 && posy <= 4;                  // block straight
    bool b35 = posx >= 54 && posx <= 58 && posy >= 15 && posy <= 83;                 // masala mix dms
    bool b36 = posx >= 58 && posx <= 177 && posy >= 68 && posy <= 72;                // dms himadri
    bool b37 = posx >= 173 && posx < 177;                                            // himadri back
    bool b38 = posx >= 58 && posx <= 83 && posy <= 3;                                // gir-udai upper
    bool b39 = posx >= 20 && posx <= 24 && posy >= 82;
    bool b40 = posx + posy >= 141 && posx + posy <= 143 && posy <= 15 && posy >= 6; // blocks-mech
    bool b41 = posx - posy >= 129 && posx - posy <= 132 && posy <= 15 && posy >= 6; // himadri circle mech lawn
    bool b42 = posy >= 16 && posy <= 22 && posx <= 122 && posx >= 116;              // exhall CCD

    // if (true)
    if (b41 || b40 || b39 || b1 || b2 || b3 || b4 || b5 || b6 || b7 || b8 || b9 || b10 || b11 || b12 || b13 || b14 || b15 || b16 || b17 || b18 || b19 || b20 || b21 || b22 || b23 || b24 || b25 || b26 || b27 || b28 || b29 || b30 || b31 || b32 || b33 || b34 || b35 || b36 || b37 || b38)
    {
        return true;
    }
    return false;
}

// Scene textures
LTexture gDotTexture1;
LTexture gDotTexture2;
LTexture gBGTexture;
LTexture scoredsip;
LTexture hbox;
LTexture welcome, endpage;
LTexture createbtn, joinbtn, livestable, tasktext[13], tasklist;
LTexture lifedisp, helpdisp, coindisp, lesslife, clientleft;
LButton create, join;
LButton taskclaim[13];
LButton claimcard, debitcard, earncoin, idverify, idshow;
// Rendered text texture
LTexture gTextTexture1, gTextTexture2, lifeText, coinText, timetext;
LTexture claimcardtext, debitcardtext, earncointext, idverifytext, idshowtext, winpage;

// The music that will be played
Mix_Music *gMusic = NULL;

// The sound effects that will be used
Mix_Music *taskmus = NULL;
Mix_Music *foodmus = NULL;
Mix_Music *coinmus = NULL;
Mix_Music *lifeendmus = NULL;
Mix_Music *yulumus = NULL;
Mix_Music *startmus = NULL;

LButton getlife;

LTexture::LTexture()
{
    // Initialize
    mTexture = NULL;
    mWidth = 0;
    mHeight = 0;
}

LTexture::~LTexture()
{
    // Deallocate
    free();
}

bool LTexture::loadFromFile(std::string path)
{
    // Get rid of preexisting texture
    free();

    // The final texture
    SDL_Texture *newTexture = NULL;

    // Load image at specified path
    SDL_Surface *loadedSurface = IMG_Load(path.c_str());
    if (loadedSurface == NULL)
    {
        printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
    }
    else
    {
        // Color key image
        SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));

        // Create texture from surface pixels
        newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
        if (newTexture == NULL)
        {
            printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
        }
        else
        {
            // Get image dimensions
            mWidth = loadedSurface->w;
            mHeight = loadedSurface->h;
        }

        // Get rid of old loaded surface
        SDL_FreeSurface(loadedSurface);
    }

    // Return success
    mTexture = newTexture;
    return mTexture != NULL;
}

bool LTexture::loadFromRenderedText(std::string textureText, SDL_Color textColor, TTF_Font *gFont3)
{
    // Get rid of preexisting texture
    free();

    // Render text surface
    SDL_Surface *textSurface = TTF_RenderText_Solid(gFont3, textureText.c_str(), textColor);
    // SDL_Surface *textSurface = TTF_RenderText_Shaded(gFont, textureText.c_str(), textColor, bgColor);
    if (textSurface == NULL)
    {
        printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
    }
    else
    {
        // Create texture from surface pixels
        mTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
        if (mTexture == NULL)
        {
            printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
        }
        else
        {
            // Get image dimensions
            mWidth = textSurface->w;
            mHeight = textSurface->h;
        }

        // Get rid of old surface
        SDL_FreeSurface(textSurface);
    }

    // Return success
    return mTexture != NULL;
}

void LTexture::free()
{
    // Free texture if it exists
    if (mTexture != NULL)
    {
        SDL_DestroyTexture(mTexture);
        mTexture = NULL;
        mWidth = 0;
        mHeight = 0;
    }
}

void LTexture::setColor(Uint8 red, Uint8 green, Uint8 blue)
{
    // Modulate texture rgb
    SDL_SetTextureColorMod(mTexture, red, green, blue);
}

void LTexture::setBlendMode(SDL_BlendMode blending)
{
    // Set blending function
    SDL_SetTextureBlendMode(mTexture, blending);
}

void LTexture::setAlpha(Uint8 alpha)
{
    // Modulate texture alpha
    SDL_SetTextureAlphaMod(mTexture, alpha);
}

void LTexture::render(int x, int y, SDL_Rect *clip, double angle, SDL_Point *center, SDL_RendererFlip flip)
{
    // Set rendering space and render to screen
    SDL_Rect renderQuad = {x, y, mWidth, mHeight};

    // Set clip rendering dimensions
    if (clip != NULL)
    {
        renderQuad.w = clip->w;
        renderQuad.h = clip->h;
    }

    // Render to screen
    SDL_RenderCopyEx(gRenderer, mTexture, clip, &renderQuad, angle, center, flip);
}

int LTexture::getWidth()
{
    return mWidth;
}

int LTexture::getHeight()
{
    return mHeight;
}

LButton::LButton()
{
    mPosition.x = 0;
    mPosition.y = 0;
}

void LButton::setPosition(int x, int y)
{
    mPosition.x = x;
    mPosition.y = y;
}

void LButton::handleEvent1(SDL_Event *e) // create mode
{
    // If mouse event happened
    if (e->type == SDL_MOUSEMOTION || e->type == SDL_MOUSEBUTTONDOWN || e->type == SDL_MOUSEBUTTONUP)
    {

        // Get mouse position
        int x, y;
        SDL_GetMouseState(&x, &y);

        // Check if mouse is in button
        bool inside = true;

        // Mouse is left of the button
        if (x < mPosition.x)
        {
            inside = false;
        }
        // Mouse is right of the button
        else if (x > mPosition.x + BUTTON_WIDTH)
        {
            inside = false;
        }
        // Mouse above the button
        else if (y < mPosition.y)
        {
            inside = false;
        }
        // Mouse below the button
        else if (y > mPosition.y + BUTTON_HEIGHT)
        {
            inside = false;
        }

        // Mouse is inside button
        if (inside)
        {
            // cout << "mouse" << endl;
            if (e->type == SDL_MOUSEBUTTONDOWN)
            {
                disp = 3;       // play mode
                newgame = true; // create
            }
        }
    }
}

void LButton::handleEvent2(SDL_Event *e) // create mode
{
    // If mouse event happened
    if (e->type == SDL_MOUSEMOTION || e->type == SDL_MOUSEBUTTONDOWN || e->type == SDL_MOUSEBUTTONUP)
    {

        // Get mouse position
        int x, y;
        SDL_GetMouseState(&x, &y);

        // Check if mouse is in button
        bool inside = true;

        // Mouse is left of the button
        if (x < mPosition.x)
        {
            inside = false;
        }
        // Mouse is right of the button
        else if (x > mPosition.x + BUTTON_WIDTH)
        {
            inside = false;
        }
        // Mouse above the button
        else if (y < mPosition.y)
        {
            inside = false;
        }
        // Mouse below the button
        else if (y > mPosition.y + BUTTON_HEIGHT)
        {
            inside = false;
        }

        // Mouse is inside button
        if (inside)
        {
            // cout << "mouse" << endl;
            if (e->type == SDL_MOUSEBUTTONDOWN)
            {

                disp = 3;        // play mode
                newgame = false; // join mode
            }
        }
    }
}

void LButton::handleEventtask(SDL_Event *e, int tnum) // create mode
{
    // If mouse event happened
    if (e->type == SDL_MOUSEMOTION || e->type == SDL_MOUSEBUTTONDOWN || e->type == SDL_MOUSEBUTTONUP)
    {

        // Get mouse position
        int x, y;
        SDL_GetMouseState(&x, &y);

        // Check if mouse is in button
        bool inside = true;

        // Mouse is left of the button
        if (x < mPosition.x)
        {
            inside = false;
        }
        // Mouse is right of the button
        else if (x > mPosition.x + TB_WIDTH)
        {
            inside = false;
        }
        // Mouse above the button
        else if (y < mPosition.y)
        {
            inside = false;
        }
        // Mouse below the button
        else if (y > mPosition.y + TB_HEIGHT)
        {
            inside = false;
        }
        // Mouse is inside button
        if (inside)
        {
            if (e->type == SDL_MOUSEBUTTONDOWN)
            {

                // returns the task number
                if (taskreg[tnum]) // if inside task region
                {
                    claimtask[tnum] = true;
                    tasknum = tnum;
                    if (tnum <= 4 && tnum >= 0)
                    {
                        myscore += 30;
                        Mix_PlayMusic(taskmus, 0);
                    }
                    if (tnum >= 5 && tnum <= 7)
                    {
                        myscore += 20;
                        Mix_PlayMusic(taskmus, 0);
                    }
                    if (tnum >= 8 && tnum <= 11)
                    {
                        myscore += 40;
                        Mix_PlayMusic(taskmus, 0);
                    }
                    if (tnum == 12)
                    {
                        // myscore += 50;
                        onoat = 1;
                        claimtask[tnum] = false;
                    }
                }
            }
        }
    }
}

void LButton::handleEventLife(SDL_Event *e) // create mode
{
    // If mouse event happened
    if (e->type == SDL_MOUSEMOTION || e->type == SDL_MOUSEBUTTONDOWN || e->type == SDL_MOUSEBUTTONUP)
    {

        // Get mouse position
        int x, y;
        SDL_GetMouseState(&x, &y);

        // Check if mouse is in button
        bool inside = true;

        // Mouse is left of the button
        if (x < mPosition.x)
        {
            inside = false;
        }
        // Mouse is right of the button
        else if (x > mPosition.x + LB_WIDTH)
        {
            inside = false;
        }
        // Mouse above the button
        else if (y < mPosition.y)
        {
            inside = false;
        }
        // Mouse below the button
        else if (y > mPosition.y + LB_HEIGHT)
        {
            inside = false;
        }

        // Mouse is inside button
        if (inside)
        {
            // cout << "mouse" << endl;
            if (e->type == SDL_MOUSEBUTTONDOWN)
            {
                // cout << "clicked" << endl;
                for (int i = 0; i < 6; i++)
                {
                    if (food[i])
                    {
                        // cout << to_string(i) << endl;
                        switch (i)
                        {
                        case 0:
                            if (coins >= 20)
                            {
                                lives += 25;
                                coins -= 20;
                                Mix_PlayMusic(foodmus, 1);
                            }

                            break;
                        case 1:
                            lives += 20;
                            coins -= 0;
                            Mix_PlayMusic(foodmus, 1);
                            break;
                        case 2:
                            if (coins >= 15)
                            {
                                lives += 15;
                                coins -= 15;
                                Mix_PlayMusic(foodmus, 1);
                            }
                            break;
                        case 3:
                            if (coins >= 10)
                            {
                                lives += 15;
                                coins -= 10;
                                Mix_PlayMusic(foodmus, 1);
                            }
                            break;
                        case 4:
                            lives += 5;
                            coins -= 0;
                            Mix_PlayMusic(foodmus, 1);
                            break;
                        case 5:
                            if (coins >= 10)
                            {
                                lives += 30;
                                coins -= 10;
                                Mix_PlayMusic(foodmus, 1);
                            }
                            break;
                        }
                        food[i] = false;
                    }
                }
            }
        }
    }
}

void LButton::handleEventcardclaim(SDL_Event *e) // create mode
{
    // If mouse event happened
    if (e->type == SDL_MOUSEMOTION || e->type == SDL_MOUSEBUTTONDOWN || e->type == SDL_MOUSEBUTTONUP)
    {

        // Get mouse position
        int x, y;
        SDL_GetMouseState(&x, &y);

        // Check if mouse is in button
        bool inside = true;

        // Mouse is left of the button
        if (x < mPosition.x)
        {
            inside = false;
        }
        // Mouse is right of the button
        else if (x > mPosition.x + 180)
        {
            inside = false;
        }
        // Mouse above the button
        else if (y < mPosition.y)
        {
            inside = false;
        }
        // Mouse below the button
        else if (y > mPosition.y + 80)
        {
            inside = false;
        }

        // Mouse is inside button
        if (inside)
        {
            // cout << "mouse" << endl;
            if (e->type == SDL_MOUSEBUTTONDOWN)
            {
                if (onbank == 1 && mainbuil)
                {
                    cardclaimed = true;
                    onbank = 2;
                }
            }
        }
    }
}

void LButton::handleEventcarddebit(SDL_Event *e) // create mode
{
    // If mouse event happened
    if (e->type == SDL_MOUSEMOTION || e->type == SDL_MOUSEBUTTONDOWN || e->type == SDL_MOUSEBUTTONUP)
    {

        // Get mouse position
        int x, y;
        SDL_GetMouseState(&x, &y);

        // Check if mouse is in button
        bool inside = true;

        // Mouse is left of the button
        if (x < mPosition.x)
        {
            inside = false;
        }
        // Mouse is right of the button
        else if (x > mPosition.x + 180)
        {
            inside = false;
        }
        // Mouse above the button
        else if (y < mPosition.y)
        {
            inside = false;
        }
        // Mouse below the button
        else if (y > mPosition.y + 80)
        {
            inside = false;
        }

        // Mouse is inside button
        if (inside)
        {
            // cout << "mouse" << endl;
            if (e->type == SDL_MOUSEBUTTONDOWN)
            {
                if (onbank == 3)
                {
                    onbank = 4;
                    coins += 50; // coins added
                    Mix_PlayMusic(coinmus, 1);
                }
            }
        }
    }
}

void LButton::handleearncoin(SDL_Event *e) // create mode
{
    // If mouse event happened
    if (e->type == SDL_MOUSEMOTION || e->type == SDL_MOUSEBUTTONDOWN || e->type == SDL_MOUSEBUTTONUP)
    {

        // Get mouse position
        int x, y;
        SDL_GetMouseState(&x, &y);

        // Check if mouse is in button
        bool inside = true;

        // Mouse is left of the button
        if (x < mPosition.x)
        {
            inside = false;
        }
        // Mouse is right of the button
        else if (x > mPosition.x + 180)
        {
            inside = false;
        }
        // Mouse above the button
        else if (y < mPosition.y)
        {
            inside = false;
        }
        // Mouse below the button
        else if (y > mPosition.y + 80)
        {
            inside = false;
        }

        // Mouse is inside button
        if (inside)
        {
            // cout << "mouse" << endl;
            if (e->type == SDL_MOUSEBUTTONDOWN)
            {
                if (onbank == 1)
                {
                    wanttoearn = true;
                }
            }
        }
    }
}

void LButton::handleEventidclaim(SDL_Event *e) // create mode
{
    // If mouse event happened
    if (e->type == SDL_MOUSEMOTION || e->type == SDL_MOUSEBUTTONDOWN || e->type == SDL_MOUSEBUTTONUP)
    {

        // Get mouse position
        int x, y;
        SDL_GetMouseState(&x, &y);

        // Check if mouse is in button
        bool inside = true;

        // Mouse is left of the button
        if (x < mPosition.x)
        {
            inside = false;
        }
        // Mouse is right of the button
        else if (x > mPosition.x + 180)
        {
            inside = false;
        }
        // Mouse above the button
        else if (y < mPosition.y)
        {
            inside = false;
        }
        // Mouse below the button
        else if (y > mPosition.y + 80)
        {
            inside = false;
        }

        // Mouse is inside button
        if (inside)
        {
            // cout << "mouse" << endl;
            if (e->type == SDL_MOUSEBUTTONDOWN)
            {

                if (onoat == 2)
                {
                    idclaimed = true;
                    // onoat = 2;
                }
            }
        }
    }
}

void LButton::handleEventidshow(SDL_Event *e) // create mode
{
    // If mouse event happened
    if (e->type == SDL_MOUSEMOTION || e->type == SDL_MOUSEBUTTONDOWN || e->type == SDL_MOUSEBUTTONUP)
    {

        // Get mouse position
        int x, y;
        SDL_GetMouseState(&x, &y);

        // Check if mouse is in button
        bool inside = true;

        // Mouse is left of the button
        if (x < mPosition.x)
        {
            inside = false;
        }
        // Mouse is right of the button
        else if (x > mPosition.x + 180)
        {
            inside = false;
        }
        // Mouse above the button
        else if (y < mPosition.y)
        {
            inside = false;
        }
        // Mouse below the button
        else if (y > mPosition.y + 80)
        {
            inside = false;
        }

        // Mouse is inside button
        if (inside)
        {
            // cout << "mouse" << endl;
            if (e->type == SDL_MOUSEBUTTONDOWN)
            {
                if (onoat == 3)
                {
                    idshown = true;
                    claimtask[12] = true;
                    myscore += 50;
                    onoat = 4;
                    Mix_PlayMusic(taskmus, 0);
                }
            }
        }
    }
}

void LButton::render1()
{
    // Show current button sprite
    createbtn.render(mPosition.x, mPosition.y);
}

void LButton::render2()
{
    // Show current button sprite
    joinbtn.render(mPosition.x, mPosition.y);
}

void LButton::rendertask(int taskno)
{
    // Show current button sprite

    tasktext[taskno].render(mPosition.x, mPosition.y);
}

void LButton::renderlife()
{
    // Show current button sprite
    lifedisp.render(mPosition.x, mPosition.y);
}

void LButton::rendercardclaim()
{
    // Show current button sprite
    claimcardtext.render(mPosition.x, mPosition.y);
}

void LButton::rendercarddebit()
{
    // Show current button sprite
    debitcardtext.render(mPosition.x, mPosition.y);
}

void LButton::renderearncoin()
{
    // Show current button sprite
    earncointext.render(mPosition.x, mPosition.y);
}

void LButton::renderidclaim()
{
    // Show current button sprite
    idverifytext.render(mPosition.x, mPosition.y);
}

void LButton::renderidshow()
{
    // Show current button sprite
    idshowtext.render(mPosition.x, mPosition.y);
}

LTimer::LTimer()
{
    // Initialize the variables
    mStartTicks = 0;
    mPausedTicks = 0;

    mPaused = false;
    mStarted = false;
}

void LTimer::start()
{
    // Start the timer
    mStarted = true;

    // Unpause the timer
    mPaused = false;

    // Get the current clock time
    mStartTicks = SDL_GetTicks();
    mPausedTicks = 0;
}

void LTimer::stop()
{
    // Stop the timer
    mStarted = false;

    // Unpause the timer
    mPaused = false;

    // Clear tick variables
    mStartTicks = 0;
    mPausedTicks = 0;
}

void LTimer::pause()
{
    // If the timer is running and isn't already paused
    if (mStarted && !mPaused)
    {
        // Pause the timer
        mPaused = true;

        // Calculate the paused ticks
        mPausedTicks = SDL_GetTicks() - mStartTicks;
        mStartTicks = 0;
    }
}

void LTimer::unpause()
{
    // If the timer is running and paused
    if (mStarted && mPaused)
    {
        // Unpause the timer
        mPaused = false;

        // Reset the starting ticks
        mStartTicks = SDL_GetTicks() - mPausedTicks;

        // Reset the paused ticks
        mPausedTicks = 0;
    }
}

Uint32 LTimer::getTicks()
{
    // The actual timer time
    Uint32 time = 0;

    // If the timer is running
    if (mStarted)
    {
        // If the timer is paused
        if (mPaused)
        {
            // Return the number of ticks when the timer was paused
            time = mPausedTicks;
        }
        else
        {
            // Return the current time minus the start time
            time = SDL_GetTicks() - mStartTicks;
        }
    }

    return time;
}

bool LTimer::isStarted()
{
    // Timer is running and paused or unpaused
    return mStarted;
}

bool LTimer::isPaused()
{
    // Timer is running and paused
    return mPaused && mStarted;
}

Dot::Dot()
{
    // Initialize the offsets
    mPosX = 420;
    mPosY = 0;

    // Initialize the velocity
    mVelX = 0;
    mVelY = 0;
}

void Dot::handleEvent(SDL_Event &e)
{
    // If a key was pressed
    if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
    {
        // Adjust the velocity
        switch (e.key.keysym.sym)
        {
        case SDLK_UP:
            mVelY -= DOT_VEL;
            direction = 2;
            break;
        case SDLK_DOWN:
            mVelY += DOT_VEL;
            direction = 4;
            break;
        case SDLK_LEFT:
            mVelX -= DOT_VEL;
            direction = 3;
            break;
        case SDLK_RIGHT:
            mVelX += DOT_VEL;
            direction = 1;
            break;
        case SDLK_m:
            mVelX += DOT_VEL;
            mVelY += DOT_VEL;
            direction = 1;
            break;
        case SDLK_i:
            mVelX += DOT_VEL;
            mVelY -= DOT_VEL;
            direction = 1;
            break;
        case SDLK_n:
            mVelX -= DOT_VEL;
            mVelY += DOT_VEL;
            direction = 3;
            break;
        case SDLK_u:
            mVelX -= DOT_VEL;
            mVelY -= DOT_VEL;
            direction = 3;
            break;
        }
    }
    // If a key was released
    else if (e.type == SDL_KEYUP && e.key.repeat == 0)
    {
        // Adjust the velocity
        switch (e.key.keysym.sym)
        {
        case SDLK_UP:
            mVelY += DOT_VEL;
            break;
        case SDLK_DOWN:
            mVelY -= DOT_VEL;
            break;
        case SDLK_LEFT:
            mVelX += DOT_VEL;
            break;
        case SDLK_RIGHT:
            mVelX -= DOT_VEL;
            break;
        case SDLK_m:
            mVelX -= DOT_VEL;
            mVelY -= DOT_VEL;
            break;
        case SDLK_i:
            mVelX -= DOT_VEL;
            mVelY += DOT_VEL;
            break;
        case SDLK_n:
            mVelX += DOT_VEL;
            mVelY -= DOT_VEL;
            break;
        case SDLK_u:
            mVelX += DOT_VEL;
            mVelY += DOT_VEL;
            break;
        }
    }
}

void Dot::move()
{
    // Move the dot left or right
    mPosX += mVelX;
    // arr[mPosX][mPosY] = 1;
    // If the dot went too far to the left or right or collided
    if ((mPosX < 0) || (mPosX + DOT_WIDTH > LEVEL_WIDTH) || !onroad(mPosX / 20, mPosY / 20))
    {
        // Move back
        mPosX -= mVelX;
        // arr[mPosX][mPosY] = 0;
    }
    // Move the dot up or down
    mPosY += mVelY;

    // If the dot went too far up or down
    if ((mPosY < 0) || (mPosY + DOT_HEIGHT > LEVEL_HEIGHT) || !onroad(mPosX / 20, mPosY / 20))
    {
        // Move back
        mPosY -= mVelY;
        // arr[mPosX][mPosY] = 0;
    }
    int xco = mPosX / 20;
    int yco = mPosY / 20;
    // cout << xco << " " << yco << endl;
    // arr[yco][xco] = 1;
    // cout << xco << " " << yco << endl;
    byulu = ((xco >= 16) && (xco <= 20) && (yco >= 11) && (yco < 16)) ||
            (xco >= 40 && xco < 43 && yco >= 52 && yco <= 56) ||
            (xco >= 58 && xco <= 62 && yco >= 40 && yco <= 44) ||
            (xco >= 84 && xco <= 88 && yco >= 40 && yco <= 44) ||
            (xco >= 173 && xco <= 176 && yco >= 3 && yco <= 8) ||
            (xco >= 158 && xco <= 162 && yco >= 0 && yco <= 5) ||
            (xco >= 8 && xco <= 13 && yco >= 66 && yco <= 70);

    string serverfn_res = serverfn(mPosX, mPosY, myscore, dottype, direction);
    // cout << serverfn_res << endl;
    if (serverfn_res.compare("0,0,0,0,0") == 0)
    {
        cout << "Client left" << endl;
        clientleft.render(0, 0);
        SDL_RenderPresent(gRenderer);
        SDL_Delay(5000);
    }
    int comma_pos1 = serverfn_res.find(",");
    int comma_pos2 = serverfn_res.find(",", comma_pos1 + 1);
    int comma_pos3 = serverfn_res.find(",", comma_pos2 + 1);
    int comma_pos4 = serverfn_res.find(",", comma_pos3 + 1);

    op_x = stoi(serverfn_res.substr(0, comma_pos1));
    op_y = stoi(serverfn_res.substr(comma_pos1 + 1, comma_pos2 - comma_pos1 - 1));
    opscore = stoi(serverfn_res.substr(comma_pos2 + 1, comma_pos3 - comma_pos2 - 1));
    op_dottype = stoi(serverfn_res.substr(comma_pos3 + 1, 1));
    op_direction = stoi(serverfn_res.substr(comma_pos3 + 3, 1));

    taskreg[0] = ((xco >= 116 && xco <= 130 && yco >= 43 && yco <= 47) || // hockey
                  (xco >= 116 && xco <= 119 && yco >= 42 && yco <= 47));
    taskreg[1] = (yco >= 40 && yco <= 46 && xco >= 92 && xco <= 97);  // cricket
    taskreg[2] = (xco >= 64 && xco <= 72 && yco >= 41 && yco <= 45);  // football
    taskreg[3] = yco >= 16 && yco <= 20 && xco <= 122 && xco >= 116;  // ex-hall                                            // exhall
    taskreg[4] = yco >= 18 && yco <= 22 && xco <= 122 && xco >= 118;  // CSC                                             // CSC
    taskreg[5] = xco >= 22 && xco <= 25 && yco <= 22 && yco >= 16;    // volleyball
    taskreg[6] = xco >= 22 && xco <= 25 && yco <= 33 && yco >= 28;    // lawn tennis
    taskreg[7] = yco >= 6 && yco <= 24 && xco <= 145 && xco >= 126;   // biotech lawn
    taskreg[8] = xco >= 106 && xco <= 112 && yco <= 70 && yco >= 66;  // mittal sports
    taskreg[9] = xco >= 138 && xco <= 142 && yco <= 42 && yco >= 38;  // LHC
    taskreg[10] = xco >= 117 && xco <= 121 && yco <= 30 && yco >= 26; // library
    taskreg[11] = xco >= 43 && xco <= 47 && yco <= 57 && yco >= 53;   // SAC
    taskreg[12] = xco >= 42 && xco <= 47 && yco <= 62 && yco >= 57;   // OAT

    food[0] = (xco >= 23 && xco <= 25 && yco >= 24 && yco <= 28) ||
              (xco >= 54 && xco <= 57 && yco >= 28 && yco <= 38); // D16, RAJDHANI, CHHAyos, mmix
    food[1] = false;
    food[2] = xco >= 126 && xco <= 130 && yco >= 44 && yco <= 46;           // staff canteen, ICH
    food[3] = xco - yco >= 88 && xco - yco <= 92 && yco >= 26 && yco <= 33; // amul,nescafe,hpmc
    food[4] = xco >= 109 && xco <= 112 && yco >= 27 && yco <= 31;           // shiru
    food[5] = (xco >= 19 && xco <= 22 && yco >= 27 && yco <= 31) ||         // holistic
              (xco >= 19 && xco <= 22 && yco >= 60 && yco <= 64) ||         // nil night mess
              (xco >= 44 && xco <= 46 && yco >= 37 && yco <= 41) ||         // zanskar nm
              (xco >= 56 && xco <= 60 && yco >= 8 && yco <= 12) ||          // satpura nm
              (xco >= 173 && xco <= 177 && yco >= 9 && yco <= 13);          // himadri nm

    // onbank bool
    if ((xco >= 105 && xco <= 108 && yco >= 37 && yco <= 40) || // axis bank
        (xco >= 134 && xco <= 136 && yco >= 34 && yco <= 36))
    {
        if (onbank == 0)
        {
            // cout << "reached" << endl;
            onbank = 1;
        }
        if (onbank == 2)
        {
            onbank = 3;
        }
    }
    // cout << "onbank: " << onbank << endl;

    if (taskreg[9] && onoat == 1)
    {

        onoat = 2; // claim card from LHC
    }
    if (taskreg[12] && onoat == 2)
    {
        onoat = 3;
        // myscore+=50;//points added after showing id at OAT
    }

    mainbuil = (xco >= 132 && xco <= 136 && yco >= 24 && yco <= 28); // mainbuil
    // cout << xco << " " << yco << endl;
}

void Dot::render(int camX, int camY)
{
    // Show the dot relative to the camera
    gDotTexture1.render(mPosX - camX, mPosY - camY);
    gDotTexture2.render(op_x - camX, op_y - camY);
}

int Dot::getPosX()
{
    return mPosX;
}

int Dot::getPosY()
{
    return mPosY;
}

bool init()
{
    // Initialization flag
    bool success = true;

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
        success = false;
    }
    else
    {
        // Set texture filtering to linear
        if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
        {
            printf("Warning: Linear texture filtering not enabled!");
        }

        // Create window
        gWindow = SDL_CreateWindow("MAZE GAME - IITD", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if (gWindow == NULL)
        {
            printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
            success = false;
        }
        else
        {
            // Create vsynced renderer for window
            gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
            if (gRenderer == NULL)
            {
                printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
                success = false;
            }
            else
            {
                // Initialize renderer color
                SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

                // Initialize PNG loading
                int imgFlags = IMG_INIT_PNG;
                if (!(IMG_Init(imgFlags) & imgFlags))
                {
                    printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
                    success = false;
                }

                if (TTF_Init() == -1)
                {
                    printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
                    success = false;
                }

                if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
                {
                    printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
                    success = false;
                }
            }
        }
    }

    return success;
}

bool loadMedia()
{
    // Loading success flag
    bool success = true;

    // Load dot texture 1
    if (!gDotTexture1.loadFromFile("players/p2d.png"))
    {
        printf("Failed to load dot texture 1!\n");
        success = false;
    }

    // Load dot texture 2
    if (!gDotTexture2.loadFromFile("players/p1d.png"))
    {
        printf("Failed to load dot texture 2!\n");
        success = false;
    }

    // Load background texture
    if (!gBGTexture.loadFromFile("Map.png"))
    {
        printf("Failed to load background texture!\n");
        success = false;
    }

    // Load help box
    if (!hbox.loadFromFile("help-box.png"))
    {
        printf("Failed to load background texture!\n");
        success = false;
    }

    // load welcome page first
    if (!welcome.loadFromFile("welcome.png"))
    {
        printf("Failed to load background texture!\n");
        success = false;
    }

    // load button media
    if (!createbtn.loadFromFile("create.png"))
    {
        printf("Failed to load background texture!\n");
        success = false;
    }

    if (!joinbtn.loadFromFile("join.png"))
    {
        printf("Failed to load button texture!\n");
        success = false;
    }

    if (!livestable.loadFromFile("lives.png"))
    {
        printf("Failed to load lives texture!\n");
        success = false;
    }
    // load background music
    taskmus = Mix_LoadMUS("sounds/task.wav");
    if (taskmus == NULL)
    {
        printf("Failed to load beat music! SDL_mixer Error: %s\n", Mix_GetError());
        success = false;
    }

    foodmus = Mix_LoadMUS("sounds/food.wav");
    if (foodmus == NULL)
    {
        printf("Failed to load beat music! SDL_mixer Error: %s\n", Mix_GetError());
        success = false;
    }

    coinmus = Mix_LoadMUS("sounds/coin.wav");
    if (coinmus == NULL)
    {
        printf("Failed to load beat music! SDL_mixer Error: %s\n", Mix_GetError());
        success = false;
    }

    startmus = Mix_LoadMUS("sounds/start.wav");
    if (startmus == NULL)
    {
        printf("Failed to load beat music! SDL_mixer Error: %s\n", Mix_GetError());
        success = false;
    }

    lifeendmus = Mix_LoadMUS("sounds/lives_3_lose.wav");
    if (lifeendmus == NULL)
    {
        printf("Failed to load beat music! SDL_mixer Error: %s\n", Mix_GetError());
        success = false;
    }

    yulumus = Mix_LoadMUS("sounds/yulu.wav");
    if (yulumus == NULL)
    {
        printf("Failed to load beat music! SDL_mixer Error: %s\n", Mix_GetError());
        success = false;
    }

    // Load sound effects

    endpage.loadFromFile("gameover.png");
    tasklist.loadFromFile("tasktable.png");
    tasktext[0].loadFromFile("1.png");   // hockey
    tasktext[1].loadFromFile("2.png");   // cricket
    tasktext[2].loadFromFile("3.png");   // football
    tasktext[3].loadFromFile("4.png");   // exhall
    tasktext[4].loadFromFile("5.png");   // CSC
    tasktext[5].loadFromFile("6.png");   // volleyball
    tasktext[6].loadFromFile("7.png");   // lawn tennis
    tasktext[7].loadFromFile("8.png");   // biotech lawn
    tasktext[8].loadFromFile("9.png");   // mittal sports
    tasktext[9].loadFromFile("10.png");  // LHC
    tasktext[10].loadFromFile("11.png"); // Library
    tasktext[11].loadFromFile("12.png"); // SAC
    tasktext[12].loadFromFile("13.png"); // OAT
    lifedisp.loadFromFile("getlife.png");
    scoredsip.loadFromFile("scoredisp.png");
    helpdisp.loadFromFile("helpdisp.png");
    coindisp.loadFromFile("coindisp.png");
    clientleft.loadFromFile("clientleft.png");
    lesslife.loadFromFile("lesslife.png");
    claimcardtext.loadFromFile("atm.png");
    debitcardtext.loadFromFile("debit.png");
    earncointext.loadFromFile("earncoin.png");
    idverifytext.loadFromFile("idverify.png");
    idshowtext.loadFromFile("mickeyid.png");
    winpage.loadFromFile("mickeywin.png");

    // set positions for buttons heredisp

    create.setPosition(480, 360);
    // join.setPosition(480, 510);
    taskclaim[0].setPosition(0, 0);     // hockey
    taskclaim[1].setPosition(0, 40);    // cricket
    taskclaim[2].setPosition(0, 80);    // football
    taskclaim[3].setPosition(0, 120);   // exhall
    taskclaim[4].setPosition(0, 160);   // CSC
    taskclaim[5].setPosition(0, 200);   // volleyball
    taskclaim[6].setPosition(60, 0);    // lawn tennis
    taskclaim[7].setPosition(60, 40);   // biotech lawn
    taskclaim[8].setPosition(60, 80);   // mittal sports
    taskclaim[9].setPosition(60, 120);  // LHC
    taskclaim[10].setPosition(60, 160); // Library
    taskclaim[11].setPosition(60, 200); // SAC
    taskclaim[12].setPosition(30, 240); // OAT
    getlife.setPosition(650, 620);      // lifes claim
    claimcard.setPosition(450, 620);
    debitcard.setPosition(450, 620);
    earncoin.setPosition(450, 620);
    idverify.setPosition(550, 520);
    idshow.setPosition(550, 520);

    return success;
}

void close()
{

    // Free loaded images
    gDotTexture1.free();
    gDotTexture2.free();
    gBGTexture.free();
    livestable.free();
    tasklist.free();

    // Free loaded images
    gTextTexture1.free();
    gTextTexture2.free();
    lifeText.free();
    timetext.free();
    coinText.free();
    lifedisp.free();
    helpdisp.free();
    coindisp.free();
    clientleft.free();
    lesslife.free();

    Mix_FreeMusic(taskmus);
    taskmus = NULL;
    Mix_FreeMusic(coinmus);
    coinmus = NULL;
    Mix_FreeMusic(startmus);
    startmus = NULL;
    Mix_FreeMusic(yulumus);
    yulumus = NULL;
    Mix_FreeMusic(lifeendmus);
    lifeendmus = NULL;

    // Free global font
    TTF_CloseFont(gFont);
    gFont = NULL;

    TTF_CloseFont(gFont2);
    gFont2 = NULL;
    // Destroy window
    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    gWindow = NULL;
    gRenderer = NULL;

    // Quit SDL subsystems
    Mix_Quit();
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

int main(int argc, char *args[])
{
    for (int i = 0; i < 13; i++)
    {
        claimtask[i] = false;
    }
    for (int i = 0; i < 13; i++)
    {
        taskreg[i] = false;
    }
    for (int i = 0; i < 6; i++)
    {
        food[i] = false;
    }
    if (!init())
    {
        printf("Failed to initialize!\n");
    }
    else
    {
        // Load media
        if (!loadMedia())
        {
            printf("Failed to load media!\n");
        }
        else
        {

            // Main loop flag
            bool quit = false;

            // Event handler
            SDL_Event e;

            // The dot that will be moving around on the screen
            Dot dot;

            // The camera area
            SDL_Rect camera = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
            SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
            SDL_RenderClear(gRenderer);

            SDL_Rect helpbox = {0, 0, 700, 700};
            SDL_Rect livesbox = {0, 0, 130, 250};

            // LTimer fpsTimer;

            // The frames per second cap timer
            LTimer capTimer;
            LTimer fpsTimer;

            int countedFrames = 0;
            fpsTimer.start();

            // While application is running

            int serverinit_res = serverinit();
            //
            while (!quit)
            {
                capTimer.start();

                // Handle events on queue
                while (SDL_PollEvent(&e) != 0)
                {
                    // User requests quit
                    if (e.type == SDL_QUIT)
                    {
                        string serverfn_res = clientfn(0, 0, 0, 0, 0);
                        quit = true;
                        gameend = true;
                    }
                    if (e.type == SDL_KEYDOWN)
                    {
                        switch (e.key.keysym.sym)
                        {
                        case SDLK_h:
                            disp = 1;
                            break;
                        case SDLK_ESCAPE:
                            disp = 2;
                            break;
                        }
                        if (byulu && e.key.keysym.sym == SDLK_s)
                        {
                            dottype = 1;
                            Mix_PlayMusic(yulumus, 1);
                            DOT_VEL = 15;
                            coins -= 15;
                        }
                        if (byulu && e.key.keysym.sym == SDLK_e)
                        {
                            Mix_PlayMusic(yulumus, 1);
                            dottype = 0;
                            DOT_VEL = 5;
                        }
                    }
                    // Handle input for the dot
                    dot.handleEvent(e);
                    create.handleEvent1(&e);
                    // join.handleEvent2(&e);
                    getlife.handleEventLife(&e);

                    for (int j = 0; j < 13; j++)
                    {
                        taskclaim[j].handleEventtask(&e, j);
                    }
                    debitcard.handleEventcarddebit(&e);
                    claimcard.handleEventcardclaim(&e);
                    earncoin.handleearncoin(&e);
                    idverify.handleEventidclaim(&e);
                    idshow.handleEventidshow(&e);
                }

                // Move the dot
                dot.move();

                // Center the camera over the dot
                camera.x = (dot.getPosX() + Dot::DOT_WIDTH / 2) - SCREEN_WIDTH / 2;
                camera.y = (dot.getPosY() + Dot::DOT_HEIGHT / 2) - SCREEN_HEIGHT / 2;
                bool b = true;

                // for the player on the system
                if (dottype == 0) // dottype is 0 when the current player is not on yulu
                {
                    switch (direction)
                    {
                    case 1:
                        b = gDotTexture1.loadFromFile("players/p2r.png");

                        break;
                    case 2:
                        b = gDotTexture1.loadFromFile("players/p2u.png");

                        break;
                    case 3:
                        b = gDotTexture1.loadFromFile("players/p2l.png");

                        break;
                    case 4:
                        b = gDotTexture1.loadFromFile("players/p2d.png");

                        break;
                    }
                }
                else // when player is on yulu
                {
                    switch (direction)
                    {
                    case 1:
                        b = gDotTexture1.loadFromFile("players/p2ry.png");

                        break;
                    case 2:
                        b = gDotTexture1.loadFromFile("players/p2uy.png");

                        break;
                    case 3:
                        b = gDotTexture1.loadFromFile("players/p2ly.png");

                        break;
                    case 4:
                        b = gDotTexture1.loadFromFile("players/p2dy.png");

                        break;
                    }
                }

                // for the opponent
                if (op_dottype == 0)
                {
                    switch (op_direction)
                    {
                    case 1:
                        b = gDotTexture2.loadFromFile("players/p1r.png");

                        break;
                    case 2:
                        b = gDotTexture2.loadFromFile("players/p1u.png");

                        break;
                    case 3:
                        b = gDotTexture2.loadFromFile("players/p1l.png");

                        break;
                    case 4:
                        b = gDotTexture2.loadFromFile("players/p1d.png");

                        break;
                    }
                }
                else
                {
                    switch (op_direction)
                    {
                    case 1:
                        b = gDotTexture2.loadFromFile("players/p1ry.png");

                        break;
                    case 2:
                        b = gDotTexture2.loadFromFile("players/p1uy.png");

                        break;
                    case 3:
                        b = gDotTexture2.loadFromFile("players/p1ly.png");

                        break;
                    case 4:
                        b = gDotTexture2.loadFromFile("players/p1dy.png");

                        break;
                    }
                }

                // Keep the camera in bounds
                if (camera.x < 0)
                {
                    camera.x = 0;
                }
                if (camera.y < 0)
                {
                    camera.y = 0;
                }
                if (camera.x > LEVEL_WIDTH - camera.w)
                {
                    camera.x = LEVEL_WIDTH - camera.w;
                }
                if (camera.y > LEVEL_HEIGHT - camera.h)
                {
                    camera.y = LEVEL_HEIGHT - camera.h;
                }

                // Clear screen

                switch (disp)
                {
                case 0:
                    // render welcome
                    welcome.render(0, 0, &camera);
                    create.render1();
                    // join.render2();
                    break;

                case 1: // display help box
                    hbox.render(250, 0, &helpbox);
                    break;

                case 2: // back to game
                    disp = 3;
                    break;

                case 3:
                    // Render background and sprite; basically load the game on pressing enter key
                    Mix_PlayMusic(startmus, 1);
                    startmus = NULL;
                    gBGTexture.render(0, 0, &camera);
                    dot.render(camera.x, camera.y);
                    livestable.render(1050, 450, &livesbox);
                    tasklist.render(0, 0);
                    scoredsip.render(543, 0);
                    createbtn.free();
                    // joinbtn.free();
                    helpdisp.render(0, 300);
                    coindisp.render(10, 650);

                    if ((onbank == 0) || (onbank == 1 && !wanttoearn))
                    {
                        earncoin.renderearncoin();
                    }
                    if (onoat == 1 || onoat == 2 && !idclaimed)
                    {
                        idverify.renderidclaim();
                    }
                    if ((!idshown && idclaimed) && (onoat == 2 || onoat == 3))
                    {
                        idshow.renderidshow();
                    }

                    // cout << "not played" << endl;

                    // cout << "played " << endl;

                    for (int i = 0; i < 13; i++)
                    {
                        if (!claimtask[i])
                            taskclaim[i].rendertask(i);
                    }
                    getlife.renderlife();
                    gFont = TTF_OpenFont("scorefont.ttf", 22);
                    gFont2 = TTF_OpenFont("scorefont.ttf", 30);

                    // Render text
                    SDL_Color textColor = {0xFF, 0XFF, 0XFF};
                    SDL_Color text2 = {0, 0, 0};
                    string str1 = "MICKEY: ";
                    string str2 = to_string(myscore);
                    string strf = str1 + str2;
                    string str3 = "MINNIE: ";
                    string str4 = to_string(opscore);
                    string strf2 = str3 + str4;
                    string str5 = to_string(lives);
                    string str6 = to_string(coins);
                    string str7 = "TIME: ";
                    string str8 = to_string(time1);
                    bool sc1 = gTextTexture1.loadFromRenderedText(strf, textColor, gFont);
                    bool sc2 = gTextTexture2.loadFromRenderedText(strf2, textColor, gFont);
                    bool sc3 = lifeText.loadFromRenderedText(str5, text2, gFont2);
                    bool sc4 = coinText.loadFromRenderedText(str6, text2, gFont2);
                    bool sc5 = timetext.loadFromRenderedText(str7 + str8, textColor, gFont);
                    // Render current frame
                    gTextTexture1.render(550, 25);
                    gTextTexture2.render(550, 47);
                    lifeText.render(780, 665);
                    coinText.render(140, 663);
                    timetext.render(580, 70);

                    break;
                }

                if (onbank == 1 && wanttoearn)
                {
                    // cout << "I am on bank" << endl;
                    claimcard.rendercardclaim();
                }
                if (onbank == 2 || onbank == 3)
                {
                    debitcard.rendercarddebit();
                }

                float avgFPS = countedFrames / (fpsTimer.getTicks() / 1000.f);
                if (avgFPS > 2000000)
                {
                    avgFPS = 0;
                }
                // cout << "Average time " << avgFPS << endl;
                // cout << "Frame Rate " << countedFrames << endl;

                countedFrames++;

                if (countedFrames % 200 == 0)
                {
                    time1 += 3;
                    lives -= 1;
                }

                // If frame finished early
                int frameTicks = capTimer.getTicks();
                if (frameTicks < SCREEN_TICKS_PER_FRAME)
                {
                    // Wait remaining time
                    SDL_Delay(SCREEN_TICKS_PER_FRAME - frameTicks);
                }
                if (lives == 3)
                {
                    Mix_PlayMusic(lifeendmus, 1);
                }
                if (lives == 0)
                {
                    gameend = true;
                }
                if (lives <= 5)
                {
                    lesslife.render(550, 530);
                }
                if (myscore == 470 || opscore == 470)
                {
                    gameend = true;
                }

                if (gameend)
                {

                    if (myscore > opscore)
                    {
                        winpage.loadFromFile("minniewin.png");
                    }
                    else
                    {
                        winpage.loadFromFile("mickeywin.png");
                    }

                    winpage.render(0, 0);
                    // SDL_Delay(4000);
                    // quit = true;
                }

                SDL_RenderPresent(gRenderer);
            }
        }
    }

    // Free resources and close SDL

    // SDL_RenderPresent(gRenderer);
    close();
    Mix_HaltMusic();

    return 0;
}