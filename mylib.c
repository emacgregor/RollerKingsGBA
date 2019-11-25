//Name: Ethan MacGregor

#include <stdio.h>
#include "mylib.h"
#include "start.h"
#include "stories.h"
#include "pause.h"
#include "text.h"
#include "smile.h"
#include "sad.h"
#include "fireballL.h"
#include "fireballR.h"
#include "endscreens.h"

unsigned short* videoBuffer = (unsigned short *) 0x6000000;

void drawRect(int row, int col, int height, int width, unsigned short color) {
    if (height == 0 || width == 0) {
        return;
    }
    for(int r = 0; r < height; r++) {
        DMA[3].src = &color;
        DMA[3].dst = videoBuffer + (row + r) * 240 + col;
        DMA[3].cnt = width | DMA_SOURCE_FIXED | DMA_ON;
    }
}

void setPixel(int row, int col, unsigned short color) {
    videoBuffer[row * GBA_WIDTH + col] = color;
}
void waitForVblank() {
    while(SCANLINECOUNTER > 160);
    while(SCANLINECOUNTER < 159);
}
void drawStart() {
    drawFullScreenImage(start);
}
void drawFullScreenImage(const unsigned short *image) {
    DMA[3].src = image;
    DMA[3].dst = videoBuffer;
    DMA[3].cnt = 240 * 160 | DMA_SOURCE_INCREMENT | DMA_ON;
}

void drawYellowStory() {
    drawFullScreenImage(yellow_story);
}
void drawRedStory() {
    drawFullScreenImage(red_story);
}


void drawGameBackground(int yellowWins, int redWins) {
    drawRect(150, 0, 10, 240, BROWN);
    drawRect(145, 0, 5, 240, GREEN);
    drawRect(0, 0, 145, 240, CYAN);
    char buffer[7];
    sprintf(buffer, "HEALTH:");
    drawString(0, 0, buffer, RED);
    drawString(0, 143, buffer, RED);

    char buffer2[7];
    sprintf(buffer2, "ENERGY:");
    drawString(11, 0, buffer2, BLACK);
    drawString(11, 183, buffer2, BLACK);

    char buffer3[30];
    sprintf(buffer3, "Yellow vs Red: %d - %d ", yellowWins, redWins);
    drawString(24, 55, buffer3, BLACK);
}

void drawImage(int x, int y, int width, int height, const unsigned short *image) {
    for (int i = 0; i < height; i++) {
        DMA[3].src = image + i * width;
        DMA[3].dst = videoBuffer + (y + i) * 240 + x;
        DMA[3].cnt = width | DMA_SOURCE_INCREMENT | DMA_ON;
    }
}
void drawFireball(int x, int y, int active, int direction) {
    if (active) {
        if (direction) {
            for (int i = 0; i < 10; i++) {
                //I can't use DMA for fireball because I need to get rid of some white pixels
                for (int j = 0; j < 10; j++) {
                    if (fireballR[i * 10 + j] != BLACK) {
                        setPixel(j + y, i + x, fireballR[i + j * 10]);
                    }
                }
            }
        } else {
            for (int i = 0; i < 10; i++) {
                //I can't use DMA for fireball because I need to get rid of some white pixels
                for (int j = 0; j < 10; j++) {
                    if (fireballL[i * 10 + j] != BLACK) {
                        setPixel(j + y, i + x, fireballL[i + j * 10]);
                    }
                }
            }
        }
    }
}
void unDrawFireball(int x, int y, int active, int direction) {
    if (active) {
        if (direction) {
            for (int i = 0; i < 10; i++) {
                //I can't use DMA for fireball because I need to get rid of some white pixels
                for (int j = 0; j < 10; j++) {
                    if (fireballR[i * 10 + j] != BLACK) {
                        setPixel(j + y, i + x, CYAN);
                    }
                }
            }
        } else {
            for (int i = 0; i < 10; i++) {
                //I can't use DMA for fireball because I need to get rid of some white pixels
                for (int j = 0; j < 10; j++) {
                    if (fireballL[i * 10 + j] != BLACK) {
                        setPixel(j + y, i + x, CYAN);
                    }
                }
            }
        }
    }
}

void drawPause() {
    drawImage(60, 20, PAUSE_WIDTH, PAUSE_HEIGHT, pause);
}

void drawLineUpRight(int x, int y, int length, unsigned short color) {
    for (int i = 0; i < length; i++) {
        setPixel(y - i, x + i, color);
    }
}
void drawLineUpLeft(int x, int y, int length, unsigned short color) {
    for (int i = 0; i < length; i++) {
        setPixel(y - i, x - i, color);
    }
}
void drawVerticalLine(int x, int y, int length, unsigned short color) {
    for (int i = 0; i < length; i++) {
        setPixel(y - i, x , color);
    }
}
void drawHorizontalLine(int x, int y, int length, unsigned short color) {
    DMA[3].src = &color;
    DMA[3].dst = videoBuffer + y * 240 + x;
    DMA[3].cnt = length | DMA_SOURCE_FIXED | DMA_ON;
}

//draw circle always draws of size 10 because letting the length be controllable is too hard
//draws from center
//While this code is monstrously ugly it is actually pretty efficient for having no for loop
void drawCircle(int x, int y, unsigned short color) {
    setPixel(y - 10, x, color);
    setPixel(y, x + 10, color);
    setPixel(y + 10, x, color);
    setPixel(y, x - 10, color);

    setPixel(y - 10, x + 1, color);
    setPixel(y - 10, x + 2, color);
    setPixel(y - 10, x + 3, color);
    setPixel(y - 9, x + 4, color);
    setPixel(y - 9, x + 5, color);
    setPixel(y - 8, x + 6, color);
    setPixel(y - 8, x + 7, color);
    setPixel(y - 7, x + 8, color);
    setPixel(y - 6, x + 8, color);
    setPixel(y - 5, x + 9, color);
    setPixel(y - 4, x + 9, color);
    setPixel(y - 3, x + 10, color);
    setPixel(y - 2, x + 10, color);
    setPixel(y - 1, x + 10, color);

    setPixel(y + 10, x + 1, color);
    setPixel(y + 10, x + 2, color);
    setPixel(y + 10, x + 3, color);
    setPixel(y + 9, x + 4, color);
    setPixel(y + 9, x + 5, color);
    setPixel(y + 8, x + 6, color);
    setPixel(y + 8, x + 7, color);
    setPixel(y + 7, x + 8, color);
    setPixel(y + 6, x + 8, color);
    setPixel(y + 5, x + 9, color);
    setPixel(y + 4, x + 9, color);
    setPixel(y + 3, x + 10, color);
    setPixel(y + 2, x + 10, color);
    setPixel(y + 1, x + 10, color);

    setPixel(y - 10, x - 1, color);
    setPixel(y - 10, x - 2, color);
    setPixel(y - 10, x - 3, color);
    setPixel(y - 9, x - 4, color);
    setPixel(y - 9, x - 5, color);
    setPixel(y - 8, x - 6, color);
    setPixel(y - 8, x - 7, color);
    setPixel(y - 7, x - 8, color);
    setPixel(y - 6, x - 8, color);
    setPixel(y - 5, x - 9, color);
    setPixel(y - 4, x - 9, color);
    setPixel(y - 3, x - 10, color);
    setPixel(y - 2, x - 10, color);
    setPixel(y - 1, x - 10, color);

    setPixel(y + 10, x - 1, color);
    setPixel(y + 10, x - 2, color);
    setPixel(y + 10, x - 3, color);
    setPixel(y + 9, x - 4, color);
    setPixel(y + 9, x - 5, color);
    setPixel(y + 8, x - 6, color);
    setPixel(y + 8, x - 7, color);
    setPixel(y + 7, x - 8, color);
    setPixel(y + 6, x - 8, color);
    setPixel(y + 5, x - 9, color);
    setPixel(y + 4, x - 9, color);
    setPixel(y + 3, x - 10, color);
    setPixel(y + 2, x - 10, color);
    setPixel(y + 1, x - 10, color);
}
//Draws from middle of head
void drawCrown(int x, int y, unsigned short color) {
    drawRect(y - 15, x - 10, 10, 21, color);
    drawRect(y - 20, x - 10, 5, 3, color);
    drawRect(y - 20, x - 1, 5, 3, color);
    drawRect(y - 20, x + 8, 5, 3, color);
}

void drawEyes(int x, int y, unsigned short color) {
    drawRect(y - 4, x - 6, 3, 3, color);
    drawRect(y - 4, x + 4, 3, 3, color);
}

//draw face starts at the center of the face
void drawFace(int x, int y, unsigned short color) {
    drawHorizontalLine(x - 5, y + 5, 11, color);
    drawLineUpRight(x, y - 5, 5, color);
    drawLineUpLeft(x, y - 5, 5, color);
    drawEyes(x, y, color);
}

void drawFighterColor(int x, int y, int squat, int rotation, int fighterNum, int punch, unsigned short color) {
    if (!squat) { //This loop is for normal drawing
        drawLineUpRight(x, y, 10, color);
        drawLineUpLeft(x + 20, y, 10, color);
        drawVerticalLine(x + 10, y - 10, 20, color);
        drawCircle(x + 10, y - 40, color);
        if (punch == 0 || punch > 5) {
            drawHorizontalLine(x, y - 20, 21, color);
        } else {
            drawHorizontalLine(x + 7, y - 20, 6, color);
            drawLineUpLeft(x + 7, y - 20, 10, color);
            drawLineUpRight(x + 13, y - 20, 10, color);
        }
        if (fighterNum == -1) {
            drawCrown(x + 10, y - 40, CYAN);
            if (punch == 0 || punch > 5) {
                drawRect(y - 22, x, 5, 5, CYAN);
                drawRect(y - 22, x + 16, 5, 5, CYAN);
            } else {
                drawRect(y - 32, x - 3, 5, 5, CYAN);
                drawRect(y - 32, x + 19, 5, 5, CYAN);
            }
        } else if (fighterNum) {
            drawCrown(x + 10, y - 40, RED);
            if (punch == 0 || punch > 5) {
                drawRect(y - 22, x, 5, 5, RED);
                drawRect(y - 22, x + 16, 5, 5, RED);
            } else {
                drawRect(y - 32, x - 3, 5, 5, RED);
                drawRect(y - 32, x + 19, 5, 5, RED);
            }
        } else {
            drawCrown(x + 10, y - 40, YELLOW);
            if (punch == 0 || punch > 5) {
                drawRect(y - 22, x, 5, 5, YELLOW);
                drawRect(y - 22, x + 16, 5, 5, YELLOW);
            } else {
                drawRect(y - 32, x - 3, 5, 5, YELLOW);
                drawRect(y - 32, x + 19, 5, 5, YELLOW);
            }
        }
        drawFace(x + 10, y - 40, color);
    } else { //The else loop takes care of when the fighters are balls
        if (rotation == 0) {
            drawCircle(x + 10, y - 10, color);
            drawFace(x + 10, y - 10, color);
        } else if (rotation == 1) {
            drawCircle(x + 10, y - 10, color);
            drawVerticalLine(x + 5, y - 5, 11, color);
            drawRect(y - 6, x + 12, 3, 3, color);
            drawRect(y - 16, x + 12, 3, 3, color);
            drawLineUpLeft(x + 19, y - 6, 5, color);
            drawLineUpRight(x + 15, y - 10, 5, color);
        } else if (rotation == 2) {
            drawCircle(x + 10, y - 10, color);
            drawHorizontalLine(x + 5, y - 15, 11, color);
            drawRect(y - 9, x + 4, 3, 3, color);
            drawRect(y - 9, x + 14, 3, 3, color);
            drawLineUpLeft(x + 14, y - 1, 5, color);
            drawLineUpRight(x + 6, y - 1, 5, color);
        } else if (rotation == 3) {
            drawCircle(x + 10, y - 10, color);
            drawVerticalLine(x + 15, y - 5, 11, color);
            drawRect(y - 6, x + 6, 3, 3, color);
            drawRect(y - 16, x + 6, 3, 3, color);
            drawLineUpLeft(x + 5, y - 10, 5, color);
            drawLineUpRight(x + 1, y - 6, 5, color);
        }
    }
}

//Sprites to draw fighters of this size will fail to draw correctly so I harcode it in
void drawFighter(int x, int y, int squat, int rotation, int fighterNum, int punch) {
    drawFighterColor(x, y, squat, rotation, fighterNum, punch, BLACK);
}
void unDrawFighter(int x, int y, int squat, int rotation, int punch) {
    drawFighterColor(x, y, squat, rotation, -1, punch, CYAN);
}
void drawHealth(int health, int fighterNum) {
    if (fighterNum) {
        drawRect(0, 188, 10, health / 2, RED);
        drawRect(0, 188 + (health / 2), 10, 50 - (health / 2), CYAN); 
    } else {
        drawRect(0, 45, 10, health, RED);
        drawRect(0, 45 + health, 10, 10 - health, CYAN); 
    }
}
void drawEnergy(int energy, int fighterNum) {
    if (fighterNum) {
        drawRect(11, 228, 10, energy / 10, BLACK);
        drawRect(11, 228 + (energy / 10), 10, 10 - (energy / 10), CYAN); 
    } else {
        drawRect(11, 45, 10, energy / 10, BLACK);
        drawRect(11, 45 + energy / 10, 10, 10 - (energy / 10), CYAN); 
    }
}

void delay(int n) {
    volatile int x = 0;
    for(int i = 0; i < n * 6000; i++) {
        x = x + 1;
    }
}
void drawGif(const unsigned short** gif_Frames, int numFrames, int delayFrames) {
    for(int i = 0; i < numFrames; i++) {
        drawFullScreenImage(gif_Frames[i]);
        delay(delayFrames);
    }

}
void drawSmile() {
    drawGif(smile_frames, SMILE_FRAMES, 2);
}
void drawSad() {
    drawGif(sad_frames, SAD_FRAMES, 5);
}
void drawVictory() {
    drawFullScreenImage(VICTORY);
}
void drawDefeat() {
    drawFullScreenImage(DEFEAT);
}