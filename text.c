//This is Bill's text code from lecture for drawing modified to my liking

#include "text.h"
#include "mylib.h"


void drawChar(int row, int col, char ch, unsigned short color) {
    for(int r = 0; r < 8; r++) {
        for(int c = 0; c < 6; c++) {
            if(fontdata_6x8[r * 6 + c + ch * 48]) {
                setPixel(row + r, col + c, color);
            }
        }
    }
}

void drawString(int row, int col, char *str, unsigned short color) {
    while(*str) {
        drawChar(row, col, *str++, color);
        col += 6;
    }
}