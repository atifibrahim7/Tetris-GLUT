
#include <GL/glut.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sstream>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


int curr_type = 2; // Variable to store the current tetrimino type   / 2 is the first type to be generated(rand)
const int interval = 500;
const int block_size = 20;
const int column_count = 20;
const int row_count = 10;
const int init_pos[2] = { 1, 0 };
const float colors[7][3] = {
    {1.0f, 0.0f, 0.0f},   // Red
    {0.0f, 1.0f, 0.0f},   // Green
    {0.0f, 0.0f, 1.0f},   // Blue
    {1.0f, 1.0f, 0.0f},   // Yellow
    {1.0f, 0.5f, 0.0f},   // Orange
    {0.5f, 0.0f, 1.0f},   // Purple
    {0.0f, 1.0f, 1.0f}    // Cyan
};
const int tetrominos[7][4][4] = {
  {{0,1,0,0},
   {0,1,0,0},
   {0,1,0,0},
   {0,1,0,0}},
  {{0,0,0,0},
   {0,1,1,0},
   {0,1,0,0},
   {0,1,0,0}},
  {{0,0,0,0},
   {0,1,0,0},
   {0,1,1,0},
   {0,1,0,0}},
  {{0,0,0,0},
   {0,1,0,0},
   {0,1,1,0},
   {0,0,1,0}},
  {{0,0,0,0},
   {0,1,1,0},
   {0,1,1,0},
   {0,0,0,0}},
  {{0,0,0,0},
   {0,1,1,0},
   {0,0,1,0},
   {0,0,1,0}},
  {{0,0,0,0},
   {0,0,1,0},
   {0,1,1,0},
   {0,1,0,0}},
};

int points = 0;
int curr_block[4][4] = {};
int next_block[4][4] = {};
int curr_pos[2] = {};
int blocks[column_count][row_count] = {
  {0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0},
};
struct ImageData {
    unsigned char* data;
    int width;
    int height;
    int channels;

    ImageData() : data(nullptr), width(0), height(0), channels(0) {}
};
// global constants for images 
ImageData backgroundImage;
bool gameStarted = false;
// Load start screen image
ImageData startScreenImage;
// Load game over screen image
ImageData gameOverImage;
bool gameOver = false;
ImageData loadImage(const std::string& filename) {
    ImageData imageData;

    // Load image using stb_image
    imageData.data = stbi_load(filename.c_str(), &imageData.width, &imageData.height, &imageData.channels, 0);
    if (!imageData.data) {
        std::cerr << "Failed to load image: " << filename << std::endl;
        // Default initialize all members in case of failure
        return ImageData();
    }

    // Flip image data vertically
    unsigned char* flippedData = new unsigned char[imageData.width * imageData.height * imageData.channels];
    for (int y = 0; y < imageData.height; ++y) {
        unsigned char* srcRow = imageData.data + (y * imageData.width * imageData.channels);
        unsigned char* dstRow = flippedData + ((imageData.height - y - 1) * imageData.width * imageData.channels);
        memcpy(dstRow, srcRow, imageData.width * imageData.channels);
    }

    // Update image data with flipped data
    delete[] imageData.data; // Delete original image data
    imageData.data = flippedData;

    return imageData;
}

void init_curr_block()
{
    curr_type = rand() % 7;
    memcpy(curr_block, next_block, 4 * 4 * sizeof(int));
    memcpy(next_block, tetrominos[curr_type], 4 * 4 * sizeof(int));
    curr_pos[0] = init_pos[0];
    curr_pos[1] = init_pos[1];
}

void init()
{
    glClearColor(1, 1, 1, 0);
    glColor3f(0, 0, 0);
    srand(time(NULL));
    backgroundImage = loadImage("assets/mainscreen.png");
    startScreenImage = loadImage("assets/startscreen.png");
    gameOverImage = loadImage("assets/gameover.png");

    init_curr_block();
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);

    if (!gameStarted && startScreenImage.data) {
        glRasterPos2i(0, 0);
        glDrawPixels(startScreenImage.width, startScreenImage.height, GL_RGBA, GL_UNSIGNED_BYTE, startScreenImage.data);
    }
    else if (gameOver && gameOverImage.data) {

        glRasterPos2i(0, 0);
        glDrawPixels(gameOverImage.width, gameOverImage.height, GL_RGBA, GL_UNSIGNED_BYTE, gameOverImage.data);

        // print score as well
        glColor3f(1.0f, 1.0f, 1.0f);
        glRasterPos2f(9.0f, 5.0f);
        std::string scoreStr = std::to_string(points * 10);
        for (const char& c : scoreStr) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
        }
    }
    else {

        if (backgroundImage.data) {
            glRasterPos2i(0, 0);
            glDrawPixels(backgroundImage.width, backgroundImage.height, GL_RGBA, GL_UNSIGNED_BYTE, backgroundImage.data);
        }

        glColor3f(1.0f, 1.0f, 1.0f);
        glRasterPos2f(16.0f, 13.0f);
        std::string pointsStr = std::to_string(points * 10);
        for (const char& c : pointsStr) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
        }


        //Draw next block 
        glColor3f(1.0f, 1.0f, 1.0f); // Set color to white

        glRasterPos2f(5.0f, 5.5f);

        float blockSize = 0.8f;

        glBegin(GL_QUADS);
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                if (next_block[i][j]) {
                    glColor3fv(colors[curr_type]);
                    glVertex2f((j + 10) * blockSize + 7, (column_count - i - 1) * blockSize + 5);
                    glVertex2f((j + 10) * blockSize + 7, (column_count - i - 2) * blockSize + 5);
                    glVertex2f((j + 11) * blockSize + 7, (column_count - i - 2) * blockSize + 5);
                    glVertex2f((j + 11) * blockSize + 7, (column_count - i - 1) * blockSize + 5);
                }
            }
        }
        glEnd();

        float scaleFactor = 1.2f;
        // Draw Tetris blocks
        glBegin(GL_QUADS);
        glColor3f(1.0f, 0.0f, 0.0f); // Set color to red
        for (int i = 0; i < column_count; i++) {
            for (int j = 0; j < row_count; j++) {
                if (blocks[i][j]) {
                    // Scale up Tetris blocks
                    glColor3fv(colors[curr_type]);
                    glVertex2f(j * scaleFactor, (column_count - i - 1) * scaleFactor + 1);
                    glVertex2f(j * scaleFactor, (column_count - i - 2) * scaleFactor + 1);
                    glVertex2f((j + 1) * scaleFactor, (column_count - i - 2) * scaleFactor + 1);
                    glVertex2f((j + 1) * scaleFactor, (column_count - i - 1) * scaleFactor + 1);
                }
            }
        }
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                if (curr_block[i][j]) {
                    // Scale up current Tetrimino
                    glVertex2f((curr_pos[0] + j) * scaleFactor, (column_count - (curr_pos[1] + i) - 1) * scaleFactor + 1);
                    glVertex2f((curr_pos[0] + j) * scaleFactor, (column_count - (curr_pos[1] + i + 1) - 1) * scaleFactor + 1);
                    glVertex2f((curr_pos[0] + j + 1) * scaleFactor, (column_count - (curr_pos[1] + i + 1) - 1) * scaleFactor + 1);
                    glVertex2f((curr_pos[0] + j + 1) * scaleFactor, (column_count - (curr_pos[1] + i) - 1) * scaleFactor + 1);
                }
            }
        }
        glEnd();
    }
    glFlush();
}



void reshape(int width, int height)
{
    glViewport(0, 0, width, height);
    glLoadIdentity();
    gluOrtho2D(0, width / block_size, 0, height / block_size);
}

bool is_empty(int next_x, int next_y)
{
    if (next_x < 0 || row_count <= next_x) {
        return false;
    }
    if (next_y < 0 || column_count <= next_y) {
        return false;
    }
    if (blocks[next_y][next_x]) {
        return false;
    }
    return true;
}

bool move(int x, int y)
{
    if (!gameStarted) {
        return false;
    }
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (curr_block[i][j]) {
                int next_x = curr_pos[0] + j + x;
                int next_y = curr_pos[1] + i + y;
                if (!is_empty(next_x, next_y)) {
                    return false;
                }
            }
        }
    }
    curr_pos[0] += x;
    curr_pos[1] += y;
    glutPostRedisplay();
    return true;
}

void game_over()
{
    memset(curr_block, 0, 4 * 4 * sizeof(int));
    curr_pos[0] = 0;
    curr_pos[1] = 0;
    glutSetWindowTitle("GAME OVER");
    gameOver = true;
}

void timer(int value);
void flush(int value);
void shift(int value);

void timer(int value)
{


    if (!gameStarted) {
        glutTimerFunc(interval, timer, 0);
        return;
    }
    if (!move(0, 1)) {
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                if (curr_block[i][j]) {
                    blocks[curr_pos[1] + i][curr_pos[0] + j] = 1;
                }
            }
        }
        memset(curr_block, 0, 4 * 4 * sizeof(int));
        curr_pos[0] = 0;
        curr_pos[1] = 0;
        flush(0);
        return;
    }
    glutTimerFunc(interval, timer, 0);
}

void throw_new_block()
{
    init_curr_block();
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (curr_block[i][j] && blocks[curr_pos[1] + i][curr_pos[0] + j]) {
                game_over();
                return;
            }
        }
    }
    glutPostRedisplay();
    glutTimerFunc(interval, timer, 0);
}

void flush(int value)
{
    for (int i = column_count - 1; i >= 0; i--) {
        int j;
        for (j = 0; j < row_count; j++) {
            if (!blocks[i][j]) {
                break;
            }
        }
        if (j == row_count) {
            std::ostringstream os;
            os << "Ty-Tetris: " << ++points * 10 << " points";
            glutSetWindowTitle(os.str().c_str());
            memset(blocks[i], 0, row_count * sizeof(int));
            glutPostRedisplay();
            glutTimerFunc(interval, shift, i);
            return;
        }
    }
    throw_new_block();
}

void shift(int y)
{
    for (int i = y; i >= 1; i--) {
        for (int j = 0; j < row_count; j++) {
            blocks[i][j] = blocks[i - 1][j];
        }
    }
    glutPostRedisplay();
    glutTimerFunc(interval, flush, 0);
}

void rotate()
{
    int tmp[4][4] = {};

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (curr_block[i][j]) {
                int next_x = curr_pos[0] + (3 - i);
                int next_y = curr_pos[1] + j;
                if (!is_empty(next_x, next_y)) {
                    return;
                }
                tmp[j][3 - i] = 1;
            }
        }
    }

    memcpy(curr_block, tmp, 4 * 4 * sizeof(int));

    glutPostRedisplay();
}


void special(int key, int x, int y)
{
    switch (key) {
    case GLUT_KEY_LEFT:
        move(-1, 0);
        break;
    case GLUT_KEY_RIGHT:
        move(1, 0);
        break;
    case GLUT_KEY_UP:
        rotate();
        break;
    case GLUT_KEY_DOWN:
        move(0, 1);
        break;
    }
}


void keyboard(unsigned char key, int x, int y)
{
    if (!gameStarted && key == 13) { // ASCII code for Enter key
        gameStarted = true;

    }
}

int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA);
    glutInitWindowSize(block_size * row_count + 200, block_size * column_count + 100);
    glutCreateWindow("Ty-Tetris: 0 points");
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutTimerFunc(interval, timer, 0);
    glutSpecialFunc(special);
    glutKeyboardFunc(keyboard);
    memcpy( next_block , tetrominos[rand() % 7], 4 * 4 * sizeof(int));    //to init next for the first time 
    init();
  
    glutMainLoop();
    return 0;
}
