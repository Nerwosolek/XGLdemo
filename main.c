#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<X11/X.h>
#include<X11/Xlib.h>
#include<GL/gl.h>
#include<GL/glx.h>
#include<GL/glu.h>
#include "process_keyevent.h"

Display					*dpy;
Window					root;
GLint att[] =			{GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
XVisualInfo				*vi;
Colormap					cmap;
XSetWindowAttributes	swa;
Window					win;
GLXContext				glc;
XWindowAttributes		gwa;
XEvent					xev;
struct timespec ts_current;
struct timespec ts_last;
keys_t keys;

struct Player {
	GLfloat x;
	GLfloat y;
	GLfloat vx;
	GLfloat vy;
	GLfloat width;
	GLfloat half_w;
	GLfloat velocity;
};
void InitTime() {
	if (clock_gettime(CLOCK_MONOTONIC, &ts_last) == -1) {
		perror("clock_gettime");
		exit(EXIT_FAILURE);
	}
}

void InitPlayer(struct Player *p) {
	p->width = 0.2;
	p->half_w = p->width / 2.0;
	p->velocity = p->width * 3.0;
	p->x = 0.0;
	p->y = 0.0;
	p->vx = 0.0;
	p->vy = 0.0;
}

float delta() {/*
	current_frame = clock();
	float dt = (float)(current_frame - last_frame) / CLOCKS_PER_SEC;
	last_frame = current_frame;
	return dt;
*/
	clock_gettime(CLOCK_MONOTONIC, &ts_current);
	time_t diff_sec = ts_current.tv_sec - ts_last.tv_sec;
	float dt_secs = (float)(ts_current.tv_nsec - ts_last.tv_nsec) / 1000000000L;
	dt_secs += diff_sec;
	ts_last = ts_current;
	return dt_secs;
}

void Update(float dt, struct Player *p) {
	p->x += p->vx;
	p->y += p->vy;
	p->vx = (keys.right - keys.left) * (p->velocity * dt);
	p->vy = (keys.up - keys.down) * (p->velocity * dt);
	printf("dt: %f, [x: %f, y: %f], <vx: %f, vy %f>\n", dt, p->x, p->y, p->vx, p->vy);
}

void DrawAQuad(struct Player *p) {

	glClearColor(0., 0., 0., 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1., 1., -1., 1., 1., 20.);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0., 0., 10., 0., 0., 0., 0., 1., 0.);

	glBegin(GL_QUADS);
		glColor3f(1., 1., 0.); glVertex3f(p->x - p->half_w, 
													 p->y - p->half_w, 0.); // bottom left
		glColor3f(1., 1., 0.); glVertex3f(p->x + p->half_w,
													 p->y - p->half_w, 0.); // bottom right
		glColor3f(1., 0., 1.); glVertex3f(p->x + p->half_w,
													 p->y + p->half_w, 0.); // top right
		glColor3f(1., 0., 1.); glVertex3f(p->x - p->half_w,
													 p->y + p->half_w, 0.); // top left
	glEnd();
}

int main(void) {
	InitTime();
	struct Player pl;
	InitPlayer(&pl);
	dpy = XOpenDisplay(NULL);

	if (dpy == NULL) {
		printf("\n\tcannot connect to X server\n\n");
		exit(0);
	}

	root = DefaultRootWindow(dpy);

	vi = glXChooseVisual(dpy, 0, att);
	if(vi == NULL) {
		printf("\n\tno appropriate visual found\n\n");
		exit(0);
	}
	else {
		printf("\n\tvisual %p selected\n", (void *)vi->visualid);
	}

	cmap = XCreateColormap(dpy, root, vi->visual, AllocNone);

	swa.colormap = cmap;
	swa.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask;

	win = XCreateWindow(dpy, root, 0, 0, 800, 800, 0, vi->depth, InputOutput, vi->visual, CWColormap | CWEventMask, &swa);

	XMapWindow(dpy, win);
	XStoreName(dpy, win, "My OpenGL X Window App!");

	glc = glXCreateContext(dpy, vi, NULL, GL_TRUE);
	glXMakeCurrent(dpy, win, glc);
	glEnable(GL_DEPTH_TEST);
	float dt;	
	while(1) {
		int p;
		dt = delta();
		Update(dt, &pl);
		if ((p = XPending(dpy)) > 0) {
			printf("pending: %i\n", p);
			XNextEvent(dpy, &xev);
		
			if(xev.type == Expose) {
				XGetWindowAttributes(dpy, win, &gwa);
				glViewport(0, 0, gwa.width, gwa.height);
				DrawAQuad(&pl);
				glXSwapBuffers(dpy, win);
			}
			else if(xev.type == KeyPress || xev.type == KeyRelease) {
				printf("{type: %i, code: %x, time: %ld}\n", xev.type, xev.xkey.keycode, xev.xkey.time);
				ProcessKeyEvent(&xev.xkey, &keys, dpy);
				if (keys.q) {
					glXMakeCurrent(dpy, None, NULL);
					glXDestroyContext(dpy, glc);
					XDestroyWindow(dpy, win);
					XCloseDisplay(dpy);
					exit(0);
				}
			}
		}
		XGetWindowAttributes(dpy, win, &gwa);
		glViewport(0, 0, gwa.width, gwa.height);
		DrawAQuad(&pl);
		glXSwapBuffers(dpy, win);
	}
	return 0;
}
