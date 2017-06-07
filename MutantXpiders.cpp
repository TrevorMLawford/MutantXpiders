//============================================================================
// Name        : MutantXpiders.cpp
// Author      : Trevor Lawford
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================
// MUTANT XPIDERS
// Version 1.4
//
//	History
//	Date			Comments
//	01/06/1999	Built skeleton. Got keys to work (zx/)
// 02/06/99  Designed ship and implemented BOB for it
// 03/06/99  Got firing working, just right
// 04/06/99  Refined firing code further
// 05/06/99  Ship destroys. Started on spider-web code + finished.
// 06/06/99  Procedurised code and finished game logic (needs tweaking)
// 07/06/99  Added firing/launching sounds. Added backdrop.
// 08/06/99  Added 'wave' logic.
// 09/06/99  Re-coded between-screen logic
// 10/06/99  Reduced number of objects to speed up cycles. Created variables-per-wave.
// 12/06/99	 Found bug in Spider-moving/firing routine. Pesky joystick bug also produces random up/left moves!!
// 13/06/99  Finished off level-logic and sounds. Title screen and re-start.
//	15/06/99	 Corrected minor sound bug and added version number to front screen.
// 16/06/99  Made 'start' level easier.
// 19/06/99  Added 'Continue' option. Added 'wait until all fire out' code.
// 20/06/99  Tidied up end wave/game logic. Added end-of-wave message. Various screen dissolves implemented. (V1.3)
// 11/07/99  Started making cSpider class (v1.4)
// 12/07/99	 Continued "". Created generic cBOB class.
// 13/07/99	 Finished making cSpider class. Started on cSpiderFire class
// 14/07/99	 Created classes for all multiple sounds
// 21/07/99	 Created simple base class for Sounds
// 22/07/99	 Did cBaseFire class
// 23/07/99  Changing globals into class statics for neatness
// Released as verion 1.4
// 16/07/00  Version 1.5 - Added sounds/logic for extra life feature. Added PAUSE facility.
// 17/07/00  Started adding DEMO mode.
// 18/07/00  Ship movement for demo mode
// 21/07/00  Implement realtime before demo starts
// 22/07/00  Timer countdown display

// INCLUDES ///////////////////////////////////////////////

//#define ship_nohit	//When defined, this stop our Base Ship from dying - for debugging purposes

#define WIN32_LEAN_AND_MEAN

#include <windows.h>   // include important windows stuff
#include <windowsx.h>
#include <mmsystem.h>
#include <iostream> // include important C/C++ stuff
#include <conio.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <math.h>
#include <io.h>
#include <fcntl.h>
#include <time.h>

#include <ddraw.h>  // directX includes
#include <dsound.h>
#include <dinput.h>
#include "gpdumb1.h" // game library includes
#include "gpdumb2.h"

int Game_Main(void *parms);

// DEFINES ////////////////////////////////////////////////

// defines for windows
#define WINDOW_CLASS_NAME "WINXCLASS"  // class name

#define WINDOW_WIDTH    640   // size of window
#define WINDOW_HEIGHT   480

#define BASE_FIRE_COUNT           26    // Number of individual ship firing at any one time.
#define BASE_FIRE_SOUND_COUNT     10    // Number of simultaneous base fire sounds

#define SPIDER_COUNT              20    // Number of Mutant Spiders catered for

#define SPIDER_FIRE_COUNT         10	 // Count of Spider Fire on the go at any one time
#define SPIDER_FIRE_SOUND_COUNT   10    // Number of simultaneous spider fire sounds

#define SPIDER_START_SOUND_COUNT   5    // Number of simultaneous spider start sounds
#define SPIDER_LAUNCH_SOUND_COUNT  5    // Number of simultaneous spider move sounds
#define SPIDER_HIT_SOUND_COUNT     5    // Number of simultaneous spider hits


// CLASSES   //////////////////////////////////////////////

// BOB classes

class cBOB
{
	public:
		cBOB()
		{}

		~cBOB()
		{
			Destroy_BOB2(&the_BOB);
		}

		BOB* get_pBOB()
		{
			return &the_BOB;
		}

		int isAlive() const
		{
			if (the_BOB.state == BOB_STATE_ALIVE)
				return TRUE;
			else
				return FALSE;
		}

		void setAlive()
		{
			the_BOB.state = BOB_STATE_ALIVE;
		}

		int isDying() const
		{
			if (the_BOB.state == BOB_STATE_DYING)
				return TRUE;
			else
				return FALSE;
		}

		void setDying()
		{
			the_BOB.state = BOB_STATE_DYING;
		}

		int isDead() const
		{
			if (the_BOB.state == BOB_STATE_DEAD)
				return TRUE;
			else
				return FALSE;
		}

		void setDead()
		{
			the_BOB.state = BOB_STATE_DEAD;
		}

		int getX() const
		{
				return the_BOB.x;
		}

		void setX(int x_coord)
		{
				the_BOB.x = x_coord;
		}

		int getY() const
		{
				return the_BOB.y;
		}

		void setY(int y_coord)
		{
				the_BOB.y = y_coord;
		}

		int getXV() const
		{
				return the_BOB.xv;
		}

		int getYV() const
		{
				return the_BOB.yv;
		}

	protected:
		BOB the_BOB;		// Multi-purpose BOB with generic methods - basis for specific BOBs
};


class cBase : public cBOB
{
	public:

		cBase() : demo_vel_x(0), demo_vel_y(0)
		{}	// Does nothing - all initialisation are done in loadBOB as they cannot be performed at object instantiation time

		~cBase()
		{}	// Does nothing - all clearing up are done in cBOB

		void loadBOB()
		{
			Load_Bitmap_File(&bitmap8bit, "base_ship.bmp");
			// now create the Base FIRE
			Create_BOB(&the_BOB,0,0,
				32,32, 						// Size of each frame
				12,							// Number of frames
				BOB_ATTR_VISIBLE | BOB_ATTR_MULTI_ANIM,
				DDSCAPS_SYSTEMMEMORY);
			setAlive();
			Set_Vel_BOB(&the_BOB,6,6); 	// Set vertical/horizontal speed - doesn't change;
			// load base_ship frames
			for (int index=0; index<6; index++)
			{
				Load_Frame_BOB(&the_BOB,&bitmap8bit,index,index,0,BITMAP_EXTRACT_MODE_CELL);
				Load_Frame_BOB(&the_BOB,&bitmap8bit,index+6,index,1,BITMAP_EXTRACT_MODE_CELL);
			}
			Set_Anim_Speed_BOB(&the_BOB,1);	// set animation speed
			int base_ship_OK[6] = {0,1,2,3,4,5};
			Load_Animation_BOB(&the_BOB,0,6,base_ship_OK);
			int base_ship_explode[6] = {6,7,8,9,10,11};
			Load_Animation_BOB(&the_BOB,1,6,base_ship_explode);
			Unload_Bitmap_File(&bitmap8bit);	// unload data infile
			Set_Animation_BOB(&the_BOB,0);	// Set Base Ship animation to normal
		}

		void reset()
		{
			setAlive();
			Set_Animation_BOB(&the_BOB,0); 										// reset Base Ship animation
			the_BOB.curr_frame = 0;													// ... and manually set first frame
			Set_Pos_BOB(&the_BOB,SCREEN_WIDTH/2, SCREEN_HEIGHT-31-20);	// set position (X_position of base at bottom)
			base_firing = 0;
			base_fire2  = 99;
			dead_cycle_count = 0;
			ship_fire_move = 0;
		}

		void move(enum {DEMO_OFF, DEMO_ON} demo)
		// Move ship according to input
		{
			if (demo == DEMO_OFF)
			{
				// Process MOUSE movements separately
				// LEFT/RIGHT
				the_BOB.x += mouse_state.lX;		// Left/Right
				the_BOB.y += mouse_state.lY;		// Up/Down

				if (mouse_state.lX < 0)
					ship_fire_move = -2;	 			// Momentum of fire if initiated whilst Base Ship is moving left
				if (mouse_state.lX > 0)
					ship_fire_move = 2;	 			// Momentum of fire if initiated whilst Base Ship is moving right

				// LEFT
				if ( (keyboard_state[DIK_Z])		// Z = move base_ship LEFT
					|| (joy_state.lX < -230) ) 	// Joystick left
				{
					the_BOB.x -= the_BOB.xv;
					ship_fire_move = -2;	 			// Momentum of fire if initiated whilst Base Ship is moving left
				}

				// RIGHT
				if ( (keyboard_state[DIK_X])		// X = move base_ship RIGHT
					|| (joy_state.lX > 230) )  	// Joystick right
				{
					the_BOB.x += the_BOB.xv;
					ship_fire_move = +2;				// Momentum of fire if initiated whilst Base Ship is moving right
					if (keyboard_state[DIK_Z])
						ship_fire_move = 0;			// If both left/right are pressed then nullify sideways movement.
				}

				// UP
				if (( keyboard_state[DIK_K])		// K = move base_ship UP
					|| (joy_state.lY < -230) ) 	// Joystick up
					the_BOB.y -= the_BOB.yv;

				// DOWN
				if ( (keyboard_state[DIK_M])		// M = move base_ship DOWN
					|| (joy_state.lY > 230) )  	// Joystick down
					the_BOB.y += the_BOB.yv;
			}
			else
			{
				// Demo ship movement
				the_BOB.x += demo_vel_x;
				the_BOB.y += demo_vel_y;
				if (rand()%20 == 0) demo_vel_x = (6 - rand()%13); // Random speed/dir -6 to 6
				if (rand()%20 == 0) demo_vel_y = (3 - rand()%7); // Random speed/dir -3 to 3
				// 'Bounce' if hugging side of screen
				if (the_BOB.x <= 0)							// Too near the left?
					demo_vel_x = (rand()%6)+1;
				if (the_BOB.x >= SCREEN_WIDTH - 31)		// Too near the right?
					 demo_vel_x = -((rand()%6)+1);
				if (the_BOB.y <= SCREEN_HEIGHT - 120)	// Too near the top?
					demo_vel_y = (rand()%3)+1;
				if (the_BOB.y >= SCREEN_HEIGHT - 51)	// Too near the bottom
					demo_vel_y = -((rand()%3)+1);
			}

			// Don't move Base Ship off screen
			if (the_BOB.x < 3)
				the_BOB.x = 3;
			if (the_BOB.x > SCREEN_WIDTH - 35)
				the_BOB.x = SCREEN_WIDTH - 35;
			if (the_BOB.y < SCREEN_HEIGHT - 120)
				the_BOB.y = SCREEN_HEIGHT - 120;
			if (the_BOB.y > SCREEN_HEIGHT - 51)
				the_BOB.y = SCREEN_HEIGHT - 51;

		}

		int isFiring() const
		// Is the base currently firing?
		{
			if (base_firing == 0)
				return FALSE;
			else
				return TRUE;
		}

		void startFiring()
		{
			base_firing = 1;
		}

		void setBaseFire2(int value)
		{
			base_fire2 = value;
		}

		int animate_and_check_second_fire()
		// Animate base_ship if currently in firing cycle - check whether to release 2nd base_ship_fire yet (on cycle 3)
		{
			int release_second_fire = 0;
			if (base_firing > 0)
			{
				if (base_firing < 7)
					Animate_BOB(&the_BOB);
				base_firing++;
				if (base_firing == 10)									// Have we finished Base Ship firing cycle?
					base_firing = 0;										// Yes - ready to start another one!
				if ( (base_firing == 4) && (base_fire2 != 99) )	// Ready to set 2nd fire loose
				{
					release_second_fire = base_fire2;				// Signal to caller to start 2nd fire
					base_fire2 = 99;
				}
			}
			return release_second_fire;
		}

void draw()
		// Draw base as normal, splat with random pixels if current dying.
		{
			if (!isDead())
			{
				Draw_BOB(&the_BOB,lpddsback);
				if (isDying())
				{
					DD_Lock_Back_Surface();		// For pixel-drawing
					for (int i=0; i < dead_cycle_count*10; i++)
					{
						Draw_Pixel(the_BOB.x+4+(rand()%24),the_BOB.y+4+(rand()%24),rand()%256,back_buffer, back_lpitch);
					}
					DD_Unlock_Back_Surface();
				}
			}
		}

	private:
		int	base_firing;			// Base Ship firing cycle?
		int  	base_fire2;				// Index of 2nd fire (99 = none)
		int   demo_vel_x;				// Demo speed/direction
		int   demo_vel_y;				// Demo speed/direction

	public:
		int 	ship_fire_move;		// Momentum of sideways movement of ship passed on to current ship fire
		int  	dead_cycle_count;		// Cycle size of ship when dying
};


class cBaseFire : public cBOB
{
	public:

		cBaseFire()
		{}	// Does nothing - all initialisation are done in loadBOB as they cannot be performed at object instantiation time

		~cBaseFire()
		{
			count--;
			if (count == 0)
				Destroy_BOB2(&master_base_fire);
		}

		void loadBOB()
		{
			count++;
			if (count == 0)
			{
				// First call - create master base fire, from which standard base fires are made
				Load_Bitmap_File(&bitmap8bit, "base_fire.bmp");
				// now create the Base FIRE
				Create_BOB(&master_base_fire,0,0,
					8,16, 						// Size of each frame
					16,							// Number of frames
					BOB_ATTR_VISIBLE | BOB_ATTR_MULTI_ANIM,
					DDSCAPS_SYSTEMMEMORY);
				master_base_fire.state = BOB_STATE_DEAD;
				master_base_fire.yv = 15;		// Vertical speed for intial momentum of Base Ship Fire(y=y-this)
				// load base_fire frames
				for (int index=0; index<8; index++)
				{
					Load_Frame_BOB(&master_base_fire,&bitmap8bit,index,index,0,BITMAP_EXTRACT_MODE_CELL);
					Load_Frame_BOB(&master_base_fire,&bitmap8bit,index+8,index,1,BITMAP_EXTRACT_MODE_CELL);
				}
				Set_Anim_Speed_BOB(&master_base_fire,1);	// set animation speed
				int base_fire_up[8] = {0,1,2,3,4,5,6,7};
				Load_Animation_BOB(&master_base_fire,0,8,base_fire_up);
				int base_fire_down[8] = {8,9,10,11,12,13,14,15};
				Load_Animation_BOB(&master_base_fire,1,8,base_fire_down);
				Unload_Bitmap_File(&bitmap8bit);	// unload data infile
			}

			Create_BOB(&the_BOB,0,0,
				8,16, 						// Size of each frame
				16,							// Number of frames
				BOB_ATTR_VISIBLE | BOB_ATTR_MULTI_FRAME,
				DDSCAPS_SYSTEMMEMORY);
			Clone_BOB(&master_base_fire, &the_BOB);
		}

		void reset()
		{
			Set_Animation_BOB(&the_BOB,0);
			the_BOB.curr_frame = 0;
			the_BOB.state = BOB_STATE_DEAD;						// De-activate all base ship Fires
		}

		void start_new_base_fire(const int new_x, const int base_ship_y, const int ship_fire_move)
		// Set a new base fire going on its merry way up
		{
			setAlive();
			Set_Pos_BOB(&the_BOB,new_x,			 				// Horiz position based on ship when fired
										base_ship_y - 4);				// Vert pos matched to base ship height
			Set_Vel_BOB(&the_BOB, ship_fire_move,				// Move sideways with the movement of the sideways ship at this time.
										 master_base_fire.yv);		// Vertical motion matched to master BOB
			Set_Animation_BOB(&the_BOB,0);						// Going UP animation-frames
		}

		void move_draw()
		{
			if (the_BOB.yv == -1)									// Hit the top!
				Set_Animation_BOB(&the_BOB,1);					// Going DOWN
			Animate_BOB(&the_BOB);
			Draw_BOB(&the_BOB,lpddsback);
			if (the_BOB.curr_frame%9 == 0)						//	Reduce/increase velocity once per cycle
			{
				if ((the_BOB.yv < 3) && (the_BOB.yv > -3))
					the_BOB.yv -= 1;									// Almost at top - slow down velocity changes
				else if ((the_BOB.yv < 5) && (the_BOB.yv > -5))
					the_BOB.yv -= 2;									// Approaching top - slow down velocity changes
				else
					the_BOB.yv -= 3;									// Normal velocity changes
			}
			the_BOB.x += the_BOB.xv;
			the_BOB.y -= the_BOB.yv;
			// If base fire is returning to, and reached, ground then mark it dead
			if (
				 (the_BOB.x < 2) ||
				 (the_BOB.x > (SCREEN_WIDTH-10) ) ||
				 (the_BOB.y > (SCREEN_HEIGHT - 32 - the_BOB.yv) )
				)
				setDead();
		}

	private:
		static int count;								// Count of cBaseFires in existence
		static BOB master_base_fire;				// Master BOB from which others are created
	public:
		static int base_fire_currently_alive;	// Can't end wave until all base fire out.

};
// Actual versions of cBaseFire class 'statics'
int cBaseFire::count = -1;
BOB cBaseFire::master_base_fire;
int cBaseFire::base_fire_currently_alive;

class cSpider : public cBOB
{
	public:

		cSpider()
		{}	// Does nothing - all initialisation are done in loadBOB as they cannot be performed at object instantiation time

		~cSpider()
		{
			count--;
			if (count == 0)
				Destroy_BOB2(&master_spider);
		}

		void loadBOB()
		{
			count++;
			if (count == 0)
			{
				// First call - create master spider, from which standard spiders are made
				Load_Bitmap_File(&bitmap8bit, "spider.bmp");
				// now create the SPIDER
				Create_BOB(&master_spider,0,0,
					32,32, 							// Size of each frame
					14,								// Number of frames
					BOB_ATTR_VISIBLE | BOB_ATTR_MULTI_ANIM,
					DDSCAPS_SYSTEMMEMORY);
				master_spider.state = SPIDER_INACTIVE;
				// load Spider frames
				int index;
				for (index=0; index<4; index++)	// Spider descending
				{
					Load_Frame_BOB(&master_spider,&bitmap8bit,index,index,0,BITMAP_EXTRACT_MODE_CELL);
				}
				for (index=0; index<3; index++)	// Spider flying
				{
					Load_Frame_BOB(&master_spider,&bitmap8bit,index+4,index,1,BITMAP_EXTRACT_MODE_CELL);
				}
				for (index=0; index<7; index++)	// Spider dying
				{
					Load_Frame_BOB(&master_spider,&bitmap8bit,index+7,index,2,BITMAP_EXTRACT_MODE_CELL);
				}

				int spider_descending_web[12] = {0,1,0,1,2,1,2,3,2,3,0,3};
				Load_Animation_BOB(&master_spider,0,12,spider_descending_web);
				int spider_flying[4] 			= {4,6,5,6};
				Load_Animation_BOB(&master_spider,1,4,spider_flying);
				int spider_dying[7] 				= {7,8,9,10,11,12,13};
				Load_Animation_BOB(&master_spider,2,7,spider_dying);
				Unload_Bitmap_File(&bitmap8bit);	// unload data infile
			}

			Create_BOB(&the_BOB,0,0,
				32,32, 							// Size of each frame
				14,								// Number of frames
				BOB_ATTR_VISIBLE | BOB_ATTR_MULTI_ANIM,
				DDSCAPS_SYSTEMMEMORY);
			Clone_BOB(&master_spider, &the_BOB);
		}

		void reset()
		{
			Set_Animation_BOB(&the_BOB,0);
			the_BOB.curr_frame = 0;
			the_BOB.state = SPIDER_INACTIVE;			// De-activate spiders
		}

		void draw()
		// Draw Spider if actually there!
		{
			if (isActive())
			{
				Animate_BOB(&the_BOB);
				Draw_BOB(&the_BOB,lpddsback);
				// If spider has gone through 1 dying graphic cycle then RIP
				if ( (the_BOB.state == SPIDER_DYING) && (the_BOB.curr_frame == 13) )
					the_BOB.state = SPIDER_INACTIVE;
			}
		}

		void growWeb()
		{
			the_BOB.y += descend_speed;
		}

		int isWebFinished() const
		{
			if (the_BOB.y < max_web_length)
				return FALSE;
			else
				return TRUE;
		}

		int isActive() const
		{
			if (the_BOB.state == SPIDER_INACTIVE)
				return FALSE;
			else
				return TRUE;
		}

		int isInactive() const
		{
			if (the_BOB.state == SPIDER_INACTIVE)
				return TRUE;
			else
				return FALSE;
		}

		void setInactive()
		{
			the_BOB.state = SPIDER_INACTIVE;
		}

		int isDescending() const
		{
			if (the_BOB.state == SPIDER_DESCENDING)
				return TRUE;
			else
				return FALSE;
		}
		void setDescending()
		{
			the_BOB.state = SPIDER_DESCENDING;
			Set_Animation_BOB(&the_BOB,0);
			Animate_BOB(&the_BOB);													// Get first frame right.
			Set_Anim_Speed_BOB(&the_BOB,5);										// set animation speed
			Set_Pos_BOB(&the_BOB, (rand()%(SCREEN_WIDTH-66))+33, 1);		// Web is 16 length at start
			max_web_length = (rand()%300+20);									// How long will the final web be?
		}

		int isFlying() const
		{
			if (the_BOB.state == SPIDER_FLYING)
				return TRUE;
			else
				return FALSE;
		}
		void setFlying()
		{
			the_BOB.state = SPIDER_FLYING;
			Set_Animation_BOB(&the_BOB,1);		 											// Spider 'flying' graphic
			Set_Anim_Speed_BOB(&the_BOB,2);	 												// set animation speed (wings flapping)
			setTimeBeforeChangeDir(); 															// Time before change of direction.
			if (rand()%5 > 2)		// 2/5ths of the time spider stays still
				Set_Vel_BOB(&the_BOB,0,0);
			else
				Set_Vel_BOB(&the_BOB,(rand()%(max_speed*2+1))-max_speed, 			// Set initial flying velocities
										 (rand()%(max_speed*2+1))-max_speed);
			setTimeBeforeFiring();																// Time to first fire after launch
		}

		int isDying() const
		{
			if (the_BOB.state == SPIDER_DYING)
				return TRUE;
			else
				return FALSE;
		}

		void setDying()
		{
			the_BOB.state = SPIDER_DYING;
			Set_Animation_BOB(&the_BOB,2);				// Reset Spider graphic to 'dying'
			Set_Anim_Speed_BOB(&the_BOB,2);				// set animation speed
		}

		void setTimeBeforeChangeDir()
		{
			 time_before_change_dir = (rand()%max_time_before_move)+1;
		}

		int checkChangeDir()
		{
			time_before_change_dir--;
			if (time_before_change_dir == 0)
			{
				setTimeBeforeChangeDir(); 													// Time before change of direction.
				if (rand()%5 > 2)																// 2/5ths of the time spider stays still
					Set_Vel_BOB(&the_BOB,0, 0);
				else
					Set_Vel_BOB(&the_BOB, (rand()%(max_speed*2+1))-max_speed, 	// Set initial flying velocities
												 (rand()%(max_speed*2+1))-max_speed );
			}
			// Move spider around screen
			the_BOB.x += the_BOB.xv;														// Horizontal movement
			if (the_BOB.x < 0)
			{
				the_BOB.x = 0;
				the_BOB.xv = -the_BOB.xv;
			}
			if (the_BOB.x > SCREEN_WIDTH-33)
			{
				the_BOB.x = SCREEN_WIDTH-33;
				the_BOB.xv = -the_BOB.xv;
			}

			the_BOB.y += the_BOB.yv;														// Vertical movement
			if (the_BOB.y < 0)
			{
				the_BOB.y = 0;
				the_BOB.yv = -the_BOB.yv;
			}
			if (the_BOB.y > SCREEN_HEIGHT-70)
			{
				the_BOB.y = SCREEN_HEIGHT-70;
				the_BOB.yv = -the_BOB.yv;
			}
		}

		void setTimeBeforeFiring()
		{
			 time_before_firing = (rand()%max_time_before_fire)+1;
		}

		int fireNow()
		{
			time_before_firing--;
			if (time_before_firing == 0)
				return TRUE;
			else
				return FALSE;
		}

		static void setWaveVariables(const int wave)
		{
			allowed_onscreen = wave;						// How many spiders at onec
			descend_speed = (wave+1)/3;					// Speed at which spiders descend web;
			if (descend_speed > 3)
				descend_speed = 3;
			this_wave = wave * 8;							// Spiders left to die until the end of this wave
			max_speed = wave/2;						   	// The maximum speed which the spiders can move
			if (max_speed > 8)
				max_speed = 8;
			max_time_before_move = 400-(wave<<4);		// The maximum amount of time before the spider will change direction
			if (max_time_before_move < 40)
				max_time_before_move = 40;
			max_time_before_fire = 400-(wave<<5);	 	// The maximum amount of time before the spider will fire
			if (max_time_before_fire < 40)
				max_time_before_fire = 40;
		}

	private:
		int	max_web_length;								// Max length of web line before spider detaches
		int	time_before_change_dir;						// Counter before we change the direction.
		int   time_before_firing;							// Counter before next firing.

		static const int SPIDER_INACTIVE;
		static const int SPIDER_DESCENDING;
		static const int SPIDER_FLYING;
		static const int SPIDER_DYING;
		static int count;							// Count of cSpiders in existence
		static BOB master_spider;				// Master BOB from which others are created

		static int max_time_before_move; 	// The maximum amount of time before the spider will change direction
		static int max_time_before_fire; 	// The maximum amount of time before the spider will fire
		static int max_speed;					// The maximum speed which the spiders can move
		static int descend_speed;				// Speed at which spiders descend web;


	public:
		static int currently_alive;			// Spiders currently alive
		// Wave variables
		static int allowed_onscreen;			// Max num of spiders allowed on-screen in this wave at any one time
		static int this_wave;					// Spiders left to die until the end of this wave


};
// Actual versions of cSpider class 'statics'
const int cSpider::SPIDER_INACTIVE   = 0;
const int cSpider::SPIDER_DESCENDING = 1;
const int cSpider::SPIDER_FLYING     = 2;
const int cSpider::SPIDER_DYING      = 3;
int cSpider::count = -1;
BOB cSpider::master_spider;
int cSpider::max_time_before_move;
int cSpider::max_time_before_fire;
int cSpider::max_speed;
int cSpider::descend_speed;
int cSpider::currently_alive;
int cSpider::allowed_onscreen;
int cSpider::this_wave;


class cSpiderFire : public cBOB
{
	public:

		cSpiderFire()
		{}	// Does nothing - all initialisation are done in loadBOB as they cannot be performed at object instantiation time

		~cSpiderFire()
		{
			count--;
			if (count == 0)
				Destroy_BOB2(&master_spider_fire);
		}

		void loadBOB()
		{
			count++;
			if (count == 0)
			{
				// First call - create master spider fire, from which standard spider fires are made
				Load_Bitmap_File(&bitmap8bit, "spider_fire.bmp");
				// now create the Spider FIRE
				Create_BOB(&master_spider_fire,0,0,
					16,16, 						// Size of each frame
					4,								// Number of frames
					BOB_ATTR_VISIBLE | BOB_ATTR_MULTI_FRAME,
					DDSCAPS_SYSTEMMEMORY);
				master_spider_fire.state = BOB_STATE_DEAD;
				// load spider_fire frames
				for (int index=0; index<4; index++)
				{
					Load_Frame_BOB(&master_spider_fire,&bitmap8bit,index,index,0,BITMAP_EXTRACT_MODE_CELL);
				}
				Set_Anim_Speed_BOB(&master_spider_fire,1);	// set animation speed
				int spider_fire_frames[4] = {0,1,2,3};
				Load_Animation_BOB(&master_spider_fire,0,4,spider_fire_frames);
				Unload_Bitmap_File(&bitmap8bit);	// unload data infile
			}

			Create_BOB(&the_BOB,0,0,
				16,16, 						// Size of each frame
				4,								// Number of frames
				BOB_ATTR_VISIBLE | BOB_ATTR_MULTI_FRAME,
				DDSCAPS_SYSTEMMEMORY);
			Clone_BOB(&master_spider_fire, &the_BOB);
		}

		void reset()
		{
			Set_Animation_BOB(&the_BOB,0);
			the_BOB.curr_frame = 0;
			setDead();	// Remove all spider Fires
		}

		void startFire(const int X_coord, const int Y_coord, const int XV, const int YV, const int spider_owner_sub)
		{
			setAlive();
			// Set fire to start at position of Spider, velocity half of sideways and faster downwards
			Set_Pos_BOB(&the_BOB, X_coord+8, Y_coord+16);
			Set_Vel_BOB(&the_BOB, XV/2, YV+additional_spider_fire_vertical_velocity);
			if (the_BOB.yv < 0)
				the_BOB.yv = additional_spider_fire_vertical_velocity;  // No fire upwards!
			spider_fire_owner = spider_owner_sub;  	// Assign 'owner' of this fire so it can't be detroyed by it.
		}

		int getOwner() const
		{
			return spider_fire_owner;
		}

		void move_draw()
		{
			if (the_BOB.curr_frame == 0)
				the_BOB.yv+=1;					// Speed up downward journey of spider fire once per cycle
			Move_BOB(&the_BOB);
			// Check whether spider fire reached bottom of screen or gone off sides
			if ( (the_BOB.x < 0) ||
				  (the_BOB.x > (SCREEN_WIDTH - 17 - abs(the_BOB.xv)) )  ||
				  (the_BOB.y > (SCREEN_HEIGHT - 9 - (the_BOB.yv))) )
				  setDead();
			else
			{
				Animate_BOB(&the_BOB);
				Draw_BOB(&the_BOB,lpddsback);
			}
		}

		static void setWaveVariables(const int wave)
		{
			additional_spider_fire_vertical_velocity = wave/4;	// Amount added to spider vertical velocity when firing

		}

	private:
		int	spider_fire_owner;						// Owner of this fire (so it can't be killed by it)
		static int count;									// Count of cSpiderFires in existence
		static BOB master_spider_fire;				// Master BOB from which others are created
		static int additional_spider_fire_vertical_velocity;
	public:
		static int spider_fire_currently_alive;	// Can't end wave until all spider fire out.

};
// Actual versions of cSpiderFire class 'statics'
int cSpiderFire::count = -1;
BOB cSpiderFire::master_spider_fire;
int cSpiderFire::additional_spider_fire_vertical_velocity;
int cSpiderFire::spider_fire_currently_alive;

// SOUND classes

class cSound
{
	public:
		int getSoundId() const
		{
			return sound;
		}
		void init_pan()
		{
			Set_Sound_Pan(sound,((rand()%201)-100)*10);	// Give random panning to sound interesting
		}
	protected:
		int sound;	// Sound id
};

class cBaseFireSound : public cSound
{
	public:
		void loadSound()
		{
			count++;
			if (count == 0)	// First call - create master spider, from which standard spiders are made
				master_sound = Load_VOC("base_fire.voc");
			sound = Replicate_Sound(master_sound);
		}
		int static next_free_sound(int max)
		{
			if (current_sound < max-1)			// Ready for next 'free' slot (which should have finished playing by now)
				current_sound++;
			else
				current_sound = 0;
			return current_sound;
		}
	private:
		static int master_sound;			  	// Used to create 'clone' sounds
		static int count;							// Count of number of cSpiderFireSounds in existence
		static int current_sound;				// Current sound to play - loop around
};
// Actual versions of cBaseFireSound class 'statics'
int cBaseFireSound::master_sound;
int cBaseFireSound::count = -1;
int cBaseFireSound::current_sound = 0;


class cSpiderStartSound : public cSound
{
	public:
		void loadSound()
		{
			count++;
			if (count == 0)						// First call - create master spider, from which standard spiders are made
				master_sound = Load_VOC("spider_start.voc");
			sound = Replicate_Sound(master_sound);
		}
		int static next_free_sound(int max)
		{
			if (current_sound < max-1)			// Ready for next 'free' slot (which should have finished playing by now)
				current_sound++;
			else
				current_sound = 0;
			return current_sound;
		}
	private:
		static int master_sound;			  	// Used to create 'clone' sounds
		static int count;							// Count of number of cSpiderFireSounds in existence
		static int current_sound;				// Current sound to play - loop around
};
// Actual versions of cSpiderFireSound class 'statics'
int cSpiderStartSound::master_sound;
int cSpiderStartSound::count = -1;
int cSpiderStartSound::current_sound = 0;


class cSpiderLaunchSound : public cSound
{
	public:
		void loadSound()
		{
			count++;
			if (count == 0)						// First call - create master spider, from which standard spiders are made
				master_sound = Load_VOC("spider_launch.voc");
			sound = Replicate_Sound(master_sound);
		}
		int static next_free_sound(int max)
		{
			if (current_sound < max-1)			// Ready for next 'free' slot (which should have finished playing by now)
				current_sound++;
			else
				current_sound = 0;
			return current_sound;
		}
	private:
		static int master_sound;			  	// Used to create 'clone' sounds
		static int count;							// Count of number of cSpiderFireSounds in existence
		static int current_sound;				// Current sound to play - loop around
};
// Actual versions of cSpiderFireSound class 'statics'
int cSpiderLaunchSound::master_sound;
int cSpiderLaunchSound::count = -1;
int cSpiderLaunchSound::current_sound = 0;


class cSpiderFireSound : public cSound
{
	public:
		void loadSound()
		{
			count++;
			if (count == 0)						// First call - create master spider, from which standard spiders are made
				master_sound = Load_VOC("spider_fire.voc");
			sound = Replicate_Sound(master_sound);
		}
		int static next_free_sound(int max)
		{
			if (current_sound < max-1)			// Ready for next 'free' slot (which should have finished playing by now)
				current_sound++;
			else
				current_sound = 0;
			return current_sound;
		}
	private:
		static int master_sound;			  	// Used to create 'clone' sounds
		static int count;							// Count of number of cSpiderFireSounds in existence
		static int current_sound;				// Current sound to play - loop around
};
// Actual versions of cSpiderFireSound class 'statics'
int cSpiderFireSound::master_sound;
int cSpiderFireSound::count = -1;
int cSpiderFireSound::current_sound = 0;


class cSpiderHitSound : public cSound
{
	public:
		void loadSound()
		{
			count++;
			if (count == 0)						// First call - create master spider, from which standard spiders are made
				master_sound = Load_VOC("spider_hit.voc");
			sound = Replicate_Sound(master_sound);
		}
		int static next_free_sound(int max)
		{
			if (current_sound < max-1)			// Ready for next 'free' slot (which should have finished playing by now)
				current_sound++;
			else
				current_sound = 0;
			return current_sound;
		}
	private:
		static int master_sound;			  	// Used to create 'clone' sounds
		static int count;							// Count of number of cSpiderFireSounds in existence
		static int current_sound;				// Current sound to play - loop around
};
// Actual versions of cSpiderFireSound class 'statics'
int cSpiderHitSound::master_sound;
int cSpiderHitSound::count = -1;
int cSpiderHitSound::current_sound = 0;


class cBaseHitSound : public cSound
{
	public:
		void loadSound()
		{
			sound = Load_VOC("base_hit.voc");
		}
		void playSound()
		{
			Play_Sound(sound,0);
		}
};

class cExtraLife : public cSound
{
	public:
		void loadSound()
		{
			sound = Load_VOC("extra_life.voc");
		}
		void playSound()
		{
			Play_Sound(sound,0);
		}
};


// PROTOTYPES /////////////////////////////////////////////

// game console
int Game_Init(void *parms=NULL);
int Game_Shutdown(void *parms=NULL);
int Game_Main(void *parms=NULL);

// GLOBALS ////////////////////////////////////////////////

HWND main_window_handle  = NULL; // save the window handle
HINSTANCE main_instance  = NULL; // save the instance
char buffer[80];                 // used to print text

BITMAP_IMAGE backdrop;      		// the background image

// Instantiate sounds
cBase					 base_ship;
cBaseHitSound  	 base_hit_sound;
cExtraLife		  	 extra_life_sound;

cBaseFire			 base_fire[BASE_FIRE_COUNT];
cBaseFireSound 	 base_fire_sound[BASE_FIRE_SOUND_COUNT];

cSpider 				 spider[SPIDER_COUNT];
cSpiderFire 		 spider_fire[SPIDER_FIRE_COUNT];
cSpiderStartSound  spider_start_sound[SPIDER_START_SOUND_COUNT];
cSpiderLaunchSound spider_launch_sound[SPIDER_LAUNCH_SOUND_COUNT];
cSpiderFireSound   spider_fire_sound[SPIDER_FIRE_SOUND_COUNT];
cSpiderHitSound 	 spider_hit_sound[SPIDER_HIT_SOUND_COUNT];

// Game globals
enum {start_title,		  								// Display he title screen
		start_new_wave,									// Reset variables, display wave message, and begin
		in_progress,										// Game running normally
		game_over											// Player out of lives
		} game_status;										// The status of the game

int				escape_pressed	= FALSE;				// Pressing ESCape at any time will terminate game/
int 				wave;										// Subsequent waves get harder
int				wave_end_message;						// Random message at end of screen
int				initial_wave_selected;				// The wave the user chose to start with, for repeat play.
int 				player_lives;							// Number of lives
int				player_lives_disp;					// Display the correct number (as actual gets decremented at exact moment);
unsigned long 	score;									// The score!
unsigned long	score_at_last_end_wave;				// Keep score at last wave completed for Continue
unsigned int	extra_life_val;						// Extra life awared every 'this' number of points
unsigned long	next_extra_life_awarded;			// Next extra life awarded at 'this' score
int				end_of_wave_nigh;						// End of wave in sight?
char				starting_level_description[10];	// For use in Game Over message
enum				{PAUSE_OFF, PAUSE_ON} pause; 	   // Game Pause facility
enum				{PAUSE_PRESSED_NO, PAUSE_PRESSED_YES} pause_press;
time_t 			demo_start_wait;						// Demo will start x seconds from here
int 				demo_start_wait_elapsed;			// Seconds since above
enum				{DEMO_OFF, DEMO_ON} demo; 	      // Game Demo facility

// FUNCTIONS //////////////////////////////////////////////

LRESULT CALLBACK WindowProc(HWND hwnd,
									 UINT msg,
									 WPARAM wparam,
									 LPARAM lparam)
{
// this is the main message handler of the system
PAINTSTRUCT	ps;		   // used in WM_PAINT
HDC			hdc;	   // handle to a device context

// what is the message
switch(msg)
	{
	case WM_CREATE:
		  {
			// do initialization stuff here
			return(0);
		  } break;

	 case WM_PAINT:
			{
			// start painting
			hdc = BeginPaint(hwnd,&ps);
			// end painting
			EndPaint(hwnd,&ps);
			return(0);
		  } break;

	case WM_DESTROY:
		{
		// kill the application
		PostQuitMessage(0);
		return(0);
		} break;

	default:break;

	 } // end switch

// process any messages that we didn't take care of
return (DefWindowProc(hwnd, msg, wparam, lparam));

} // end WinProc

// WINMAIN ////////////////////////////////////////////////

int WINAPI WinMain(	HINSTANCE hinstance,
					HINSTANCE hprevinstance,
					LPSTR lpcmdline,
					int ncmdshow)
{
// this is the winmain function

WNDCLASS winclass;	// this will hold the class we create
HWND	 	hwnd;			// generic window handle
MSG		msg;			// generic message
HDC      hdc;     	// generic dc
PAINTSTRUCT ps;   	// generic paintstruct

// first fill in the window class stucture
winclass.style				= CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
winclass.lpfnWndProc		= WindowProc;
winclass.cbClsExtra		= 0;
winclass.cbWndExtra		= 0;
winclass.hInstance		= hinstance;
winclass.hIcon				= LoadIcon(NULL, IDI_APPLICATION);
winclass.hCursor			= LoadCursor(NULL, IDC_ARROW);
winclass.hbrBackground	= GetStockObject(BLACK_BRUSH);
winclass.lpszMenuName	= NULL;
winclass.lpszClassName	= WINDOW_CLASS_NAME;

// register the window class
if (!RegisterClass(&winclass))
	return(0);

// create the window, note the use of WS_POPUP
if (!(hwnd = CreateWindow(WINDOW_CLASS_NAME, // class
						  "WinX Game Console",	 // title
						  WS_POPUP | WS_VISIBLE,
						  0,0,	   		// x,y
						  WINDOW_WIDTH,  	// width
						  WINDOW_HEIGHT, 	// height
						  NULL,	   		// handle to parent
						  NULL,	   		// handle to menu
						  hinstance,		// instance
						  NULL)))			// creation parms
return(0);

// save the window handle and instance in a global
main_window_handle = hwnd;
main_instance      = hinstance;

// perform all game console specific initialization
Game_Init();

// enter main event loop
while(1)
	{
	if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))
		{
		// test if this is a quit
		  if (msg.message == WM_QUIT)
			  break;

		// translate any accelerator keys
		TranslateMessage(&msg);

		// send the message to the window proc
		DispatchMessage(&msg);
		} // end if

	 // main game processing goes here
		Game_Main();
		if (escape_pressed)
			break;

	} // end while

// shutdown game and release all resources
Game_Shutdown();

// return to Windows like this
return(msg.wParam);

} // end WinMain

// WINX GAME PROGRAMMING CONSOLE FUNCTIONS ////////////////

int Game_Init(void *parms)
{
// this function is where you do all the initialization
// for your game
game_status = start_title;

// seed random number generate
srand(Start_Clock());

// hide the mouse
ShowCursor(FALSE);


int index;

// start up DirectDraw (replace the parms as you desire)
DD_Init(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP);

// initilize DirectSound
DSound_Init();

// initialize DirectInput
DInput_Init();

// initialize keyboard
DI_Init_Keyboard();

// initialize mouse
DI_Init_Mouse();

// initialize joystick
DI_Init_Joystick();

// Process the backdrop
Load_Bitmap_File(&bitmap8bit, "backdrop.bmp");
Set_Palette(bitmap8bit.palette);
Create_Bitmap(&backdrop, 0,0, SCREEN_WIDTH, SCREEN_HEIGHT);
Load_Image_Bitmap(&backdrop,&bitmap8bit,0,0,BITMAP_EXTRACT_MODE_ABS);
Unload_Bitmap_File(&bitmap8bit);

// load the player's BASE SHIP and Hit sound
base_ship.loadBOB();
base_hit_sound.loadSound();

// load the extra life sound
extra_life_sound.loadSound();


// Load BASE FIRE object BOBs
for (int base_fire_sub=0; base_fire_sub < BASE_FIRE_COUNT; base_fire_sub++)
	base_fire[base_fire_sub].loadBOB();

for (int base_fire_sound_sub=0; base_fire_sound_sub < BASE_FIRE_SOUND_COUNT; base_fire_sound_sub++)
	base_fire_sound[base_fire_sound_sub].loadSound();

// Load SPIDER object BOBs
for (int spider_sub=0; spider_sub < SPIDER_COUNT; spider_sub++)
	spider[spider_sub].loadBOB();

// Load SPIDER FIRE object BOBs
for (int spider_fire_sub=0; spider_fire_sub < SPIDER_FIRE_COUNT; spider_fire_sub++)
	spider_fire[spider_fire_sub].loadBOB();

// Load SOUNDS
for (int spider_start_sound_sub=0; spider_start_sound_sub < SPIDER_START_SOUND_COUNT; spider_start_sound_sub++)
	spider_start_sound[spider_start_sound_sub].loadSound();

for (int spider_launch_sound_sub=0; spider_launch_sound_sub < SPIDER_LAUNCH_SOUND_COUNT; spider_launch_sound_sub++)
	spider_launch_sound[spider_launch_sound_sub].loadSound();

for (int spider_fire_sound_sub=0; spider_fire_sound_sub < SPIDER_FIRE_SOUND_COUNT; spider_fire_sound_sub++)
	spider_fire_sound[spider_fire_sound_sub].loadSound();

for (int spider_hit_sound_sub=0; spider_hit_sound_sub < SPIDER_HIT_SOUND_COUNT; spider_hit_sound_sub++)
	spider_hit_sound[spider_hit_sound_sub].loadSound();


// return success
return(1);

} // end Game_Init

///////////////////////////////////////////////////////////

int Game_Shutdown(void *parms)
{
// this function is where you shutdown your game and
// release all resources that you allocated (automatically done in cBOB class)

// shut everything down
Destroy_Bitmap(&backdrop);

Delete_All_Sounds();

// shutdown directdraw last
DD_Shutdown();

// now directsound
DSound_Shutdown();

// finally directinput
DI_Release_Keyboard();
DI_Release_Mouse();
DI_Release_Joystick();

DInput_Shutdown();

// return success
return(1);
} // end Game_Shutdown


// Prorotypes for Game_Main;
void display_backdrop();
void check_user_input();
void reset_all_objects();
void print_left(char* text, int y_coord, int colour);
void print_centre(char* text, int y_coord, int colour);
void screen_fadeout(int effect);
void screen_scrunch();
void show_title_screen();
void new_wave();
void animate_base_ship();
void check_ship_hit_and_process_dying();
void draw_base_fires_and_check_if_hit_ship();
void create_new_spider();
void check_wave_end();
void check_descending_spiders_and_draw_webs();
void move_draw_check_spiders();
void play_spider_move_sound();
void check_for_spider_hit();
void draw_spider_fire_and_check_ship_hit();
void play_spider_hit_sound();
void print_status_line(char surface);
void game_over_screen();
///////////////////////////////////////////////////////////###########################################################

int Game_Main(void *parms)
{
	if (game_status == start_title)
		show_title_screen();

	else if (game_status == start_new_wave)
		new_wave();

	else if (game_status == in_progress)
	{
		Start_Clock();  // start the timing clock

		check_user_input(); // process input...test keyboard_state, joy_state

		if (pause == PAUSE_OFF)
		{
			display_backdrop();

			animate_base_ship();
			check_ship_hit_and_process_dying();
			draw_base_fires_and_check_if_hit_ship();

			create_new_spider();
			check_descending_spiders_and_draw_webs();

			check_for_spider_hit();
			move_draw_check_spiders();
			draw_spider_fire_and_check_ship_hit();

			if (game_status == in_progress)
				base_ship.draw();

			print_status_line('B');

			DD_Flip(); // flip the surfaces
		}
		else
			print_status_line('B');

		Wait_Clock(20); // Time sync
	}

	else if (game_status == game_over)
	{
		game_over_screen();
	}

	// return success
	return(0);

} // end Game_Main

///////////////////////////////////////////////////////////###########################################################
///////////////////////////////////////////////////////////###########################################################

///////////////////////////////////////////////////////////

void init_game()
// Initialise all active game switches.
{
	DD_Fill_Surface(lpddsback, 0);
	DD_Fill_Surface(lpddsprimary, 0);
	wave		 	    = 0;					// Initial wave-1
	player_lives    = 3;					// Number of lives player starts game with
	score			    = 0;					// Initial score
	pause 			 = PAUSE_OFF;
	pause_press 	 = PAUSE_PRESSED_NO;
	demo 			 	 = DEMO_OFF;

	game_status     = start_new_wave;
	reset_all_objects();
}


///////////////////////////////////////////////////////////

void reset_all_objects()
// Initialise all object to their centre/off position.
{
	// set position (X_position of base at bottom), Y_position of base at bottom - height of ship - area for displays
	base_ship.reset();

	for (int base_fire_sub=0; (base_fire_sub < BASE_FIRE_COUNT); base_fire_sub++)
		base_fire[base_fire_sub].reset();

	for (int spider_sub=0; spider_sub < SPIDER_COUNT; spider_sub++)
		spider[spider_sub].reset();

	for (int spider_fire_sub=0; (spider_fire_sub < SPIDER_FIRE_COUNT); spider_fire_sub++)
		spider_fire[spider_fire_sub].reset();

	int temp_message;
	do
   {
	  temp_message = rand()%10;
	} while (temp_message == wave_end_message);
	wave_end_message = temp_message;		// Random message from 0 to 9 (doesn't repeat)
}

///////////////////////////////////////////////////////////

void print_left(char* text, int y_coord, int colour)
// Print TEXT to primary buffer
{
		Draw_Text_GDI(text, 20, y_coord, colour, lpddsprimary);
}

///////////////////////////////////////////////////////////

void print_centre(char* text, int y_coord, int colour)
// Print TEXT to primary buffer
{
		Draw_Text_GDI(text, (SCREEN_WIDTH/2)-(7*strlen(text)/2), y_coord, colour, lpddsprimary);
}

///////////////////////////////////////////////////////////

void screen_fadeout(int effect = SCREEN_DARKNESS)
{
	PALETTEENTRY the_palette[256];
	Save_Palette(the_palette);
	DD_Lock_Primary_Surface();
	Screen_Transitions(effect,primary_buffer,primary_lpitch);
	DD_Unlock_Primary_Surface();
	DD_Fill_Surface(lpddsprimary,0);
	DD_Fill_Surface(lpddsback, 0);
	Set_Palette(the_palette);
}

///////////////////////////////////////////////////////////

void show_title_screen()
{
	init_game();
	DD_Fill_Surface(lpddsprimary, 1);
	Draw_Text_GDI("v 1.5",600,0,144,lpddsprimary);	// Version number top-right
	print_centre("=============  ",8,198);
	print_centre("Mutant Xpiders",20,204);
	print_centre("=============  ",34,198);
	print_centre("(A PC DirectX sequel to my 1983 BBC Microcomputer game, by Trevor LAWFORD)",56,59);
	print_left("A simple left/right/up/down/fire game, written as an exercise in DirectX5 programming.",105,63);
	print_left("Play using keyboard (Z=left, X=right, K=up, M=down, SPACE=fire), or a digital joystick.",130,156);
	print_left("For *best* play mode use the MOUSE.  P=toggle Pause.  SPACE=end Demo.  ESC=Quit.",145,156);
	print_left("Game Basics:",185,125);
	print_left("1) You can use the sustained-fire capability of your Base Cannon as much as you want,",200,125);
	print_left("    but *do* bear in mind that what goes up must come down, with attitude!",215,125);
	print_left("    Use the sideways movement of your Cannon to affect angle of fire.",230,125);
	print_left("2) All on-screen shots are lethal to yourself, the Xpiders, and any opposing shots.",245,125);
	print_left("3) SCORE: Descending Xpiders=50, Flying Xpiders=200, Completed wave=500.",260,125);
	print_centre("Comments are welcome to me at <trevor.lawford@btinternet.com>    ",305,215);
	print_centre("My Web page is at <http://www.btinternet.com/~~trevor.lawford/>",325,215);
	print_left("                "\
				  "-------------------------------------------------------------------------------------------------------------------",360,100);
	print_centre("Enter starting difficulty - press...",380,210);
	print_centre("(1) Trainee (start wave 1, extra life every 10000)",405,180);
	print_centre("(2) Regular (start wave 3, extra life every 15000)",420,174);
	print_centre("                     (3) Pro        (start wave 7, extra life every 20000)",435,30);

	demo_start_wait = time(NULL);	// Time now
	for (int dx=0; dx<19; dx++)
	{
		Draw_Text_GDI(".",580+(dx*3),460,144,lpddsprimary);	// Print initial countdown 'dots'
	}
	int selected = FALSE;
	do
	{
		demo_start_wait_elapsed = time(NULL) - demo_start_wait;	// Time since demo wait started in seconds
		DI_Read_Keyboard(); // get input
		if (keyboard_state[DIK_ESCAPE])
		{
			escape_pressed = TRUE;	// Finished!
			return; // Fast track attahere
		}
		else if (keyboard_state[DIK_1])
		{
			selected = TRUE;
			strcpy(starting_level_description,"trainee");
			wave=0;
			extra_life_val = 10000;
		}
		else if (keyboard_state[DIK_2])
		{
			selected = TRUE;
			strcpy(starting_level_description,"regular");
			wave=2;
			extra_life_val = 15000;
		}
		else if (keyboard_state[DIK_3])
		{
			selected = TRUE;
			strcpy(starting_level_description,"pro");
			wave=6;
			extra_life_val = 20000;
		}
		else if (demo_start_wait_elapsed >= 19)		// Wait 20 seconds before starting demo
		{
			demo = DEMO_ON;
			selected = TRUE;
			strcpy(starting_level_description,"DEMO");
			wave=(rand()%5)+8;
			extra_life_val = 60000;		// No extra life in demo.
		}
		else
			Draw_Text_GDI(".",577+(demo_start_wait_elapsed*3),460,1,lpddsprimary);	// Erase timed-out 'dots' with background-colour print
	} while (!selected);
	next_extra_life_awarded = extra_life_val;

	initial_wave_selected = wave;
	screen_fadeout(SCREEN_SCRUNCH);
	game_status = start_new_wave;

}

///////////////////////////////////////////////////////////

void new_wave()
// Message at New Wave
{
		wave++;	// Move up to first/next wave
		DD_Fill_Surface(lpddsprimary, 1);
		if (demo == DEMO_OFF)
			sprintf(buffer,"Wave #%d",wave);
		else
			sprintf(buffer,"DEMO Wave #%d",wave);
		print_centre(buffer,200,210);
		player_lives_disp = player_lives;
		print_status_line('P');
		Sleep(600);
		reset_all_objects();

		// Wave-specific variables
		end_of_wave_nigh = 0;
		cSpider::setWaveVariables(wave);
		cSpiderFire::setWaveVariables(wave);

		for (int spider_start_sound_sub=0; spider_start_sound_sub < SPIDER_START_SOUND_COUNT; spider_start_sound_sub++)
			spider_start_sound[spider_start_sound_sub].init_pan();

		for (int spider_launch_sound_sub=0; spider_launch_sound_sub < SPIDER_LAUNCH_SOUND_COUNT; spider_launch_sound_sub++)
			spider_launch_sound[spider_launch_sound_sub].init_pan();

		for (int spider_fire_sound_sub=0; spider_fire_sound_sub < SPIDER_FIRE_SOUND_COUNT; spider_fire_sound_sub++)
			spider_fire_sound[spider_fire_sound_sub].init_pan();

		for (int spider_hit_sound_sub=0; spider_hit_sound_sub < SPIDER_HIT_SOUND_COUNT; spider_hit_sound_sub++)
			spider_hit_sound[spider_hit_sound_sub].init_pan();

		screen_fadeout();
		cSpider::currently_alive = 0;

		game_status = in_progress;
}

///////////////////////////////////////////////////////////

void display_backdrop()
// Display the backdrop (screen-sized BMP)
{
	// lock the back buffer
	DD_Lock_Back_Surface();
	// draw the background reactor image
	Draw_Bitmap(&backdrop, back_buffer, back_lpitch, 0);
	// unlock the back buffer
	DD_Unlock_Back_Surface();
}

///////////////////////////////////////////////////////////

void check_user_input()
{
	DI_Read_Keyboard(); // get input
	DI_Read_Joystick();
	DI_Read_Mouse();

	// Exit if (a) player presses ESCape, or (b) is player is out of lives
	if (keyboard_state[DIK_ESCAPE])
	{
		PostMessage(main_window_handle, WM_DESTROY,0,0);
		escape_pressed = TRUE;	// Finished!
	}

	// Pause gameplay if player presses 'P'
	// Player must have released P before toggle works again
	if (keyboard_state[DIK_P])
	{
		if (pause_press == PAUSE_PRESSED_NO)
		{
			pause_press = PAUSE_PRESSED_YES;
			if (pause == PAUSE_OFF)
				pause = PAUSE_ON;
			else
				pause = PAUSE_OFF;
		}
	}
	else
		pause_press = PAUSE_PRESSED_NO;
	if (pause == PAUSE_ON)
		return;

	// Stop demo if player presses SPACE
	if (keyboard_state[DIK_SPACE])
	{
		if (demo == DEMO_ON)
		{
			demo = DEMO_OFF;
			init_game();
			game_status = start_title;
			return;
		}
	}

	base_ship.ship_fire_move = 0;			 										// Reset Momentum of sideways movement of ship passed on to current ship fire

	if (base_ship.isAlive())														// Only move/fire if Base Ship OK
	{
		base_ship.move(demo);														// Process ship movement according to user input

		// FIRE!
		if ( (demo == DEMO_OFF) &&
			 ((keyboard_state[DIK_SPACE])	   									// Space = base_ship Fire!
		  || (joy_state.rgbButtons[0]) 											// Joystick FIRE button
		  || (mouse_state.rgbButtons[0]))
		  || ((demo == DEMO_ON) &&
				(rand()%10==0)) )

		{
			if ( (!base_ship.isFiring()) && (end_of_wave_nigh == 0) )	// Can only fire when outside firing cycle
			{
				if (demo == DEMO_OFF)
					Play_Sound(base_fire_sound[cBaseFireSound::next_free_sound(BASE_FIRE_SOUND_COUNT)].getSoundId(),0);	// Play firing sound
				base_ship.startFiring();											// Start cycle
				int start_base_ship_fire_count = 0;								// Count is for 1 of 2 subsequent firings to be initiated here
				// Find the next 2 free slots in firing list
				for (int base_fire_sub=0; (base_fire_sub < BASE_FIRE_COUNT) && (start_base_ship_fire_count < 2); base_fire_sub++)
				{
					if (base_fire[base_fire_sub].isDead())
					{
						// Start off a new base fire
						if (start_base_ship_fire_count == 0)					// Set 1st fire loose NOW
						{
							base_fire[base_fire_sub].start_new_base_fire(base_ship.getX()-1, base_ship.getY(), base_ship.ship_fire_move);
						}
						else
							base_ship.setBaseFire2(base_fire_sub);				// Record 2nd fire for NEXT TIME (in animate_base_ship)
						start_base_ship_fire_count++;
					}
				}
			}
		}
	}
}

///////////////////////////////////////////////////////////

void animate_base_ship()
// Animate base_ship if currently in firing cycle - release 2nd of 2 fire if ready
{
	int fire_second = base_ship.animate_and_check_second_fire();
	if (fire_second)
		base_fire[fire_second].start_new_base_fire(base_ship.getX()+25, base_ship.getY(), base_ship.ship_fire_move);
}

///////////////////////////////////////////////////////////

void check_ship_hit_and_process_dying()
// Ship has been hit!
{
	if ( (player_lives == 0) && (!base_ship.isDying()) )
			game_status = game_over;

	if (base_ship.isDying())
	{
		if (base_ship.dead_cycle_count > 0)
			Animate_BOB(base_ship.get_pBOB());
		base_ship.dead_cycle_count++;

		if (base_ship.dead_cycle_count > 28)						// Leave ship image upside down
		{
			base_ship.setDead();	  // Ship now gone
			// Reset everything for next life
			if (end_of_wave_nigh == 0)
				reset_all_objects();										//else this will happen when new wave starts anyway
			cSpider::this_wave += cSpider::currently_alive; 	// Keep number of spiders in current wave to start again
			// Fade to Black before we start the next life.
			if (end_of_wave_nigh == 0)
				screen_fadeout(SCREEN_DISOLVE);
			player_lives_disp = player_lives;
		}
	}
}

///////////////////////////////////////////////////////////

void draw_base_fires_and_check_if_hit_ship()
// Move and display all current base fires
{
	cBaseFire::base_fire_currently_alive = 0;
	for (int base_fire_sub=0; (base_fire_sub < BASE_FIRE_COUNT); base_fire_sub++)
	{
		if (base_fire[base_fire_sub].isAlive())
		{
			cBaseFire::base_fire_currently_alive++;
			//Draw_Text_GDI("X",base_fire_sub*10,03,RGB(255,150,0),lpddsback); // LIVE fires
			if ( (base_ship.isAlive()) &&
				  (base_fire[base_fire_sub].getYV() < 0) &&													// Fire coming back down
				  (Collision_BOBS(base_ship.get_pBOB(),base_fire[base_fire_sub].get_pBOB())) )	// and ship in same place
			{
				base_fire[base_fire_sub].setDead();																// Fire destroyed as well
#ifndef ship_nohit
				if (demo == DEMO_OFF)
				{
					Stop_All_Sounds();
					base_hit_sound.playSound();																	// Play 'hit' sound
					base_ship.setDying();																			// Ship hit by own fire - DYING
					player_lives--;
					Set_Animation_BOB(base_ship.get_pBOB(),1);												// Explode!!
				}
#endif
			}

			//sprintf(buffer,"%d",base_fire_clone[0].yv);
			//Draw_Text_GDI(buffer,0,0,RGB(255,150,0),lpddsback);
			base_fire[base_fire_sub].move_draw();
		}
		//else
			//Draw_Text_GDI("x",base_fire_sub*10,0,RGB(0,150,255),lpddsback); // DEAD fires
	}
}

///////////////////////////////////////////////////////////

void create_new_spider()
// Starting up new spiders as DESCENDERS on webs
{

	cSpider::currently_alive = 0;
	for (int spider_sub=0; spider_sub < SPIDER_COUNT; spider_sub++)
	{
		if (spider[spider_sub].isActive())
			cSpider::currently_alive++;
	}

	//sprintf(buffer,"%d",cSpider::spiders_currently_alive);						// Write number of live spiders
	//Draw_Text_GDI(buffer,0,0,RGB(255,150,0),lpddsback);

	if (cSpider::currently_alive > cSpider::allowed_onscreen)					// In earlier waves we don't have all of them at once;
		return;

	check_wave_end();		// Finished wave yet?

	for (spider_sub=0; spider_sub < SPIDER_COUNT; spider_sub++)
	{
		//The higher the wave, the more frequent they are!!
		if ( (spider[spider_sub].isInactive()) && (rand()%3000 < (wave<<1)) && (cSpider::this_wave > 0))
		{
			if (demo == DEMO_OFF)
				Play_Sound(spider_start_sound[cSpiderStartSound::next_free_sound(SPIDER_START_SOUND_COUNT)].getSoundId(),0);
			cSpider::this_wave--;															// Count down for Spiders this wave
			spider[spider_sub].setDescending();											// set Spider starting down the web
		}
	}
}

///////////////////////////////////////////////////////////

void check_wave_end()
// Check whether we start a new wave yet
{
	// If no more spiders to launch this wave, wait until all the current ones are gone.
	if ( (cSpider::this_wave == 0) && (cSpider::currently_alive == 0) )
	{
		if (end_of_wave_nigh == 0)
			end_of_wave_nigh = 1;
		if ( (cSpiderFire::spider_fire_currently_alive == 0) && (cBaseFire::base_fire_currently_alive == 0) )
		{
			end_of_wave_nigh++;
			if ( (end_of_wave_nigh > 3) && (!base_ship.isDying()) )		// Allow for game logic to undraw last fire on screen
			{
				screen_fadeout();
				if (demo == DEMO_ON)													// If DEMO mode then go back to title screen
				{
					demo = DEMO_OFF;
					init_game();
					game_status = start_title;
					return;
				}
				score += 500;     													// Score 500 for finishing a wave
				score_at_last_end_wave = score;									// Keep score at last wave completed for Continue
				if (player_lives != 0)												// Did we lose final life at end of wave?
					game_status = start_new_wave;
				return;
			}
		}
	}
}

///////////////////////////////////////////////////////////

void check_descending_spiders_and_draw_webs()
//Draw all the current webs for descending spiders - check when they are turning into Flying Spiders!
// Must be done on LOCKed surface
{
	DD_Lock_Back_Surface();
	for (int spider_sub=0; spider_sub < SPIDER_COUNT; spider_sub++)
	{
		if (spider[spider_sub].isDescending())
		{
			VLine(0,spider[spider_sub].getY(),spider[spider_sub].getX()+15,180,back_buffer, back_lpitch);
			// Grow until max_web_length - then becomes FLYING SPIDER
			if (!spider[spider_sub].isWebFinished())
				spider[spider_sub].growWeb();
			else
			{
				// Spider now reached end of web and turned in FLYING SPIDER - armed and dangerous!!!
				spider[spider_sub].setFlying();
				if (demo == DEMO_OFF)
					Play_Sound(spider_launch_sound[cSpiderLaunchSound::next_free_sound(SPIDER_LAUNCH_SOUND_COUNT)].getSoundId(),0);
			}
		}

	}
	DD_Unlock_Back_Surface();
}
///////////////////////////////////////////////////////////

void move_draw_check_spiders()
// Has flying spider hit ship?
// Ready to change direction?
// Ready to fire?
// Draw Spider.
{
	for (int spider_sub=0; spider_sub < SPIDER_COUNT; spider_sub++)
	{
		if (spider[spider_sub].isFlying())
		{
			if ( (base_ship.isAlive()) &&
				  (Collision_BOBS(base_ship.get_pBOB(),spider[spider_sub].get_pBOB())) )	// Ship hit by flying Spider?
			{
				spider[spider_sub].setInactive();														// Spider is hit as well!
				play_spider_hit_sound();
#ifndef ship_nohit
				if (demo == DEMO_OFF)
				{
					Stop_All_Sounds();
					base_hit_sound.playSound();															// Play 'hit' sound
					base_ship.setDying();																	// Ship hit by flying Spider - DYING
					player_lives--;
					Set_Animation_BOB(base_ship.get_pBOB(),1);										// Explode!!
				}
#endif
			}

			spider[spider_sub].checkChangeDir();

			// Check for Spider firing
			if (spider[spider_sub].fireNow())
			{
				// Time for Spider to fire
				int spider_fire_found = FALSE;
				for (int spider_fire_sub=0; (spider_fire_sub < SPIDER_FIRE_COUNT) && (!spider_fire_found); spider_fire_sub++)
				{
					if (spider_fire[spider_fire_sub].isDead())	// Find first available spider fire
					{
						spider_fire[spider_fire_sub].startFire(spider[spider_sub].getX(), spider[spider_sub].getY(),
																			spider[spider_sub].getXV(), spider[spider_sub].getYV(),
																			spider_sub);
						spider_fire_found = TRUE;
						if (demo == DEMO_OFF)
							Play_Sound(spider_fire_sound[cSpiderFireSound::next_free_sound(SPIDER_FIRE_SOUND_COUNT)].getSoundId(),0);	// Play firing sound
					}
				}
				spider[spider_sub].setTimeBeforeFiring();			// Set counter for next spider firing
			}
		}

		spider[spider_sub].draw();
	}
}

///////////////////////////////////////////////////////////

void check_for_spider_hit()
// Check whether a live Spider has been hit by Base Ship fire.
{
	for (int spider_sub=0; spider_sub < SPIDER_COUNT; spider_sub++)
	{
		if ( spider[spider_sub].isDescending() || spider[spider_sub].isFlying() )
		{
			// First check... have we hit one with 'live' fire
			for (int base_fire_sub=0; (base_fire_sub < BASE_FIRE_COUNT); base_fire_sub++)
			{
				if (base_fire[base_fire_sub].isAlive())
				{
					if (Collision_BOBS(spider[spider_sub].get_pBOB(),base_fire[base_fire_sub].get_pBOB()))
					{
						// We've hit a spider!
						if (spider[spider_sub].isFlying())
							score += 200;							// Score 200 for FLYING spiders
						else
							score += 50; 							// Score 50 for DESCENDING spiders
						spider[spider_sub].setDying();
				  play_spider_hit_sound();
						base_fire[base_fire_sub].setDead();	// And fire is out, too.
					}
				}
			}

			// Now check... get this... that a Spider Fire hasn't hit another Spider... YES!!!!
			for (int spider_fire_sub=0; (spider_fire_sub < SPIDER_FIRE_COUNT); spider_fire_sub++)
			{
				  if (spider_fire[spider_fire_sub].isAlive())
				  {
						if ( (Collision_BOBS(spider[spider_sub].get_pBOB(),spider_fire[spider_fire_sub].get_pBOB()))
						  && (spider_fire[spider_fire_sub].getOwner() != spider_sub) )
						{
							// A SPIDER hit another spider (not self!)
							spider[spider_sub].setDying();
					 play_spider_hit_sound();
							spider_fire[spider_fire_sub].setDead();	// And Spider fire is out, too.
						}
				  }
			}

		}
	}
}
///////////////////////////////////////////////////////////

void draw_spider_fire_and_check_ship_hit()
// Draw and advance all Spider Fires
{
	cSpiderFire::spider_fire_currently_alive = 0;
	for (int spider_fire_sub=0; spider_fire_sub < SPIDER_FIRE_COUNT; spider_fire_sub++)
	{
		if (spider_fire[spider_fire_sub].isAlive())
		{
			cSpiderFire::spider_fire_currently_alive++;
			// Draw_Text_GDI("X",spider_fire_sub*10,03,RGB(255,150,0),lpddsback); // LIVE fires DEBUG
			// Has Spider fire hit Base Ship?
			if (base_ship.isAlive())
			{
				if (Collision_BOBS(base_ship.get_pBOB(),spider_fire[spider_fire_sub].get_pBOB()))	// Ship hit by Spider Fire?
				{
					spider_fire[spider_fire_sub].setDead();			// Stop displaying fire.
#ifndef ship_nohit
					if (demo == DEMO_OFF)
					{
					   Stop_All_Sounds();
						base_hit_sound.playSound();						// Play 'hit' sound
						base_ship.setDying();								// Ship hit by flying Spider fire - DYING
						player_lives--;
						Set_Animation_BOB(base_ship.get_pBOB(),1);	// Explode!!
					}
#endif
				}
			}

			// Ship Fire hit Spider fire - null both
			for (int base_fire_sub=0; (base_fire_sub < BASE_FIRE_COUNT); base_fire_sub++)
			{
				if (base_fire[base_fire_sub].isAlive())
				{
					if (Collision_BOBS(spider_fire[spider_fire_sub].get_pBOB(),base_fire[base_fire_sub].get_pBOB()))
					{
						spider_fire[spider_fire_sub].setDead();		// Stop displaying fire.
						base_fire[base_fire_sub].setDead();		  		// And fire is out, too.
					}
				}
			}
			spider_fire[spider_fire_sub].move_draw();
		}
		// else
			// Draw_Text_GDI("x",spider_fire_sub*10,0,RGB(0,150,255),lpddsback); // DEAD fires - DEBUG
	}
}

///////////////////////////////////////////////////////////

void play_spider_hit_sound()
// Spider has been hit - make the sound.
{
	if (demo == DEMO_OFF)
		Play_Sound(spider_hit_sound[cSpiderHitSound::next_free_sound(SPIDER_HIT_SOUND_COUNT)].getSoundId(),0);
}

///////////////////////////////////////////////////////////

void print_status_line(char surface)
// Print the player's life and the score
{
	// First check whether player should get another life
	if (score >= next_extra_life_awarded)
	{
		next_extra_life_awarded += extra_life_val;
		if (demo == DEMO_OFF)
			extra_life_sound.playSound();						 // Play 'extra life' sound
		player_lives++;
		player_lives_disp = player_lives;
	}

	sprintf(buffer,"%d : %.8d",player_lives_disp,score);
	if (surface == 'P')
		Draw_Text_GDI(buffer,8,SCREEN_HEIGHT-20,67,lpddsprimary);
	else
		Draw_Text_GDI(buffer,8,SCREEN_HEIGHT-20,67,lpddsback);

	if ( (end_of_wave_nigh > 0) && (demo == DEMO_OFF) )
	{
		switch (wave_end_message)
		{
			case 0:
				Draw_Text_GDI("< <   FINISHED 'EM OFF   > >", 220, 100, (rand()%214)+1, lpddsback);
				break;
			case 1:
				Draw_Text_GDI("< <   GOOD SHOOTIN' THERE   > >", 210, 100, (rand()%214)+1, lpddsback);
				break;
			case 2:
				Draw_Text_GDI("< <   SENT 'EM TO HELL!   > >", 220, 100, (rand()%214)+1, lpddsback);
				break;
			case 3:
				Draw_Text_GDI("< <   SHOOT THEM SUCKERS DOWN   > >", 200, 100, (rand()%214)+1, lpddsback);
				break;
			case 4:
				Draw_Text_GDI("< <   SPLATTED 'EM GOOD!!   > >", 210, 100, (rand()%214)+1, lpddsback);
				break;
			case 5:
				Draw_Text_GDI("< <   MAKE 'EM SCREAM   > >", 220, 100, (rand()%214)+1, lpddsback);
				break;
			case 6:
				Draw_Text_GDI("< <   ATTACK OVER   > >", 230, 100, (rand()%214)+1, lpddsback);
				break;
			case 7:
				Draw_Text_GDI("< <   WAVE COMPLETED   > >", 220, 100, (rand()%214)+1, lpddsback);
				break;
			case 8:
				Draw_Text_GDI("< <   MAKE MY DAY   > >", 230, 100, (rand()%214)+1, lpddsback);
				break;
			default:
				Draw_Text_GDI("< <   WELL DONE PLAYER   > >", 220, 100, (rand()%214)+1, lpddsback);
				break;
		}
	}
	if (pause == PAUSE_ON)
	{
		Draw_Text_GDI("<<  DEMO MODE (SPACE to end)  >>", 204, 200, 0, lpddsprimary); // Wipeout pre-existing text
		Draw_Text_GDI("< <   G A M E   P A U S E D   > >", 220, 200, (rand()%214)+1, lpddsprimary);
	}
	if (demo == DEMO_ON)
	{
		Draw_Text_GDI("<<  DEMO MODE (SPACE to end)  >>", 204, 200, (rand()%214)+1, lpddsback);
	}
}
///////////////////////////////////////////////////////////

void game_over_screen()
{
		Stop_All_Sounds();
		int wave_reached = wave-1;		// Current wave, for 'Continue' option.
		DD_Fill_Surface(lpddsprimary, 0);
		print_centre("G A M E    O V E R",100,30);
		sprintf(buffer,"You reached wave %d, and scored %d points at '%s' level",wave,score,starting_level_description);
		print_centre(buffer,180,155);
		print_centre("Another Game? (SPACE = yes,   C = Continue,   ESC = Main Menu)",240,210);
		print_centre("                        ('Continue' starts at last completed wave with score at that time)",260,210);
		print_status_line('P');
		int chosen = FALSE;
		while (!chosen)
		{
			DI_Read_Keyboard(); // get input
			if (keyboard_state[DIK_SPACE])
			{
				chosen=TRUE;
				init_game();
				wave = initial_wave_selected;		// Start with what user requested originally
				print_centre("< <   GET READY   > >",210,69);
			}
			if (keyboard_state[DIK_C])
			{
				chosen=TRUE;
				init_game();
				wave = wave_reached;					// Continue with current wave
			score = score_at_last_end_wave;	// Refresh score from last successful wave end
				print_centre("< <   GET READY   > >",210,69);
			}
			if (keyboard_state[DIK_ESCAPE])
			{
				chosen = TRUE;
				init_game();
				game_status = start_title;
				print_centre("Returning to Main Menu",210,69);
			}
		}
		screen_fadeout();
}
