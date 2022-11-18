#include<stdio.h>
#include<raylib.h>
#include<time.h>

#define BLOCKS_X 10
#define BLOCKS_Y 20

#define PIXEL_PER_BLOCKS 50

#define TEXTURE_PATH "images/tetris_blocks.png"

enum { LEFT = -1, RIGHT = 1 };

const int FIELD_WIDTH = BLOCKS_X * PIXEL_PER_BLOCKS;
const int FIELD_HEIGHT = BLOCKS_Y * PIXEL_PER_BLOCKS;

char field[BLOCKS_Y][BLOCKS_X];

int score;

const Color blocks_colors[7] = {
    WHITE,
    GREEN,
    RED,
    BLUE,
    VIOLET,
    LIME,
    MAGENTA,
};

typedef struct {
    int row_pos;
    int col_pos;

    int blocks[4][2];
    float rot_point[2];
    int color; /* 1 - 7 */
    int rotate_state; /* 1 - 4 */
} TetrisBlock;

TetrisBlock block_I = {
    0, 0,
    { { 0, 0 }, { -1, 0 }, { -2, 0 }, { 1, 0 } },
    { 0, 0 },
    1, 1
};

TetrisBlock block_O = {
    0, 0,
    { { 0, 0 }, { -1, 0 }, { -1, -1 }, { 0, -1 } },
    { 0, 0 },
    2, 1
};

TetrisBlock block_J = {
    0, 0,
    { { 0, 0 }, { -1, 0 }, { 1, 0 }, { 1, -1 } },
    { 0, 0 },
    3, 1
};

TetrisBlock block_L = {
    0, 0,
    { { 0, 0 }, { 1, 0 }, { -1, 0 }, { -1, -1 } },
    { 0, 0 },
    4, 1
};

TetrisBlock block_S = {
    0, 0,
    { { 0, 0 }, { -1, -1 }, { 0, -1 }, { 1, 0 } },
    { 0, 0 },
    5, 2
};

TetrisBlock block_T = {
    0, 0,
    { { 0, 0 }, { -1, 0 }, { 1, 0 }, { 0, -1 } },
    { 0, 0 },
    6, 1
};

TetrisBlock block_Z = {
    0, 0,
    { { 0, 0 }, { -1, 0 }, { 0, -1 }, { 1, -1 } },
    { 0, 0 },
    7, 2
};


TetrisBlock blocks[7];

TetrisBlock curr_tb;
TetrisBlock new_tb;

void tetris_block_debug(TetrisBlock *tetris_block) {
    printf("row_pos: %d col_pos: %d\n", tetris_block->row_pos, tetris_block->col_pos);
    int i;
    for (i = 0; i < 4; i++) {
        printf("blocks[%d]: %d, %d\n", i, tetris_block->blocks[i][0], tetris_block->blocks[i][1]);
    }
    printf("rot_point: %f, %f\n", tetris_block->rot_point[0], tetris_block->rot_point[1]);
}

bool tetris_block_would_collide(TetrisBlock *tetris_block)
{
    /* walls */
    int i;
    int row, col;
    for (i = 0; i < 4; i++) {
        row = new_tb.blocks[i][1] + new_tb.row_pos;
        col = new_tb.blocks[i][0] + new_tb.col_pos;

        if (col >= BLOCKS_X)
            return 1;
        if (col < 0)
            return 1;

        if (field[row][col])
            return 1;
    }

    return 0;
}

bool tetris_block_end_position(void)
{
    int pos_x, pos_y;
    int i;
    for (i = 0; i < 4; i++) {
        pos_x =  new_tb.col_pos + new_tb.blocks[i][0];
        pos_y = new_tb.row_pos - new_tb.blocks[i][1];

        if (field[pos_y][pos_x]) { return 1; }
        if (pos_y >= BLOCKS_Y) { return 1; }
    }
    return 0;
}

void clean_line(void)
{
    int row, col;

    for (row = 0; row < BLOCKS_Y; row++) {
        for (col = 0; col < BLOCKS_X; col++) {
            if (field[row][col] == 0)
                break;
        }
        if (col == BLOCKS_X && field[row][BLOCKS_X - 1]) {
            score += 100;
            int i, j;
            while (row--) {
                for (i = 0; i < BLOCKS_X; i++)
                    field[row + 1][i] = field[row][i];
            }
        }
    }
}

TetrisBlock next_tb;

void tetris_block_spawn(void) {
    clean_line();
    int index = GetRandomValue(0, 6);
    new_tb = next_tb;
    next_tb = blocks[index];
    new_tb.row_pos = 0;
    new_tb.col_pos = 5;
    curr_tb = new_tb;
    if (tetris_block_would_collide(NULL)) {
        while (!WindowShouldClose()) {
            BeginDrawing();
            DrawText("Game Over", 30, 30, 20, WHITE);
            EndDrawing();
        }
    }
}

void tetris_block_fall(TetrisBlock *tetris_block)
{
    new_tb.row_pos++;

    /* clear */
    tetris_block = &curr_tb;
    int i;
    int row, col;
    for (i = 0; i < 4; i++) {
        row = tetris_block->row_pos - tetris_block->blocks[i][1];
        col = tetris_block->col_pos + tetris_block->blocks[i][0];
        if (row >= 0 && col >= 0)
            field[row][col] = 0;
    }
    if (tetris_block_end_position()) {
        tetris_block = &curr_tb;
        for (i = 0; i < 4; i++) {
            row = tetris_block->row_pos - tetris_block->blocks[i][1];
            col = tetris_block->col_pos + tetris_block->blocks[i][0];
            if (row >= 0 && col >= 0)
                field[row][col] = tetris_block->color;
        }
        tetris_block_spawn();
    } else {
        tetris_block = &new_tb;
        for (i = 0; i < 4; i++) {
            row = tetris_block->row_pos - tetris_block->blocks[i][1];
            col = tetris_block->col_pos + tetris_block->blocks[i][0];
            if (row >= 0 && col >= 0)
                field[row][col] = tetris_block->color;
        }
        curr_tb = new_tb;
    }
}

void tetris_block_move(TetrisBlock *tetris_block, int direction)
{
    new_tb.col_pos += direction;
    /* clear */
    tetris_block = &curr_tb;
    int i;
    int row, col;
    for (i = 0; i < 4; i++) {
        row = tetris_block->row_pos - tetris_block->blocks[i][1];
        col = tetris_block->col_pos + tetris_block->blocks[i][0];
        if (row >= 0 && col >= 0)
            field[row][col] = 0;
    }

    if (tetris_block_would_collide(&curr_tb)) {
        tetris_block = &curr_tb;
        for (i = 0; i < 4; i++) {
            row = tetris_block->row_pos - tetris_block->blocks[i][1];
            col = tetris_block->col_pos + tetris_block->blocks[i][0];
            if (row >= 0 && col >= 0)
                field[row][col] = tetris_block->color;
        }
        new_tb = curr_tb;
    } else {
        tetris_block = &new_tb;
        for (i = 0; i < 4; i++) {
            row = tetris_block->row_pos - tetris_block->blocks[i][1];
            col = tetris_block->col_pos + tetris_block->blocks[i][0];
            if (row >= 0 && col >= 0)
                field[row][col] = tetris_block->color;
        }
        curr_tb = new_tb;
    }
}

void tetris_block_rotate(TetrisBlock *tetris_block)
{
    float x, y;
    int i;

    if (new_tb.color == 1 || new_tb.color == 5 || new_tb.color == 7) {
        /* rotate right */
        if (new_tb.rotate_state == 1) {
            for (i = 0; i < 4; i++) {
                x = (float)new_tb.rot_point[0] + (new_tb.blocks[i][1] - (float)new_tb.rot_point[1]);
                y = (float)new_tb.rot_point[1] + ((float)new_tb.rot_point[0] - new_tb.blocks[i][0]); 

                new_tb.blocks[i][0] = x;
                new_tb.blocks[i][1] = y;
            }
            new_tb.rotate_state++;
        } else {
            /* reverse */
            for (i = 0; i < 4; i++) {
                x = new_tb.rot_point[0] + new_tb.rot_point[1] - new_tb.blocks[i][1];
                y = new_tb.rot_point[1] + new_tb.blocks[i][0] - new_tb.rot_point[0];

                new_tb.blocks[i][0] = x;
                new_tb.blocks[i][1] = y;
            }
            new_tb.rotate_state--;
        }
    }

    if (new_tb.color == 2) {
        return;
    }

    if (new_tb.color == 3 || new_tb.color == 4 || new_tb.color == 6) {
        for (i = 0; i < 4; i++) {
            x = (float)new_tb.rot_point[0] + (new_tb.blocks[i][1] - (float)new_tb.rot_point[1]);
            y = (float)new_tb.rot_point[1] + ((float)new_tb.rot_point[0] - new_tb.blocks[i][0]); 

            new_tb.blocks[i][0] = x;
            new_tb.blocks[i][1] = y;
        }
    }

    /* clear */
    tetris_block = &curr_tb;
    int row, col;
    for (i = 0; i < 4; i++) {
        row = tetris_block->row_pos - tetris_block->blocks[i][1];
        col = tetris_block->col_pos + tetris_block->blocks[i][0];
        if (row >= 0 && col >= 0)
            field[row][col] = 0;
    }

    if (tetris_block_would_collide(&curr_tb)) {
        tetris_block = &curr_tb;
        for (i = 0; i < 4; i++) {
            row = tetris_block->row_pos - tetris_block->blocks[i][1];
            col = tetris_block->col_pos + tetris_block->blocks[i][0];
            if (row >= 0 && col >= 0)
                field[row][col] = tetris_block->color;
        }
        new_tb = curr_tb;
    } else {
        tetris_block = &new_tb;
        for (i = 0; i < 4; i++) {
            row = tetris_block->row_pos - tetris_block->blocks[i][1];
            col = tetris_block->col_pos + tetris_block->blocks[i][0];
            if (row >= 0 && col >= 0)
                field[row][col] = tetris_block->color;
        }
        curr_tb = new_tb;
    }
}

Texture2D tex;
    
void field_draw(void)
{
    int r, c, value;
    for (c = 0; c < BLOCKS_X; c++) {
        for (r = 0; r < BLOCKS_Y; r++) {
            value = field[r][c];
            if (value) {
                //DrawRectangle(c * PIXEL_PER_BLOCKS, r * PIXEL_PER_BLOCKS, PIXEL_PER_BLOCKS, PIXEL_PER_BLOCKS, blocks_colors[value - 1]);
                DrawTextureRec(tex, (struct Rectangle){ (value - 1) * PIXEL_PER_BLOCKS, 0.0f, PIXEL_PER_BLOCKS, PIXEL_PER_BLOCKS }, (struct Vector2){ c * PIXEL_PER_BLOCKS, r * PIXEL_PER_BLOCKS }, WHITE);
            }
        }
    }
}

void preview_draw(void) {
    /* wall */
    int offset_x = BLOCKS_X * PIXEL_PER_BLOCKS + PIXEL_PER_BLOCKS;
    int offset_y = 2 * PIXEL_PER_BLOCKS;
    DrawText("Next:", offset_x, offset_y, 50, WHITE);
    offset_x += 2 * PIXEL_PER_BLOCKS;
    offset_y += PIXEL_PER_BLOCKS;

    /* tetris block */
    int i;
    for (i = 0; i < 4; i++) {
        // DrawRectangle(offset_x + PIXEL_PER_BLOCKS * next_tb.blocks[i][0], offset_y - PIXEL_PER_BLOCKS * next_tb.blocks[i][1], PIXEL_PER_BLOCKS, PIXEL_PER_BLOCKS, blocks_colors[next_tb.color - 1]);
        DrawTextureRec(tex, (struct Rectangle){ (next_tb.color - 1) * PIXEL_PER_BLOCKS, 0.0f, PIXEL_PER_BLOCKS, PIXEL_PER_BLOCKS }, (struct Vector2){ offset_x + PIXEL_PER_BLOCKS * next_tb.blocks[i][0], offset_y - PIXEL_PER_BLOCKS * next_tb.blocks[i][1] }, WHITE);
    }
}

void key_input(void)
{
    if (IsKeyPressed(KEY_UP)) {
        tetris_block_rotate(&curr_tb);
    }

    if (IsKeyDown(KEY_DOWN)) {
        tetris_block_fall(&curr_tb);
    }

    if (IsKeyPressed(KEY_LEFT)) {
        tetris_block_move(&curr_tb, LEFT);
    }

    if (IsKeyPressed(KEY_RIGHT)) {
        tetris_block_move(&curr_tb, RIGHT);
    }
}

void game_loop(void) {
    tetris_block_spawn();
    tetris_block_spawn();

    TetrisBlock *tetris_block = &curr_tb;
    int row, col;
    int i;
    for (i = 0; i < 4; i++) {
        row = tetris_block->row_pos - tetris_block->blocks[i][1];
        col = tetris_block->col_pos + tetris_block->blocks[i][0];
        if (row >= 0 && col >= 0)
            field[row][col] = tetris_block->color;
    }

    score = 0;

    int speed = 0;
    while (!WindowShouldClose()) {

        key_input();

        if (speed >= 5) {
            tetris_block_fall(&curr_tb);
            speed = 0;
        } else { speed++; }

        BeginDrawing();
        ClearBackground(BLACK);
        /* Score */
        char buffer_score[50];
        sprintf(buffer_score, "Score ~ %d", score);
        /* Level */
        DrawText("Level ~ 9", PIXEL_PER_BLOCKS * BLOCKS_X + PIXEL_PER_BLOCKS, 300, 40, WHITE);

        /* Priview */
        preview_draw();
        DrawText(buffer_score, PIXEL_PER_BLOCKS * BLOCKS_X + PIXEL_PER_BLOCKS, 10, 40, WHITE);
        // DrawRectangleLines(0, 0, FIELD_WIDTH, FIELD_HEIGHT, WHITE);
        DrawRectangle(0, PIXEL_PER_BLOCKS * BLOCKS_Y, PIXEL_PER_BLOCKS * BLOCKS_X, PIXEL_PER_BLOCKS, GRAY);
        field_draw();
        EndDrawing();
    }
}

Image img;

int main(void)
{
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");
    SetTargetFPS(60);

    /* blocks texture */
    img = LoadImage("images/tetris_blocks.png");
    ImageResize(&img, 7 * PIXEL_PER_BLOCKS, PIXEL_PER_BLOCKS);
    tex = LoadTextureFromImage(img);

    /* blocks init */
    blocks[0] = block_I;
    blocks[1] = block_O;
    blocks[2] = block_J;
    blocks[3] = block_L;
    blocks[4] = block_S;
    blocks[5] = block_T;
    blocks[6] = block_Z;

    game_loop();
    /*
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);
        DrawText("Start Game", 15, 15, 50, WHITE);
        DrawText("Exit", 15, 60, 50, WHITE);
        EndDrawing();
    }
    */

    
    CloseWindow();

    return 0;
}
