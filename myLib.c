#include "myLib.h"

u16 *videoBuffer = (u16 *)0x6000000;

void setPixel(int r, int c, u16 color) {
	videoBuffer[OFFSET(r,c, 240)] = color; 
}

void drawRect(int row, int col, int height, int width, u16 color) {
	int r, c;
	for (r = 0; r < height; r++)	{
		for (c = 0; c < width; c++) {
			setPixel (r + row, c + col, color);
		}
	}
}

void drawHollow(int row, int col, int height, int width, u16 color) {
	for (int i = row; i < row + height; i++) {
		setPixel(i, col, color);
		setPixel(i, col + width, color);
	}
	for (int j = col; j < col + width; j++) {
		setPixel(row, j, color);
		setPixel(row + height, j, color);
	}

}

void drawImage3(int x, int y, int width, int height, const u16 *image, const u16 *bg) {
	int k = 0;
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			if (image[k] != 0x0721f) {
				videoBuffer[(i + x) * 240 + (j + y)] = image[k++];
			} else {
				videoBuffer[(i + x) * 240 + (j + y)] = bg[(i + x) * 240 + (j + y)];
				k++;
			}
		}
	}
}

void waitForVblank() {
	while(SCANLINECOUNTER > 160);
	while(SCANLINECOUNTER < 160);
}

