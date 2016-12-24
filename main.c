
#include <stdlib.h>
#include "myLib.h"
#include "stdio.h"
#include "start.h"
#include "ball.h"
#include "text.h"
#include "winner.h"
#include "loser.h"
//Kalya Subramanian


enum GBAState {
	START, //draw start screen
	START_ND, //wait for input
	RESET, //resets ball/paddle after life is lost
	SETUP, //draws bricks
	SETUP_P, //position paddle
	LAUNCH, //ball is launched
	WINNER, //player won game
	LOSER //player lost game
};

int main()
{
	//mode 3 set up
	REG_DISPCTL = MODE3 | BG2_ENABLE;
	enum GBAState state = START;

	//intialize paddle;
	PADDLE paddle;
	paddle.width = 50;
	paddle.height = 5;
	paddle.col = 240/2 - paddle.width/2;
	paddle.row = 145;
	paddle.cd = 0;
	paddle.color = BROWN;
	u16 bgcolor = GREY;

	//buffer for drawString
	char buffer[41];

	//initialize oldPaddle, will hold old position of paddle
	PADDLE oldPaddle;

	oldPaddle.width = 50;
	oldPaddle.height = 5;
	oldPaddle.col = 240/2 - oldPaddle.width/2;
	oldPaddle.row = 145;
	oldPaddle.cd = 0;
	oldPaddle.color = BROWN;
	


	//declare array of bricks
	BRICK bricks[NUMBRICKS];
	
	//initialize ball
	BALL b;
	b.row =  paddle.row - BALL_HEIGHT - 1;
	b.col =  240/2 - BALL_WIDTH/2;
	b.width = BALL_WIDTH;
	b.height = BALL_HEIGHT;
	b.rd = 0;
	b.cd = 0;

	//initialize old ball, will hold old position of ball
	BALL oldB;
	oldB.row =  paddle.row - BALL_HEIGHT - 1;
	oldB.col =  240/2 - BALL_WIDTH/2;
	oldB.width = BALL_WIDTH;
	oldB.height = BALL_HEIGHT;
	oldB.rd = 0;
	oldB.cd = 0;


	//initial number of lives
	int numLives = 3;

	//initial number of bricks hit
	int bricksHit = 0;

	//colors for bricks
	//red = paddle size decreases
	//green = paddle size increases
	//blue = gain a life
	u16 colors[] = {RED, GREEN, BLUE};
	int numColors = sizeof(colors) / sizeof(colors[0]);

	
	
	while(1)
	{
		
		
		switch(state) {
			//draw the start screen image
			case START:
				
				drawImage3(0, 0, START_WIDTH, START_HEIGHT, start);
				state = START_ND;
				break;

			//wait for input to continue from start screen
			case START_ND:
				if(KEY_DOWN_NOW(BUTTON_A))
				{
					state = SETUP;
				}
				break;

			//initial set up, bricks are drawn in original form
			case SETUP:
				//draw gray background
				REG_DMA3SAD = (u32)&bgcolor;
				REG_DMA3DAD = (u32)videoBuffer;
				REG_DMA3CNT = 38400 | DMA_ON | DMA_SOURCE_FIXED;
				
				//make sure all bricks have the proper color
				for(int i = 0; i < NUMBRICKS; i++) {
					bricks[i].row = 0;
					bricks[i].col = i * 20;
					bricks[i].width = 20;
					bricks[i].height = 7;
					bricks[i].color = colors[i % numColors];

				}
				
				//draw the bricks
				for(int i = 0; i < NUMBRICKS; i++) {
					drawRect(bricks[i].row, bricks[i].col, bricks[i].width, bricks[i].height, bricks[i].color);
				}

				//display life count
				sprintf(buffer, "Lives: %d", numLives);	
				drawString(paddle.row + paddle.height + 2, 2, buffer, BLACK);
				
				
				state = SETUP_P;


				
				break;


			//resets ball/paddle after a life is lost
			case RESET:

				//if select button is pressed, go back to title screen
				//reset game properties
				
				if(KEY_DOWN_NOW(BUTTON_SELECT))
				{
					numLives = 3;
					bricksHit = 0;
					paddle.width = 50;
					paddle.height = 5;
					paddle.col = 240/2 - paddle.width/2;
					paddle.row = 145;
					paddle.cd = 0;
					paddle.color = BROWN;
					
					b.row =  paddle.row - BALL_HEIGHT - 1;
					b.col =  240/2 - BALL_WIDTH/2;
					b.width = BALL_WIDTH;
					b.height = BALL_HEIGHT;
					b.rd = 0;
					b.cd = 0;
					state = START;
				}

				//draw paddle in original position
				drawRect(145, 240/2 - paddle.width/2, paddle.width, paddle.height, paddle.color);
				
				//draw ball in original position
				drawImage3(paddle.row - b.height - 1, 240/2 - b.width/2, b.width, b.height, ball);
				//draw life count
				sprintf(buffer, "Lives: %d", numLives);	
				drawString(paddle.row + paddle.height + 2, 2, buffer, BLACK);
				state = SETUP_P;
				break;


			//state during which player positions paddle to begin game
			case SETUP_P:
				
				//if out of lives, game over
				if(numLives <= 0)
				{
					state = LOSER;
				}
				
				//if select pressed, go back to title screen
				if(KEY_DOWN_NOW(BUTTON_SELECT))
				{
					numLives = 3;
					bricksHit = 0;
					paddle.width = 50;
					paddle.height = 5;
					paddle.col = 240/2 - paddle.width/2;
					paddle.row = 145;
					paddle.cd = 0;
					paddle.color = BROWN;
					
					b.row =  paddle.row - BALL_HEIGHT - 1;
					b.col =  240/2 - BALL_WIDTH/2;
					b.width = BALL_WIDTH;
					b.height = BALL_HEIGHT;
					b.rd = 0;
					b.cd = 0;
					state = START;
				}
				b.cd = 0;
				paddle.cd = 0;

				//prevent paddle from going too far left
				if(paddle.col < 0)
				{
					paddle.col = 0;
					paddle.cd = 0;
					
				}

				//prevent paddle from going too far right
				if(paddle.col > (239 - paddle.width))
				{
					paddle.col = (239 - paddle.width);
					paddle.cd = 0;
					
				}

				//if left pressed, move paddle and ball to the left
				if(KEY_DOWN_NOW(BUTTON_LEFT)  && paddle.col > 2)
				{
					b.cd = -2;
					paddle.cd = -2;

					
				}
				//if right pressed, move paddle and ball to the right
				if(KEY_DOWN_NOW(BUTTON_RIGHT) && paddle.col < 238 - paddle.width)
				{
					b.cd = 2;
					paddle.cd = 2;
					
				}

				//erase old ball position
				drawRect(oldB.row, oldB.col, oldB.width, oldB.height, GREY);
				b.col = b.col + b.cd;
				//redraw ball
				drawImage3(b.row, b.col, b.width, b.height, ball);

				//erase old paddle position
				drawRect(oldPaddle.row, oldPaddle.col, oldPaddle.width, oldPaddle.height, GREY);
				paddle.col = paddle.col + paddle.cd;
				//redraw paddle
				drawRect(paddle.row, paddle.col, paddle.width, paddle.height, BROWN);

				//if start pressed, launch ball
				if(KEY_DOWN_NOW(BUTTON_START))
				{
					state = LAUNCH;
					b.rd = -1;
					b.cd = paddle.cd/2;
					
				}

				//update old ball/paddle
				oldB = b;
				oldPaddle = paddle;
				break;


			//ball is moving
			case LAUNCH:
				//if all bricks hit, WINNER
				if(bricksHit == NUMBRICKS)
				{
					state = WINNER;
				}

				//if all lives lost, LOSER
				if(numLives <= 0)
				{
					state = LOSER;
				}

				//don't let ball go too far up
				if(b.row <0)
				{
					b.row = 0;
					b.rd = -b.rd;
				}
				//don't let ball go too far down
				if(b.row > 159 - b.height)
				{
					b.row = 159 - b.height;
					b.rd = -b.rd;
				} 
				//don't let ball go too far left
				if(b.col < 0)
				{
					b.col = 0;
					b.cd = -b.cd;
				}
				//don't let ball go too far right
				if(b.col > (239 - b.width))
				{
					b.col = (239 - b.width);
					b.cd = -b.cd;
				}
				//don't let paddle go too far left
				if(paddle.col < 0)
				{
					paddle.col = 0;
					paddle.cd = 0;
					
				}
				//don't let paddle go too far right
				if(paddle.col > (239 - paddle.width))
				{
					paddle.col = (239 - paddle.width);
					paddle.cd = 0;
					
				}

				paddle.cd = 0;
				//move paddle left
				if(KEY_DOWN_NOW(BUTTON_LEFT) && paddle.col > 2)
				{

					
					paddle.cd = -2;
				}
				//move paddle right
				if(KEY_DOWN_NOW(BUTTON_RIGHT) && paddle.col < 238 - paddle.width)
				{
					
					//drawRect(paddle.row, paddle.col, paddle.width, paddle.height, GREY);
					paddle.cd = 2;
					//drawRect(paddle.row, paddle.col, paddle.width, paddle.height, paddle.color);
				}
				//if ball hits paddle, make ball bounce up
				//ball gets horizontal velocity of the paddle at time of collision
				if((b.row + b.height == paddle.row) && (b.col >= paddle.col && b.col <= paddle.col + paddle.width)) {
					b.rd = -b.rd;
					b.cd = paddle.cd /2;
					
				}

				//if ball hits bottom of a brick
				if(b.row == (bricks[0].row + bricks[0].height)) {
					int first = b.col/20;
					int second = (b.col+b.width)/20;
					
					//if brick is blue, gain life
					if(bricks[first].color == BLUE || bricks[second].color == BLUE)
					{
						numLives++;
						drawString(paddle.row + paddle.height + 2, 2, buffer, GREY);
						sprintf(buffer, "Lives: %d", numLives);	
						drawString(paddle.row + paddle.height + 2, 2, buffer, BLACK);
					


					}
					//if brick wasn't hit before, bounce ball back
					if(bricks[second].color != GREY || bricks[first].color != GREY)
					{
						b.rd = -b.rd;
					}
					//if brick is red, paddle size decreases
					if(bricks[first].color == RED|| bricks[second].color == RED)
					{
						paddle.width= paddle.width - 5;
					}
					//if brick is green, paddle size increases
					if(bricks[first].color == GREEN|| bricks[second].color == GREEN)
					{
						paddle.width = paddle.width + 5;
					}

					//if left side of ball hits brick, turn the brick gray
					//increase bricksHit
					if(bricks[first].color != GREY)
					{
						bricksHit++;
						bricks[first].color = GREY;
						
					}
					//if right side of ball hits brick, turn the brick gray
					//increase bricksHit
					if(bricks[second].color != GREY)
					{
						bricks[second].color = GREY;
						bricksHit++;
						
					}
					
					//draw the hit bricks gray
					drawRect(bricks[0].row, (b.col/20) * 20, bricks[0].width, bricks[0].height, GREY);
					drawRect(bricks[0].row, 20*((b.col + b.width)/20), bricks[0].width, bricks[0].height, GREY);
				}
				
				//if ball hits side of brick
				if(b.row  < (bricks[0].row + bricks[0].height)) {
					int first = b.col/20;
					int second = (b.col+b.width)/20;
					//blue brick, plus one life
					if(bricks[first].color == BLUE || bricks[second].color == BLUE)
					{
						numLives++;
						drawString(paddle.row + paddle.height + 2, 2, buffer, GREY);
						sprintf(buffer, "Lives: %d", numLives);	
						drawString(paddle.row + paddle.height + 2, 2, buffer, BLACK);
					


					}
					//bounce ball back
					if(bricks[second].color != GREY || bricks[first].color != GREY)
					{
						b.cd = -b.cd;
					}
					//red brick, paddle small
					if(bricks[first].color == RED|| bricks[second].color == RED)
					{
						paddle.width= paddle.width + 5;
					}
					//green brick, paddle big
					if(bricks[first].color == GREEN|| bricks[second].color == GREEN)
					{
						paddle.width = paddle.width - 5;
					}
					//if left side of ball hits, make brick gray
					if(bricks[first].color != GREY)
					{
						bricksHit++;
						bricks[first].color = GREY;
						
					}
					//if right side of ball hits, make brick gray
					if(bricks[second].color != GREY)
					{
						bricks[second].color = GREY;
						bricksHit++;
						
					}
					
					
					//draw gray bricks
					drawRect(bricks[0].row, (b.col/20) * 20, bricks[0].width, bricks[0].height, GREY);
					drawRect(bricks[0].row, 20*((b.col + b.width)/20), bricks[0].width, bricks[0].height, GREY);
				}
				//if ball is under paddle level (didn't catch the ball)
				if(b.row + b.height > paddle.row) {
					//lose a life
					numLives--;
					//if no lives left, loser
					if(numLives <= 0) 
					{
						state = LOSER;
					}
					//otherwise, reset level to play again
					else 
					{
						state = RESET;
					}
					
					//reset paddle and ball properties
					paddle.width = 50;
					paddle.height = 5;
					paddle.col = 240/2 - paddle.width/2;
					paddle.row = 145;
					paddle.cd = 0;
					paddle.color = BROWN;
					
					b.row =  paddle.row - BALL_HEIGHT - 1;
					b.col =  240/2 - BALL_WIDTH/2;
					b.width = BALL_WIDTH;
					b.height = BALL_HEIGHT;
					b.rd = 0;
					b.cd = 0;
					drawString(paddle.row + paddle.height + 2, 2, buffer, GREY);



				}
				//erase old paddle, draw new one
				drawRect(oldPaddle.row, oldPaddle.col, oldPaddle.width, oldPaddle.height, GREY);
				paddle.col = paddle.col + paddle.cd;
				drawRect(paddle.row, paddle.col, paddle.width, paddle.height, paddle.color);

				//erase old ball, draw new one
				drawRect(oldB.row, oldB.col, oldB.width, oldB.height, GREY);
				b.col = b.col + b.cd;
				b.row = b.row + b.rd;

				drawImage3(b.row, b.col, b.width, b.height, ball);

				//if select pressed, go to title screen
				if(KEY_DOWN_NOW(BUTTON_SELECT))
				{
					numLives = 3;
					bricksHit = 0;
					paddle.width = 50;
					paddle.height = 5;
					paddle.col = 240/2 - paddle.width/2;
					paddle.row = 145;
					paddle.cd = 0;
					paddle.color = BROWN;
					
					b.row =  paddle.row - BALL_HEIGHT - 1;
					b.col =  240/2 - BALL_WIDTH/2;
					b.width = BALL_WIDTH;
					b.height = BALL_HEIGHT;
					b.rd = 0;
					b.cd = 0;
					state = START;
				}
				
				oldB = b;
				oldPaddle = paddle;



				break;

			//winner screen!
			case WINNER:
				//draw winner image
				drawImage3(0, 0, WINNER_WIDTH, WINNER_HEIGHT, winner);
				//if select pressed, go to title screen
				if(KEY_DOWN_NOW(BUTTON_SELECT))
				{
					numLives = 3;
					bricksHit = 0;
					paddle.width = 50;
					paddle.height = 5;
					paddle.col = 240/2 - paddle.width/2;
					paddle.row = 145;
					paddle.cd = 0;
					paddle.color = BROWN;
					
					b.row =  paddle.row - BALL_HEIGHT - 1;
					b.col =  240/2 - BALL_WIDTH/2;
					b.width = BALL_WIDTH;
					b.height = BALL_HEIGHT;
					b.rd = 0;
					b.cd = 0;
					state = START;
				}
				break;
			//loser screen!
			case LOSER:
				//draw loser screen
				drawImage3(0, 0, LOSER_WIDTH, LOSER_HEIGHT, loser);
				//if select pressed, go to title screen
				if(KEY_DOWN_NOW(BUTTON_SELECT))
				{
					numLives = 3;
					paddle.width = 50;
					paddle.height = 5;
					paddle.col = 240/2 - paddle.width/2;
					paddle.row = 145;
					paddle.cd = 0;
					paddle.color = BROWN;
					
					b.row =  paddle.row - BALL_HEIGHT - 1;
					b.col =  240/2 - BALL_WIDTH/2;
					b.width = BALL_WIDTH;
					b.height = BALL_HEIGHT;
					b.rd = 0;
					b.cd = 0;
					state = START;
				}
				break;


		}

		waitForVblank();
	}
	return 0;
	
}

