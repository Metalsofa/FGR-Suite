/* This header file defines some opengl rendering shortcuts */
#pragma once

#ifndef __customgl_h__
#define __customgl_h__

#include "fcolor.h"

//Global variables to help keep track of window size
GLsizei windowHeight;
GLsizei windowWidth;

void setcolor(const fcolor& col) {
	glColor4f(col.getLevel('r'), col.getLevel('g'), col.getLevel('b'), col.getLevel('a'));
}

void drawText(const point& location, const string& text) {
	glRasterPos2f(location.x(), location.y());
	for (unsigned int i = 0; i < text.size(); i++) { //glutBitmapString() https://stackoverflow.com/questions/544079/how-do-i-use-glutbitmapstring-in-c-to-draw-text-to-the-screen
		//Draw each character    
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, text[i]);
	}
}

void glVertexPoint(const point& dot) {
	glVertex2f(dot.x(), dot.y());
}
void glVertexSegment(const segment& seg) {
	glVertexPoint(seg.p1);
	glVertexPoint(seg.p2);
}
void glVertexTriangle(const triangle& tri) {
	glVertexPoint(tri.p1);
	glVertexPoint(tri.p2);
	glVertexPoint(tri.p3);
}
void glVertexFermatPoint(const triangle& tri) {
	glVertexPoint(tri.fermatpoint(0));
}

void drawPoint(const point& dot, float size, bool label) {
	glPointSize(size);
	glBegin(GL_POINTS);
	glVertexPoint(dot);
	glEnd();
	if (label)
		drawText(dot, dot.label());
}

//Like glTranslatef, but it takes a single argument: a point
void glTranslatePoint(point& offset) {
	glTranslatef(offset.x(), offset.y(), 0.0f);
}

//Render a line segment using OpenGL
void drawSegment(const segment& seg, float thickness, bool endpoints, bool labels) {
	glLineWidth(thickness);
	glBegin(GL_LINES);
	glVertexSegment(seg);
	glEnd();
	if (endpoints) {
		drawPoint(seg.p1, thickness * 2.0f, labels);
		drawPoint(seg.p2, thickness * 2.0f, labels);
	}
}


void rendertext(const point& location, const string& text) {
	// set position to text    
	glRasterPos2f(location.x(), location.y());

	for (unsigned int i = 0; i < text.size(); i++) {
		// draw each character    
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, text[i]);
	}
	//glTranslatef(-dot.x(), -dot.y(), 0.0f);
}



void drawTriangle(const triangle& tri, bool filled, bool vertices, float pointsize, bool labels,
	bool fermatPoint) {
	if (filled)
		glBegin(GL_TRIANGLES);
	else
		glBegin(GL_LINE_LOOP);
	glVertexTriangle(tri);
	glEnd();
	if (vertices) {
		glBegin(GL_POINTS);
		glVertexTriangle(tri);
		glEnd();
		drawPoint(tri.p1, pointsize, labels);
		drawPoint(tri.p2, pointsize, labels);
		drawPoint(tri.p3, pointsize, labels);
	}
	if (fermatPoint) {
		drawPoint(tri.fermatpoint(0), pointsize, labels);
	}
}

//bounds.p1 is bottom-left, bounds.p2 is top-right.
void drawXYgrid(segment bounds, unsigned int levels) {
	unsigned int level = 1;
	while (level <= levels) {
		glLineWidth(float(levels) - float(level) + 1.0f);
		float lightness = 1.0f / float(level);
		glColor3f(lightness, lightness, lightness);
		glBegin(GL_LINES);
		for (unsigned int i = 1; i < level; i++) {
			bounds.p1 *= 2.0f; bounds.p1 *= 2.0f;
			bounds.p2 *= 2.0f; bounds.p2 *= 2.0f;
		}
		int xfirst, xlast, yfirst, ylast;
		if (bounds.p1.x() < 0)
			xfirst = int(floor(bounds.p1.x()));
		else
			xfirst = int(ceil(bounds.p1.x()));
		if (bounds.p1.y() < 0)
			yfirst = int(floor(bounds.p1.y()));
		else
			yfirst = int(ceil(bounds.p1.y()));
		if (bounds.p2.x() > 0)
			xlast = int(floor(bounds.p2.x()));
		else
			xlast = int(ceil(bounds.p2.x()));
		if (bounds.p2.y() > 0)
			ylast = int(floor(bounds.p2.y()));
		else
			ylast = int(ceil(bounds.p2.y()));
		for (unsigned int i = 1; i < level; i++) {
			bounds.p1 /= 2.0f; bounds.p1 /= 2.0f;
			bounds.p2 /= 2.0f; bounds.p2 /= 2.0f;
		}
		float lv = float(level);
		for (int i = xfirst; i <= xlast; i++) {
			float it = float(i);
			glVertex2f(it / powf(2, lv - 1), bounds.p1.y()); glVertex2f(it / powf(2, lv - 1), bounds.p2.y());
		}
		for (int i = yfirst; i <= ylast; i++) {
			float it = float(i);
			glVertex2f(bounds.p1.x(), it / powf(2, lv - 1)); glVertex2f(bounds.p2.x(), it / powf(2, lv - 1));
		}
		glEnd();
		level++;
	}
}

//Use OpenGL to draw a point on the screen
void drawPoint(const point &dot) {
	glColor3f(1, 1, 1);
	glTranslatef(dot.x(), dot.y(), 0.0f);
	glutSolidSphere(0.05, 10, 10);
	glTranslatef(-dot.x(), -dot.y(), 0.0f);
	glColor3f(0, 0, 0);
	glTranslatef(dot.x(), dot.y(), 0.0f);
	glutSolidSphere(0.025, 10, 10);
	glTranslatef(-dot.x(), -dot.y(), 0.0f);
}

//Use OpenGL to draw a segment on the screen
void drawsegment(segment &seg) {
	glBegin(GL_LINES);
	glVertex2f(seg.p1.x(), seg.p1.y());
	glVertex2f(seg.p2.x(), seg.p2.y());
	glEnd();
}

/*Always at (0,0) in the matrix, drawn along the x-axis, left bound is like the phase angle.
Resolution represents the number of points used to paint a single crest/trough. Appears clamped by bounds for aesthetic,
but only if passed TRUE for the clamping of that bound.*/
//
//
//metastat colorfromID(int colorID) {
//	switch (colorID) {
//	case 0:
//		return clBlack;
//	case 1:
//		return clRed;
//		break;
//	case 2:
//		return clOrange;
//	case 3:
//		return clYellow;
//	case 4:
//		return clLime;
//	case 5:
//		return clGreen;
//	case 6:
//		return clTeal;
//	case 7:
//		return clCyan;
//	case 8:
//		return clIndigo;
//	case 9:
//		return clBlue;
//	case 10:
//		return clPurple;
//	case 11:
//		return clMagenta;
//	case 12:
//		return clViolet;
//	case 13:
//		return clWhite;
//	}
//	return clBlack;
//}


//Use openGL to render a shape at the origin of the matrix
void drawFGR(const shape &obj) {
	setcolor(obj.color);
	glLineWidth(obj.lineThickness);
	glBegin(obj.mode);
	obj.applyToAll(glVertexPoint);
	glEnd();
}

//Use openGL to render a graphic at the origin of the matrix
void drawFGR(const graphic& obj) {
	obj.applyToAll(drawFGR);
}

//Use openGL to render an animation at the correct frame
void drawFGR(const animation& obj) {
	drawFGR(*(obj.currentframe));
}

//Use openGL to render an animation at the correct frame, and advance it
void animate(animation& obj) {
	drawFGR(obj.feed());
}


//Clears the screen
void ClearScreen() { glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); }


/////////////////////WARNING: MOSTLY BORING STUFF AFTER THIS LINE/////////////////////////

//Callback for when the window changes size
void changeSize(int width, int height) {
	windowHeight = height;
	windowWidth = width;
	//To avoid divide by zero:
	if (height == 0)
		height = 1;
	float ratio = 1.0f * width / height;
	//Use the Projection Matrix
	glMatrixMode(GL_PROJECTION);
	//Reset Matrix
	glLoadIdentity();
	//Multiply the current matrix by one that makes the view orthographic
	glOrtho(0.0, windowWidth, 0.0, windowHeight, -1.0, 1.0);
	//Set viewport to be the entire window
	//glViewport(0, 0, width, height);
	//Get back to the modelview
	glMatrixMode(GL_MODELVIEW);
}

#endif
