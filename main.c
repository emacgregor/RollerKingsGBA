//Name: Ethan MacGregor

#include <stdio.h>
#include <stdlib.h>
#include "mylib.h"
#include "text.h"

//our states
enum GBAState {
    START,
    START_NODRAW,
    YELLOW_STORY,
    YELLOW_NO_DRAW,
    RED_STORY,
    RED_NO_DRAW,
    GAME_DRAW,
    GAME,
    LOSE,
    LOSE_NODRAW,
    WIN,
    WIN_NODRAW
};

//the fighter has a lot of variables to keep track of
typedef struct 
{
    int x;
    int oldx;
    int y;
    int oldy;
    int xvel;
    int yvel;
    int canPunch;
    int canJump;
    int squat;
    int oldsquat;
    int rotation;
    int oldrotation;
    int energy;
    int health;
    int punch;
    int oldpunch;

    int firex;
    int firey;
    int oldfirex;
    int oldfirey;
    int active;
    int oldactive;
    int direction;
    int olddirection;
} FIGHTER;

int canPause = 1;
int canSquat = 1;

int yellowWins = 0;
int redWins = 0;

void fighterCollisions(FIGHTER* fighters) {
    //I don't bother myself about two fists overlapping and interacting weirdly because it's a rare occurance and having fists never overlap would be stranger
    //Lots of variables to check but it all works out in the end. Four distinct cases to worry about although you could change it to two conceviably
    if (fighters[0].x + 21 > fighters[1].x && fighters[0].x < fighters[1].x && fighters[1].xvel < 0 && !fighters[0].squat && !fighters[1].squat) {
        fighters[1].x = fighters[0].x + 21;
        fighters[0].xvel = 0;
        fighters[1].xvel = 0;

        //wall collission
        if (fighters[1].x + 21 > 240) {
            fighters[1].x = 219;
            fighters[0].x = 198;
        }
        //special "punching" wall collision
        if (fighters[1].x + 21 + 3 > 240 && fighters[1].punch != 0) {
            fighters[1].x = 216;
            fighters[0].x = 195;
        }
    }
    if (fighters[1].x + 21 > fighters[0].x && fighters[1].x < fighters[0].x && fighters[1].xvel > 0 && !fighters[0].squat && !fighters[1].squat) {
        fighters[1].x = fighters[0].x - 21;
        fighters[0].xvel = 0;
        fighters[1].xvel = 0;

        //wall collission
        if (fighters[1].x  < 0) {
            fighters[1].x = 0;
            fighters[0].x = 21;
        }
        //special "punching" wall collision
        if (fighters[1].x < 3 && fighters[1].punch != 0) {
            fighters[1].x = 3;
            fighters[0].x = 27;
        }
    }
    if (fighters[1].x + 21 > fighters[0].x && fighters[1].x < fighters[0].x && fighters[0].xvel < 0 && !fighters[0].squat && !fighters[1].squat) {
        fighters[0].x = fighters[1].x + 21;
        fighters[0].xvel = 0;
        fighters[1].xvel = 0;

        //wall collission
        if (fighters[0].x + 21 > 240) {
            fighters[0].x = 219;
            fighters[1].x = 198;
        }
        //special "punching" wall collision
        if (fighters[1].x + 21 + 3 > 240 && fighters[1].punch != 0) {
            fighters[1].x = 216;
            fighters[0].x = 195;
        }
    }
    if (fighters[0].x + 21 > fighters[1].x && fighters[0].x < fighters[1].x && fighters[0].xvel > 0 && !fighters[0].squat && !fighters[1].squat) {
        fighters[0].x = fighters[1].x - 21;
        fighters[0].xvel = 0;
        fighters[1].xvel = 0;

        //wall collission
        if (fighters[0].x  < 0) {
            fighters[0].x = 0;
            fighters[1].x = 21;
        }
        //special "punching" wall collision
        if (fighters[1].x < 3 && fighters[1].punch != 0) {
            fighters[1].x = 3;
            fighters[0].x = 27;
        }
    }
}

//In the players turn we just look at button inputs and react accordingly
void playersTurn(FIGHTER* fighters) {

    //move right
    if(KEY_DOWN_NOW(BUTTON_RIGHT)) {
        //speed cap
        if(fighters[0].xvel < 4) {
            fighters[0].xvel = fighters[0].xvel + 2;
        }
    }

    //move left
    if(KEY_DOWN_NOW(BUTTON_LEFT)) {
        //speed cap
        if(fighters[0].xvel > -4) {
            fighters[0].xvel = fighters[0].xvel - 2;
        }
    }

    //Jump is up or a and we don't let them jump if they're in the air
    if((KEY_DOWN_NOW(BUTTON_A) || KEY_DOWN_NOW(BUTTON_UP)) && fighters[0].canJump) {
        fighters[0].yvel = fighters[0].yvel - 10;
        fighters[0].canJump = 0;
    }

    //squat / roll plus providence to make sure one doesn't move up and down from squat from holding down down
    if (KEY_DOWN_NOW(BUTTON_DOWN) && canSquat) {
        fighters[0].squat = fighters[0].squat ^ 1;
        fighters[0].rotation = 0;
        canSquat = 0;
    } else if (!KEY_DOWN_NOW(BUTTON_DOWN) && !canSquat) {
        canSquat = 1;
    }

    //punch. we don't let them punch if they are rolling
    if (KEY_DOWN_NOW(BUTTON_B) && !fighters[0].squat) {
        if (fighters[0].punch == 0 && fighters[0].energy > 49) {
            fighters[0].punch = 1;
            fighters[0].energy = fighters[0].energy - 50;
        }
    }

    //fire hadouken left
    if (KEY_DOWN_NOW(BUTTON_L) && fighters[0].x > 10 && !fighters[0].squat && fighters[0].punch == 0 && fighters[0].energy > 25 && !fighters[0].active) {
        fighters[0].firex = fighters[0].x - 10;
        fighters[0].firey = fighters[0].y - 20;
        fighters[0].active = 1;
        fighters[0].direction = 0;
        fighters[0].energy = fighters[0].energy - 25;
    }
    //fire hadouken right
    if (KEY_DOWN_NOW(BUTTON_R) && fighters[0].x < 210 && !fighters[0].squat && fighters[0].punch == 0 && fighters[0].energy > 25 && !fighters[0].active) {
        fighters[0].firex = fighters[0].x + 20;
        fighters[0].firey = fighters[0].y - 20;
        fighters[0].active = 1;
        fighters[0].direction = 1;
        fighters[0].energy = fighters[0].energy - 25;
    }
}

void randysTurn(FIGHTER* fighters) {
    //Randy takes his turn by almost entirely 

    if(rand() % 1000 < 400) {
        //He gets too herky jerky if you don't give him some restrictions
        //Randy also gets a higher speed cap than you do
        if(fighters[1].xvel < 8 && fighters[1].xvel > -1) {
            fighters[1].xvel = fighters[1].xvel + 2;
        }
    }
    if(rand() % 1000 < 400) {
        //He gets too herky jerky if you don't give him some restrictions
        if(fighters[1].xvel > -8 && fighters[1].xvel < 1) {
            fighters[1].xvel = fighters[1].xvel - 2;
        }
    }
    //jump
    if(rand() % 1000 < 8 && fighters[1].canJump) {
        fighters[1].yvel = fighters[1].yvel - 10;
        fighters[1].canJump = 0;
    }
    //squat/roll
    if (rand() % 1000 < 8) {
        fighters[1].squat = fighters[1].squat ^ 1;
        fighters[1].rotation = 0;
    }
    //punch
    if (rand() % 1000 < 8 && !fighters[1].squat) {
        if (fighters[1].punch == 0 && fighters[1].energy > 49) {
            fighters[1].punch = 1;
            fighters[1].energy = fighters[1].energy - 50;
        }
    }
    //fire hadouken left
    if (rand() % 1000 < 16 && fighters[1].x > 10 && !fighters[1].squat && fighters[1].punch == 0 && fighters[1].energy > 25 && !fighters[1].active) {
        fighters[1].firex = fighters[1].x - 10;
        fighters[1].firey = fighters[1].y - 20;
        fighters[1].active = 1;
        fighters[1].direction = 0;
        fighters[1].energy = fighters[1].energy - 25;
    }
    //fire hadouken right
    if (rand() % 1000 < 16 && fighters[1].x < 210 && !fighters[1].squat && fighters[1].punch == 0 && fighters[1].energy > 25 && !fighters[1].active) {
        fighters[1].firex = fighters[1].x + 20;
        fighters[1].firey = fighters[1].y - 20;
        fighters[1].active = 1;
        fighters[1].direction = 1;
        fighters[1].energy = fighters[1].energy - 25;
    }
}

void updateStuff(FIGHTER* fighters) {
    for (int i = 0; i < NUMFIGHTERS; i++) {
        
        //gravity
        fighters[i].yvel = fighters[i].yvel + 1;

        //address velocity
        fighters[i].x = fighters[i].x + fighters[i].xvel;
        fighters[i].y = fighters[i].y + fighters[i].yvel;

        //slow guys down
        if (fighters[i].xvel > 0) {
            fighters[i].xvel = fighters[i].xvel - 1;
        }
        if (fighters[i].xvel < 0) {
            fighters[i].xvel = fighters[i].xvel + 1;
        }

        //rolling / squating (as it is named because I named the variables before I really implemented it)
        if (fighters[i].xvel > 0 && fighters[i].squat) {
            fighters[i].rotation++;
            if (fighters[i].rotation == 4) {
                fighters[i].rotation = 0;
            }
        }
        if (fighters[i].xvel < 0 && fighters[i].squat) {
            fighters[i].rotation--;
            if (fighters[i].rotation == -1) {
                fighters[i].rotation = 3;
            }
        }

        //energy updates. squatting always takes away energy and standing gives it back
        if (fighters[i].squat) {
            fighters[i].energy--;
        } else {
            if (fighters[i].energy < 100) {
                fighters[i].energy++;
            }
        }
        if (fighters[i].energy < 0) {
            fighters[i].energy = 0;
            fighters[i].squat = 0;
            if (!i) {
                canSquat = 1;
            }
        }

        //updates punch, the punch can only be used again after 10 frames and it is only active frames 1-4
        if (fighters[i].punch != 0) {
            if (fighters[i].punch < 10) {
                fighters[i].punch++;
            } else {
                fighters[i].punch = 0;
            }
        }

        //wall collision
        if (fighters[i].x < 0) {
            fighters[i].x = 0;
            fighters[i].xvel = 0;
        }
        if (fighters[i].x + 21 > GBA_WIDTH) {
            fighters[i].x = GBA_WIDTH - 21;
            fighters[i].xvel = 0;
        }

        //floor collision
        if (fighters[i].y > 144) {
            fighters[i].y = 144;
            fighters[i].yvel = 0;
            fighters[i].canJump = 1;
        }

        //wall collisions with punch active
        if (fighters[i].x < 3 && fighters[i].punch != 0) {
            fighters[i].x = 3;
            fighters[i].xvel = 0;
        }
        if (fighters[i].x > 240 - 21 - 3 && fighters[i].punch != 0) {
            fighters[i].x = 216;
            fighters[i].xvel = 0;
        }

        //kill hadoukens if collide with wall
        if (fighters[i].active) {
            if (fighters[i].direction) {
                fighters[i].firex = fighters[i].firex + 5;
                if (fighters[i].firex > 230) {
                    fighters[i].active = 0;
                }
            } else {
                fighters[i].firex = fighters[i].firex - 5;
                if (fighters[i].firex < 0) {
                    fighters[i].active = 0;
                }
            }
        }
    }
}
enum GBAState punch(FIGHTER* fighters) {
    //Checks for whether a punch has landed. A punch lands if the fist is within three pixels of the other fighter's face

    //Time stops when a punch is landed so we need a very clear visualazation of what happenned
    if (fighters[0].x + 26 > fighters[1].x && fighters[0].x < fighters[1].x && fighters[0].punch > 0 && fighters[0].punch < 5 && !fighters[1].squat) {
        unDrawFighter(fighters[0].oldx, fighters[0].oldy,fighters[0].oldsquat, fighters[0].oldrotation, fighters[0].oldpunch);
        drawFighter(fighters[0].x, fighters[0].y, fighters[0].squat, fighters[0].rotation, 0, fighters[0].punch);
        
        waitForVblank();

        delay(10);//Time stops for a bit when a hit is landed

        fighters[0].oldpunch = 1; //Makes sure it will undraw a punch figure
        fighters[0].punch = 5;//End punch active frames (ends at frame five)
        fighters[1].health = fighters[1].health - 5; //Update health

        //check for victory
        if (fighters[1].health < 1) {
            return WIN;
        }
    } 
    if (fighters[0].x + 26 > fighters[1].x && fighters[0].x < fighters[1].x && fighters[1].punch > 0 && fighters[1].punch < 5 && !fighters[0].squat) {
        unDrawFighter(fighters[1].oldx, fighters[1].oldy,fighters[1].oldsquat, fighters[1].oldrotation, fighters[1].oldpunch);
        drawFighter(fighters[1].x, fighters[1].y, fighters[1].squat, fighters[1].rotation, 1, fighters[1].punch);
        
        waitForVblank();

        delay(10);

        fighters[1].oldpunch = 1;
        fighters[1].punch = 5;
        fighters[0].health = fighters[0].health - 5;
        if (fighters[0].health < 1) {
            return LOSE;
        }
    } 
    if (fighters[1].x + 26 > fighters[0].x && fighters[1].x < fighters[0].x && fighters[0].punch > 0 && fighters[0].punch < 5 && !fighters[1].squat) {
        unDrawFighter(fighters[0].oldx, fighters[0].oldy,fighters[0].oldsquat, fighters[0].oldrotation, fighters[0].oldpunch);
        drawFighter(fighters[0].x, fighters[0].y, fighters[0].squat, fighters[0].rotation, 0, fighters[0].punch);
        
        waitForVblank();
        
        delay(10);//Time stops for a bit when a hit is landed

        fighters[0].oldpunch = 1; //Makes sure it will undraw a punch figure
        fighters[0].punch = 5; //End punch active frames (ends at frame five)
        fighters[1].health = fighters[1].health - 5;
        if (fighters[1].health < 1) {
            return WIN;
        }
    }
    if (fighters[1].x + 26 > fighters[0].x && fighters[1].x < fighters[0].x && fighters[1].punch > 0 && fighters[1].punch < 5 && !fighters[0].squat) {
        unDrawFighter(fighters[1].oldx, fighters[1].oldy,fighters[1].oldsquat, fighters[1].oldrotation, fighters[1].oldpunch);
        drawFighter(fighters[1].x, fighters[1].y, fighters[1].squat, fighters[1].rotation, 1, 1);
        
        waitForVblank();
        
        delay(10);

        fighters[1].oldpunch = 1;
        fighters[1].punch = 5;
        fighters[0].health = fighters[0].health - 5;
        if (fighters[0].health < 1) {
            return LOSE;
        }
    }

    //HEYYY punch method does fireballs now too
    if (fighters[1].firex + 10 > fighters[0].x && fighters[1].firex < fighters[0].x && fighters[0].y - 10 > fighters[1].firey && !fighters[0].squat && fighters[1].active) {
        delay(4);
        fighters[1].active = 0;

        fighters[0].health = fighters[0].health - 2;
        if (fighters[0].health < 1) {
            return LOSE;
        }
    }
    if (fighters[1].firex < fighters[0].x + 20 && fighters[1].firex > fighters[0].x && fighters[0].y - 10 > fighters[1].firey && !fighters[0].squat && fighters[1].active) {
        delay(4);
        fighters[1].active = 0;

        fighters[0].health = fighters[0].health - 2;
        if (fighters[0].health < 1) {
            return LOSE;
        }
    }
    if (fighters[0].firex + 10 > fighters[1].x && fighters[0].firex < fighters[1].x && fighters[1].y - 10 > fighters[0].firey && !fighters[1].squat && fighters[0].active) {
        delay(4);
        fighters[0].active = 0;

        fighters[1].health = fighters[1].health - 2;
        if (fighters[1].health < 1) {
            return WIN;
        }
    }
    if (fighters[0].firex < fighters[1].x + 20 && fighters[0].firex > fighters[1].x && fighters[1].y - 10 > fighters[0].firey && !fighters[1].squat && fighters[0].active) {
        delay(4);
        fighters[0].active = 0;

        fighters[1].health = fighters[1].health - 2;
        if (fighters[1].health < 1) {
            return WIN;
        }
    }

    return GAME;
}

int main() {

    REG_DISPCNT = MODE_3 | BG2_EN;

    enum GBAState state = START;

    while (1) {
        //waitForVblank();
        switch(state) {
            case START:
                //In start we just draw the start screen
                drawStart();
                state = START_NODRAW;
                break;
            case START_NODRAW:
                //wait for A
                if(KEY_DOWN_NOW(BUTTON_A)) {
                    state = YELLOW_STORY;
                }
                break;
            case YELLOW_STORY:
                //draw yellow story
                drawYellowStory();
                //Account for holding A
                while(KEY_DOWN_NOW(BUTTON_A));
                state = YELLOW_NO_DRAW;
                break;
            case YELLOW_NO_DRAW:
                //wait for a or select
                if(KEY_DOWN_NOW(BUTTON_A)) {
                    state = RED_STORY;
                }
                if (KEY_DOWN_NOW(BUTTON_SELECT)) {
                    state = START;
                }
                break;
            case RED_STORY:
                //similar to yellow story
                drawRedStory();
                while(KEY_DOWN_NOW(BUTTON_A));
                state = RED_NO_DRAW;
                break;
            case RED_NO_DRAW:
                if(KEY_DOWN_NOW(BUTTON_A)) {
                    state = GAME_DRAW;
                }
                if (KEY_DOWN_NOW(BUTTON_SELECT)) {
                    state = START;
                }
                break;
            case GAME_DRAW:
                //Initialize everything that is needed for the game
                drawGameBackground(yellowWins, redWins);
                FIGHTER fighters[NUMFIGHTERS];

                //starting x is different between two fighters
                fighters[0].x = 20;
                fighters[0].oldx = 20;

                fighters[1].x = GBA_WIDTH - 40;
                fighters[1].oldx = GBA_WIDTH - 40;

                fighters[0].health = 10;
                fighters[1].health = 100; //Randy gets more health than you do

                //Starting values for everything else are same
                for (int i = 0; i < NUMFIGHTERS; i++) {
                    //I initiially wanted to record most coordinates in terms of the size of 
                    //the container but it turned out to be less convienent. there's still 
                    //some code like this that uses it
                    fighters[i].y = 144;
                    fighters[i].oldy = 144;
                    fighters[i].xvel = 0;
                    fighters[i].yvel = 0;
                    fighters[i].canPunch = 1;
                    fighters[i].canJump = 1;
                    fighters[i].squat = 0;
                    fighters[i].oldsquat = 0;
                    fighters[i].rotation = 0;
                    fighters[i].oldrotation = 0;
                    fighters[i].energy = 100;
                    fighters[i].punch = 0;
                    fighters[i].oldpunch = 0;

                    fighters[i].firex = 0;
                    fighters[i].firey = 0;
                    fighters[i].oldfirex = 0;
                    fighters[i].oldfirey = 0;
                    fighters[i].active = 0;
                    fighters[i].oldactive = 0;
                    fighters[i].direction = 0;
                    fighters[i].olddirection = 0;
                }
                //hold back the a pressers
                while(KEY_DOWN_NOW(BUTTON_A));
                state = GAME;
                break;
            case GAME:

                for (int i = 0; i < NUMFIGHTERS; i++) {
                    unDrawFighter(fighters[i].oldx, fighters[i].oldy,fighters[i].oldsquat, fighters[i].oldrotation, fighters[i].oldpunch);
                    drawFighter(fighters[i].x, fighters[i].y, fighters[i].squat, fighters[i].rotation, i, fighters[i].punch);
                    drawHealth(fighters[i].health, i);
                    drawEnergy(fighters[i].energy, i);
                    unDrawFireball(fighters[i].oldfirex, fighters[i].oldfirey, fighters[i].oldactive, fighters[i].olddirection);
                    drawFireball(fighters[i].firex, fighters[i].firey, fighters[i].active, fighters[i].direction);
                    fighters[i].oldx = fighters[i].x;
                    fighters[i].oldy = fighters[i].y;
                    fighters[i].oldsquat = fighters[i].squat;
                    fighters[i].oldrotation = fighters[i].rotation;
                    fighters[i].oldpunch = fighters[i].punch;
                    fighters[i].oldfirex = fighters[i].firex;
                    fighters[i].oldfirey = fighters[i].firey;
                    fighters[i].oldactive = fighters[i].active;
                    fighters[i].olddirection = fighters[i].direction;
                    waitForVblank();
                }

                //player's turn
                playersTurn(fighters);
                if (KEY_DOWN_NOW(BUTTON_START) && canPause) {
                    drawPause();
                    while (KEY_DOWN_NOW(BUTTON_START));
                    while (!KEY_DOWN_NOW(BUTTON_B) && !KEY_DOWN_NOW(BUTTON_START)){
                        if (KEY_DOWN_NOW(BUTTON_SELECT)) {
                            state = START;
                            break;
                        }
                    }
                    canPause = 0;
                    drawGameBackground(yellowWins, redWins);
                } else if (!KEY_DOWN_NOW(BUTTON_START) && !canPause) {
                    canPause = 1;
                }
                //end player's turn

                //randy's turn
                randysTurn(fighters);

                //canSquat gets set in there so its convenient to return it
                updateStuff(fighters);

                //collision between fighters. It doesn't work quite correctly if 
                //neither fighter is making an attempt to move but randy usually takes 
                //care of that for us
                fighterCollisions(fighters);

                state = punch(fighters);

                if (KEY_DOWN_NOW(BUTTON_SELECT)) {
                    state = START;
                }

                //delay(1);
                break;
            case WIN:
                yellowWins++;
                drawSmile();
                //Just giving them more chances to reset. They can't do it during a loop of the gif
                if (KEY_DOWN_NOW(BUTTON_SELECT)) {
                    state = START;
                    break;
                }
                drawSmile();
                drawVictory();
                state = WIN_NODRAW;
                break;
            case WIN_NODRAW:
                if (KEY_DOWN_NOW(BUTTON_SELECT)) {
                    state = START;
                }
                break;
            case LOSE:
                redWins++;
                drawSad();
                //Just giving them more chances to reset. They can't do it during a loop of the gif
                if (KEY_DOWN_NOW(BUTTON_SELECT)) {
                    state = START;
                    break;
                }
                drawSad();
                drawDefeat();
                state = LOSE_NODRAW;
                break;
            case LOSE_NODRAW:
                if (KEY_DOWN_NOW(BUTTON_SELECT)) {
                    state = START;
                }
                break;
        }
    }
    return 0;
}

/*TODO
    *Implement rolling
    *implement attacking
    *implement health
    *implement ai
    *implement victory
    *draw arms/fists/boxing gloves
    spruce up background 
    *implement beginning story? yellow king is resourceful but weak Randy the red king has something going on in the brain, but is beyond superhuman
    *change splash screen?
    make stick figures not transparent?
    implement platforms?
    *perhaps down triggers squat, which removes all but head
    *maybe heads can move past anything and avoid most things, put there's an energy limit
    *NAME IS HENCE FORTH KNOWN AD ROLLER KINGS FEATURING RANDY THE RED KING
    maybe add little ground hazards for the gameplay image?
*/