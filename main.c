#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#define SNAKE_LENGTH   256
#define SQUARE_SIZE     60

typedef struct Snake {
    Vector2 position;
    Vector2 size;
    Vector2 speed;
    Color color;
} Snake;

typedef struct Food {
    Vector2 position;
    Vector2 size;
    bool active;
    Color color;
} Food;


const int screenWidth = 1920;
const int screenHeight = 1080;


int framesCounter = 0;
bool gameOver = false;
bool pause = false;
bool allowMove = false;
int counterTail = 0;
bool pauseMusic = false;
float fps = 60;
float score = 70;

Food fruit = { 0 };
Snake snake[SNAKE_LENGTH] = { 0 };
Vector2 snakePosition[SNAKE_LENGTH] = { 0 };


void InitGame(void);         // Initialize game
void UpdateGame(Sound _crash, Sound _eat, Sound _ding, Sound _move);       // Update game (one frame)
void DrawGame(Texture2D _bg, Texture2D _apple, Texture2D _block, Music _music);         // Draw game (one frame)
void UnloadGame(Texture2D _bg, Texture2D _apple, Texture2D _block, Music _music, Sound _crash, Sound _eat, Sound _ding, Sound _move);       // Unload game
void UpdateDrawFrame(Texture2D _bg, Texture2D _apple, Texture2D _block, Music _music, Sound _crash, Sound _eat, Sound _ding, Sound _move);  // Update and Draw (one frame)
void pausemusic(Music _music);
void loadingscreen(Texture2D _bg, Texture2D _apple, Texture2D _block, Music _music);
void options(Texture2D _bg, Music _music, Sound _ding);
void win(Texture2D _bg, Music _music);

Rectangle GameButtonRec = { screenWidth / 6 - 220 / 2, screenHeight / 2 + (2*40), 220, 60 };
Rectangle OptionButtonRec = { screenWidth / 6 - 220 / 2, screenHeight / 2 + (4*40), 220, 60 };
Rectangle exitButtonRec = { screenWidth / 6 - 220 / 2, screenHeight / 2 + (6*40), 220, 60 };
Rectangle speedButtonRec = { screenWidth - 850, 605, 200, 30 };
Rectangle scoreButtonRec = { screenWidth - 850, 685, 200, 30 };
Rectangle resetButtonRec = { screenWidth - 790, 760, 80, 40 };
Rectangle musicButtonRec = { screenWidth - 790, 840, 80, 40 };
Rectangle backbuttonRec = { 50, 30, 80, 40 };

int main(void)
{

    InitWindow(screenWidth, screenHeight, "Snake and Apples");
    InitAudioDevice();
    ToggleFullscreen();

    Texture2D bg = LoadTexture("resources/background.png");
    Texture2D apple = LoadTexture("resources/apple.png");
    Texture2D block = LoadTexture("resources/block.png");
    Music music = LoadMusicStream("resources/bg_music_1.mp3");
    Sound crash = LoadSound("resources/Crash.mp3");
    Sound ding = LoadSound("resources/ding.mp3");
    Sound eat = LoadSound("resources/Eat.mp3");
    Sound move = LoadSound("resources/Move.mp3");


    InitGame();
    SetTargetFPS((int)fps);
    //--------------------------------------------------------------------------------------
    loadingscreen(bg, apple, block, music);
    // Main game loop
    while (true)    // Detect window close button or ESC key
    {
        BeginDrawing();
        ClearBackground((Color){14,50,22,255});
        DrawTexture(bg, 0, 0, WHITE);
        DrawRectangle(0, 0, screenWidth / 3, screenHeight, (Color){14,50,22,255});
        InitGame();
        UpdateMusicStream(music);

        DrawText("SNAKE GAME", screenWidth / 6 - MeasureText("SNAKE GAME", 80) / 2, screenHeight / 8, 80, GRAY);
        DrawText("Main Menu", screenWidth / 6 - MeasureText("Main Menu", 60) / 2, screenHeight / 3, 60, YELLOW);
        DrawTexture(apple, screenWidth - 400 , screenHeight / 4 + 60, WHITE);
        DrawTexture(block , screenWidth - 400, screenHeight / 4 + 180, SKYBLUE);
        DrawTexture(block , screenWidth - 400, screenHeight / 4 + 240, WHITE);
        DrawTexture(block , screenWidth - 400, screenHeight / 4 + 300, WHITE);
        DrawTexture(block , screenWidth - 400, screenHeight / 4 + 360, WHITE);
        DrawTexture(block , screenWidth - 460, screenHeight / 4 + 360, WHITE);
        DrawTexture(block , screenWidth - 520, screenHeight / 4 + 360, WHITE);
        DrawTexture(block , screenWidth - 580, screenHeight / 4 + 360, WHITE);
        DrawTexture(block , screenWidth - 640, screenHeight / 4 + 360, WHITE);
        DrawTexture(block , screenWidth - 700, screenHeight / 4 + 360, WHITE);
        GuiSetStyle(DEFAULT, TEXT_SIZE, 20);


        if (GuiButton(GameButtonRec, "Start")) {
            PlaySound(ding);
            while (!WindowShouldClose())   
            {
                UpdateDrawFrame(bg, apple, block, music, crash, eat, ding, move);
            }

        }
        if (GuiButton(OptionButtonRec, "Options")) {
            PlaySound(ding);
            options(bg, music, ding);

        }
        if (GuiButton(exitButtonRec, "Exit")) {
            PlaySound(ding);
            break;

        }

        EndDrawing();

    }

    UnloadGame(bg, apple, block, music, crash, eat, ding, move);         // Unload loaded data (textures, sounds, models...)

    CloseWindow();  

    return 0;
}


void InitGame(void)
{
    framesCounter = 0;
    gameOver = false;
    pause = false;

    counterTail = 1;
    allowMove = false;

    for (int i = 0; i < SNAKE_LENGTH; i++)
    {
        snake[i].position = (Vector2){ SQUARE_SIZE/2, SQUARE_SIZE/2 };
        snake[i].size = (Vector2){ SQUARE_SIZE, SQUARE_SIZE };
        snake[i].speed = (Vector2){ SQUARE_SIZE, 0 };

        if (i == 0) snake[i].color = SKYBLUE;
        else snake[i].color = WHITE;
    }

    for (int i = 0; i < SNAKE_LENGTH; i++)
    {
        snakePosition[i] = (Vector2){ 0.0f, 0.0f };
    }

    fruit.size = (Vector2){ SQUARE_SIZE, SQUARE_SIZE };
    fruit.color = WHITE;
    fruit.active = false;
}

// Update game (one frame)
void UpdateGame(Sound _crash, Sound _eat, Sound _ding, Sound _move)
{
    if (!gameOver)
    {   if (IsKeyPressed('P')) pause = !pause;

        if (!pause)
        {
            
            // Player control
            if (IsKeyPressed(KEY_RIGHT) && (snake[0].speed.x == 0) && allowMove)
            {
                snake[0].speed = (Vector2){ SQUARE_SIZE, 0 };
                allowMove = false;
                PlaySound(_move);
            }
            if (IsKeyPressed(KEY_LEFT) && (snake[0].speed.x == 0) && allowMove)
            {
                snake[0].speed = (Vector2){ -SQUARE_SIZE, 0 };
                allowMove = false;
                PlaySound(_move);

            }
            if (IsKeyPressed(KEY_UP) && (snake[0].speed.y == 0) && allowMove)
            {
                snake[0].speed = (Vector2){ 0, -SQUARE_SIZE };
                allowMove = false;
                PlaySound(_move);

            }
            if (IsKeyPressed(KEY_DOWN) && (snake[0].speed.y == 0) && allowMove)
            {
                snake[0].speed = (Vector2){ 0, SQUARE_SIZE };
                allowMove = false;
                PlaySound(_move);

            }

            // Snake movement
            for (int i = 0; i < counterTail; i++) snakePosition[i] = snake[i].position;

            if ((framesCounter%5) == 0)
            {
                for (int i = 0; i < counterTail; i++)
                {
                    if (i == 0)
                    {
                        snake[0].position.x += snake[0].speed.x;
                        snake[0].position.y += snake[0].speed.y;
                        allowMove = true;
                    }
                    else snake[i].position = snakePosition[i-1];
                }
            }

            // Wall behaviour
            if (((snake[0].position.x) > (screenWidth - 60)) ||
                ((snake[0].position.y) > (screenHeight - 60)) ||
                (snake[0].position.x < 0) || (snake[0].position.y < 0))
            {
                gameOver = true;
                PlaySound(_crash);

            }

            // Collision with yourself
            for (int i = 1; i < counterTail; i++)
            {
                if ((snake[0].position.x == snake[i].position.x) && (snake[0].position.y == snake[i].position.y)) {
                    gameOver = true;
                    PlaySound(_crash);

                }
            }

            // Fruit position calculation
            if (!fruit.active)
            {
                fruit.active = true;
                fruit.position = (Vector2){ GetRandomValue(0, (screenWidth/SQUARE_SIZE) - 1.5)*SQUARE_SIZE + SQUARE_SIZE/2, GetRandomValue(0, (screenHeight/SQUARE_SIZE) - 1.5)*SQUARE_SIZE + SQUARE_SIZE/2 };

                for (int i = 0; i < counterTail; i++)
                {
                    while ((fruit.position.x == snake[i].position.x) && (fruit.position.y == snake[i].position.y))
                    {
                        fruit.position = (Vector2){ GetRandomValue(0, (screenWidth/SQUARE_SIZE) - 1)*SQUARE_SIZE + SQUARE_SIZE/2, GetRandomValue(0, (screenHeight/SQUARE_SIZE) - 1)*SQUARE_SIZE + SQUARE_SIZE/2 };
                        i = 0;
                    }
                }
            }

            // Collision
            if ((snake[0].position.x < (fruit.position.x + fruit.size.x) && (snake[0].position.x + snake[0].size.x) > fruit.position.x) &&
                (snake[0].position.y < (fruit.position.y + fruit.size.y) && (snake[0].position.y + snake[0].size.y) > fruit.position.y))
            {
                snake[counterTail].position = snakePosition[counterTail - 1];
                counterTail += 1;
                fruit.active = false;
                PlaySound(_eat);

            }

            framesCounter++;
        }
    }
    else
    {
        if (IsKeyPressed(KEY_ENTER))
        {
            InitGame();
            gameOver = false;
        }
    }
}

// Draw game (one frame)
void DrawGame(Texture2D _bg, Texture2D _apple, Texture2D _block, Music _music)
{   
    BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawTexture(_bg, 0, 0, WHITE);
        GuiSetStyle(DEFAULT, TEXT_SIZE, 20);

        if (!gameOver)
        {   // Draw snake
            for (int i = 0; i < counterTail; i++) DrawTextureV(_block, snake[i].position, snake[i].color);

            // Draw fruit to pick
            DrawTextureV(_apple, fruit.position, fruit.color);

            UpdateMusicStream(_music);
            win(_bg, _music);
            DrawText(TextFormat("SCORE: %i", counterTail - 1), screenWidth - MeasureText("SCORE:     ", 40), 40, 40, GRAY);
            DrawRectangleLines(20, 20, 1880, 1040, YELLOW);
            if (pause) DrawText("GAME PAUSED", screenWidth/2 - MeasureText("GAME PAUSED", 40)/2, screenHeight/2 - 40, 40, GRAY);
        }
        else DrawText("PRESS [ENTER] TO PLAY AGAIN", GetScreenWidth()/2 - MeasureText("PRESS [ENTER] TO PLAY AGAIN", 40)/2, GetScreenHeight()/2 - 50, 40, GRAY);

    EndDrawing();
}

// Unload game variables
void UnloadGame(Texture2D _bg, Texture2D _apple, Texture2D _block, Music _music, Sound _crash, Sound _eat, Sound _ding, Sound _move)
{
    UnloadTexture(_bg);
    UnloadTexture(_apple);
    UnloadTexture(_block);
    UnloadMusicStream(_music);
    UnloadSound(_crash);
    UnloadSound(_eat);
    UnloadSound(_ding);
    UnloadSound(_move);

}

// Update and Draw (one frame)
void UpdateDrawFrame(Texture2D _bg, Texture2D _apple, Texture2D _block, Music _music, Sound _crash, Sound _eat, Sound _ding, Sound _move)
{
    UpdateGame(_crash, _eat, _ding, _move);
    DrawGame(_bg, _apple, _block, _music);
}

void pausemusic(Music _music){

    pauseMusic = !pauseMusic;

    if (pauseMusic) PauseMusicStream(_music);
    else ResumeMusicStream(_music);
}
 
void loadingscreen(Texture2D _bg, Texture2D _apple, Texture2D _block, Music _music){

    int block_x = 420;
    int block_y = screenHeight / 2 + 150;
    int block_speed = 10;
    PlayMusicStream(_music);


    
    while (true) {
        BeginDrawing();
        UpdateMusicStream(_music);

        block_x += block_speed;

            if(block_x >= screenWidth - 420 || block_x <= 420){
            block_speed*=-1;
        }

        ClearBackground(DARKGREEN);
        DrawTexture(_bg, 0, 0, WHITE);
        DrawTexture(_apple, screenWidth / 2 - 30 , screenHeight / 2 - 30, WHITE);
        DrawTexture(_block , block_x , block_y, SKYBLUE);
        DrawTexture(_block , block_x - 60, block_y, WHITE);
        DrawTexture(_block , block_x - 120, block_y, WHITE);
        
        DrawText(TextFormat("LOADING..."), screenWidth - MeasureText("LOADING...", 40) - 30, screenHeight - 60, 40, GRAY);
        
        framesCounter++; 

        if (framesCounter > 500)
        {
            break;
        }

        EndDrawing();
    }
}

    void options(Texture2D _bg, Music _music, Sound _ding){

            while (true) {
            BeginDrawing();
            UpdateMusicStream(_music);


            ClearBackground((Color){14,50,22,255});
            DrawTexture(_bg, 0, 0, WHITE);
            DrawRectangle(screenWidth / 6, 0, screenWidth - 2*(screenWidth / 6), screenHeight, (Color){14,50,22,255});

            DrawText("SNAKE GAME", screenWidth / 2 - MeasureText("SNAKE GAME", 80) / 2, screenHeight / 12, 80, WHITE);

            DrawText("Options", screenWidth / 2 - MeasureText("Options", 60) / 2, 330, 60, YELLOW);

            DrawText(TextFormat("Snake Speed"), screenWidth / 2 - 300, 600, 40, WHITE);
            DrawText(TextFormat("Score Points"), screenWidth / 2 - 300, 680, 40, WHITE);
            DrawText(TextFormat("Reset"), screenWidth / 2 - 300, 760, 40, WHITE);
            DrawText(TextFormat("Music"), screenWidth / 2 - 300, 840, 40, WHITE);


            DrawText(TextFormat("%i", (int)fps), screenWidth - 630, 610, 20, WHITE);
            DrawText(TextFormat("%i", (int)score), screenWidth - 630, 690, 20, WHITE);
            SetTargetFPS((int)fps);

            if (GuiButton(backbuttonRec, "BACK")) {
            PlaySound(_ding);
            break;
            }

            GuiSliderBar(speedButtonRec, NULL, NULL, &fps, 30, 150);

            GuiSliderBar(scoreButtonRec, NULL, NULL, &score, 1, 200);
        
            if (GuiButton(resetButtonRec, "Reset")) {

                fps = 60;
                score = 70;

            }
            if (GuiButton(musicButtonRec, "Music")) {

                pausemusic(_music);

            }

            EndDrawing();
        }

    }

void win(Texture2D _bg, Music _music){

        if ( (counterTail - 1)>= (int)score){

            while (true) {

                BeginDrawing();
                UpdateMusicStream(_music);
                ClearBackground((Color){14,50,22,255});
                DrawTexture(_bg, 0, 0, WHITE);
                DrawText(TextFormat("Congradulations!!!"), screenWidth / 2 - MeasureText("Congradulations!!!", 80) / 2 , 120, 80, GRAY);
                DrawText(TextFormat("You wins..."), screenWidth / 2 - MeasureText("You wins..", 60) / 2 , 300, 60, YELLOW);
                DrawText(TextFormat("SCORE: %i" , counterTail - 1), screenWidth / 2 - MeasureText("SCORE:   ", 50) / 2 , 500, 50, GRAY);
                DrawText(TextFormat("Press ENTER to return"), screenWidth - MeasureText("Press ENTER to return", 30) - 60 , screenHeight - 80 , 30, GRAY);

                if (IsKeyPressed(KEY_ENTER)){
                    InitGame();
                    break;
                }

                EndDrawing();
            }
        }
}

