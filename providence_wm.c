#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <unistd.h>

#define MAX(a, b) ((a) > (b) ? (a) : (b))

int main(void) {
    Display * dpy;
    XWindowAttributes attr;
    XButtonEvent start;
    XEvent ev;
    KeyCode space_code;

    if (!(dpy = XOpenDisplay(0x0))) return 1;

    space_code = XKeysymToKeycode(dpy, XK_Space);
    
    // Grab Alt+Space for launching xterm
    XGrabKey(dpy, space_code, Mod1Mask, DefaultRootWindow(dpy), True, GrabModeAsync, GrabModeAsync);
    
    // Grab Alt+Left/Right click for Move/Resize
    XGrabButton(dpy, 1, Mod1Mask, DefaultRootWindow(dpy), True, ButtonPressMask|PointerMotionMask, GrabModeAsync, GrabModeAsync, None, None);
    XGrabButton(dpy, 3, Mod1Mask, DefaultRootWindow(dpy), True, ButtonPressMask|PointerMotionMask, GrabModeAsync, GrabModeAsync, None, None);

    for(;;) {
        XNextEvent(dpy, &ev);
        if(ev.type == KeyPress && ev.xkey.keycode == space_code) {
            if (fork() == 0) execlp("xterm", "xterm", NULL);
        } else if(ev.type == ButtonPress && ev.xbutton.subwindow != None) {
            XGetWindowAttributes(dpy, ev.xbutton.subwindow, &attr);
            start = ev.xbutton;
        } else if(ev.type == MotionNotify && start.subwindow != None) {
            int xdiff = ev.xbutton.x_root - start.x_root;
            int ydiff = ev.xbutton.y_root - start.y_root;
            XMoveResizeWindow(dpy, start.subwindow,
                attr.x + (start.button==1 ? xdiff : 0),
                attr.y + (start.button==1 ? ydiff : 0),
                MAX(1, attr.width + (start.button==3 ? xdiff : 0)),
                MAX(1, attr.height + (start.button==3 ? ydiff : 0)));
        }
    }
}
