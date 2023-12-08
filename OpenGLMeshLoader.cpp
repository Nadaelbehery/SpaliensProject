#include "TextureBuilder.h"
#include "Model_3DS.h"
#include "GLTexture.h"
#include <glut.h>
#include <math.h>
#include <ctime>
#include <stdio.h>
#include <iostream>
#include <sstream>
using namespace std;
int WIDTH = 1280;
int HEIGHT = 720;
int Fuel = 30;

GLuint tex;
GLuint fuel_tex;
GLuint moon_tex;
GLuint jrocket_tex;
GLuint metal_tex;

char title[] = "3D Model Loader Sample";
//DAREENLaserBeam
bool isLaserActive = false;
float laserY = 0.0f;
float laserRotation = 0.0f;
//DAREEN firstPersonCamera
bool firstPerson = false;
// 3D Projection Options
GLdouble fovy = 35.0;
GLdouble aspectRatio = (GLdouble)WIDTH / (GLdouble)HEIGHT;
GLdouble zNear = 0.1;
GLdouble zFar = 10000000;


// Model Variables
Model_3DS model_spacecraft;
Model_3DS model_commet[12];
float commetsPosition[12][2] = { {-650,-100},{40,-900} ,{-100,-123} ,{-460,-400} ,{199,-480} ,{40,-700} ,{220,-330} ,{-20,-800} ,{480,-1000} ,{50,-300} ,{220,-450} ,{-190,-450} };
int commets[12] = { 1,1,1,1,1,1,1,1,1,1,1,1 };

Model_3DS model_alienship[4];
float alienshipsposition[4][2] = { {-90,-70},{70,-400} ,{0,-150} ,{-40,-370} };
int alienships[4] = { 1,1,1,1 };

Model_3DS model_tree;
Model_3DS model_tank;
Model_3DS model_moon;
Model_3DS model_speedBooster;



int score;
int health = 12;
bool GameOver = false;
bool firstEnvironment = true; // in first environemt
bool wonOne = false; //won first game move to second environment
bool wonTwo = false;// won second game move to next scene
int playerSpeed = 10;
int boosterX = 400;
int boosterY = 0;
int boosterZ = -400;
int tankX = 0;
int tankY = -5;
int tankZ = -200;
float playerX = 0;
float playerY = 0;
float playerZ = 0;

enum GameState { PLAYING, GAME_OVER_WIN, GAME_OVER_LOSE };
GameState gameState = PLAYING;
// Textures
GLTexture tex_ground;



#define GLUT_KEY_ESCAPE 27
#define DEG2RAD(a) (a * 0.0174532925)

class Vector3f {
public:
	float x, y, z;

	Vector3f(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f) {
		x = _x;
		y = _y;
		z = _z;
	}

	Vector3f operator+(Vector3f& v) {
		return Vector3f(x + v.x, y + v.y, z + v.z);
	}

	Vector3f operator-(Vector3f& v) {
		return Vector3f(x - v.x, y - v.y, z - v.z);
	}

	Vector3f operator*(float n) {
		return Vector3f(x * n, y * n, z * n);
	}

	Vector3f operator/(float n) {
		return Vector3f(x / n, y / n, z / n);
	}

	Vector3f unit() {
		return *this / sqrt(x * x + y * y + z * z);
	}

	Vector3f cross(Vector3f v) {
		return Vector3f(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
	}
};

class Camera {
public:
	Vector3f eye, center, up;


	void setTopView() {
		eye = Vector3f(0.0f, 5.0f, 0.0f);
		center = Vector3f(0.0f, 0.0f, 0.0f);
		up = Vector3f(0.0f, 0.0f, -1.0f);

	}

	void setFrontView() {
		if (firstPerson) {
			eye = Vector3f(playerX, playerY, 20-playerZ);
			center = Vector3f(0.0f, 0.0f, 0.0f);
			up = Vector3f(0.0f, 1.0f, 0.0f);
		}
		else {
			eye = Vector3f(0.0f, 15.0f, 60.0f);
			center = Vector3f(0.0f, 0.0f, 0.0f);
			up = Vector3f(0.0f, 1.0f, 0.0f);
		}
	}

	void setSideView() {
		eye = Vector3f(2.0f, 2.0f, 0.0f);
		center = Vector3f(0.0f, 0.0f, 0.0f);
		up = Vector3f(0.0f, 1.0f, 0.0f);
	}

	void rotateTopView90() {
		// Rotate the top view by 90 degrees about the z-axis
		float angle = 90.0f;
		float radianAngle = DEG2RAD(angle);

		Vector3f view = (center - eye).unit();
		Vector3f right = up.cross(view).unit();

		view = view * cos(radianAngle) + right * sin(radianAngle);
		right = view.cross(up);
		up = right * sin(-radianAngle) + up * cos(-radianAngle);

		center = eye + view;
	}

	Camera(float eyeX = 1.0f, float eyeY = 1.0f, float eyeZ = 1.0f, float centerX = 0.0f, float centerY = 0.0f, float centerZ = 0.0f, float upX = 0.0f, float upY = 1.0f, float upZ = 0.0f) {
		eye = Vector3f(eyeX, eyeY, eyeZ);
		center = Vector3f(centerX, centerY, centerZ);
		up = Vector3f(upX, upY, upZ);
	}

	void moveX(float d) {
		Vector3f right = up.cross(center - eye).unit();
		eye = eye + right * d;
		center = center + right * d;
	}

	void moveY(float d) {
		eye = eye + up.unit() * d;
		center = center + up.unit() * d;
	}

	void moveZ(float d) {
		Vector3f view = (center - eye).unit();
		eye = eye + view * d;
		center = center + view * d;
	}

	void rotateX(float a) {
		Vector3f view = (center - eye).unit();
		Vector3f right = up.cross(view).unit();
		view = view * cos(DEG2RAD(a)) + up * sin(DEG2RAD(a));
		up = view.cross(right);
		center = eye + view;
	}

	void rotateY(float a) {
		Vector3f view = (center - eye).unit();
		Vector3f right = up.cross(view).unit();
		view = view * cos(DEG2RAD(a)) + right * sin(DEG2RAD(a));
		right = view.cross(up);
		center = eye + view;
	}

	void look() {
		gluLookAt(
			eye.x, eye.y, eye.z,
			center.x, center.y, center.z,
			up.x, up.y, up.z
		);
	}
};

Camera camera;

void InitMaterial()
{
	// Enable Material Tracking
	glEnable(GL_COLOR_MATERIAL);

	// Sich will be assigneet Material Properties whd by glColor
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	// Set Material's Specular Color
	// Will be applied to all objects
	GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);

	// Set Material's Shine value (0->128)
	GLfloat shininess[] = { 96.0f };
	glMaterialfv(GL_FRONT, GL_SHININESS, shininess);
}

void print(float x, float y, float z, float r, float g, float b, char* string)
{
	glPushMatrix();
	glColor3f(r, g, b);
	int len, i;

	//set the position of the text in the window using the x and y coordinates
	glRasterPos3f(x, y, z);

	//get the length of the string to display
	len = (int)strlen(string);
	//loop to display character by character
	for (i = 0; i < len; i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, string[i]);
	}
	glPopMatrix();
	glColor3f(1, 1, 1);

}

void drawWall(double thickness) {
	glPushMatrix();
	glTranslated(0.5, 0.5 * thickness, 0.5);
	glScaled(1.0, thickness, 1.0);
	glutSolidCube(1);
	glPopMatrix();
}
void drawTableLeg(double thick, double len) {
	glPushMatrix();
	glTranslated(0, len / 2, 0);
	glScaled(thick, len, thick);
	glutSolidCube(1.0);
	glPopMatrix();
}
void drawJackPart() {
	glPushMatrix();
	glScaled(0.2, 0.2, 1.0);
	glutSolidSphere(1, 15, 15);
	glPopMatrix();
	glPushMatrix();
	glTranslated(0, 0, 1.2);
	glutSolidSphere(0.2, 15, 15);
	glTranslated(0, 0, -2.4);
	glutSolidSphere(0.2, 15, 15);
	glPopMatrix();
}
void drawJack() {
	glPushMatrix();
	drawJackPart();
	glRotated(90.0, 0, 1, 0);
	drawJackPart();
	glRotated(90.0, 1, 0, 0);
	drawJackPart();
	glPopMatrix();
}
void drawExplosion(float explosionX, float explosionY, float explosionZ) {
	const int numSpheres = 12; // Number of spheres
	const float spacing = 0.8f; // Spacing between spheres

	float startOffset = -((numSpheres - 1) * spacing) / 2.0f; // Offset to center spheres

	for (int i = 0; i < numSpheres; ++i) {
		glPushMatrix();
		glColor3f(1.0f, 1.0f, 0.0f); // Yellow color
		glTranslatef(explosionX + startOffset + i * spacing, explosionY, explosionZ); // Translate to the next position
		glutSolidSphere(1.0f, 20, 20); // Render a yellow sphere (radius: 0.3)
		glPopMatrix();
	}
}
void drawLaser() {
	glPushMatrix();
	glColor3f(1.0f, 0.0f, 0.0f); // Red color for the laser

	// Rotate the laser beam
	glTranslatef(playerX, laserY, 0.0f);
	glRotatef(laserRotation, 0.0f, 0.0f, 1.0f);

	// Draw three parallel lines with increased thickness
	glLineWidth(3.0f); // Set the line width to make it thicker

	glBegin(GL_LINE_STRIP);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 1.0f, 0.0f); // First line
	glEnd();

	glBegin(GL_LINE_STRIP);
	glVertex3f(0.05f, 0.0f, 0.0f);
	glVertex3f(0.05f, 1.0f, 0.0f); // Second line
	glEnd();

	glBegin(GL_LINE_STRIP);
	glVertex3f(-0.05f, 0.0f, 0.0f);
	glVertex3f(-0.05f, 1.0f, 0.0f); // Third line
	glEnd();

	glLineWidth(1.0f); // Reset the line width to the default value

	glPopMatrix();
}
void LoadAssets()
{
	// Loading Model files
	model_spacecraft.Load("Models/spacecraft/spacecraft.3DS");
	for (int i = 0; i < 12; i++) {
		model_commet[i].Load("Models/commet/asteroid 3DS.3DS");
	}
	model_tree.Load("Models/tree/Tree1.3ds");
	for (int i = 0; i < 4; i++) {
		model_alienship[i].Load("Models/spacecraft/alienship2.3DS");
	}
	
	//model_moon.Load("Models/MoonZ/moon.3ds");
	model_tank.Load("Models/fuelTank/uploads_files_3640174_jerrycan_1.3ds");
	//model_speedBooster.Load("Models/speedBooster/uploads_files_1914765_Rocket.3ds");
	// Loading texture files
	//tex_ground.Load("Textures/universe.bmp");
	loadBMP(&tex, "Textures/nightSky.bmp", true);
	loadBMP(&moon_tex, "Textures/moon.bmp", true);
	loadBMP(&metal_tex, "Textures/moon.bmp", true);
	/*loadBMP(&fuel_tex, "Textures/fuel.bmp", true);*/
}
void initComets() {
	for (int i = 0; i < 12 && commets[i] == 1; i++) {

		//float xPosition = -100 + (rand() % 1); // Calculate x position based on spacing
		//float zPosition = -100 + (rand() % 1);
		float xPosition = commetsPosition[i][0];
		float zPosition = commetsPosition[i][1];

		//model_commet[i].pos.z = zPosition - model_spacecraft.pos.z;

		//model_commet[i].pos.y = model_spacecraft.pos.y;
		glPushMatrix();
		glTranslatef(xPosition, 0, zPosition);
		//glScalef(0.2, 0.2, 0.2);
		model_commet[i].Draw();
		glPopMatrix();
		model_commet[i].lit = true;


	}
}

void initAlienShips() {
	for (int i = 0; i < 4 && alienships[i] == 1; i++) {

		//float xPosition = -100 + (rand() % 1); // Calculate x position based on spacing
		//float zPosition = -100 + (rand() % 1);
		float xPosition = alienshipsposition[i][0];
		float zPosition = alienshipsposition[i][1];

		//model_commet[i].pos.z = zPosition - model_spacecraft.pos.z;

		//model_commet[i].pos.y = model_spacecraft.pos.y;
		glPushMatrix();
		glTranslatef(xPosition, 0, zPosition);
		glScalef(0.02, 0.02, 0.02);
		glRotatef(90, 1, 0, 0);
		//glScalef(0.2, 0.2, 0.2);
		model_alienship[i].Draw();
		glPopMatrix();
		model_alienship[i].lit = true;
	}
}


void drawAlienShips() {
	int numAlienShips = 4;
	float viewWidth = 780;
	float spacing = viewWidth / numAlienShips;

	for (int i = 0; i < numAlienShips && alienships[i] == 1; i++) {

		glPushMatrix();
		glTranslatef(alienshipsposition[i][0], 0, alienshipsposition[i][1]);
		glScalef(0.02, 0.02, 0.02);
		glRotatef(90, 1, 0, 0);
		model_alienship[i].Draw();
		glPopMatrix();
	}
}



void drawComets() {
	int numComets = 12;
	float viewWidth = 780;
	float spacing = viewWidth / numComets;

	for (int i = 0; i < numComets && commets[i] == 1; i++) {

		glPushMatrix();
		//glScalef(0.2, 0.2, 0.2);
		glTranslatef(commetsPosition[i][0], 0, commetsPosition[i][1]);
		model_commet[i].Draw();
		glPopMatrix();


	}
}
BOOLEAN playerHitComet() {
	cout << "X player" << playerX;
	cout << "\n";

	cout << "Y player " << playerY;
	cout << "\n";

	cout << "Z player " << playerZ;
	cout << "\n";
	for (int i = 0; i < 12 && commets[i] == 1; i++) {
		float posX = commetsPosition[i][0];
		float posY = 0;
		float posZ = commetsPosition[i][1];
		/*cout << "X" << posX;
		cout << "\n";

		cout << "Y" << posY;
		cout << "\n";

		cout << "Z" << posZ;
		cout << "\n";*/




		if (
			(abs(playerZ - posZ) <= 33) && (abs(playerX - posX) <= 25)) {
			cout << "here";
			cout << "CollisionX" << posX;
			cout << "\n";

			cout << "CollisionY" << posY;
			cout << "\n";

			cout << "CollisionZ" << posZ;
			cout << "\n";
			PlaySound(TEXT("cometHit.wav"), NULL, SND_ASYNC);
			health--;
			glPushMatrix();
			drawExplosion(posX, posY, posZ);
			glPopMatrix();
			return true;
			//commets[i] = 0;

		}


	}
	return false;

}
bool isGameOver() {
	if (health == 0) {
		return true;
	}
	return false;
}



bool checkCollision(double playerX, double playerY, double playerZ, double tankX, double tankY, double tankZ) {


	if (playerX == 0 && playerY == 0 && playerZ == -204) {
		return true; // Collision detected
	}

	return false; // No collision
}

void checkPlanetReached() {


	if (playerX == 0 && playerY == 0 && playerZ == -470) {
		if (firstEnvironment) {
		firstEnvironment = false; // Collision detected
		camera.setFrontView();
		playerX = 0;
		playerY = 0;
		playerZ = 0;
	}
		cout << "Planet reached" << firstEnvironment;
	}

}

void drawTank() {

	// Draw tank Model
	glPushMatrix();
	glTranslatef(tankX, tankY, tankZ);
	glScalef(0.2, 0.15, 0.2);
	model_tank.Draw();
	glPopMatrix();

}
void tankCollided() {

	bool collided = checkCollision(playerX, playerY, playerZ, tankX, tankY, tankZ);
	if (collided) {
		tankX = -100000;
		tankY = -100000;
		tankZ = -100000;
		Fuel = 30;
	}
}
//void drawAlienShip() {
//	// Push the matrix to save the current transformation state
//	glPushMatrix();
//
//	// Create a new quadric object
//	GLUquadricObj* qobj = gluNewQuadric();
//
//	// Scale, rotate, and translate the alien ship
//	glScalef(0.03, 0.03, 0.03);
//	glRotated(90, 1, 0, 0);
//	glTranslated(0, 7, 0);
//
//	// Bind the metal texture
//	glBindTexture(GL_TEXTURE_2D, metal_tex);
//
//	// Enable texture coordinate generation
//	gluQuadricTexture(qobj, true);
//
//	// Enable smooth shading
//	gluQuadricNormals(qobj, GL_SMOOTH);
//
//	// Draw the alien ship with the metal texture
//	model_alienship.Draw();
//
//	// Delete the quadric object
//	gluDeleteQuadric(qobj);
//
//	// Pop the matrix to restore the previous transformation state
//	glPopMatrix();
//}

float moonRotationAngle = 0.0f;

void drawMoon() {
	glPushMatrix();

	GLUquadricObj* qobj = gluNewQuadric();

	glTranslated(0, 0, -470);

	glRotated(moonRotationAngle, 0, 1, 0);

	glBindTexture(GL_TEXTURE_2D, moon_tex);

	gluQuadricTexture(qobj, true);

	gluQuadricNormals(qobj, GL_SMOOTH);

	gluSphere(qobj, 10, 50, 50);  

	gluDeleteQuadric(qobj);

	glPopMatrix();
}


void drawBooster() {
	// Draw Booster Model
	glPushMatrix();
	glScalef(0.2, 0.05, 0.2);
	model_speedBooster.Draw();
	glPopMatrix();

}
void boosterCollided() {

	bool collided = checkCollision(playerX, playerY, playerZ, boosterX, boosterY, boosterZ);
	if (collided) {
		boosterX = -100000;
		boosterY = -100000;
		boosterZ = -100000;
		playerSpeed = 50;

	}
}

void fuelDuration() {
	int i = 0;
	for (i; i < 50; i++) {
		Fuel--;
		if (Fuel == 0)
			gameState = GAME_OVER_LOSE;
		break;
	}

}


void drawLoseScreen() {

	glClearColor(1, 0, 0, 0);
	print(44, 70, 100, 1, 1, 1, "Game Over!");
	//printScore(45, 65, 100, 1, 1, 1);
	print(40, 60, 100, 1, 1, 1, "Press Enter to Play Again");
}
void drawWinScreen() {

	glClearColor(0, 1, 0, 0);
	print(40, 70, 100, 1, 1, 1, "Congratulations You won !");
	//printScore(46, 65, 100, 1, 1, 1);
	print(40, 60, 100, 1, 1, 1, "Press Enter to Play Again");
}
void drawHealthBar() {
	glPushMatrix();


	// Draw the health bar background
	glColor3f(0.8f, 0.2f, 0.2f); // Red color for background
	glBegin(GL_QUADS);
	glVertex2f(50, 30); // Top-left corner of the health bar
	glVertex2f(50 + health * 10, 30); // Top-right corner (increase width based on health)
	glVertex2f(50 + health * 10, 50); // Bottom-right corner
	glVertex2f(50, 50); // Bottom-left corner
	glEnd();

	// Outline for the health bar
	//glColor3f(0.0f, 0.0f, 0.0f); // Black color for outline
	glBegin(GL_LINE_LOOP);
	glVertex2f(50, 30); // Top-left corner of the health bar
	glVertex2f(250, 30); // Top-right corner (fixed width)
	glVertex2f(250, 50); // Bottom-right corner (fixed width)
	glVertex2f(50, 50); // Bottom-left corner
	glEnd();

	glPopMatrix();
}


void gameScreen() {
	switch (gameState) {
	case PLAYING://normal
		break;
	case GAME_OVER_WIN:
		drawWinScreen();
		break;
	case GAME_OVER_LOSE:
		drawLoseScreen();
		break;
	}

}

void setupLights() {
	GLfloat ambient[] = { 0.7f, 0.7f, 0.7, 1.0f };
	GLfloat diffuse[] = { 0.6f, 0.6f, 0.6, 1.0f };
	GLfloat specular[] = { 1.0f, 1.0f, 1.0, 1.0f };
	GLfloat shininess[] = { 50 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, shininess);

	GLfloat lightIntensity[] = { 0.7f, 0.7f, 1, 1.0f };
	GLfloat lightPosition[] = { -7.0f, 6.0f, 3.0f, 0.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, lightIntensity);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightIntensity);
}
void setupCamera() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glLoadIdentity();
	gluPerspective(fovy, aspectRatio, zNear, zFar);


	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	camera.look();
}


void printHealth(float x, float y, float z, float r, float g, float b) {
	std::ostringstream ss;
	ss << health;

	string h = "Health : " + ss.str();
	char hChar[1024];
	strncpy(hChar, h.c_str(), sizeof(hChar));
	hChar[sizeof(hChar) - 1] = 0;
	print(x, y, z, r, g, b, hChar);

}
void printFuel(float x, float y, float z, float r, float g, float b) {
	std::ostringstream ss;
	ss << Fuel;

	string h = "Fuel : " + ss.str();
	char hChar[1024];
	strncpy(hChar, h.c_str(), sizeof(hChar));
	hChar[sizeof(hChar) - 1] = 0;
	print(x, y, z, r, g, b, hChar);

}
void timer(int value) {
	// Redraw the scene
	glutPostRedisplay();

	// Restart the timer
	int timerInterval = 1000; // 1000 milliseconds = 1 second
	glutTimerFunc(timerInterval, timer, 0);
}


void Display() {
	setupCamera();
	setupLights();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if (firstEnvironment) {
		glPushMatrix();
		glScalef(-1, 2.0, -1);
		glRotatef(90, 1, 0, 0);
		glutSolidCube(1.0);
		glPopMatrix();

	   glPushMatrix();
       drawTank();
       glPopMatrix();
       glPushMatrix();
      //drawBooster();
        glPopMatrix();
       tankCollided();
     //boosterCollided();
		 fuelDuration();
		printFuel(-40, 20, 0, 1, 0, 0);
      gameScreen();
		moonRotationAngle += 1.5f;
		
		
	


		// Draw spacecraft Model
		glPushMatrix();
		glTranslatef(playerX, playerY, playerZ);
		glScalef(0.1, 0.1, 0.1);
		model_spacecraft.Draw();
		glPopMatrix();
		glPushMatrix();
		drawComets();
		glPopMatrix();



		//sky box
		glPushMatrix();

		GLUquadricObj* qobj;
		qobj = gluNewQuadric();
		glTranslated(50, 0, 0);
		glRotated(90, 1, 0, 1);
		glBindTexture(GL_TEXTURE_2D, tex);
		gluQuadricTexture(qobj, true);
		gluQuadricNormals(qobj, GL_SMOOTH);
		gluSphere(qobj, 1000, 100, 1000);
		gluDeleteQuadric(qobj);

		glPopMatrix();
		glPushMatrix();
		drawMoon();
		glPopMatrix();

		//drawlaserBeam
		//
		//glPushMatrix();
		//drawLaser();
		//glPopMatrix();
	}
	else {
		//second environmet
			//sky box
		
		glPushMatrix();

		GLUquadricObj* qobj;
		qobj = gluNewQuadric();
		glTranslated(50, 0, 0);
		glRotated(90, 1, 0, 1);
		glBindTexture(GL_TEXTURE_2D, tex);
		gluQuadricTexture(qobj, true);
		gluQuadricNormals(qobj, GL_SMOOTH);
		gluSphere(qobj, 1000, 100, 1000);
		gluDeleteQuadric(qobj);


		glPopMatrix();
		glPushMatrix();
		drawAlienShips();
		glPopMatrix();


		// Draw spacecraft Model
		glPushMatrix();
		glTranslatef(playerX, playerY, playerZ);
		glScalef(0.1, 0.1, 0.1);
		model_spacecraft.Draw();
		glPopMatrix();

	}
	if (isGameOver()) {
		//display game over won/lose
	}
	glFlush();
	//glutSwapBuffers();
}

bool rotateLeft = false;
bool rotateRight = false;
void Keyboard(unsigned char key, int x, int y) {
	float d = 0.50;

	switch (key) {
	case '5':
		camera.moveY(d);
		break;
	case '2':
		camera.moveY(-d);
		break;
	case '1':
		camera.moveX(d);
		break;
	case '3':
		camera.moveX(-d);
		break;
	case '6':
		camera.moveZ(d);
		break;
	case '4':
		camera.moveZ(-d);
		break;
		//case 'd':
		//	movePlayer(0.0, 0, d);
		//	break;
		//case 'a':
		//	movePlayer(0.0, 0, -d);
		//	break;
		//case 's':
		//	movePlayer(-d, 0.0, 0);
		//	break;
		//case 'w':
		//	movePlayer(d, 0.0, 0);
		//	break;
			//break;
	case '7':
		camera.setTopView();
		break;
	case '8':
		camera.setFrontView();
		break;
	case '9':
		camera.setSideView();
		break;
	case 'w':
		checkPlanetReached();
		
		if (!playerHitComet()) {
			if (firstPerson) {
				camera.moveZ( d/2);//needs to be adjusted based on player speed
				//model_spacecraft.pos.z = model_spacecraft.pos.z - playerSpeed*0.1;

				//playerZ = playerZ - 1;
			}
			else{
				camera.moveZ(2 * d);//needs to be adjusted based on player speed
			camera.moveY(d / 2);
		}
			//model_spacecraft.pos.z = model_spacecraft.pos.z - playerSpeed*0.1;

			playerZ = playerZ - 1;
		}
		//model_spacecraft.pos.z = model_spacecraft.pos.z - playerSpeed;
		//playerHitComet();
		break;


	case 'a':
		camera.moveX(2 * d);

		if (!rotateLeft) {
			model_spacecraft.rot.z = model_spacecraft.rot.z + 15.0f;
			rotateLeft = true; // Set the flag to true to indicate rotation occurred
		}
		//model_spacecraft.pos.x = model_spacecraft.pos.x - playerSpeed;
		playerX = playerX - 1;

		break;
	case 's':
		camera.moveZ(-2 * d);
		camera.moveY(-d / 2);
		playerZ = playerZ + 1;

		//model_spacecraft.pos.z = model_spacecraft.pos.z + playerSpeed;
		break;
	case 'd':
		camera.moveX(-2 * d);

		if (!rotateRight) {
			model_spacecraft.rot.z = model_spacecraft.rot.z - 15.0f;
			rotateRight = true; // Set the flag to true to indicate rotation occurred
		}
		//model_spacecraft.pos.x = model_spacecraft.pos.x + playerSpeed;
		playerX = playerX + 1;

		break;
	case 'e':
		model_spacecraft.rot.z = model_spacecraft.rot.z + 15.0f;
		break;
	case 'k':
		// Toggle the laser when the 'k' key is pressed
		isLaserActive = !isLaserActive;
		if (isLaserActive) {
			laserY = playerY + 0.1f; // Adjust the laser starting position
			// Increase the score when firing the laser
			score += 10;
		}
		break;
	case 'l':
		firstPerson = true;
		camera.eye=Vector3f(playerX, playerY,  playerZ-20);
		//camera.setFrontView();
		break;
	case 'm':
		firstPerson = false;
		camera.eye = Vector3f(playerX, playerY+15, playerZ+60);

		//camera.setFrontView();
		break;
	case GLUT_KEY_ESCAPE:
		exit(EXIT_SUCCESS);
	}

	glutPostRedisplay();
}
void update(int value) {
	// Update the rotation angle for the laser beam
	laserRotation += 5.0f;
	if (laserRotation > 360.0f) {
		laserRotation -= 360.0f;
	}

	glutPostRedisplay();
	glutTimerFunc(30, update, 0); // Set the timer to call the update function every 30 milliseconds
}

void KeyUp(unsigned char key, int x, int y) {
	switch (key) {
	case 'a':
		rotateLeft = false;
		// Reset the rotation when 'a' key is released
		model_spacecraft.rot.z = 0.0f;
		break;
	case 'd':
		rotateRight = false;
		// Reset the rotation when 'd' key is released
		model_spacecraft.rot.z = 0.0f;
		break;
	}
	glutPostRedisplay();
}


void Special(int key, int x, int y) {
	float a = 1.0;

	switch (key) {
	case GLUT_KEY_UP:
		camera.rotateX(a);
		break;
	case GLUT_KEY_DOWN:
		camera.rotateX(-a);
		break;
	case GLUT_KEY_LEFT:
		camera.rotateY(a);
		break;
	case GLUT_KEY_RIGHT:
		camera.rotateY(-a);
		break;
	}

	glutPostRedisplay();
}
void init() {

	initComets();
	camera.setFrontView();





}

void main(int argc, char** argv) {
	glutInit(&argc, argv);

	glutInitWindowSize(640, 480);
	glutInitWindowPosition(50, 50);

	glutCreateWindow("Spaliens");
	glutDisplayFunc(Display);
	glutKeyboardFunc(Keyboard);
	glutKeyboardUpFunc(KeyUp);  // Register KeyUp function
	glutSpecialFunc(Special);
	srand(time(NULL));
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);

	glShadeModel(GL_SMOOTH);
	int timerInterval = 1000; // 1000 milliseconds = 1 second
	glutTimerFunc(timerInterval, timer, 0);

	LoadAssets();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);

	glShadeModel(GL_SMOOTH);

	init();
	glutMainLoop();
}