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

//
//gcc main.c  -L lib/ -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL lib/libraylib.a -o Incognitoaim
//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);// |  FLAG_WINDOW_UNDECORATED);
    const int screenWidth = 1280;
    const int screenHeight = 800;

    InitWindow(screenWidth, screenHeight, "IncognitoAim");

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
        
        //toggle fullscreen mode
       if(IsKeyPressed(KEY_P)){
         ToggleFullscreen();         
       }
       bg_dest_rect.width = GetScreenWidth();
       bg_dest_rect.height = GetScreenHeight();

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

             if (image_loaded) {
                DrawTexturePro(bg_texture, bg_source_rect,bg_dest_rect,bg_pos,0.0f, WHITE);  // Draw image at (200,150)
            } else {
                DrawText("Drop an image file here", 300, 280, 20, BLACK);
            }

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