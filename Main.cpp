#include <iostream>
#include <Gl/glew.h>
#include "vgl.h"
#include "vmath.h"
#include "LoadShaders.h"
using namespace std;

/*
* This is an example of drawing a cone as an intro to OpenGL.
*
* Author:  Jerry Heuring
* Date:    October 6, 2014
*
* Bugs:
*
*
* Revisions:
*   8/24/2016 -- Changed to handle call for generateCone.  Removed some
*                "dead" code as well.
*
*
*/
#define BUFFER_OFFSET(x)  ((const void*) (x))

float * generateCone(float baseRadius, float height, float topRadius, int nbrOfSteps, int &nbrOfValues);

GLuint vao[10], vbo[10], transformLocation;

float  rotationAngle;
bool   elements;
int    nbrTriangles;

void timer(int value) {
	rotationAngle += 1.0f;
	if (rotationAngle > 360) {
		rotationAngle = rotationAngle - 360;
	}
	glutPostRedisplay();
	glutTimerFunc(1000 / 30, timer, 1);
}

void init() {

	int nbrOfElements = 0;
	float * vertices = generateCone(0.5, 0.7, 0.1, 10, nbrOfElements);

	glLineWidth(4.0);

	glGenVertexArrays(1, vao);  // generates ID for VAO
	glBindVertexArray(vao[0]);  // Binds it for use
	
	ShaderInfo shaders[] = {
		{ GL_VERTEX_SHADER, "pass.vert" },
		{ GL_FRAGMENT_SHADER, "pass.frag" },
		{ GL_NONE, NULL }
	};
	GLuint program = LoadShaders(shaders);
	glUseProgram(program);

	glGenBuffers(1, vbo);        // Generates ID vor Buffer
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]); // Binds it for use
	glBufferData(GL_ARRAY_BUFFER, nbrOfElements * sizeof(float), vertices, GL_STATIC_DRAW);
	nbrTriangles = nbrOfElements / 9;

	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE,
		0, BUFFER_OFFSET(0));

	GLfloat * colors = new GLfloat[nbrOfElements];

	for (int i = 0; i < nbrOfElements/3; i++) {
		colors[3 * i] = 0.2 + (i / (nbrOfElements / 3.0) * 0.8);
		colors[3 * i + 1] = 0.0;
		colors[3 * i + 2] = 0.0;
	}

	glGenBuffers(1, &vbo[1]);       
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]); 
	glBufferData(GL_ARRAY_BUFFER, nbrOfElements * sizeof(float), colors, GL_STATIC_DRAW);

	GLuint vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 3, GL_FLOAT, GL_FALSE,
		0, BUFFER_OFFSET(0));


	transformLocation = glGetUniformLocation(program, "transform");

	glEnable(GL_DEPTH_TEST);
}

void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	vmath::mat4 currentMatrix = vmath::scale(1.0f) *
		vmath::rotate(rotationAngle, 0.0f, 1.0f, 0.0f);
	glUniformMatrix4fv(transformLocation, 1, GL_TRUE, currentMatrix);
	glBindVertexArray(vao[0]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glDrawArrays(GL_TRIANGLES, 0, nbrTriangles * 3);
	glFlush();
}

int main(int argCount, char *argValues[]) {
	glutInit(&argCount, argValues);

	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutCreateWindow("Cone Test Code");

	glutInitContextVersion(3, 1);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glewInit();
	init();
	glutDisplayFunc(display);
	glutTimerFunc(100 / 30, timer, 1);
	glutMainLoop();
	return 0;
}

float * generateCone(float baseRadius, float height, float topRadius, int nbrOfSteps, int & nbrOfValues)
{
	nbrOfValues = 18 * nbrOfSteps;

	GLfloat *conePoints = new GLfloat[18 * nbrOfSteps];
	int index = 0;

	float angleStep = 2 * 3.1415 / nbrOfSteps;

	// this for loop creates a shape with 2 triangles like the image below
	//     
	//	    /\  /
	//	   /  \/
	//   
	// the coordinates are determined by going in around the bottom and top circles in increments of 2pi/nbrOfSteps

	for (int i = 0; i < nbrOfSteps; i++) {
		// bottom point 1
		conePoints[index] = baseRadius * cos(i * angleStep);
		conePoints[index + 1] = 0;
		conePoints[index + 2] = baseRadius * sin(i * angleStep);

		// bottom point 2
		// both traingles have this point in common so both can be set at the same time
		conePoints[index + 3] = conePoints[index + 9] = baseRadius * cos((i + 1) * angleStep);
		conePoints[index + 4] = conePoints[index + 10] = 0;
		conePoints[index + 5] = conePoints[index + 11] = baseRadius * sin((i + 1) * angleStep);

		// top point 1
		// both triangles have this point in common as well
		conePoints[index + 6] = conePoints[index + 12] = topRadius * cos((i * angleStep) + (angleStep / 2));
		conePoints[index + 7] = conePoints[index + 13] = height;
		conePoints[index + 8] = conePoints[index + 14] = topRadius * sin((i * angleStep) + (angleStep / 2));

		// top point 2
		conePoints[index + 15] = topRadius * cos((i * angleStep) + (3 * (angleStep / 2)));
		conePoints[index + 16] = height;
		conePoints[index + 17] = topRadius * sin((i * angleStep) + (3 * (angleStep / 2)));

		index += 18;
	}

	return conePoints;
}
