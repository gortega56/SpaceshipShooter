#include <iostream>
#include <gl/glut.h>
#include <math.h>

int width = 800, height = 600;
float theta = 0, distance1 = -600, distance2 = -800, distance3 = -1000, distance4 = -1200;
float shipX = 0, shipThetaZ = 0, shipY = -5, shipThetaX = 0;
float fx = 0.5, fxRate=0.05;
bool* specialKeyStates = new bool[246];
bool* keyStates = new bool[256];

int cameraPos = 0;
float cameraPositions[3][6] = {{0, 0, -200, 0, 0, -600}, {10, -5, -215, 0, -5, -215}, {0, 7, -214, 0, -5, -215}};
float currentCamX = cameraPositions[0][0];
float currentCamY = cameraPositions[0][1];
float currentCamZ = cameraPositions[0][2];
float currentLookX = cameraPositions[0][3];
float currentLookY = cameraPositions[0][4];
float currentLookZ = cameraPositions[0][5];

float vWing1[4][3] = {{1,0,0},{0,0.3,0},{-1,0,0},{0,0,-1}};
int sWing1[4][3] = {{0,1,2},{0,3,1},{3,1,2},{3,2,0}};
float **nWing1;

float vWing2[4][3] = {{1,0,0},{0,-0.2,0},{-1,0,0},{0,0,-1}};
int sWing2[4][3] = {{0,1,2},{0,3,1},{3,1,2},{3,2,0}};
float **nWing2;

float vFin1[4][3] = {{0.4,0,0},{0,1,0.5},{-0.4,0,0},{0,0,-1.2}};
int sFin1[4][3] = {{0,1,2},{0,3,1},{3,1,2},{3,2,0}};
float **nFin1;

float vFin2[4][3] = {{0.4,0,0},{0,-0.3,0.5},{-0.4,0,0},{0,0,-1.2}};
int sFin2[4][3] = {{0,1,2},{0,3,1},{3,1,2},{3,2,0}};
float **nFin2;

void cameraOperations()
{
	if (currentCamX<cameraPositions[cameraPos][0]) currentCamX++;
	else if (currentCamX>cameraPositions[cameraPos][0]) currentCamX--;

	if (currentCamY<cameraPositions[cameraPos][1]) currentCamY++;
	else if (currentCamY>cameraPositions[cameraPos][1]) currentCamY--;

	if (currentCamZ<cameraPositions[cameraPos][2]) currentCamZ++;
	else if (currentCamZ>cameraPositions[cameraPos][2]) currentCamZ--;

	if (currentLookX<cameraPositions[cameraPos][3]) currentLookX++;
	else if (currentLookX>cameraPositions[cameraPos][3]) currentLookX--;

	if (currentLookY<cameraPositions[cameraPos][4]) currentLookY++;
	else if (currentLookY>cameraPositions[cameraPos][4]) currentLookY--;

	if (currentLookZ<cameraPositions[cameraPos][5]) currentLookZ++;
	else if (currentLookZ>cameraPositions[cameraPos][5]) currentLookZ--;
}
void normalize(GLfloat *v)
{
    float len = sqrtf((v[0]*v[0]) + (v[1]*v[1]) + (v[2]*v[2]));
    
    // Avoid division by 0
    if (len == 0.0f)
        len = 1.0f;
    
    // Reduce to unit size
    v[0] /= len;
    v[1] /= len;
    v[2] /= len;
}

GLfloat** getNormalsForAllSides(float v[][3], int sides[][3], int numberOfSides)
{
    GLfloat **normals = new GLfloat*[numberOfSides];    // Store all normals
    for (int i=0;i<numberOfSides;i++) {
        // Get 2 vectors for this side
        GLfloat *vector1 = new GLfloat[3];
        vector1[0] = v[sides[i][0]][0] - v[sides[i][1]][0];
        vector1[1] = v[sides[i][0]][1] - v[sides[i][1]][1];
        vector1[2] = v[sides[i][0]][2] - v[sides[i][1]][2];
    
        GLfloat *vector2 = new GLfloat[3];
        vector2[0] = v[sides[i][1]][0] - v[sides[i][2]][0];
        vector2[1] = v[sides[i][1]][1] - v[sides[i][2]][1];
        vector2[2] = v[sides[i][1]][2] - v[sides[i][2]][2];

        // Cross Product of 2 vectors
        GLfloat *normal = new GLfloat[3];   // Store one normal at a time
        normal[0] = (vector1[1] * vector2[2]) - (vector1[2] * vector2[1]);  // vec1.y*vec2.z - vec1.z*vec2.y
        normal[1] = (vector1[2] * vector2[0]) - (vector1[0] * vector2[2]);  // vec1.z*vec2.x - vec1.x*vec2.z
        normal[2] = (vector1[0] * vector2[1]) - (vector1[1] * vector2[0]);  // vec1.x*vec2.y - vec1.y*vec2.x
    
        normalize(normal);
        
        normals[i] = normal;
    }
    return normals;
}
void stageInit()
{
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glShadeModel(GL_SMOOTH);

    GLfloat light_position[]={ -3,3,-4, 1.0 };
    GLfloat ambient[]={ 1.0, 1.0, 1.0, 1.0 };
    GLfloat diffuse[]={ 1.0, 1.0, 1.0, 1.0 };
    GLfloat specular[]={ 1.0, 1.0, 1.0, 1.0 };
    
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
      
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    glMateriali(GL_FRONT, GL_SHININESS, 96);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    
    // Get normals matrix for each polygon side
    nWing1 = getNormalsForAllSides(vWing1, sWing1, 4);
    nWing2 = getNormalsForAllSides(vWing1, sWing2, 4);
    nFin1 = getNormalsForAllSides(vFin1, sFin1, 4);
    nFin2 = getNormalsForAllSides(vFin2, sFin2, 4);

	// Set all key states = false
	std::fill_n(specialKeyStates,246,false);
	std::fill_n(keyStates,256,false);
}

void drawCloud()
{
	glPushMatrix();
	glTranslated(1,0,-2);
	glutSolidSphere(4,10,10);
	glTranslated(-2,0,-5);
	glutSolidSphere(4,10,10);
	glTranslated(-1,0,3);
	glutSolidSphere(4,10,10);
	glPopMatrix();

}

void drawCloudFormation()
{
	glColor3f(1,1,1);
	glPushMatrix();
	glTranslated(-60,30,-300);
	drawCloud();
	glPopMatrix();

	glPushMatrix();
	glTranslated(15,3,-150);
	drawCloud();
	glPopMatrix();

	glPushMatrix();
	glTranslated(50,30,-200);
	drawCloud();
	glPopMatrix();

	glPushMatrix();
	glTranslated(-15,-15,-250);
	drawCloud();
	glPopMatrix();

	glPushMatrix();
	glTranslated(25,-25,-100);
	drawCloud();
	glPopMatrix();

	glPushMatrix();
	glTranslated(-30,0,-50);
	drawCloud();
	glPopMatrix();
}

void drawFire()
{
    glColor3f(1, 0.27, 0);
    
    float coneBase = 1;
    float coneHeight = 1;
    for (int i=0; i<2; i++) {
        for (float j=0;j<360;j+=45) {
            glPushMatrix();
            glRotated(j, 0, 0, 1);
            glRotated(15, 1, 0, 0);
            glScaled(fx, fx, fx);
            glutSolidCone(coneBase, coneBase, 12, 2);
            glPopMatrix();
            
        }
        coneBase /= 2;
        coneHeight /= 2;
    }
}


void drawSpaceShip()
{
    glBegin(GL_TRIANGLES);
    for (int i=0; i<4; i++) {
        
        // Wing 1
        glColor3f(0.6, 0.2, 0.2);
        glNormal3fv(&nWing1[i][0]);
        glVertex3fv(vWing1[sWing1[i][0]]);
        glVertex3fv(vWing1[sWing1[i][1]]);
        glVertex3fv(vWing1[sWing1[i][2]]);
        
        // Wing 2
        glNormal3fv(&nWing2[i][0]);
        glVertex3fv(vWing2[sWing2[i][0]]);
        glVertex3fv(vWing2[sWing2[i][1]]);
        glVertex3fv(vWing2[sWing2[i][2]]);
        
        // Fin 1
        glColor3f(0.2, 0.2, 0.2);
        glNormal3fv(&nFin1[i][0]);
        glVertex3fv(vFin1[sFin1[i][0]]);
        glVertex3fv(vFin1[sFin1[i][1]]);
        glVertex3fv(vFin1[sFin1[i][2]]);
        
        // Fin 2
        glNormal3fv(&nFin2[i][0]);
        glVertex3fv(vFin2[sFin2[i][0]]);
        glVertex3fv(vFin2[sFin2[i][1]]);
        glVertex3fv(vFin2[sFin2[i][2]]);
    }
    glEnd();
    
    // Boosters
    
    GLUquadric *cylinder = gluNewQuadric();
    gluQuadricDrawStyle(cylinder, GL_FILL);
    gluQuadricNormals(cylinder, GLU_SMOOTH);
    gluQuadricOrientation(cylinder, GLU_OUTSIDE);
    gluQuadricTexture(cylinder, GL_TRUE);
    
    glColor3f(0.2, 0.2, 0.2);
    glPushMatrix();
    glTranslated(0, 0.3, 0);
    gluCylinder(cylinder, 0.17, 0.17, 0.35, 10, 10);
    glPopMatrix();
    
    glPushMatrix();
    glTranslated(0, 0.05, 0);
    gluCylinder(cylinder, 0.1, 0.1, 0.3, 10, 10);
    glPopMatrix();
    
    // Cockpit
    glPushMatrix();
    glTranslated(0, 0.2, -0.63);
    glScaled(0.2, 0.2, 0.2);
    glBegin(GL_TRIANGLES);
    for (int i=0; i<4; i++) {
        glColor3f(0,0,0);
        glNormal3fv(nFin1[sFin1[i][0]]);
        glVertex3fv(vFin1[sFin1[i][0]]);
        glNormal3fv(nFin1[sFin1[i][1]]);
        glVertex3fv(vFin1[sFin1[i][1]]);
        glNormal3fv(nFin1[sFin1[i][2]]);
        glVertex3fv(vFin1[sFin1[i][2]]);
    }
    glEnd();
    glPopMatrix();
 
    glPushMatrix();
    glTranslated(0, 0.1,0.4);
    drawFire();
    glPopMatrix();
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();
	gluLookAt(currentCamX, currentCamY, currentCamZ, 
			  currentLookX, currentLookY, currentLookZ, 0, 1, 0);
	//gluLookAt(10, -5, -215, 0, -5, -215, 0, 1, 0);
	//gluLookAt(0, 7, -214, 0, -5, -215, 0, 1, 0);

	glPushMatrix();
	glTranslated(0,0,distance1);
	drawCloudFormation();
	glPopMatrix();

	glPushMatrix();
	glTranslated(0,0,distance2);
	glScaled(-1,1,1);
	drawCloudFormation();
	glPopMatrix();

	glPushMatrix();
	glTranslated(0,0,distance3);
	glScaled(1,1,-1);
	drawCloudFormation();
	glPopMatrix();

	glPushMatrix();
	glTranslated(0,0,distance4);
	glScaled(-1,1,1);
	glScaled(1,1,-1);
	drawCloudFormation();
	glPopMatrix();

	glPushMatrix();
	glTranslated(shipX,shipY,-205);
	glRotated(shipThetaZ,0,0,1);
	glRotated(shipThetaX,1,0,0);
	drawSpaceShip();
	glPopMatrix();

	glutSwapBuffers();
}

void keyOperations()
{
	if (keyStates['0']) cameraPos=0;
	else if (keyStates['1']) cameraPos=1;
	else if (keyStates['2']) cameraPos=2;
}

void specialKeyOperations()
{
	if (specialKeyStates[GLUT_KEY_RIGHT]) {
		if (shipX<7) shipX+=0.2;
		if (shipThetaZ>-60) shipThetaZ-=2;
	} else
		if (shipThetaZ<0) shipThetaZ+=2;
	if (specialKeyStates[GLUT_KEY_LEFT]) {
		if (shipX>-7) shipX-=0.2;
		if (shipThetaZ<60) shipThetaZ+=2;
	} else 
		if (shipThetaZ>0) shipThetaZ-=2;

	if (specialKeyStates[GLUT_KEY_UP]) {
		if (shipY<5) shipY+=0.2;
		if (shipThetaX<60) shipThetaX+=2;
	} else
		if (shipThetaX>0) shipThetaX-=2;
	if (specialKeyStates[GLUT_KEY_DOWN]) {
		if (shipY>-5) shipY-=0.2;
		if (shipThetaX>-60) shipThetaX-=2;
	} else 
		if (shipThetaX<0) shipThetaX+=2;

}

void idle()
{
	theta += 0.2;
	if (theta == 360) theta = 360;

	// Cloud Formations
	distance1 += 5;
	if (distance1 > 200) distance1 = -600;

	distance2 += 5;
	if (distance2 > 200) distance2 = -800;

	distance3 += 5;
	if (distance3 > 200) distance3 = -1000;

	distance4 += 5;
	if (distance4 > 200) distance4 = -1200;

	// Fire
	fx+=fxRate;
	if (fx>0.5)
        fxRate = -1*fxRate;
    else if (fx<0.2)
        fxRate = -1*fxRate;

	keyOperations();
	specialKeyOperations();
	cameraOperations();

	glutPostRedisplay();
}

void key(unsigned char key, int x, int y)
{
	keyStates[key]=true;
}

void keyUp(unsigned char key, int x, int y)
{
	keyStates[key]=false;
}

void specialKey(int key, int x, int y)
{
	specialKeyStates[key]=true;
}

void specialKeyUp(int key, int x, int y)
{
	specialKeyStates[key]=false;
}


void main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(width, height);
    glutInitWindowPosition(100, 200);
    glutCreateWindow("Space Ship");
    glClearColor(0, 0, 1, 1);
    
	stageInit();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(120, 1, 0.1, 600);
    //glOrtho(-2, 2, -2, 2, 0.1, 200);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    //gluLookAt(0, 0, -200, 0, 0, -600, 0, 1, 0);

    glutDisplayFunc(display);
    glutIdleFunc(idle);
	glutKeyboardFunc(key);
	glutKeyboardUpFunc(keyUp);
	glutSpecialFunc(specialKey);
	glutSpecialUpFunc(specialKeyUp);
    
    glutMainLoop();
}