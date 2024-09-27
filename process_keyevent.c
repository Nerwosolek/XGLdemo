#include<stdio.h>
#include "process_keyevent.h"

void ProcessKeyEvent(XKeyEvent *ev, keys_t *keys, Display *disp) {
	if (ev->type == KeyRelease) {
		bool fakeRel = false;
		if (XEventsQueued(disp, QueuedAlready)) {
			XEvent nev;
			XPeekEvent(disp, &nev);
			if (nev.xkey.keycode == ev->keycode && nev.type == KeyPress &&
				 nev.xkey.time == ev->time) {
				XNextEvent(disp, &nev);
				
				fakeRel = true;
			}
		}
		
		if(!fakeRel) {
//			printf("KeyRelease: %#x(code) %ld(serial) %ld(time) %i(state)\n",
//				ev->keycode, ev->serial, ev->time, ev->state);
			switch(ev->keycode) {
				case 0x18:
					keys->q = false;
					break;
				case 0x71:
					keys->left = false;
					break;
				case 0x72:
					keys->right = false;
					break;
				case 0x6f:
					keys->up = false;
					break;
				case 0x74:
					keys->down = false;
					break;
			}
		}
	}
	else if (ev->type == KeyPress) {
//		printf("KeyPress: %#x(code) %ld(serial) %ld(time) %i(state)\n",
//				ev->keycode, ev->serial, ev->time, ev->state);
		switch(ev->keycode) {
			case 0x18:
				keys->q = true;
				break;
			case 0x71:
				keys->left = true;
				break;
			case 0x72:
				keys->right = true;
				break;
			case 0x6f:
				keys->up = true;
				break;
			case 0x74:
				keys->down = true;
				break;
			}
	}
}

