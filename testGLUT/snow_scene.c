/******************************************************************************
 *
 * Animation v1.0 (23/02/2021)
 *
 * This template provides a basic FPS-limited render loop for an animated scene.
 *
 ******************************************************************************/

#include <Windows.h>
#include <freeglut.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>





 /******************************************************************************
  * Animation & Timing Setup
  ******************************************************************************/

  // Target frame rate (number of Frames Per Second).
#define TARGET_FPS 60				

// Ideal time each frame should be displayed for (in milliseconds).
const unsigned int FRAME_TIME = 1000 / TARGET_FPS;

// Frame time in fractional seconds.
// Note: This is calculated to accurately reflect the truncated integer value of
// FRAME_TIME, which is used for timing, rather than the more accurate fractional
// value we'd get if we simply calculated "FRAME_TIME_SEC = 1.0f / TARGET_FPS".
const float FRAME_TIME_SEC = (1000 / TARGET_FPS) / 1000.0f;

// Time we started preparing the current frame (in milliseconds since GLUT was initialized).
unsigned int frameStartTime = 0;

/******************************************************************************
 * Keyboard Input Handling Setup
 ******************************************************************************/

 // Define all character keys used for input (add any new key definitions here).
 // Note: USE ONLY LOWERCASE CHARACTERS HERE. The keyboard handler provided converts all
 // characters typed by the user to lowercase, so the SHIFT key is ignored.

#define KEY_EXIT			'q' // Escape key.
#define KEY_STOP           115
#define KEY_DiaOFF          'd'
/******************************************************************************
 * GLUT Callback Prototypes
 ******************************************************************************/

void display(void);
void reshape(int width, int h);
void keyPressed(unsigned char key, int x, int y);
void idle(void);
void drawLand(void);
void drawSky(void);
void drawSnowCircle(float x, float y, float radius, float r, float g, float b, float a);
void drawTree(void);
void drawLine(void);
void drawLine_Bulb(float line_x, float line_y, float line_Btm_x, float line_Btm_y);
void draw_Snow_Arm(void);
void draw_Left_Arm(void);
void drawInformation(float x, float y, void *font, char *string);
void drawSnow(void);

/******************************************************************************
 * Animation-Specific Function Prototypes (add your own here)
 ******************************************************************************/

void main(int argc, char **argv);
void init(void);
void think(void);

/******************************************************************************
 * Animation-Specific Setup (Add your own definitions, constants, and globals here)
 ******************************************************************************/
 //flag
int animation = 1;
boolean dia = 1;
#define PI 3.1415926
#define DEG_TO_RAD PI/180.0

float line_Angle = 0.0f;
float direction = 1.0f;
float extend_Arm = 0.001f;
float snowBall_angel = 0.0f;
float button_Move = 0.15f;
int start = 0;
int frameCount = 0;
int repeatParticles = 0;
int totalNumber = 0;
char str[30];

typedef struct
{
	float x;
	float y;
}land_Position;

land_Position point[1];

typedef struct
{
	float x;
	float y;
}snowVer;

typedef struct
{
	snowVer location;
	float size;
	float alpha;
	float speed;
	boolean active;
}snowPoint;

snowPoint snow_Point[1000];

typedef struct
{
	float x;
	float y;
}armVer;

armVer arm_Ver[1];

typedef struct
{
	float x;
	float y;
}leftArm;

leftArm left_Arm[1];

/******************************************************************************
 * Entry Point (don't put anything except the main function here)
 ******************************************************************************/

void main(int argc, char **argv)
{
	// Initialize the OpenGL window.
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(1000, 800);
	glutCreateWindow("Animation");

	// Set up the scene.
	init();

	// Disable key repeat (keyPressed or specialKeyPressed will only be called once when a key is first pressed).
	glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF);

	// Register GLUT callbacks.
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyPressed);
	glutIdleFunc(idle);

	// Record when we started rendering the very first frame (which should happen after we call glutMainLoop).
	frameStartTime = (unsigned int)glutGet(GLUT_ELAPSED_TIME);

	// Enter the main drawing loop (this will never return).
	glutMainLoop();
}

/******************************************************************************
 * GLUT Callbacks (don't add any other functions here)
 ******************************************************************************/

 /*
	 Called when GLUT wants us to (re)draw the current animation frame.

	 Note: This function must not do anything to update the state of our simulated
	 world. Animation (moving or rotating things, responding to keyboard input,
	 etc.) should only be performed within the think() function provided below.
 */
void display(void)
{
	frameCount++;
	drawSky();
	drawLand();
	drawSnowCircle(0.25f, 0.25f, 0.1f, 0.5f, 0.5f, 0.5f, 0.9f);
	drawSnowCircle(0.25f, 0.35f, 0.06f, 0.5f, 0.5f, 0.5f, 0.9f);
	drawSnowCircle(0.22f, 0.37f, 0.007f, 0.0f, 0.0f, 0.0f, 0.9f);
	drawSnowCircle(0.28f, 0.37f, 0.007f, 0.0f, 0.0f, 0.0f, 0.9f);
	drawTree();
	drawLine(); // draw the line and bulb
	drawLine_Bulb(0.5f, 0.9f, 0.5f, 0.8f);
	drawLine_Bulb(0.25f, 0.9f, 0.25f, 0.8f);
	drawLine_Bulb(0.75f, 0.9f, 0.75f, 0.8f);
	draw_Snow_Arm();
	draw_Left_Arm();
	if (dia)
	{
		drawInformation(0.01f, 0.98f, GLUT_BITMAP_HELVETICA_12, "Diagnostics:");

		snprintf(str, 30, "particles: %d of 1000", totalNumber);
		drawInformation(0.02f, 0.965f, GLUT_BITMAP_HELVETICA_12, str);
		drawInformation(0.01f, 0.95f, GLUT_BITMAP_HELVETICA_12, "Scene controls:");
		drawInformation(0.02f, 0.935f, GLUT_BITMAP_HELVETICA_12, "s: toggle snow");
		drawInformation(0.02f, 0.92f, GLUT_BITMAP_HELVETICA_12, "q: quit");
		drawInformation(0.02f, 0.905f, GLUT_BITMAP_HELVETICA_12, "d: hide information");
	}

	drawSnow();

	glutSwapBuffers();
}

/*
	Called when the OpenGL window has been resized.
*/
void reshape(int width, int h)
{
}

/*
	Called each time a character key (e.g. a letter, number, or symbol) is pressed.
*/
void keyPressed(unsigned char key, int x, int y)
{
	switch (tolower(key)) {
	case KEY_STOP:
		animation = !animation;
		break;

	case KEY_EXIT:
		exit(0);
		break;

	case KEY_DiaOFF:
		dia = !dia;
		break;
	}
}

/*
	Called by GLUT when it's not rendering a frame.

	Note: We use this to handle animation and timing. You shouldn't need to modify
	this callback at all. Instead, place your animation logic (e.g. moving or rotating
	things) within the think() method provided with this template.
*/
void idle(void)
{
	// Wait until it's time to render the next frame.

	unsigned int frameTimeElapsed = (unsigned int)glutGet(GLUT_ELAPSED_TIME) - frameStartTime;
	if (frameTimeElapsed < FRAME_TIME)
	{
		// This frame took less time to render than the ideal FRAME_TIME: we'll suspend this thread for the remaining time,
		// so we're not taking up the CPU until we need to render another frame.
		unsigned int timeLeft = FRAME_TIME - frameTimeElapsed;
		Sleep(timeLeft);
	}

	// Begin processing the next frame.

	frameStartTime = glutGet(GLUT_ELAPSED_TIME); // Record when we started work on the new frame.

	think(); // Update our simulated world before the next call to display().

	glutPostRedisplay(); // Tell OpenGL there's a new frame ready to be drawn.
}

void drawLand(void)
{

	glBegin(GL_POLYGON);
	glColor4f(1.0f, 1.0f, 1.0f, 0.2f);
	glVertex2f(0.0f, 0.0f);
	glColor4f(1.0f, 1.0f, 1.0f, 0.2f);
	glVertex2f(0.3f, 0.0f);
	glColor4f(0.5f, 0.5f, 0.5f, 1.0f);
	glVertex2f(point[0].x, point[0].y);
	glColor4f(0.5f, 0.5f, 0.5f, 1.0f);
	glVertex2f(0.0f, point[1].y);
	glEnd();


	glBegin(GL_POLYGON);
	glColor4f(1.0f, 1.0f, 1.0f, 0.2f);
	glVertex2f(0.3f, 0.0f);
	glColor4f(1.0f, 1.0f, 1.0f, 0.2f);
	glVertex2f(0.7f, 0.0f);
	glColor4f(0.5f, 0.5f, 0.5f, 1.0f);
	glVertex2f(point[1].x, point[0].y);
	glColor4f(0.5f, 0.5f, 0.5f, 1.0f);
	glVertex2f(point[0].x, point[0].y);
	glEnd();


	glBegin(GL_POLYGON);
	glColor4f(1.0f, 1.0, 1.0, 0.2);
	glVertex2f(0.7f, 0.0);
	glColor4f(1.0f, 1.0, 1.0, 0.2);
	glVertex2f(1.0f, 0.0);
	glColor4f(0.5f, 0.5f, 0.5f, 1.0f);
	glVertex2f(1.0f, point[1].y);
	glColor4f(0.5f, 0.5f, 0.5f, 1.0f);
	glVertex2f(point[1].x, point[0].y);
	glEnd();

}

void drawSky(void)
{
	glClear(GL_COLOR_BUFFER_BIT);

	glBegin(GL_POLYGON);
	glColor4f(0.0f, 0.1f, 1.0f, 0.2f);
	glVertex2f(0.0f, 0.0f);
	glColor4f(0.0f, 0.1f, 1.0f, 0.2f);
	glVertex2f(1.0f, 0.0f);
	glColor4f(0.0f, 0.1f, 0.5f, 1.0f);
	glVertex2f(1.0f, 1.0f);
	glColor4f(0.0f, 0.1f, 0.5f, 1.0f);
	glVertex2f(0.0f, 1.0f);
	glEnd();
}

void drawSnowCircle(float x, float y, float radius, float r, float g, float b, float a)
{
	glBegin(GL_TRIANGLE_FAN);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glVertex2f(x, y);
	for (int theta = 0; theta <= 360; theta += 1)
	{
		glColor4f(r, g, b, a);
		glVertex2f(cos(theta * DEG_TO_RAD) * radius + x,
			sin(theta*DEG_TO_RAD) * radius + y);
	}
	glEnd();

}


void drawTree(void)
{
	glBegin(GL_POLYGON);
	glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
	glVertex2f(0.6f, 0.2f);
	glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
	glVertex2f(0.8f, 0.2f);
	glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
	glVertex2f(0.7f, 0.35f);
	glEnd();

	glBegin(GL_POLYGON);
	glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
	glVertex2f(0.68f, 0.1f);
	glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
	glVertex2f(0.72f, 0.1f);
	glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
	glVertex2f(0.72f, 0.2f);
	glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
	glVertex2f(0.68f, 0.2f);
	glEnd();
}

void drawLine(void)
{
	glBegin(GL_LINES);
	glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
	glVertex2f(0.0f, 0.9f);
	glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
	glVertex2f(1.0f, 0.9f);
	glEnd();
}

// draw the line and bulb
void drawLine_Bulb(float line_x, float line_y, float line_Btm_x, float line_Btm_y)
{
	glBegin(GL_LINES);
	glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
	glVertex2f(line_x, line_y);
	glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
	if (button_Move <= 0.1f)
	{
		glVertex2f((float)cos(DEG_TO_RAD * (line_Angle + 270)) * 0.1f + line_x, (float)sin(DEG_TO_RAD * (line_Angle + 270)) * 0.1f + line_y);
	}
	else
	{
		glVertex2f(line_Btm_x, line_Btm_y);
	}
	glEnd();

	glBegin(GL_TRIANGLE_FAN);
	if (button_Move <= 0.1f)
	{
		glColor4f(1.0, 1.0, 0.0, 1.0);
		glVertex2f((float)cos(DEG_TO_RAD * (line_Angle + 270)) * 0.1f + line_x, (float)sin(DEG_TO_RAD * (line_Angle + 270)) * 0.1f + line_y);
		for (int theta = 0; theta <= 360; theta += 1)
		{
			glColor4f(1.0, 1.0, 0.0, 0.4);
			glVertex2f(cos(theta * DEG_TO_RAD) * 0.01f + ((float)cos(DEG_TO_RAD * (line_Angle + 270)) * 0.1f + line_x),
				sin(theta*DEG_TO_RAD) * 0.01f + ((float)sin(DEG_TO_RAD * (line_Angle + 270)) * 0.1f + line_y));
		}
	}
	else
	{
		glColor4f(1.0, 1.0, 1.0, 1.0);
		glVertex2f(line_Btm_x, line_Btm_y);
		for (int theta = 0; theta <= 360; theta += 1)
		{
			glColor4f(1.0, 1.0, 1.0, 0.4);
			glVertex2f(cos(theta * DEG_TO_RAD) * 0.01f + line_Btm_x,
				sin(theta*DEG_TO_RAD) * 0.01f + line_Btm_y);
		}
	}
	glEnd();
}

void draw_Snow_Arm(void)
{
	glBegin(GL_LINES);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glVertex2f(0.3f, 0.25f);
	glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
	glVertex2f(arm_Ver[1].x, arm_Ver[1].y);
	glEnd();

	glBegin(GL_TRIANGLE_FAN);
	if (arm_Ver[1].x <= 0.4f && arm_Ver[1].y <= 0.35f)
	{
		glColor4f(0.8f, 0.2f, 0.1f, 1.0f);
	}
	glVertex2f(arm_Ver[1].x, arm_Ver[1].y);
	for (int theta = 0; theta <= 360; theta += 1)
	{
		if (arm_Ver[1].x <= 0.4f && arm_Ver[1].y <= 0.35f)
		{
			glColor4f(0.8f, 0.2f, 0.1f, 1.0f);
			glVertex2f(cos(theta * DEG_TO_RAD) * 0.01f + arm_Ver[1].x,
				sin(theta*DEG_TO_RAD) * 0.01f + arm_Ver[1].y);
		}
	}
	glEnd();

	if (arm_Ver[1].x > 0.4f && arm_Ver[1].y > 0.35f)
	{
		glBegin(GL_TRIANGLE_FAN);
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		glVertex2f((float)cos(DEG_TO_RAD * (snowBall_angel + 180)) * 0.15f + 0.55f, (float)sin(DEG_TO_RAD * (snowBall_angel + 180)) * 0.15f + 0.35f);
		for (int theta = 0; theta <= 360; theta += 1)
		{
			glColor4f(0.8, 0.2, 0.1, 1.0);
			glVertex2f(cos(theta * DEG_TO_RAD) * 0.01f + ((float)cos(DEG_TO_RAD * (snowBall_angel + 180)) * 0.15f + 0.55f),
				sin(theta*DEG_TO_RAD) * 0.01f + ((float)sin(DEG_TO_RAD * (snowBall_angel + 180)) * 0.15f + 0.35f));
		}
		glEnd();
	}

}

void draw_Left_Arm(void)
{
	glBegin(GL_LINES);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glVertex2f(0.2f, 0.25f);
	glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
	glVertex2f(left_Arm[1].x, left_Arm[1].y);
	glEnd();

	glBegin(GL_POLYGON);
	glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
	glVertex2f(0.09f, 0.35f);
	glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
	glVertex2f(0.09f, 0.3f);
	glColor4f(0.0f, 0.0f, 0.0f, 0.2f);
	glVertex2f(button_Move, 0.3f);
	glColor4f(0.0f, 0.0f, 0.0f, 0.2f);
	glVertex2f(button_Move, 0.35f);
	glEnd();
}

void drawInformation(float x, float y, void *font, char *string)
{
	glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
	glRasterPos2f(x, y);
	glutBitmapString(font, string);
}

void drawSnow(void)
{
	if (animation == 0)
		repeatParticles = 0;

	if (frameCount % 10 == 0 && animation)
	{
		for (int snowNumber = 0; snowNumber < 5 && repeatParticles < 1000; snowNumber++)
		{
			if (snow_Point[repeatParticles].active == 0)
			{
				snow_Point[repeatParticles].active = 1;
				snow_Point[repeatParticles].location.x = 0.0f + 1.0f*rand() / RAND_MAX * (1.0f - 0.0f); // between 0 to 1
				snow_Point[repeatParticles].location.y = 1.0f;
				totalNumber++;				
			}
			repeatParticles++;
		}
	}

	for (int i = 0; i < 1000; i++)
	{
		if (snow_Point[i].active)
		{
			glPointSize(snow_Point[i].size);
			glColor4f(1.0f, 1.0f, 1.0f, snow_Point[i].alpha);
			glBegin(GL_POINTS);
			glVertex2f(snow_Point[i].location.x, snow_Point[i].location.y);
			glEnd();
		}
	}
}

/******************************************************************************
 * Animation-Specific Functions (Add your own functions at the end of this section)
 ******************************************************************************/

 /*
	 Initialise OpenGL and set up our scene before we begin the render loop.
 */
void init(void)
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);


	gluOrtho2D(0.0f, 1.0f, 0.0f, 1.0f);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_LINE_SMOOTH);

	srand((unsigned)time(NULL));

	point[0].x = 0.15f + 1.0f*rand() / RAND_MAX * (0.4f - 0.15f);
	point[0].y = 0.25f + 1.0f*rand() / RAND_MAX * (0.5f - 0.25f);

	point[1].x = 0.6f + 1.0f*rand() / RAND_MAX * (0.85f - 0.6f);
	point[1].y = 0.1f + 1.0f*rand() / RAND_MAX * (0.25f - 0.1f);

	for (int i = 0; i < 1000; i++)
	{
		snow_Point[i].size = 3.0f + 1.0f*rand() / RAND_MAX * (8.0f - 3.0f);// size between 0.2 to 0.8
		snow_Point[i].alpha = 0.2f + 1.0f*rand() / RAND_MAX * (1.0f - 0.2f); // alpha between 0.4 to 1
		snow_Point[i].active = 0;
	}

	for (int i = 0; i < 1000; i++)
	{
		if (snow_Point[i].size >= 3.0 && snow_Point[i].size <= 4.5)
		{
			snow_Point[i].speed = (float)(0.05 + 1.0*(rand() % RAND_MAX) / RAND_MAX * (0.1 - 0.05));
		}
		else if (snow_Point[i].size > 4.5 && snow_Point[i].size <= 6.5)
		{
			snow_Point[i].speed = (float)(0.1 + 1.0*(rand() % RAND_MAX) / RAND_MAX * (0.2 - 0.1));
		}
		else if (snow_Point[i].size > 6.5 && snow_Point[i].size <= 8.0)
		{
			snow_Point[i].speed = (float)(0.2 + 1.0*(rand() % RAND_MAX) / RAND_MAX * (0.3 - 0.2));
		}
	}

	arm_Ver[1].x = 0.35f;
	arm_Ver[1].y = 0.3f;

	left_Arm[1].x = 0.15f;
	left_Arm[1].y = 0.3f;
}

/*
	Advance our animation by FRAME_TIME milliseconds.

	Note: Our template's GLUT idle() callback calls this once before each new
	frame is drawn, EXCEPT the very first frame drawn after our application
	starts. Any setup required before the first frame is drawn should be placed
	in init().
*/
void think(void)
{
	if (animation)
	{
		for (int i = 0; i < 1000; i++)
		{
			if (snow_Point[i].active)
			{
				snow_Point[i].location.y -= snow_Point[i].speed* FRAME_TIME_SEC;

				if (snow_Point[i].location.y <= 0.02)
				{
					snow_Point[i].size = 3.0f + 1.0f*rand() / RAND_MAX * (8.0f - 3.0f);// size between 3.0 to 8.0
					snow_Point[i].alpha = 0.2f + 1.0f*rand() / RAND_MAX * (1.0f - 0.2f); // alpha between 0.4 to 1
					snow_Point[i].location.x = 0.0f + 1.0f*rand() / RAND_MAX * (1.0f - 0.0f); // between 0 to 1
					snow_Point[i].location.y = 1.0f;
				}
			}
		}
	}
	else
	{
		for (int i = 0; i < 1000; i++)
		{
			if (snow_Point[i].active)
			{
				snow_Point[i].location.y -= snow_Point[i].speed* FRAME_TIME_SEC;

				if (snow_Point[i].location.y <= 0.02)
				{
					snow_Point[i].active = 0;
					totalNumber--;
				}
			}
		}
	}

	//this move for the sky bulb
	if (line_Angle >= 30.0f)
	{
		direction = -1.0f;
	}
	if (line_Angle <= -30.0f)
	{
		direction = 1.0f;
	}

	line_Angle += 0.2f * direction;

	//this is for the right arm move
	if (arm_Ver[1].x <= 0.4f)
	{
		arm_Ver[1].x += extend_Arm;
	}
	if (arm_Ver[1].y <= 0.35f)
	{
		arm_Ver[1].y += extend_Arm;
	}

	//this is the snowball's parabola
	if (snowBall_angel >= -180.0f)
	{
		snowBall_angel -= 0.2f;
	}

	//For left arm movement
	if (left_Arm[1].x >= 0.1f)
	{
		left_Arm[1].x -= 0.0001f;
	}
	if (left_Arm[1].y <= 0.35f)
	{
		left_Arm[1].y += 0.0001f;
	}

	if (button_Move >= 0.1f)
	{
		button_Move -= 0.0001f;
	}
}



/******************************************************************************/




