#ifndef DESKTOP_WALLPAPER_H
#define DESKTOP_WALLPAPER_H

// Platform-specific code to set window as desktop wallpaper
// Works on macOS, Windows, and Linux/X11

#if defined(__APPLE__)
    #include <objc/objc-runtime.h>
    #include <CoreGraphics/CoreGraphics.h>
#elif defined(_WIN32)
    #include <windows.h>
#elif defined(__linux__) || defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__)
    #include <X11/Xlib.h>
    #include <X11/Xatom.h>
#endif

namespace DesktopWallpaper {

#if defined(__APPLE__)
    // macOS: Set window level below desktop icons
    #include <objc/message.h>
    #include <objc/runtime.h>

    void SetAsWallpaper(void* nativeWindow) {
        if (nativeWindow == nullptr) {
            fprintf(stderr, "macOS: nativeWindow is null!\n");
            return;
        }

        // Get NSWindow from GLFW window
        id window = (id)nativeWindow;

        // Cast objc_msgSend to correct function pointer type
        // typedef void (*MsgSendFunc)(id, SEL, long);  // Unused for now
        typedef void (*MsgSendBoolFunc)(id, SEL, bool);
        typedef void (*MsgSendUIntFunc)(id, SEL, unsigned long);
        typedef long (*MsgSendGetFunc)(id, SEL);

        typedef void (*MsgSendFunc)(id, SEL, long);
        MsgSendFunc setLevel = (MsgSendFunc)objc_msgSend;
        MsgSendBoolFunc setMovable = (MsgSendBoolFunc)objc_msgSend;
        MsgSendUIntFunc setCollectionBehavior = (MsgSendUIntFunc)objc_msgSend;
        MsgSendGetFunc getLevel = (MsgSendGetFunc)objc_msgSend;

        // Use window level -1 (just below normal windows)
        // This is the lowest level that keeps the window visible for non-privileged apps
        // Desktop-level (INT_MIN + 20) requires special macOS entitlements
        long windowLevel = -1;

        printf("macOS: Setting window level to %ld (below all windows)\n", windowLevel);
        setLevel(window, sel_registerName("setLevel:"), windowLevel);

        // Make window non-movable
        setMovable(window, sel_registerName("setMovable:"), false);

        // Set collection behavior (visible on all spaces, stationary)
        setCollectionBehavior(window, sel_registerName("setCollectionBehavior:"),
                             (1UL << 0) | (1UL << 4));

        long currentLevel = getLevel(window, sel_registerName("level"));
        printf("macOS: Window level set to %ld (wallpaper-like behavior)\n", currentLevel);
    }

    // Get menubar height dynamically (accounts for Notch on newer Macs)
    int GetMenuBarHeight() {
        // Get NSScreen class
        Class nsScreenClass = objc_getClass("NSScreen");
        if (!nsScreenClass) return 25; // Fallback

        typedef id (*MsgSendIdFunc)(Class, SEL);
        MsgSendIdFunc getMainScreen = (MsgSendIdFunc)objc_msgSend;

        // Get main screen: [NSScreen mainScreen]
        id mainScreen = getMainScreen(nsScreenClass, sel_registerName("mainScreen"));
        if (!mainScreen) return 25; // Fallback

        // Get frame and visibleFrame
        typedef CGRect (*MsgSendRectFunc)(id, SEL);
        MsgSendRectFunc getFrame = (MsgSendRectFunc)objc_msgSend;

        CGRect frame = getFrame(mainScreen, sel_registerName("frame"));
        CGRect visibleFrame = getFrame(mainScreen, sel_registerName("visibleFrame"));

        // Menubar height = frame.height - visibleFrame.height - visibleFrame.origin.y
        int menuBarHeight = (int)(frame.size.height - visibleFrame.size.height - visibleFrame.origin.y);

        printf("macOS: Detected menubar height: %dpx\n", menuBarHeight);
        return menuBarHeight > 0 ? menuBarHeight : 25;
    }

#elif defined(_WIN32)
    // Windows: Set window as desktop background (WorkerW method)
    HWND workerw = nullptr;

    BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
        HWND shelldll = FindWindowEx(hwnd, nullptr, L"SHELLDLL_DefView", nullptr);
        if (shelldll != nullptr) {
            // Found the window that contains SHELLDLL_DefView
            // The WorkerW we want is the one after this in Z-order
            workerw = FindWindowEx(nullptr, hwnd, L"WorkerW", nullptr);
        }
        return TRUE;
    }

    void SetAsWallpaper(HWND hwnd) {
        // Find the Progman window
        HWND progman = FindWindow(L"Progman", nullptr);

        // Send message to spawn WorkerW
        SendMessageTimeout(progman, 0x052C, 0, 0, SMTO_NORMAL, 1000, nullptr);

        // Find the WorkerW window
        EnumWindows(EnumWindowsProc, 0);

        if (workerw != nullptr) {
            // Set our window as child of WorkerW
            SetParent(hwnd, workerw);
        } else {
            // Fallback: set as child of Progman directly
            SetParent(hwnd, progman);
        }

        // Make window fill the screen
        RECT rect;
        SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);
        SetWindowPos(hwnd, HWND_BOTTOM,
                     rect.left, rect.top,
                     rect.right - rect.left,
                     rect.bottom - rect.top,
                     SWP_SHOWWINDOW);
    }

#elif defined(__linux__) || defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__)
    // Linux/BSD X11: Set window type to desktop
    void SetAsWallpaper(Display* display, Window window) {
        // Set window type to desktop
        Atom windowType = XInternAtom(display, "_NET_WM_WINDOW_TYPE", False);
        Atom desktop = XInternAtom(display, "_NET_WM_WINDOW_TYPE_DESKTOP", False);

        XChangeProperty(display, window, windowType, XA_ATOM, 32,
                       PropModeReplace, (unsigned char*)&desktop, 1);

        // Set window to be below all other windows
        Atom state = XInternAtom(display, "_NET_WM_STATE", False);
        Atom below = XInternAtom(display, "_NET_WM_STATE_BELOW", False);

        XChangeProperty(display, window, state, XA_ATOM, 32,
                       PropModeAppend, (unsigned char*)&below, 1);

        // Make sticky (visible on all desktops)
        Atom sticky = XInternAtom(display, "_NET_WM_STATE_STICKY", False);
        XChangeProperty(display, window, state, XA_ATOM, 32,
                       PropModeAppend, (unsigned char*)&sticky, 1);
    }
#endif

} // namespace DesktopWallpaper

#endif // DESKTOP_WALLPAPER_H
