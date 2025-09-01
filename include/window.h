#include <pthread.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Globals
Display *display;
Window main_window, text_window, log_window, input_window;
pthread_t teleport_menu_thread;
GC gc;
XFontStruct *font_info;
bool teleport_menu_active = false;

// Prototypes
void create_main_window();
void create_teleport_menu();
void *teleport_menu_thread_func(void *arg);

// Event Handlers
void handle_expose(XEvent *event);
void handle_keypress(XEvent *event);
void handle_destroy();

/* .... */
Display* dlgdisplay;
Window dlgwindow;
GC dlggc;
XFontStruct* dlgfontInfo;
Window hwndEditBox, hwndListBox;
char szResponse[257];
int wSelection;

const char* listItems[] = {"Item 1", "Item 2", "Item 3", "Item 4"};
int numItems = sizeof(listItems) / sizeof(listItems[0]);
int selectedItemIndex = -1;
