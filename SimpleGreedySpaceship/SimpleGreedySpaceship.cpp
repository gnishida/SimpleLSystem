#include "SimpleGreedySpaceship.h"
#include <QGLWidget>

SimpleGreedySpaceship::SimpleGreedySpaceship() {
}

void SimpleGreedySpaceship::generate() {

}

void SimpleGreedySpaceship::draw() {
	drawBox(30, 20, 60);
	glTranslatef(0, 0, 60);
	drawCylinder(15, 60);
}

void SimpleGreedySpaceship::drawBox(float width, float height, float depth) {
	glBegin(GL_QUADS);
	glNormal3f(0, 0, -1);
	glVertex3f(-width*0.5f, -height*0.5f, 0);
	glVertex3f(-width*0.5f, height*0.5f, 0);
	glVertex3f(width*0.5f, height*0.5f, 0);
	glVertex3f(width*0.5f, -height*0.5f, 0);
	glNormal3f(-1, 0, 0);
	glVertex3f(-width*0.5f, -height*0.5f, 0);
	glVertex3f(-width*0.5f, -height*0.5f, depth);
	glVertex3f(-width*0.5f, height*0.5f, depth);
	glVertex3f(-width*0.5f, height*0.5f, 0);
	glNormal3f(0, -1, 0);
	glVertex3f(-width*0.5f, -height*0.5f, 0);
	glVertex3f(width*0.5f, -height*0.5f, 0);
	glVertex3f(width*0.5f, -height*0.5f, depth);
	glVertex3f(-width*0.5f, -height*0.5f, depth);
	glNormal3f(0, 0, 1);
	glVertex3f(-width*0.5f, -height*0.5f, depth);
	glVertex3f(width*0.5f, -height*0.5f, depth);
	glVertex3f(width*0.5f, height*0.5f, depth);
	glVertex3f(-width*0.5f, height*0.5f, depth);
	glNormal3f(1, 0, 0);
	glVertex3f(width*0.5f, -height*0.5f, 0);
	glVertex3f(width*0.5f, height*0.5f, 0);
	glVertex3f(width*0.5f, height*0.5f, depth);
	glVertex3f(width*0.5f, -height*0.5f, depth);
	glNormal3f(0, 1, 0);
	glVertex3f(-width*0.5f, height*0.5f, 0);
	glVertex3f(-width*0.5f, height*0.5f, depth);
	glVertex3f(width*0.5f, height*0.5f, depth);
	glVertex3f(width*0.5f, height*0.5f, 0);
	glEnd();
}

void SimpleGreedySpaceship::drawCylinder(float radius, float height, int slices, bool capped) {
	glBegin(GL_QUADS);
	for (int i = 0; i < slices; ++i) {
		float theta1 = 3.1415926535 * 2.0f * i / slices;
		float theta2 = 3.1415926535 * 2.0f * (i + 1) / slices;

		glNormal3f(cosf(theta1), sinf(theta1), 0);
		glVertex3f(cosf(theta1) * radius, sinf(theta1) * radius, 0);
		glNormal3f(cosf(theta2), sinf(theta2), 0);
		glVertex3f(cosf(theta2) * radius, sinf(theta2) * radius, 0);
		glNormal3f(cosf(theta2), sinf(theta2), 0);
		glVertex3f(cosf(theta2) * radius, sinf(theta2) * radius, height);
		glNormal3f(cosf(theta1), sinf(theta1), 0);
		glVertex3f(cosf(theta1) * radius, sinf(theta1) * radius, height);
	}
	glEnd();

	if (capped) {
		glBegin(GL_TRIANGLES);
		for (int i = 0; i < slices; ++i) {
			float theta1 = 3.1415926535 * 2.0f * i / slices;
			float theta2 = 3.1415926535 * 2.0f * (i + 1) / slices;

			glNormal3f(0, 0, -1);
			glVertex3f(0, 0, 0);
			glVertex3f(cosf(theta2) * radius, sinf(theta2) * radius, 0);
			glVertex3f(cosf(theta1) * radius, sinf(theta1) * radius, 0);
			glNormal3f(0, 0, 1);
			glVertex3f(0, 0, height);
			glVertex3f(cosf(theta1) * radius, sinf(theta1) * radius, height);
			glVertex3f(cosf(theta2) * radius, sinf(theta2) * radius, height);
		}
		glEnd();
	}
}
