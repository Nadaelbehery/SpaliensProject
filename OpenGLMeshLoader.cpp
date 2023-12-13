#include "TextureBuilder.h"
#include "Model_3DS.h"
#include "GLTexture.h"
#include <glut.h>
#include <math.h>
#include <ctime>
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <cmath>
using namespace std;
int WIDTH = 1280;
int HEIGHT = 720;
int Fuel = 1000;

GLuint tex;
GLuint two_tex;
GLuint fuel_tex;
GLuint moon_tex;
GLuint jrocket_tex;
GLuint metal_tex;

char title[] = "3D Model Loader Sample";
//DAREENLaserBeam
bool isLaserActive = false;
float laserY = 0.0f;
float laserRotation = 0.0f;
bool alienCollision_1 = false;
bool alienCollision_2 = false;
bool alienCollision_3 = false;
bool alienCollision_4 = false;
//DAREEN 2ND EVIRO LEVEL
int countLevel = 0;

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
float commetsPosition[12][2] = { {-65,-100},{50,-800} ,{-120,-123} ,{-460,-400} ,{199,-480} ,{70,-700} ,{100,-330} ,{-70,-800} ,{108,-1000} ,{50,-300} ,{-22,-450} ,{99,-450} };
int commets[12] = { 1,1,1,1,1,1,1,1,1,1,1,1 };

Model_3DS model_alienship[4];
//, { 0,-150 }
float alienshipsposition[3][2] = { {40,-90},{50,-500} ,{30,-370} };
int alienships[4] = { 1,1,1,1 };


Model_3DS model_coin[4];
float coinsposition[4][2] = { {-100,-70},{-50,-300} ,{0,-180} ,{50,-170} };
int coins[4] = { 1,1,1,1 };
int flag;
Model_3DS model_tank[8];
float tanksposition[8][2] = { {-100,-70},{-50,-300} ,{0,-180} ,{50,-170},{-37,-420},{33,-650},{80,-170},{0,-700} };
int tanks[8] = { 1,1,1,1,1,1,1,1 };

Model_3DS model_tree;
Model_3DS model_moon;
Model_3DS model_speedBooster;
Model_3DS shootingStar;

int shootingX = -90;
//int shootingZ;


int score;
int health = 12;
bool GameOver = false;
bool firstEnvironment = true; // in first environemt
bool wonOne = false; //won first game move to second environment
bool wonTwo = false;// won second game move to next scene
bool lost = false; //won first game move to second environment

int playerSpeed = 10;
int coinX;
int coinY;
int coinZ;
int tankX;
int tankY;
int tankZ;
float playerX = 0;
float playerY = 0;
float playerZ = 0;
bool isCollision = false;


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

			eye = Vector3f(playerX, playerY, playerZ + 5);
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


void drawExplosion() {
	const int numSpheres = 30; // Number of spheres
	const float spacing = 0.8f; // Spacing between spheres

	float startOffset = -((numSpheres - 1) * spacing) / 2.0f; // Offset to center spheres

	for (int i = 0; i < numSpheres; ++i) {
		glPushMatrix();
		glColor3f(1.0f, 1.0f, 0.0f); // Yellow color
		glScalef(1.0, 1.0, 1.0);
		glTranslatef(playerX + i, playerY, playerZ - 30); // Translate to the next position
		glutSolidSphere(33.0, 20, 20); // Render a yellow sphere (radius: 0.3)
		glPopMatrix();
	}
}

void drawLaser() {
	glPushMatrix();
	glColor3f(1, 0, 0); // Red color for the laser
	// Rotate the laser beam
	glTranslatef(playerX, playerY, playerZ - 20);
	cout << "LASERX" << playerX;
	cout << "\n";
	cout << "LASERY" << playerY;
	cout << "\n";
	cout << "LASERZ" << playerZ - 20;
	cout << "\n";
	glRotated(-5, 0, 0, 1); // Rotation around the Z-axis to keep it in the XY plane
	glRotated(-90, 0, 1, 0); // Rotation around the Z-axis to keep it in the XY plane
	float laserLength = 10.0f;
	float laserWidth = 20.0f;

	glBegin(GL_QUADS);
	glVertex3f(-laserWidth / 2, 0, 0); // Bottom left vertex
	glVertex3f(laserWidth / 2, 0, 0); // Bottom right vertex
	glVertex3f(laserWidth / 2, laserLength, 0); // Top right vertex
	glVertex3f(-laserWidth / 2, laserLength, 0); // Top left vertex
	glEnd();
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
		model_alienship[i].Load("Models/spacecraft/COMBAT-ship_01.3DS");
	}
	for (int i = 0; i < 4; i++) {
		model_coin[i].Load("Models/coin/Coin 2.3DS");
	}

	for (int i = 0; i < 8; i++) {
		model_tank[i].Load("Models/fuelTank/gasContain.3DS");
	}
	shootingStar.Load("Models/commet/asteroid 3DS.3DS");

	//model_moon.Load("Models/MoonZ/moon.3ds");
	//model_tank.Load("Models/fuelTank/uploads_files_3640174_jerrycan_1.3ds");
	//model_speedBooster.Load("Models/speedBooster/uploads_files_1914765_Rocket.3ds");
	// Loading texture files
	//tex_ground.Load("Textures/universe.bmp");
	loadBMP(&two_tex, "Textures/universe.bmp", true);
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
	int numAlienShips = 3;
	float viewWidth = 780;
	float spacing = viewWidth / numAlienShips;

	for (int i = 0; i < numAlienShips; i++) {
		if (alienships[i]) {
			glPushMatrix();
			glTranslatef(alienshipsposition[i][0], 0, alienshipsposition[i][1]);
			glScalef(0.05, 0.05, 0.05);
			glRotatef(90, 1, 0, 0);
			model_alienship[i].Draw();
			glPopMatrix();
		}
	}
}


void initCoins() {
	for (int i = 0; i < 4 && coins[i] == 1; i++) {

		float xPosition = coinsposition[i][0];
		float zPosition = coinsposition[i][1];

		glPushMatrix();
		glTranslatef(xPosition, 0, zPosition);
		glScalef(0.02, 0.02, 0.02);
		glRotatef(90, 1, 0, 0);
		model_coin[i].Draw();
		glPopMatrix();
		model_coin[i].lit = true;
	}
}


void drawCoins() {
	int numCoins = 4;
	float viewWidth = 780;
	float spacing = viewWidth / numCoins;

	for (int i = 0; i < numCoins && coins[i] == 1; i++) {

		glPushMatrix();
		glTranslatef(coinsposition[i][0], -2, coinsposition[i][1]);
		glScalef(0.6, 0.6, 0.6);
		glRotatef(90, 1, 0, 0);
		model_coin[i].Draw();
		glPopMatrix();
	}
}

void initTanks() {
	for (int i = 0; i < 8 && tanks[i] == 1; i++) {


		tankX = tanksposition[i][0];
		tankZ = tanksposition[i][1];
		glPushMatrix();
		glTranslatef(tankX, 0, tankZ);
		glScalef(0.02, 0.02, 0.02);
		glRotatef(90, 1, 0, 0);
		//glScalef(0.2, 0.2, 0.2);
		model_tank[i].Draw();
		glPopMatrix();
		model_tank[i].lit = true;
	}
}


void drawTanks() {
	int numTanks = 8;
	float viewWidth = 780;
	float spacing = viewWidth / numTanks;


	for (int i = 0; i < numTanks; i++) {
		if (tanks[i] == 1) {
			glPushMatrix();
			glTranslatef(tanksposition[i][0], -2, tanksposition[i][1]);
			glRotatef(90, 1, 0, 0);
			glScalef(0.5, 0.15, 0.2);

			// Only draw the tank if it's visible (tanks[i] == 1)
			model_tank[i].Draw();

			glPopMatrix();
		}
	}
}


//  a function to calculate the distance between two points in 3D space
double distance(double x1, double y1, double z1, double x2, double y2, double z2) {
	return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2) + pow(z2 - z1, 2));
}

void checkTankCollision() {
	// Check collision of player with tanks
	const double playerRadius = 1.0; 

	for (int i = 0; i < 8; ++i) {
		double tankX = tanksposition[i][0];
		double tankY = 0; 
		double tankZ = tanksposition[i][1];

		const double tankRadius = 10.0; 

		
		double dist = distance(playerX, playerY, playerZ, tankX, tankY, tankZ);

		// If the distance is less than the sum of their radii, it's a collision
		if (dist < (playerRadius + tankRadius) && tanks[i] == 1) {
			tanks[i] = 0;
			score += 2;
			Fuel += 350;
			PlaySound(TEXT("fuelTank.wav"), NULL, SND_ASYNC);

			
		}
	}

}

void checkAlienShipCollision() {
	
	const double playerRadiuss = 1.0; 

	for (int i = 0; i < 3; ++i) {
		double alienshipX = alienshipsposition[i][0];
		double alienshipY = 0; 
		double alienshipZ = alienshipsposition[i][1];

		const double alienshipRadius = 10.0; 

		
		double dist = distance(playerX, playerY, playerZ, alienshipX, alienshipY, alienshipZ);

		
		if (dist < (playerRadiuss + alienshipRadius) && alienships[i] == 1) {
			alienships[i] = 0;
			health -= 2;
			playerX -= 40;
			camera.moveX(40);
			PlaySound(TEXT("alienship.wav"), NULL, SND_ASYNC);
		
		}
	}
	
}
bool checkAlienLaserCollision() {
	
	const double laserRadius = 10.0; 
	for (int i = 0; i < 3; ++i) {
		double alienshipX = alienshipsposition[i][0];
		double alienshipY = 0; 
		double alienshipZ = alienshipsposition[i][1];

		cout << "ALIENSHIPX:" << alienshipX;
		cout << "\n";
		cout << "ALIENSHIPY:" << alienshipY;
		cout << "\n";
		cout << "ALIENSHIPZ:" << alienshipZ;
		cout << "\n";

		const double alienshipRadius = 10.0; 

	
		double dist = distance(playerX + 10, playerY, playerZ + 10, alienshipX, alienshipY, alienshipZ);
		cout << "Distance:" << dist;
		cout << "\n";
		// If the distance is less than the sum of their radii, it's a collision
		if (-20 < (alienshipZ - (playerZ - 20)) && alienships[i] == 1) {
			cout << "DAREEEEENNNN";
			alienships[i] = 0;
			countLevel++;
			cout << "LEVEELL:" << countLevel;
			cout << "\n";
			return true;
			
		}
	}
	//GameOver = true;
	//if (countLevel == 0) {
		//lost = true;
	//}
	//if (countLevel == 1 || countLevel == 2|| countLevel == 3) {
		//lost = false;
	//}
	return false;
}



void checkCoinCollision() {
	
	const double playerRadius = 1.0; 

	for (int i = 0; i < 4; ++i) {
		double coinX = coinsposition[i][0];
		double coinY = 0; 
		double coinZ = coinsposition[i][1];

		const double coinRadius = 5.0; 

	
		double dist = distance(playerX, playerY, playerZ, coinX, coinY, coinZ);

		
		if (dist < (playerRadius + coinRadius) && coins[i] == 1) {
			coins[i] = 0;
			score += 2;
			PlaySound(TEXT("SpeedBooster.wav"), NULL, SND_ASYNC);

		
		}
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
	//cout << "X player" << playerX;
	//cout << "\n";

	//cout << "Y player " << playerY;
	//cout << "\n";

	//cout << "Z player " << playerZ;
	//cout << "\n";
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
			health = health - 1;
			if (health == 0) {
				GameOver = true;
				lost = true; //won first game move to second environment

			}

			isCollision = true;
			return true;
			//commets[i] = 0;

		}


	}
	return false;

}




void checkPlanetReached() {


	if ((abs(playerZ + 1000) <= 33) && (abs(playerX - 0) <= 30)) {
		if (firstEnvironment) {
			firstEnvironment = false; // Collision detected
			firstPerson = false;
			PlaySound(TEXT("start.wav"), NULL, SND_ASYNC);
			camera.setFrontView();
			playerX = 0;
			playerY = 0;
			playerZ = 0;
			score = 0;
			health = 12;
		}
		cout << "Planet reached" << firstEnvironment;
	}

}


void checkRechedMapEnd() {


	if ((abs(playerZ + 1200) <= 33)) {
		if (countLevel == 0) {
			GameOver = true;
			lost = true;
		}
		else {
			GameOver = true;
			lost = false;
		}
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

	glTranslated(0, 0, -1000);

	glRotated(moonRotationAngle, 0, 1, 0);

	glBindTexture(GL_TEXTURE_2D, moon_tex);

	gluQuadricTexture(qobj, true);

	gluQuadricNormals(qobj, GL_SMOOTH);

	gluSphere(qobj, 10, 50, 50);

	gluDeleteQuadric(qobj);

	glPopMatrix();
}




void fuelDuration() {
	int i = 0;
	for (i; i < 50; i++) {
		Fuel--;
		if (Fuel <= 0) {
			lost = true;
			GameOver = true;
		}
		break;
	}

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
	;
}
void setMoonLight() {
	glEnable(GL_LIGHT1);
	GLfloat l0Diffuse[] = { 1, 0, 0, 1.0f };
	GLfloat increasedIntensity = 2.0f;
	for (int i = 0; i < 3; ++i) {
		l0Diffuse[i] *= increasedIntensity;
	}

	GLfloat l0Spec[] = { 1.0f, 1.0f, 0.0f, 1.0f };
	GLfloat l0Ambient[] = { 0.0f, 0.0f, 0.0, 1.0f };
	GLfloat l0Position[] = { 0.0f,0, -1000,1 };
	GLfloat l0Direction[] = { 0.0, 1.0, 1.0 };

	glLightfv(GL_LIGHT1, GL_DIFFUSE, l0Diffuse);
	glLightfv(GL_LIGHT1, GL_AMBIENT, l0Ambient);
	glLightfv(GL_LIGHT1, GL_POSITION, l0Position);
	/*glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 30.0);
	glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 90.0);*/
	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, l0Direction);
}
void setupStars() {
	glEnable(GL_LIGHT3);

	GLfloat l0Diffuse[] = { 1, 1, 1, 1.0f };
	GLfloat increasedIntensity = 2.0f;
	for (int i = 0; i < 3; ++i) {
		l0Diffuse[i] *= increasedIntensity;
	}

	GLfloat l0Spec[] = { 1.0f, 1.0f, 0.0f, 1.0f };
	GLfloat l0Ambient[] = { 0.0f, 0.0f, 0.0, 1.0f };
	GLfloat l0Position[] = { shootingX ,20,playerZ - 40,1 };
	GLfloat l0Direction[] = { 0.0, 1.0, 1.0 };

	glLightfv(GL_LIGHT3, GL_DIFFUSE, l0Diffuse);
	glLightfv(GL_LIGHT3, GL_AMBIENT, l0Ambient);
	glLightfv(GL_LIGHT3, GL_POSITION, l0Position);
	/*glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 30.0);
	glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 90.0);*/
	glLightfv(GL_LIGHT3, GL_SPOT_DIRECTION, l0Direction);

}

void setspaceshipLight() {
	glEnable(GL_LIGHT2);
	GLfloat l0Diffuse[] = { 1, 1, 0, 1.0f };
	GLfloat l0Ambient[] = { 0.0f, 0.0f, 0.0, 1.0f };
	GLfloat l0Position[] = { playerX, 0.0f, playerZ - 20, 1 };
	GLfloat l0Direction[] = { 0.0, 0.0, -1.0 };
	GLfloat cutoffAngle = 5.0; // Set the cutoff angle of the spotlight

	glLightfv(GL_LIGHT2, GL_DIFFUSE, l0Diffuse);
	glLightfv(GL_LIGHT2, GL_AMBIENT, l0Ambient);
	glLightfv(GL_LIGHT2, GL_POSITION, l0Position);

	glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, l0Direction);
	glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, cutoffAngle);
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
void printScore(float x, float y, float z, float r, float g, float b) {
	std::ostringstream ss;
	ss << score;

	string h = "Score : " + ss.str();
	char hChar[1024];
	strncpy(hChar, h.c_str(), sizeof(hChar));
	hChar[sizeof(hChar) - 1] = 0;
	print(x, y, z, r, g, b, hChar);

}
void displayHealth(float x, float y, float z, float r, float g, float b) {

	std::ostringstream ss;
	ss << health;

	string h = "Health : " + ss.str();
	char hChar[1024];
	strncpy(hChar, h.c_str(), sizeof(hChar));
	hChar[sizeof(hChar) - 1] = 0;
	print(x, y, z, r, g, b, hChar);


}



void moveForward() {
	checkPlanetReached();
	checkRechedMapEnd();
	if (firstEnvironment) {
		if (!playerHitComet()) {
			if (firstPerson) {
				camera.moveZ(2 * 0.5);//needs to be adjusted based on player speed
				//camera.moveY(d / 4);

				//model_spacecraft.pos.z = model_spacecraft.pos.z - playerSpeed*0.1;

				//playerZ = playerZ - 1;
			}
			else {
				camera.moveZ(2 * 0.5);//needs to be adjusted based on player speed
				camera.moveY(0.5 / 2);
			}
			//model_spacecraft.pos.z = model_spacecraft.pos.z - playerSpeed*0.1;

			playerZ = playerZ - 1;
		}
		else {
			if (firstPerson) {
				camera.moveZ(2 * 0.5);//needs to be adjusted based on player speed
				//camera.moveY(d / 4);

				//model_spacecraft.pos.z = model_spacecraft.pos.z - playerSpeed*0.1;

				//playerZ = playerZ - 1;
			}
			else {
				camera.moveZ(2 * 0.5);//needs to be adjusted based on player speed
				camera.moveY(0.5 / 2);
			}
			camera.moveX(40);
			playerX = playerX - 40;
		}
	}
	else {
		if (firstPerson) {
			camera.moveZ(2 * 0.5);//needs to be adjusted based on player speed
			//camera.moveY(d / 4);

			//model_spacecraft.pos.z = model_spacecraft.pos.z - playerSpeed*0.1;

			//playerZ = playerZ - 1;
		}
		else {
			camera.moveZ(2 * 0.5);//needs to be adjusted based on player speed
			camera.moveY(0.5 / 2);
		}
		//model_spacecraft.pos.z = model_spacecraft.pos.z - playerSpeed*0.1;

		playerZ = playerZ - 1;
	}
}

void timer(int value) {
	moveForward();
	// Redraw the scene
	glutPostRedisplay();

	// Restart the timer
	int timerInterval = 10; // 1000 milliseconds = 1 second
	glutTimerFunc(timerInterval, timer, 0);
}
void renderBitmapString(float x, float y, void* font, const char* string) {
	glRasterPos3f(x, y, -1);

	while (*string) {
		glutBitmapCharacter(font, *string);
		string++;
	}
}


void Display() {
	if (!GameOver) {
		setupCamera();
		setupLights();
		setspaceshipLight();
		setupStars();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		if (firstEnvironment) {
			glDisable(GL_LIGHT3);

			setMoonLight();




			glPushMatrix();
			drawTanks();
			glPopMatrix();
			checkTankCollision();
			fuelDuration();

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
			gluSphere(qobj, 100000, 1000, 1000);
			gluDeleteQuadric(qobj);

			glPopMatrix();
			glPushMatrix();
			drawMoon();
			glPopMatrix();

			glBindTexture(GL_TEXTURE_2D, 0); // prevents the color of the text from being changed

			glPushMatrix();
			glDisable(GL_LIGHTING);


			displayHealth(camera.eye.x - 1, camera.eye.y, camera.eye.z - 2, 1, 0, 0);
			printFuel(camera.eye.x - 1, camera.eye.y - 0.05, camera.eye.z - 2, 1, 0, 0);
			printScore(camera.eye.x - 1, camera.eye.y - 0.1, camera.eye.z - 2, 1, 0, 0);
			glEnable(GL_LIGHTING);
			glPopMatrix();
			/*if (isCollision) {
				glPushMatrix();
				drawExplosion();
				glPopMatrix();
				isCollision = false;

			}*/
		}
		else {
			//second environmet
				//sky box

			glPushMatrix();
			glDisable(GL_LIGHT1);

			GLUquadricObj* qobj;
			qobj = gluNewQuadric();
			glTranslated(50, 0, 0);
			glRotated(90, 1, 0, 1);
			glBindTexture(GL_TEXTURE_2D, two_tex);
			gluQuadricTexture(qobj, true);
			gluQuadricNormals(qobj, GL_SMOOTH);
			gluSphere(qobj, 100000, 100, 1000);
			gluDeleteQuadric(qobj);
			glBindTexture(GL_TEXTURE_2D, 0);

			glPopMatrix();

			glPopMatrix();
			glPushMatrix();
			drawAlienShips();
			glPopMatrix();
			checkAlienShipCollision();
			glPushMatrix();
			drawCoins();
			glPopMatrix();
			checkCoinCollision();

			// Draw spacecraft Model
			glPushMatrix();
			glTranslatef(playerX, playerY, playerZ);
			glScalef(0.1, 0.1, 0.1);
			model_spacecraft.Draw();
			glPopMatrix();
			glPushMatrix();
			if (shootingX < 90) {
				shootingX += 1;
			}
			else {
				shootingX = -90;
			}
			glTranslatef(shootingX, 20, playerZ - 40);
			glScalef(0.1, 0.1, 0.1);
			shootingStar.Draw();
			glPopMatrix();
			if (isLaserActive) {
				//draw Laser Beam 
				glPushMatrix();
				drawLaser();
				PlaySound(TEXT("laser.wav"), NULL, SND_ASYNC);
				if (checkAlienLaserCollision()) {
					glPushMatrix();
					drawExplosion();
					glPopMatrix();
					isCollision = false;
				}
				glPopMatrix();

				isLaserActive = false;
			}

			glPushMatrix();
			glDisable(GL_LIGHTING);
			glBindTexture(GL_TEXTURE_2D, 0);
			displayHealth(camera.eye.x - 1, camera.eye.y, camera.eye.z - 2, 1, 0, 0);
			printScore(camera.eye.x - 1, camera.eye.y - 0.1, camera.eye.z - 2, 1, 0, 0);
			glEnable(GL_LIGHTING);
			glPopMatrix();

		}

	}
	else {
		setupCamera();
		setupLights();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glPushMatrix();
		glDisable(GL_LIGHTING);

		glBindTexture(GL_TEXTURE_2D, 0); // prevents the color of the text from being changed
		//displayHealth(camera.eye.x - 1, camera.eye.y, camera.eye.z - 2, 1, 0, 0);
		string h;
		int count = 0;
		if (lost) {
			h = "Game Over.You lost!";

		}
		else {
			for (int i = 0; i < 3; i++) {
				if (alienships[i] == 0) {
					count++;
				}
			}
			if (count == 1) {
				cout << "YOU PASSED LEVEL 1";
				cout << "\n";
				h = "You passed level 1!";
			}
			if (count == 2) {
				cout << "YOU PASSED LEVEL 2";
				cout << "\n";
				h = "You passed level 2!";

			}
			if (count == 3) {
				cout << "YOU PASSED LEVEL 2";
				cout << "\n";
				h = "You passed level 3!";
			}


		}
		char hChar[1024];
		strncpy(hChar, h.c_str(), sizeof(hChar));
		hChar[sizeof(hChar) - 1] = 0;
		print(camera.eye.x - 1, camera.eye.y - 0.1, camera.eye.z - 2, 1.0, 0.0, 0.0, hChar);
		glPopMatrix();



	}

	glFlush();
	glutSwapBuffers();
}

bool rotateLeft = false;
bool rotateRight = false;

const float boundaryLeft = -100.0f;
const float boundaryRight = 100.0f;

const float forwardSpeed = 0.1f;  // Set your desired forward speed





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
		checkRechedMapEnd();

		if (!playerHitComet()) {
			if (firstPerson) {
				camera.moveZ(2 * d);//needs to be adjusted based on player speed
				//camera.moveY(d / 4);

				//model_spacecraft.pos.z = model_spacecraft.pos.z - playerSpeed*0.1;

				//playerZ = playerZ - 1;
			}
			else {
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
		if (camera.eye.x - 2 * d >= boundaryLeft) {
			camera.moveX(2 * d);

			if (!rotateLeft) {
				model_spacecraft.rot.z = model_spacecraft.rot.z + 15.0f;
				rotateLeft = true; // Set the flag to true to indicate rotation occurred
			}
			playerX = playerX - 1;
		}
		break;
	case 's':
		camera.moveZ(-2 * d);
		camera.moveY(-d / 2);
		playerZ = playerZ + 1;

		//model_spacecraft.pos.z = model_spacecraft.pos.z + playerSpeed;
		break;
	case 'd':
		if (camera.eye.x + 2 * d <= boundaryRight) {
			camera.moveX(-2 * d);

			if (!rotateRight) {
				model_spacecraft.rot.z = model_spacecraft.rot.z - 15.0f;
				rotateRight = true; // Set the flag to true to indicate rotation occurred
			}
			playerX = playerX + 1;
		}
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
		}
		break;
	case 'l':
		firstPerson = true;
		camera.eye = Vector3f(playerX, playerY + 4, playerZ + 5);
		camera.center = Vector3f(playerX, playerY + 4, playerZ - 5);
		//camera.setFrontView();
		break;
	case 'm':
		firstPerson = false;
		camera.eye = Vector3f(playerX, playerY + 15, playerZ + 60);
		camera.center = Vector3f(playerX, playerY, playerZ);

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


void mouseButton(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		// Switch between first and third person camera views
		firstPerson = !firstPerson;

		// Update the camera view based on the new perspective
		if (firstPerson) {
			camera.eye = Vector3f(playerX, playerY + 4, playerZ + 5);
			camera.center = Vector3f(playerX, playerY + 4, playerZ - 5);
		}
		else {
			// Switch to third-person view
			camera.eye = Vector3f(playerX, playerY + 15, playerZ + 60);
			camera.center = Vector3f(playerX, playerY, playerZ);
		}
	}
}



void main(int argc, char** argv) {
	glutInit(&argc, argv);

	glutInitWindowSize(1240, 880);
	glutInitWindowPosition(50, 50);

	glutCreateWindow("Spaliens");
	glutDisplayFunc(Display);
	glutKeyboardFunc(Keyboard);
	glutMouseFunc(mouseButton);

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
