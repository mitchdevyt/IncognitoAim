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

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

#define NUM_BALL_ROWS  5
#define NUM_BALL_COLS  5
#define NUM_REACTION_BALLS 25
#define NUM_BALLS_ON_SCREEN 3

#pragma region Structs and enums
typedef enum{
    MAIN, REACTION, TRACK
}AppState;

typedef enum{
    START,PLAY,HIDE,END
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
    int activeBallIndexs[3];
    GameState gameState;
    Color ballColor;
    int score;
    int numClicks;
    float accuracy;
    float timer;
    GameState previousState;
    bool hideGame;
}ReactionGame;
typedef struct{
    int fontScale;
    int spaccing;
    Vector2 pos;
    float mouseSensitivity;
    Vector2 sliderSize;
    Rectangle ballSizeButton;
    float ballColorRectSize;
    int ballColorIndex;
}ReactionGameStartMenu;
typedef struct{
    Texture2D bg_texture;
    Rectangle bg_source_rect;
    Rectangle bg_dest_rect;
    Vector2 bg_pos;
    bool image_loaded;
}BackgroundImage;
#pragma endregion
#pragma region Forward Functions
void UpdateGame();
void UpdateAndDrawApp();
void UpdateGame();
void UpdateMainMenu();
void DrawMainMenu();
void ResetReactionGame();
void UpdateReactionGame();
void UpdateReactionGameState();
void DrawReactionGame();
void DrawReactionStartMenu();
void UpdateReactionStartMenu();
void RenderReactinGameState();
void ReactionGameCheckForHide();
void DrawReactionGameEnd();
void UpdateReactionGameEnd();
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
ReactionGameStartMenu reactionStart;
BackgroundImage bgImage;

int screenWidth = 1280;
int screenHeight = 800;
float dt = 0.0;
float fps = 0.0;
const int numBallColors = 9;
struct Color ballColors[] = {RED,GREEN,BLUE,PINK,PURPLE,ORANGE,YELLOW,BLACK,WHITE};
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

    game = (Game){ REACTION, START, 0};
    mainMenuData = (MainMenuData){ {0,0}};
    reactionStart = (ReactionGameStartMenu){};
    reactionStart.mouseSensitivity = 2.;
    reactionGame.ballSize = 1;
    reactionGame.hideGame = false;
    camSettings.sensitivity = 0.001f;
    //camSettings.sensitivity = 0.001f;
    camSettings.pitch = 0.0f;  // Up/Down angle
    camSettings.yaw = 0.0f;    // Left/Right angle
    camSettings.pitchLimit = PI / 3.0f; // Limit pitch (Up/Down) to ~60 degrees
    camSettings.yawLimit = PI / 3.0f; // Limit pitch (Up/Down) to ~60 degrees



    camera = (Camera){ { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 100.0f }, { 0.0f, 1.0f, 0.0f }, 50.0f, 0 };
    
    bgImage.bg_source_rect = (Rectangle){0};
    bgImage.bg_dest_rect = (Rectangle){0.0f,0.0f,screenWidth,screenHeight};
    bgImage.bg_pos = (Vector2){0.0f,0.0f};
    bgImage.image_loaded = false;


    int isCursorHidden = 0;

    // /SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //---------------------------------------------------------------------------------------

    // Main game loop
    #if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateGame, 0, 1);
    #else
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        UpdateGame();
    }
    #endif
    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadTexture(bgImage.bg_texture);
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------
    
    return 0;
}
void UpdateGame()
{
    // Logic
    dt = GetFrameTime();
    fps = GetFPS();
    
    screenWidth = GetScreenWidth();
    screenHeight = GetScreenHeight();
    //toggle fullscreen mode
    //if(IsKeyPressed(KEY_P)){
     //ToggleFullscreen();         
    //}
    bgImage.bg_dest_rect.width = screenWidth;
    bgImage.bg_dest_rect.height = screenHeight;
    
    //check if file is dropped and load it to background
    if (IsFileDropped()) {
        FilePathList droppedFiles = LoadDroppedFiles();
        if (droppedFiles.count > 0) {
            // Load the first dropped file as a texture
            if (bgImage.image_loaded) UnloadTexture(bgImage.bg_texture);  // Unload previous texture
            Image image = LoadImage(droppedFiles.paths[0]);
            bgImage.bg_texture = LoadTextureFromImage(image);
            bgImage.bg_source_rect.width = bgImage.bg_texture.width;
            bgImage.bg_source_rect.height = bgImage.bg_texture.height;
            UnloadImage(image);
            bgImage.image_loaded = true;
    
        }
        UnloadDroppedFiles(droppedFiles);
    }
    //Render
    BeginDrawing();
    
        ClearBackground(DARKGRAY);
    
         if (bgImage.image_loaded) {
            DrawTexturePro(bgImage.bg_texture, bgImage.bg_source_rect,bgImage.bg_dest_rect,bgImage.bg_pos,0.0f, WHITE);  // Draw image at (200,150)
        } else {
            DrawText("Drop an image file here", screenWidth * .4, screenHeight  *.05, 20, BLACK);
        }
        UpdateAndDrawApp();
    
        // DrawRectangle(0,0,250,54,BLACK);
        // DrawText(TextFormat("Delta Time: %02f", dt), 4, 4, 25, RED);
        // DrawText(TextFormat("fps: %02f", fps), 4, 25, 25, RED);
    EndDrawing();
    //----------------------------------------------------------------------------------
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
        UpdateReactionGameState();
        break;
    case TRACK:
        UpdateTrackGame();
        DrawTrackGame();
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
    
    //DrawRectangleRec(mainMenuData.reactionRect, RED);
    //DrawRectangleRec(mainMenuData.trackingRect, BLUE);
    DrawText("Reaction Practice",mainMenuData.pos.x,mainMenuData.pos.y,mainMenuData.fontScale,mainMenuData.fontColor);
    //DrawText("Tracking Practice",mainMenuData.trackingRect.x,mainMenuData.trackingRect.y,mainMenuData.fontScale,mainMenuData.fontColor);
    
    if (GuiLabelButton(mainMenuData.reactionRect, ""))
    {
        game.appState = REACTION;
    }
    // if (GuiLabelButton(mainMenuData.trackingRect, ""))
    // {

    // }
    
}
#pragma endregion
#pragma region  Reaction Game
void UpdateReactionGameState()
{
    switch (reactionGame.gameState)
    {
    case START:
        UpdateReactionStartMenu();
        DrawReactionStartMenu();
        break;
    case PLAY:
        UpdateReactionGame();
        DrawReactionGame();
        break;
    case HIDE:
        break;
    case END:
        UpdateReactionGameEnd();
        DrawReactionGameEnd();
        break;
    
    default:
        break;
    }
    ReactionGameCheckForHide();
}
void ReactionGameCheckForHide()
{
    if(IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
    {
        if(reactionGame.hideGame)
        {
            reactionGame.hideGame = false;
            reactionGame.gameState = reactionGame.previousState;
            if(reactionGame.gameState == PLAY)
                DisableCursor();
        }
        else
        {
            reactionGame.hideGame = true;
            reactionGame.previousState = reactionGame.gameState;
            reactionGame.gameState = HIDE;   
            EnableCursor(); 
        }
    }
}
void ResetReactionGame()
{
    int x=0;
    int y=0;
    for(int i=0;i<NUM_REACTION_BALLS;++i){
        reactionGame.balls[i] = false;
        reactionGame.ballPositions[i] = (Vector3){0,0,700};
    }
    for (int i = 0; i < NUM_BALLS_ON_SCREEN; ++i)
    {
        reactionGame.activeBallIndexs[i] = 0;
    }
    
    reactionGame.score = 1;
    reactionGame.numClicks = 1;
    reactionGame.accuracy = 0;
    reactionGame.timer = 30;
    PickRandomStartBalls();


}
#pragma region  Raction game Start Menu
void UpdateReactionStartMenu()
{
    reactionStart.pos.x = screenWidth * .35;
    reactionStart.pos.y = screenHeight * .2;
    reactionStart.fontScale = screenHeight * .05 ;
    reactionStart.spaccing = screenHeight * 0.1;
    reactionStart.sliderSize = (Vector2){screenWidth * .4,screenHeight *.05};
    reactionStart.ballSizeButton = (Rectangle){0,0,screenWidth * .1,screenHeight*.1};
    if (IsKeyPressed(KEY_SPACE))
    {
        reactionGame.gameState = PLAY;
        //TODO: move this to reaction game start of play
        DisableCursor();
        ResetReactionGame();
        reactionGame.ballColor = ballColors[reactionStart.ballColorIndex];
        reactionGame.ballSize *= 4;
    }
}
void DrawReactionStartMenu()
{
    float x = reactionStart.pos.x;
    float y = reactionStart.pos.y;
    int fontSize = reactionStart.fontScale;
    Color textColor = mainMenuData.fontColor;
    //mouse sensitivity buttons
    DrawText(TextFormat("Mouse Sensitivity: %f",reactionStart.mouseSensitivity),x,y,fontSize,textColor);
    y+=reactionStart.sliderSize.y;
    GuiSliderBar((Rectangle){ x, y, reactionStart.sliderSize.x, reactionStart.sliderSize.y }, "","", &reactionStart.mouseSensitivity, 0.0f, 10.0f);
    // balls size buttons
    y+=reactionStart.spaccing;
    DrawText(TextFormat("Ball Size: %d",(int)reactionGame.ballSize),x,y,fontSize,textColor);
    float ballsizeBtnX = x;
    float ballSizeBtnY = y + fontSize;
    reactionStart.ballSizeButton.x = ballsizeBtnX;
    reactionStart.ballSizeButton.y = ballSizeBtnY;
    DrawText("-",ballsizeBtnX,ballSizeBtnY,fontSize,textColor);
    if (GuiLabelButton(reactionStart.ballSizeButton, ""))
    {
        reactionGame.ballSize -= 1;
        if(reactionGame.ballSize <=1)
            reactionGame.ballSize = 1;
    }
    ballsizeBtnX+=reactionStart.ballSizeButton.width;
    reactionStart.ballSizeButton.x = ballsizeBtnX;
    DrawText("+",ballsizeBtnX,ballSizeBtnY,fontSize,textColor);
    if (GuiLabelButton(reactionStart.ballSizeButton, ""))
    {
        reactionGame.ballSize += 1;
        if(reactionGame.ballSize >=10)
            reactionGame.ballSize = 10;
    }
    //ball color buttons
    y+=reactionStart.spaccing +10;
    DrawText("Ball Color: ", x,y,fontSize,textColor);
    float rectX = x + MeasureText("Ball Color: ",fontSize);
    DrawRectangle(rectX,y,fontSize,fontSize,ballColors[reactionStart.ballColorIndex]);
    ballsizeBtnX = x;
    ballSizeBtnY = y + fontSize;
    reactionStart.ballSizeButton.x = ballsizeBtnX;
    reactionStart.ballSizeButton.y = ballSizeBtnY;
    DrawText("-",ballsizeBtnX,ballSizeBtnY,fontSize,textColor);
    if (GuiLabelButton(reactionStart.ballSizeButton, ""))
    {
        reactionStart.ballColorIndex -= 1;
        if(reactionStart.ballColorIndex <=0)
            reactionStart.ballColorIndex = numBallColors-1;
    }
    ballsizeBtnX+=reactionStart.ballSizeButton.width;
    reactionStart.ballSizeButton.x = ballsizeBtnX;
    DrawText("+",ballsizeBtnX,ballSizeBtnY,fontSize,textColor);
    if (GuiLabelButton(reactionStart.ballSizeButton, ""))
    {
        reactionStart.ballColorIndex += 1;
        if(reactionStart.ballColorIndex >=numBallColors)
            reactionStart.ballColorIndex = 0;
    }
    
    // space to start text
    y=screenHeight*.9;
    DrawText("Press SPACE to start",x,y,reactionStart.fontScale,mainMenuData.fontColor);
    
}
#pragma endregion
#pragma region Reaction Game Main Update
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
        newNum = GetRandomValue(0, NUM_REACTION_BALLS-1);
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
    reactionGame.timer -= dt;
    if(reactionGame.timer<=0)
    {
        reactionGame.gameState=END;
        EnableCursor();
    }
    //CAMERA UPDATE
    // Get mouse movement
    Vector2 mouseDelta = GetMouseDelta();
    camSettings.yaw -= mouseDelta.x * reactionStart.mouseSensitivity * camSettings.sensitivity;; // Rotate left/right
    camSettings.pitch -= mouseDelta.y * reactionStart.mouseSensitivity * camSettings.sensitivity;; // Rotate up/down
    //camSettings.yaw -= mouseDelta.x * camSettings.sensitivity; // Rotate left/right
    //camSettings.pitch -= mouseDelta.y * camSettings.sensitivity; // Rotate up/down

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
    reactionGame.ballOffset = 70;//reactionGame.ballSize * 5;
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
            reactionGame.ballPositions[counter].x = x;
            reactionGame.ballPositions[counter].y = y;
            reactionGame.ballPositions[counter].z = 700;
            ++counter;
            x -= increment;
        }
        y+=increment;            
    }
    Ray ray = GetMouseRay(screenCenter, camera);
    RayCollision col;
    if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
        reactionGame.numClicks++;
        for(int i =0; i <NUM_BALLS_ON_SCREEN;++i){
            col = GetRayCollisionSphere(ray, reactionGame.ballPositions[reactionGame.activeBallIndexs[i]],reactionGame.ballSize+5);
            if(col.hit)
            {
                //TODO: update scores when ball hit
                reactionGame.activeBallIndexs[i] = GetRandomBall();
                reactionGame.score++;
            }
        }
        reactionGame.accuracy = (float)reactionGame.score / (float)reactionGame.numClicks;
    }
}
void DrawReactionGame()
{
    BeginMode3D(camera);
    for(int i =0;i<NUM_BALLS_ON_SCREEN;++i){
        DrawSphere(reactionGame.ballPositions[reactionGame.activeBallIndexs[i]],reactionGame.ballSize, reactionGame.ballColor);
    }
    Vector3 pos = {0,0,100};
    Vector3 size = {200,100,300};
    DrawCubeWiresV(pos,size,BLACK);
    EndMode3D();
    float x = screenWidth * .2;
    float y = 10;
    DrawText(TextFormat("Score: %d",reactionGame.score),x,y,20,BLACK);
    x = x + 110;
    DrawText(TextFormat("Accuracy: %f",reactionGame.accuracy),x,y,20,BLACK);
    
    x = x + 210;
    DrawText(TextFormat("Time: %d",(int)reactionGame.timer),x,y,20,BLACK);
    
    //crosshair
    DrawText(TextFormat("+"),screenWidth/2,screenHeight/2-10,20,BLACK);
}
#pragma endregion
#pragma region Reaction Game End
void UpdateReactionGameEnd()
{
    if(IsKeyPressed(KEY_SPACE))
    {
        reactionGame.gameState = START;
    }
}
void DrawReactionGameEnd()
{   
    float x = reactionStart.pos.x;
    float y = reactionStart.pos.y;
    int fontSize = reactionStart.fontScale;
    Color textColor = mainMenuData.fontColor;
    //score
    DrawText(TextFormat("Score: %d",reactionGame.score),x,y,fontSize,textColor);
    //accuracy
    y+=reactionStart.spaccing;
    DrawText(TextFormat("Accuracy: %f",reactionGame.accuracy),x,y,fontSize,textColor);
    //numhits
    y+=reactionStart.spaccing;
    //DrawText(TextFormat("Targets Hit: %d",reactionGame.Nu),x,y,fontSize,textColor);

    //press space text
    y=screenHeight*.9;
    DrawText("Press SPACE to restart",x,y,reactionStart.fontScale,mainMenuData.fontColor);
}
#pragma endregion
#pragma endregion
#pragma region Tracking game
void UpdateTrackGame()
{

}
void DrawTrackGame()
{

}
#pragma endregion