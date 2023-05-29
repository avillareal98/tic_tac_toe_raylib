/**********************************************************************************************
*
*   raylib - Advance Game template
*
*   Gameplay Screen Functions Definitions (Init, Update, Draw, Unload)
*
*   Copyright (c) 2014-2022 Ramon Santamaria (@raysan5)
*
*   This software is provided "as-is", without any express or implied warranty. In no event
*   will the authors be held liable for any damages arising from the use of this software.
*
*   Permission is granted to anyone to use this software for any purpose, including commercial
*   applications, and to alter it and redistribute it freely, subject to the following restrictions:
*
*     1. The origin of this software must not be misrepresented; you must not claim that you
*     wrote the original software. If you use this software in a product, an acknowledgment
*     in the product documentation would be appreciated but is not required.
*
*     2. Altered source versions must be plainly marked as such, and must not be misrepresented
*     as being the original software.
*
*     3. This notice may not be removed or altered from any source distribution.
*
**********************************************************************************************/

#include <stdlib.h>
#include <time.h>
#include <stdio.h>

#include "raylib.h"
#include "screens.h"

//----------------------------------------------------------------------------------
// Module Variables Definition (local)
//----------------------------------------------------------------------------------

static int framesCounter = 0;
static int finishScreen = 0;

#define COLS 3
#define ROWS 3

#define screen_w 600
#define screen_h 600

#define cellWidth (screen_w / COLS)
#define cellHeight (screen_h / ROWS)

const char* playerXWins = "PLAYER X WINS!";
const char* playerOWins = "PLAYER O WINS!";
const char* playerDraw = "IT'S A DRAW!";
const char* pressRToRestart = "Press 'r' to play again!";
const char* playerXTurnPrompt = "PLAYER X GO";
const char* playerOTurnPrompt = "PLAYER O GO";

Texture2D atlas;

typedef enum CellValue
{
    EMPTY,
    X,
    O
} CellValue;

typedef struct Cell
{
    int i;
    int j;
    CellValue value;
} Cell;

Cell grid[COLS][ROWS];

int tilesOccupied;

typedef enum GameState
{
    PLAYERX,
    PLAYERO,
    PLAYERDRAW,
    END
} GameState;

GameState state;
CellValue winner;

float timeGameStarted;
float timeGameEnded;

void CellDraw(Cell);
bool IndexIsValid(int, int);
bool CellMark(int, int, CellValue);
void GridInit(void);
void GameInit(void);
bool GameIsOver(CellValue*);

//----------------------------------------------------------------------------------
// Gameplay Screen Functions Definition
//----------------------------------------------------------------------------------

// Gameplay Screen Initialization logic
void InitGameplayScreen(void)
{
    // TODO: Initialize GAMEPLAY screen variables here!
    framesCounter = 0;
    finishScreen = 0;

    atlas = LoadTexture("resources/tictactoe.png");

    GameInit();
}

// Gameplay Screen Update logic
void UpdateGameplayScreen(void)
{
    // TODO: Update GAMEPLAY screen variables here!
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        Vector2 mPos = GetMousePosition();
        int indexI = mPos.x / cellWidth;
        int indexJ = mPos.y / cellHeight;

        if (state != END && IndexIsValid(indexI, indexJ))
        {
            if(CellMark(indexI, indexJ, state == PLAYERX ? X : O))
            {
                if (!GameIsOver(&winner))
                {
                    state = (state == PLAYERX) ? PLAYERO : PLAYERX;
                    if (tilesOccupied > 8)
                    {
                        state = END;
                    }
                }
                else
                {
                    state = END;
                }
                PlaySound(digSound);
            }
        }
    }

    if (state == END && IsKeyPressed(KEY_R))
    {
        GameInit();
    }
}

// Gameplay Screen Draw logic
void DrawGameplayScreen(void)
{
    // TODO: Draw GAMEPLAY screen here!
    for (int i = 0; i < COLS; ++i)
    {
        for (int j = 0; j < ROWS; ++j)
        {
            CellDraw(grid[i][j]);
        }
    }
    if (state == END)
    {
        const char* text;
        if (winner == X)
        {
            text = playerXWins;
        }
        else if (winner == O)
        {
            text = playerOWins;
        }
        else
        {
            text = playerDraw;
        }

        DrawRectangle(0, 0, screen_w, screen_h, Fade(WHITE, 0.8f));
        DrawText(text, screen_w / 2 - MeasureText(text, 40) / 2, screen_h / 2 - 10, 40, DARKGRAY);
        DrawText(pressRToRestart, screen_w / 2 - MeasureText(pressRToRestart, 20) / 2, screen_h * 0.75f - 10, 20, DARKGRAY);

        int minutes = (int)(timeGameEnded - timeGameStarted) / 60;
        int seconds = (int)(timeGameEnded - timeGameStarted) % 60;
        DrawText(TextFormat("Time played: %d minutes, %d seconds.", minutes, seconds), 20, screen_h - 40, 20, DARKGRAY);
    }
    else
    {
        const char* text = (state == PLAYERX) ? playerXTurnPrompt : playerOTurnPrompt;
        
        Vector2 messagePosition = (Vector2){screen_w / 2, 40};

        int measure = MeasureText(text, 40);
        Rectangle textRect = (Rectangle){messagePosition.x - measure / 2 - 40, messagePosition.y - 40, measure + 80, 80};

        DrawRectangleRec(textRect, Fade(WHITE, 0.8f));
        DrawText(text, messagePosition.x - measure / 2, messagePosition.y - 20, 40, DARKGRAY);
    }
}

// Gameplay Screen Unload logic
void UnloadGameplayScreen(void)
{
    // TODO: Unload GAMEPLAY screen variables here!
    UnloadTexture(atlas);
}

// Gameplay Screen should finish?
int FinishGameplayScreen(void)
{
    return finishScreen;
}

void CellDraw(Cell cell)
{
    Rectangle dest = {cell.i * cellWidth, cell.j * cellHeight, cellWidth, cellHeight};

    Rectangle source = (Rectangle){0, 0, 0, 0};
    switch (cell.value)
    {
        case X:
            source = (Rectangle){0, 0, 100, 100};
            DrawTexturePro(atlas, source, dest, (Vector2){0, 0}, 0, RED);
            break;
        case O:
            source = (Rectangle){100, 0, 100, 100};
            DrawTexturePro(atlas, source, dest, (Vector2){0, 0}, 0, BLUE);
            break;
        default:
            break;
    }

    DrawRectangleLines(cell.i * cellWidth, cell.j * cellHeight, cellWidth, cellHeight, BLACK);
}

bool IndexIsValid(int i, int j)
{
    return (i >= 0 && i < COLS && j >= 0 && j < ROWS);
}

bool CellMark(int i, int j, CellValue value)
{
    if (grid[i][j].value != EMPTY || value == EMPTY)
    {
        return false;
    }

    grid[i][j].value = value;
    tilesOccupied++;
    printf("check: %d \n", (int)tilesOccupied);

    return true;
}

void GridInit(void)
{
    for (int i = 0; i < COLS; ++i)
    {
        for (int j = 0; j < ROWS; ++j)
        {
            grid[i][j] = (Cell) 
            {
                .i = i, 
                .j = j,
                .value = EMPTY
            };
        }
    }
}

void GameInit(void)
{
    GridInit();
    state = PLAYERX;
    tilesOccupied = 0;
    timeGameStarted = GetTime();
}

bool GameIsOver(CellValue* winner)
{
    // check the diagonals
    // TL BR
    if (grid[0][0].value != EMPTY && grid[0][0].value == grid[1][1].value && grid[1][1].value == grid[2][2].value)
    {
        *winner = grid[0][0].value;
        return true;
    }

    // BL TR
    if (grid[0][2].value != EMPTY && grid[0][2].value == grid[1][1].value && grid[1][1].value == grid[2][0].value)
    {
        *winner = grid[0][2].value;
        return true;
    }

    // check each horizontal line
    // TL TR
    if (grid[0][0].value != EMPTY && grid[0][0].value == grid[1][0].value && grid[1][0].value == grid[2][0].value)
    {
        *winner = grid[0][0].value;
        return true;
    }

    // LM RM
    if (grid[0][1].value != EMPTY && grid[0][1].value == grid[1][1].value && grid[1][1].value == grid[2][1].value)
    {
        *winner = grid[0][1].value;
        return true;
    }

    // BL BR
    if (grid[0][2].value != EMPTY && grid[0][2].value == grid[1][2].value && grid[1][2].value == grid[2][2].value)
    {
        *winner = grid[0][2].value;
        return true;
    }

    // check each vertical line
    // TL BL
    if (grid[0][0].value != EMPTY && grid[0][0].value == grid[0][1].value && grid[0][1].value == grid[0][2].value)
    {
        *winner = grid[0][0].value;
        return true;
    }

    // TM BM
    if (grid[1][0].value != EMPTY && grid[1][0].value == grid[1][1].value && grid[1][1].value == grid[1][2].value)
    {
        *winner = grid[1][0].value;
        return true;
    }

    // TR BR
    if (grid[2][0].value != EMPTY && grid[2][0].value == grid[2][1].value && grid[2][1].value == grid[2][2].value)
    {
        *winner = grid[2][0].value;
        return true;
    }

    *winner = EMPTY;
    return false;
}