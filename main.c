/*******************************************************************************************
*
*   raylib [core] example - Mouse input
*
*   Example complexity rating: [★☆☆☆] 1/4
*
*   Example originally created with raylib 1.0, last time updated with raylib 5.5
*
*   Example licensed under an unmodified zlib/libpng license, which is an OSI-certified,
*   BSD-like license that allows static linking with closed source software
*
*   Copyright (c) 2014-2025 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include "include/raylib.h"
#include "include/raymath.h"


typedef enum{
    MAIN, REACTION, TRACK
}AppState;

typedef enum{
    PLAY,PAUSE,END
}GameState;
typedef struct{
    Vector2 pos;
    int fontScale;
    int spaccing;
    Color fontColor;
}MainMenuData;
typedef struct {
    AppState appState;
    GameState gameState;
    int score;
}Game;

Game game;
MainMenuData mainMenuData;

int screenWidth = 1280;
int screenHeight = 800;

void UpdateAndDrawApp();
void UpdateGame();
void UpdateMainMenu();
void DrawMainMenu();
void UpdateReactionGame();
void DrawReactionGame();
void UpdateTrackGame();
void DrawTrackGame();
bool CheckPointInTextBox(Vector2 mousePos, Vector2 textPos, int width, int height);
//gcc main.c  -L lib/ -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL lib/libraylib.a -o Incognitoaim
//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);// |  FLAG_WINDOW_UNDECORATED);
    

    InitWindow(screenWidth, screenHeight, "IncognitoAim");

    game = (Game){ MAIN, END, 0};
    mainMenuData = (MainMenuData){ {0,0}, 0, 0,BLACK};
    float dt = 0.0;
    float fps = 0.0;

    Texture2D bg_texture;
    Rectangle bg_source_rect = {0};
    Rectangle bg_dest_rect = {0.0f,0.0f,screenWidth,screenHeight};
    Vector2 bg_pos = {0.0f,0.0f};
    bool image_loaded = false;

    Vector2 ballPosition = { -100.0f, -100.0f };
    Color ballColor = DARKBLUE;
    int isCursorHidden = 0;

    // /SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //---------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Logic
        //----------------------------------------------------------------------------------
        
        dt = GetFrameTime();
        fps = GetFPS();
        ballPosition = GetMousePosition();

        screenWidth = GetScreenWidth();
        screenHeight = GetScreenHeight();
        //toggle fullscreen mode
       if(IsKeyPressed(KEY_P)){
         ToggleFullscreen();         
       }
       bg_dest_rect.width = screenWidth;
       bg_dest_rect.height = screenHeight;

        //check if file is dropped and load it to background
        if (IsFileDropped()) {
            FilePathList droppedFiles = LoadDroppedFiles();
            if (droppedFiles.count > 0) {
                // Load the first dropped file as a texture
                if (image_loaded) UnloadTexture(bg_texture);  // Unload previous texture
                Image image = LoadImage(droppedFiles.paths[0]);
                bg_texture = LoadTextureFromImage(image);
                bg_source_rect.width = bg_texture.width;
                bg_source_rect.height = bg_texture.height;
                UnloadImage(image);
                image_loaded = true;

            }
            UnloadDroppedFiles(droppedFiles);
        }
        //----------------------------------------------------------------------------------

        // Render
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(DARKGRAY);
            UpdateAndDrawApp();
             if (image_loaded) {
                DrawTexturePro(bg_texture, bg_source_rect,bg_dest_rect,bg_pos,0.0f, WHITE);  // Draw image at (200,150)
            } else {
                DrawText("Drop an image file here", 300, 280, 20, BLACK);
            }
            UpdateAndDrawApp();
            
            DrawRectangle(0,0,250,54,BLACK);
            DrawText(TextFormat("Delta Time: %02f", dt), 4, 4, 25, RED);
            DrawText(TextFormat("fps: %02f", fps), 4, 25, 25, RED);
            DrawText(TextFormat("w: %02f, h:%02f", bg_dest_rect.width,bg_dest_rect.height), 4, 50, 25, RED);
        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadTexture(bg_texture);
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

void UpdateAndDrawApp()
{
    switch (game.appState)
    {
    case MAIN:
        UpdateMainMenu();
        DrawMainMenu();
        break;
    case REACTION:
        UpdateReactionGame();
        DrawReactionGame();
        break;
    case TRACK:
        UpdateTrackGame();
        DrawTrackGame();
        break;
    
    default:
        break;
    }
}

void UpdateGame()
{
    switch (game.gameState)
    {
    case PLAY:
        break;
    case PAUSE:
        break;
    case END:
        break;
    
    default:
        break;
    }
}
bool boxePressed = false;
void UpdateMainMenu()
{
    mainMenuData.pos.x = (screenWidth / 2) - 250;
    mainMenuData.pos.y = (screenHeight / 2)-50;
    mainMenuData.fontScale = 50;
    mainMenuData.spaccing = 20;
    boxePressed = false;
    //if mouse1 down
    if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        Vector2 MousePos = GetMousePosition();
        int textWidth = MeasureText("Reaction Practice",mainMenuData.fontScale);
        if(CheckPointInTextBox(MousePos,mainMenuData.pos,textWidth,mainMenuData.fontScale))
        {//check if mouse pressed int Reaction box
            boxePressed = true;
        }
        Vector2 nextPos = {mainMenuData.pos.x,mainMenuData.pos.y + mainMenuData.fontScale + mainMenuData.spaccing};
        if(CheckPointInTextBox(MousePos,nextPos,textWidth,mainMenuData.fontScale))
        {//check if pouse pressed int traking mox
            boxePressed = true;
        }

    }
    //check all button hitboxes for collision
}

void DrawMainMenu()
{
    if(!boxePressed){
        DrawText("Reaction Practice",mainMenuData.pos.x,mainMenuData.pos.y,mainMenuData.fontScale,mainMenuData.fontColor);
        DrawText("Tracking Practice",mainMenuData.pos.y,mainMenuData.pos.y + mainMenuData.fontScale + mainMenuData.spaccing,mainMenuData.fontScale,mainMenuData.fontColor);
    }
    

}
void UpdateReactionGame()
{

}
void DrawReactionGame()
{

}
void UpdateTrackGame()
{

}
void DrawTrackGame()
{

}

bool CheckPointInTextBox(Vector2 mousePos, Vector2 textPos, int width, int height)
{
    return mousePos.x >textPos.x && mousePos.x < textPos.x + width &&
    mousePos.y> textPos.y && mousePos.y <textPos.y + height;

}