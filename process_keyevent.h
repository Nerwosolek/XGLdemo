#include<stdbool.h>
#include <X11/Xlib.h>


typedef struct {
	bool q;
	bool left;
	bool right;
	bool up;
	bool down;
} keys_t;

void ProcessKeyEvent(XKeyEvent *ev, keys_t *keys, Display *disp);
