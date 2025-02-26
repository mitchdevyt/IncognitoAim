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
#define RAYGUI_IMPLEMENTATION
#include "include/raygui.h"

#define NUM_BALL_ROWS  5
#define NUM_BALL_COLS  5
#define NUM_REACTION_BALLS NUM_BALL_ROWS*NUM_BALL_COLS
#define NUM_BALLS_ON_SCREEN 3

#pragma region Structs and enums
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
    Rectangle reactionRect;
    Rectangle trackingRect;
    Color fontColor;
}MainMenuData;
typedef struct {
    AppState appState;
    GameState gameState;
    int score;
}Game;
typedef struct{
    float sensitivity;
    float pitch;
    float yaw;
    float pitchLimit;
    float yawLimit;
}CameraSettings;
typedef struct{
    bool balls[NUM_REACTION_BALLS];
    Vector3 ballPositions[NUM_REACTION_BALLS];
    float ballSize;
    int ballOffset;
    Vector3 ballTopLeftPosition;
    int score;
    int activeBallIndexs[3];
    GameState gameState;
}ReactionGame;
#pragma endregion
#pragma region Forward Functions
void UpdateAndDrawApp();
void UpdateGame();
void UpdateMainMenu();
void DrawMainMenu();
void ResetReactionGame();
void UpdateReactionGame();
void DrawReactionGame();
bool IsInActiveArray(int value);
int GetRandomBall();
void PickRandomStartBalls();
void PickRandomStartBalls();
void UpdateTrackGame();
void DrawTrackGame();
#pragma endregion
#pragma region Globals
Game game;
MainMenuData mainMenuData;
CameraSettings camSettings;
Camera camera;
ReactionGame reactionGame;

int screenWidth = 1280;
int screenHeight = 800;
#pragma endregion
#pragma region Main
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
    ResetReactionGame();

    game = (Game){ MAIN, END, 0};
    mainMenuData = (MainMenuData){ {0,0}};

    camSettings.sensitivity = 0.001f;
    camSettings.pitch = 0.0f;  // Up/Down angle
    camSettings.yaw = 0.0f;    // Left/Right angle
    camSettings.pitchLimit = PI / 3.0f; // Limit pitch (Up/Down) to ~60 degrees
    camSettings.yawLimit = PI / 3.0f; // Limit pitch (Up/Down) to ~60 degrees

    float dt = 0.0;
    float fps = 0.0;

    camera = (Camera){ { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 100.0f }, { 0.0f, 1.0f, 0.0f }, 50.0f, 0 };

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
        dt = GetFrameTime();
        fps = GetFPS();

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
        //Render
        BeginDrawing();

            ClearBackground(DARKGRAY);

             if (image_loaded) {
                DrawTexturePro(bg_texture, bg_source_rect,bg_dest_rect,bg_pos,0.0f, WHITE);  // Draw image at (200,150)
            } else {
                DrawText("Drop an image file here", screenWidth * .4, screenHeight  *.2, 20, BLACK);
            }
            UpdateAndDrawApp();

            DrawRectangle(0,0,250,54,BLACK);
            DrawText(TextFormat("Delta Time: %02f", dt), 4, 4, 25, RED);
            DrawText(TextFormat("fps: %02f", fps), 4, 25, 25, RED);
            DrawText(TextFormat("+"),screenWidth/2,screenHeight/2,20,BLACK);
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
#pragma endregion
#pragma region Main Loops
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
#pragma endregion
#pragma region MainMenu
void UpdateMainMenu()
{
    mainMenuData.pos.x = screenWidth * .35;
    mainMenuData.pos.y = screenHeight * .35;
    mainMenuData.fontScale = screenHeight * .05 ;
    mainMenuData.spaccing = mainMenuData.fontScale + (mainMenuData.fontScale * .5);
    mainMenuData.fontColor = BLACK;
    
    mainMenuData.reactionRect.x = mainMenuData.pos.x;
    mainMenuData.reactionRect.y = mainMenuData.pos.y;
    mainMenuData.reactionRect.width =  MeasureText("Reaction Practice", mainMenuData.fontScale);
    mainMenuData.reactionRect.height = mainMenuData.fontScale;

    mainMenuData.trackingRect.x = mainMenuData.pos.x;
    mainMenuData.trackingRect.y = mainMenuData.pos.y + mainMenuData.spaccing;
    mainMenuData.trackingRect.width = MeasureText("Tracking Practice", mainMenuData.fontScale);
    mainMenuData.trackingRect.height = mainMenuData.fontScale;
}
void DrawMainMenu()
{
    
    DrawRectangleRec(mainMenuData.reactionRect, RED);
    DrawRectangleRec(mainMenuData.trackingRect, BLUE);
    DrawText("Reaction Practice",mainMenuData.pos.x,mainMenuData.pos.y,mainMenuData.fontScale,mainMenuData.fontColor);
    DrawText("Tracking Practice",mainMenuData.trackingRect.x,mainMenuData.trackingRect.y,mainMenuData.fontScale,mainMenuData.fontColor);
    
    if (GuiLabelButton(mainMenuData.reactionRect, ""))
    {
        game.appState = REACTION;
        DisableCursor();
    }
    if (GuiLabelButton(mainMenuData.trackingRect, ""))
    {

    }
    
}
#pragma endregion
#pragma region  Reaction Game
void ResetReactionGame()
{
    int x=0;
    int y=0;
    for(int i=0;i<NUM_REACTION_BALLS;++i){
        reactionGame.balls[i] = false;
        reactionGame.ballPositions[i] = (Vector3){};
    }
    reactionGame.score = 0;
    PickRandomStartBalls();

}
void PickRandomStartBalls()
{
    for (int i = 0; i < NUM_BALLS_ON_SCREEN; i++) {
        reactionGame.activeBallIndexs[i] = GetRandomBall();
    }   
}
int GetRandomBall()
{
    int newNum;
    do {
        newNum = GetRandomValue(0, NUM_REACTION_BALLS);
    } while (IsInActiveArray(newNum)); // Ensure uniqueness
    return newNum;
}
bool IsInActiveArray(int value){
    for(int i=0;i<NUM_BALLS_ON_SCREEN;i++){
        if(reactionGame.activeBallIndexs[i] == value)
            return true;
    }
    return false;
}
void UpdateReactionGame()
{
    //CAMERA UPDATE
    // Get mouse movement
    Vector2 mouseDelta = GetMouseDelta();
    camSettings.yaw -= mouseDelta.x * camSettings.sensitivity; // Rotate left/right
    camSettings.pitch -= mouseDelta.y * camSettings.sensitivity; // Rotate up/down

    // Clamp pitch so the camera doesn't look too far up/down
    camSettings.pitch = Clamp(camSettings.pitch, -camSettings.pitchLimit, camSettings.pitchLimit);

    camSettings.yaw = Clamp(camSettings.yaw,-camSettings.yawLimit,camSettings.yawLimit);

    // Calculate new camera target
    Vector3 forward = {
        cosf(camSettings.pitch) * sinf(camSettings.yaw), // X-axis rotation
        sinf(camSettings.pitch),             // Y-axis rotation
        cosf(camSettings.pitch) * cosf(camSettings.yaw)  // Z-axis rotation
    };

    // Set the camera target relative to position
    camera.target = (Vector3){
        camera.position.x + forward.x,
        camera.position.y + forward.y,
        camera.position.z + forward.z
    };
    //RAY CAST
     // Get the center of the screen
    Vector2 screenCenter = { screenWidth / 2, screenHeight / 2 };

    // Generate a ray from the center of the screen
    //Calculate ball positions
    reactionGame.ballSize = 30;
    reactionGame.ballOffset = 50;
    float increment = (reactionGame.ballSize/2) + reactionGame.ballOffset;
    float startX = (reactionGame.ballSize/2) + reactionGame.ballOffset *2 + reactionGame.ballSize*2;
    startX = startX;
    float startY = (reactionGame.ballSize/2) + reactionGame.ballOffset * 2 + reactionGame.ballSize * 2;
    startY = -startY;
    int counter = 0;
    int row = 0;
    
    float y = startY;
    for(int i =0; i <NUM_BALL_COLS;++i){
        float x = startX;
        for(int j =0;j<NUM_BALL_ROWS;++j){
            reactionGame.ballPositions[counter] = (Vector3){x,y,700};
            ++counter;
            x -= increment;
        }
        y+=increment;            
    }
    Ray ray = GetMouseRay(screenCenter, camera);
    RayCollision col;
    if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)){
        for(int i =0; i <NUM_BALLS_ON_SCREEN;++i){
            col = GetRayCollisionSphere(ray, reactionGame.ballPositions[reactionGame.activeBallIndexs[i]],reactionGame.ballSize);
            if(col.hit)
            {
                //TODO: update scores when ball hit
                reactionGame.activeBallIndexs[i] = GetRandomBall();
            }
        }
    }
}
void DrawReactionGame()
{
    BeginMode3D(camera);
    for(int i =0;i<NUM_BALLS_ON_SCREEN;++i){
        DrawSphere(reactionGame.ballPositions[reactionGame.activeBallIndexs[i]],reactionGame.ballSize, GREEN);
    }
    Vector3 pos = {0,0,100};
    Vector3 size = {200,100,300};
    DrawCubeWiresV(pos,size,BLACK);
    EndMode3D();
}
#pragma endregion
#pragma region Tracking game
void UpdateTrackGame()
{

}
void DrawTrackGame()
{

}
#pragma endregion