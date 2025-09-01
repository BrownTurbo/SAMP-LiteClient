#include "main.h"
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <pthread.h>
#include <cstring>

// Child Window/Control IDs
#define IDC_LBLINFO             100
#define IDC_LSTCUSTOM           101
#define IDC_INPUTBOX            102

int main(int argc, char *argv[]) {
    // Initialize the X11 display connection
    display = XOpenDisplay(NULL);
    if (!display) {
        fprintf(stderr, "Unable to open X display.\n");
        return 1;
    }

    create_main_window();

    XEvent event;
    while (1) {
        XNextEvent(display, &event);

        switch (event.type) {
            case Expose:
                handle_expose(&event);
                break;

            case KeyPress:
                handle_keypress(&event);
                break;

            case DestroyNotify:
                handle_destroy();
                break;
        }
    }

    XCloseDisplay(display);
    return 0;
}

void create_main_window() {
    int screen = DefaultScreen(display);
    main_window = XCreateSimpleWindow(display, RootWindow(display, screen),
                                      10, 10, 800, 600, 1,
                                      BlackPixel(display, screen),
                                      WhitePixel(display, screen));

    XSelectInput(display, main_window, ExposureMask | KeyPressMask);
    XMapWindow(display, main_window);
}

void create_teleport_menu() {
    if (teleport_menu_active) {
        teleport_menu_active = False;
        pthread_cancel(teleport_menu_thread);
    }

    teleport_menu_active = True;
    pthread_create(&teleport_menu_thread, NULL, teleport_menu_thread_func, NULL);
}

void *teleport_menu_thread_func(void *arg) {
    int screen = DefaultScreen(display);

    Window teleport_window = XCreateSimpleWindow(display, RootWindow(display, screen),
                                                 200, 200, 400, 300, 1,
                                                 BlackPixel(display, screen),
                                                 WhitePixel(display, screen));

    XSelectInput(display, teleport_window, ExposureMask | KeyPressMask);
    XMapWindow(display, teleport_window);

    XEvent event;
    while (1) {
        XNextEvent(display, &event);
        if (event.xany.window == teleport_window) {
            switch (event.type) {
                case Expose:
                    // Handle teleport menu painting
                    break;

                case KeyPress:
                    // Handle teleport menu keypress
                    teleport_menu_active = False;
                    XDestroyWindow(display, teleport_window);
                    pthread_exit(NULL);
                    break;
            }
        }
    }

    return NULL;
}

void handle_expose(XEvent *event) {
    if (event->xany.window == main_window) {
        GC gc = XCreateGC(display, main_window, 0, NULL);
        XDrawString(display, main_window, gc, 50, 50, "Main Window", strlen("Main Window"));
        XFreeGC(display, gc);
    }
}

void handle_keypress(XEvent *event) {
    if (event->xany.window == main_window) {
        char buf[128];
        KeySym keysym;
        XLookupString(&event->xkey, buf, sizeof(buf), &keysym, NULL);

        if (keysym == XK_Escape) {
            handle_destroy();
        } else if (keysym == XK_T) {
            create_teleport_menu();
        }
    }
}

void handle_destroy() {
    XDestroyWindow(display, main_window);
    XCloseDisplay(display);
    exit(0);
}


/* .... */
void closeDialogBox() {
    // Handle window cleanup
    if (dlgdisplay && dlgwindow) {
        XUnloadFont(dlgdisplay, dlgfontInfo->fid);  // Unload font
        XFreeGC(dlgdisplay, dlggc);  // Free the graphic context
        XDestroyWindow(dlgdisplay, dlgwindow);  // Destroy the window
        XCloseDisplay(dlgdisplay);  // Close the X11 display
    }

    // Exit the thread cleanly
    pthread_exit(NULL);  // Terminates the thread safely
}

void DLGhandleButtonPress(XEvent* event) {
    if (event->xbutton.button == 1) {  // Left mouse click
        if (selectedItemIndex != -1) {
            // Retrieve the selected item based on the selection index
            strncpy(szResponse, listItems[selectedItemIndex], sizeof(szResponse));
            sendDialogResponse(sampDialog.wDialogID, 1, 0, szResponse);
        }
        closeDialogBox();
    }
}

void DLGhandleCreate() {
    // Example of creating buttons and a listbox in X11
    dlgfontInfo = XLoadQueryFont(dlgdisplay, "fixed");
    if (!dlgfontInfo) {
        dlgfontInfo = XLoadQueryFont(dlgdisplay, "helvetica");
    }

    // Set up graphics context
    XGCValues gc_values;
    gc_values.font = dlgfontInfo->fid;
    dlggc = XCreateGC(dlgdisplay, dlgwindow, GCFont, &gc_values);

    // Set window title (similar to WM_CREATE)
    XStoreName(dlgdisplay, dlgwindow, "SAMP Dialog");

    // Create buttons and other elements
    hwndEditBox = XCreateSimpleWindow(dlgdisplay, dlgwindow, 50, 200, 300, 24, 1, BlackPixel(dlgdisplay, DefaultScreen(dlgdisplay)), WhitePixel(dlgdisplay, DefaultScreen(dlgdisplay)));
    hwndListBox = XCreateSimpleWindow(dlgdisplay, dlgwindow, 10, 10, 375, 225, 1, BlackPixel(dlgdisplay, DefaultScreen(dlgdisplay)), WhitePixel(dlgdisplay, DefaultScreen(dlgdisplay)));

    // Example listbox (using X11 functions, you would handle drawing items in a custom way)
    // Listbox items would need to be added manually, for example by drawing text

    XMapWindow(dlgdisplay, hwndEditBox);
    XMapWindow(dlgdisplay, hwndListBox);
    XMapWindow(dlgdisplay, dlgwindow);  // Show the main window
    XFlush(dlgdisplay);
}

void DLGhandlePaint() {
    // Painting similar to WM_PAINT in Windows
    XClearWindow(dlgdisplay, dlgwindow);

    if (hwndListBox == 0) {
        // Handle custom painting for the dialog (like drawing text)
        XDrawString(dlgdisplay, dlgwindow, dlggc, 50, 50, "Sample Info: Custom Dialog", strlen("Sample Info: Custom Dialog"));
    }
}

void DLGhandleCommand() {
    // Handle commands, such as button clicks or other actions
    if (hwndEditBox != 0) {
        // For example, handling text input (simulating WM_COMMAND)
        XGetInputFocus(dlgdisplay, &dlgwindow, &wSelection);
        sendDialogResponse(1, 1, 0, szResponse);  // Simulate button click response
    }
}

void DLGhandleListBoxSelection(XEvent* event) {
    int y = event->xbutton.y;
    selectedItemIndex = y / 30;  // Assuming each list item takes 30px height (for simplicity)
    if (selectedItemIndex >= 0 && selectedItemIndex < numItems) {
        printf("Item %d selected: %s\n", selectedItemIndex, listItems[selectedItemIndex]);
    }
}

void* DialogBoxThread(void* param) {
    dlgdisplay = XOpenDisplay(NULL);
    if (!dlgdisplay) {
        fprintf(stderr, "Cannot open X display\n");
        return NULL;
    }

    int screen = DefaultScreen(dlgdisplay);
    Window rootWindow = RootWindow(dlgdisplay, screen);

    // Create dialog window
    hwndSAMPDlg = XCreateSimpleWindow(dlgdisplay, rootWindow, 100, 100, 400, 300, 1, BlackPixel(dlgdisplay, screen), WhitePixel(dlgdisplay, screen));

    // Select events we want to listen for
    XSelectInput(dlgdisplay, hwndSAMPDlg, ExposureMask | ButtonPressMask | KeyPressMask);

    // Call handleCreate to initialize the dialog (create controls)
    DLGhandleCreate();

    // Event loop for processing messages and drawing the dialog
    XEvent event;
    while (1) {
        XNextEvent(dlgdisplay, &event);

        switch (event.type) {
        case Expose:
            DLGhandlePaint();
            break;

        case ButtonPress:
            // Handle button press or list box item selection
            if (event.xbutton.window == hwndListBox) {
                DLGhandleListBoxSelection(&event);
            }
            else {
                DLGhandleButtonPress(&event);
            }
            break;

        case ClientMessage:
            closeDialogBox();
            return NULL;
            break;

        default:
            break;
        }
    }

    return NULL;
}

