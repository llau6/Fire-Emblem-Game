#include "myLib.h"
#include "Map1.h"
#include "Map2.h"
#include "chrom.h"
#include "opening.h"
#include "enemy.h"
#include "enter.h"
#include "chrom2.h"
#include "bovis.h"
#include "end.h"
#include "defeat.h"
#include "fire.h"
#include "start.h"
#include "startBG.h"
#include <stdlib.h>
#define ENEMY_NUM 4
#define FIRE_NUM 1

typedef struct {
	int row;
	int col;
	int rd;
	int cd;
	int size;
} SPRITE;

void detectCollision(SPRITE *p, int width, int height, SPRITE *e, int eWidth, int eHeight, int *collide);
void checkBoundaries(SPRITE *p, int width, int height);

int main() {
	REG_DISPCTL = MODE3 | BG2_ENABLE;

// Declare player and enemy arrays/pointers
	SPRITE player[1];	
	SPRITE enemies[ENEMY_NUM];
	SPRITE *pPlayer = player;
	SPRITE *pEnemy = enemies;
	SPRITE fireArr[FIRE_NUM];
	SPRITE *pFire = fireArr;

// Initialize player
	pPlayer->row = 20;
	pPlayer->col = 0;
// Initialize fire
	pFire->row = 95;
	pFire->col = 50;
	pFire->rd = -1;
	pFire->cd = -1;
// Initialize enemies
	for (int i = 0; i < ENEMY_NUM; i++) {
		int sign = 1;
		int speed = 2;
		if ((i & 0x1) == 1) {                                         
			sign = -1;
			speed = 1;
		}
		enemies[i].row = 50;
		enemies[i].col = 35 + 50 * i;
		enemies[i].rd =  speed * sign;
	}
// Declare States
	enum GBAState {
		START,
		CHECK_ENTER,
		MAP1,
		GAME,
		DIALOGUE,
		CHECK_A,
		MAP2,
		FINAL,
		END
	};

	int collide = 0;
	int *pCollide = &collide;
	enum GBAState state = START;
// Start game loop
	while(1) {
		// Back to Start Screen
		if (KEY_DOWN_NOW(BUTTON_SELECT)) {
			state = START;
		}
		switch (state) {
// Waiting for player to press start, blinking "Press Start"
		case START:
			drawImage3(0, 0, OPENING_WIDTH, OPENING_HEIGHT, opening, opening);
			drawImage3(120, 80, START_WIDTH, START_HEIGHT, start, opening);
			state = CHECK_ENTER;				
			break;
		case CHECK_ENTER:
			//drawImage3(120,80, STARTBG_WIDTH, STARTBG_HEIGHT, startBG, opening);
			if (KEY_DOWN_NOW(BUTTON_START)) {
				state = MAP1;
			}
			break;
// Draw first map background after player presses start
		case MAP1:
			pPlayer->row = 20;
			pPlayer->col = 0;
			drawImage3(0, 0, MAP1_WIDTH, MAP1_HEIGHT, Map1, Map1);
			drawImage3(pPlayer->row, pPlayer->col, CHROM_WIDTH, CHROM_HEIGHT, chrom, Map1);
			collide = 0;
			state = GAME;
			break;
// Objective: Run from archers
		case GAME:
			// Buttons
			if (KEY_DOWN_NOW(BUTTON_UP)) {
				pPlayer->row = pPlayer->row - 2;
				drawImage3(pPlayer->row, pPlayer->col, CHROM_WIDTH, CHROM_HEIGHT, chrom, Map1);
			}
			if (KEY_DOWN_NOW(BUTTON_DOWN)) {
				pPlayer->row = pPlayer->row + 2;
				drawImage3(pPlayer->row, pPlayer->col, CHROM_WIDTH, CHROM_HEIGHT, chrom, Map1);			
			}
			if (KEY_DOWN_NOW(BUTTON_LEFT)) {
				pPlayer->col = pPlayer->col - 2;
				drawImage3(pPlayer->row, pPlayer->col, CHROM_WIDTH, CHROM_HEIGHT, chrom, Map1);
			}
			if (KEY_DOWN_NOW(BUTTON_RIGHT)) {
				pPlayer->col = pPlayer->col + 2;
				drawImage3(pPlayer->row, pPlayer->col, CHROM_WIDTH, CHROM_HEIGHT, chrom, Map1);
			}
			checkBoundaries(pPlayer, CHROM_WIDTH, CHROM_HEIGHT);
			// Enemy movements
			for (int i = 0; i < ENEMY_NUM; i++) {
				pEnemy = enemies + i;		
				pEnemy->row =  pEnemy->row + pEnemy->rd;
				checkBoundaries(pEnemy, ENEMY_WIDTH, ENEMY_HEIGHT);
				drawImage3(pEnemy->row, pEnemy->col, ENEMY_WIDTH, ENEMY_HEIGHT, enemy, Map1);
				detectCollision(pPlayer, CHROM_WIDTH, CHROM_HEIGHT, pEnemy, ENEMY_WIDTH, ENEMY_HEIGHT, pCollide);
			}		
			waitForVblank();
			// Restart level, reload map
			if (collide == 1) {
				state = MAP1;
			}
			// If player passes enemies
			if (pPlayer->col + CHROM_WIDTH == 240) {
				state = DIALOGUE;
			}
			break;
// Talking to final boss
		case DIALOGUE:
			drawImage3(0, 0, BOVIS_WIDTH, BOVIS_HEIGHT, bovis, bovis);
			drawImage3(0, 0, DEFEAT_WIDTH, DEFEAT_HEIGHT, defeat, bovis);
			state = CHECK_A;
			break;
		case CHECK_A:
			if (KEY_DOWN_NOW(BUTTON_A)) {
				state = MAP2;
			}
			break;
// Load map and new sprite
		case MAP2:
			pPlayer->row = 50;
			pPlayer->col = 0;
			drawImage3(0, 0, MAP2_WIDTH, MAP2_HEIGHT, Map2, Map2);
			drawImage3(pPlayer->row, pPlayer->col, CHROM2_WIDTH, CHROM2_HEIGHT, chrom2, Map2);
			state = FINAL;
			break;
// Battle the boss
		case FINAL:
			for (int i = 0; i < FIRE_NUM; i++) {
				pFire->col = pFire->col + pFire->cd;
				if (pFire-> col == -10) {
					pFire->col = 240;
				}
				drawImage3(pFire->row, pFire->col, FIRE_WIDTH, FIRE_HEIGHT, fire, Map2);
				detectCollision(pPlayer, CHROM2_WIDTH, CHROM2_HEIGHT, pFire, FIRE_WIDTH, FIRE_HEIGHT, pCollide);
				if (collide == 1) {
					collide = 0;
					state = MAP1;
				}
			}
			if (KEY_DOWN_NOW(BUTTON_UP)) {
				pPlayer->row = pPlayer->row - 1;
				drawImage3(pPlayer->row, pPlayer->col, CHROM2_WIDTH, CHROM2_HEIGHT, chrom2, Map2);
			}
			if (KEY_DOWN_NOW(BUTTON_DOWN)) {
				pPlayer->row = pPlayer->row + 1;
				drawImage3(pPlayer->row, pPlayer->col, CHROM2_WIDTH, CHROM2_HEIGHT, chrom2, Map2);
			}
			if (KEY_DOWN_NOW(BUTTON_LEFT)) {
				pPlayer->col = pPlayer->col - 1;
				drawImage3(pPlayer->row, pPlayer->col, CHROM2_WIDTH, CHROM2_HEIGHT, chrom2, Map2);
			}
			if (KEY_DOWN_NOW(BUTTON_RIGHT)) {
				pPlayer->col = pPlayer->col + 1;
				drawImage3(pPlayer->row, pPlayer->col, CHROM2_WIDTH, CHROM2_HEIGHT, chrom2, Map2);
			}
			checkBoundaries(pPlayer, CHROM2_WIDTH, CHROM2_HEIGHT);
			if ((pPlayer->col + CHROM2_WIDTH > 175) 
				&& (pPlayer->col + CHROM2_WIDTH < 190)
				&& (pPlayer->row < 90) 
				&& (pPlayer->row + CHROM2_HEIGHT > 60)) {
				state = END;			
			}
			break;
			case END:
				drawImage3(0, 0, END_WIDTH, END_HEIGHT, end, end);
				break;
		} // Switch
	} // While
	return 0;
} // Main

void checkBoundaries(SPRITE *p, int width, int height) {
	if (p->row < 0) {
		p->row = 0;
		p->rd = -p->rd;
	}
	if (p->row > 160 - height) {
		p->row = 160 - height;
		p->rd = -p->rd;
	}
	if (p->col < 0) {
		p->col = 0;
		p->cd = -p->cd;
	}
	if (p->col > 240 - width) {
		p->col = 240 - width;
		p->cd = -p->cd;
	}
}

void detectCollision(SPRITE *p, int width, int height, SPRITE *e, int eWidth, int eHeight, int *collide) {
	if ((((p->row < e->row + eHeight-5) && (p->row + height > e->row + eHeight))
		|| ((p->row + height-5> e->row) && (p->row < e->row)))
		&& (((p->col + width-10 > e->col) && (p->col < e->col + eWidth))
		|| ((p->col + width-5> e->col + eWidth) && (p->col < e->col + eWidth)))) {
		*collide = 1;
	}
}
















