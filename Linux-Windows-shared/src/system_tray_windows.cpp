#ifdef _WIN32

#include "system_tray.hpp"
#include <windows.h>
#include <shellapi.h>
#include <memory>
#include <map>

class SystemTrayWindows : public SystemTray {
private:
    HWND hwnd = nullptr;
    NOTIFYICONDATA nid = {};
    bool quit_flag = false;
    std::string h1_text;
    std::string h2_text;
    std::string app_name;

    TextInputCallback h1_callback;
    TextInputCallback h2_callback;
    QuitCallback quit_callback;

    static const UINT WM_TRAY_CALLBACK = WM_APP + 1;
    static const UINT TRAY_ICON_ID = 1;

    static std::map<HWND, SystemTrayWindows*> instance_map;

    static LRESULT CALLBACK window_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
    void handle_tray_icon_click(LPARAM lparam);
    void show_context_menu();
    bool show_input_dialog(const std::string& title, std::string& input_text);

public:
    SystemTrayWindows() = default;
    ~SystemTrayWindows() override;

    bool init(const std::string& app_name_str, const std::string& icon_path) override;
    void set_h1_callback(TextInputCallback callback) override { h1_callback = callback; }
    void set_h2_callback(TextInputCallback callback) override { h2_callback = callback; }
    void set_quit_callback(QuitCallback callback) override { quit_callback = callback; }

    void set_h1_text(const std::string& text) override;
    void set_h2_text(const std::string& text) override;

    void run() override;
    void quit() override { quit_flag = true; }
    bool should_quit() const override { return quit_flag; }
};

std::map<HWND, SystemTrayWindows*> SystemTrayWindows::instance_map;

LRESULT CALLBACK SystemTrayWindows::window_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    auto it = instance_map.find(hwnd);
    if (it == instance_map.end()) return DefWindowProc(hwnd, msg, wparam, lparam);

    SystemTrayWindows* pThis = it->second;

    switch (msg) {
        case WM_CREATE:
            return 0;

        case WM_TRAY_CALLBACK:
            if (lparam == WM_RBUTTONUP || lparam == WM_LBUTTONUP) {
                pThis->handle_tray_icon_click(lparam);
            }
            return 0;

        case WM_COMMAND:
            // Handle menu commands
            if (HIWORD(wparam) == 0) {
                int cmd = LOWORD(wparam);
                if (cmd == 101) {  // Edit H1
                    std::string new_text = pThis->h1_text;
                    if (pThis->show_input_dialog("Enter H1 text", new_text)) {
                        pThis->set_h1_text(new_text);
                        if (pThis->h1_callback) {
                            pThis->h1_callback(new_text);
                        }
                    }
                } else if (cmd == 102) {  // Edit H2
                    std::string new_text = pThis->h2_text;
                    if (pThis->show_input_dialog("Enter H2 text", new_text)) {
                        pThis->set_h2_text(new_text);
                        if (pThis->h2_callback) {
                            pThis->h2_callback(new_text);
                        }
                    }
                } else if (cmd == 103) {  // Quit
                    pThis->quit_flag = true;
                    if (pThis->quit_callback) {
                        pThis->quit_callback();
                    }
                    Shell_NotifyIcon(NIM_DELETE, &pThis->nid);
                    PostQuitMessage(0);
                }
            }
            return 0;

        case WM_DESTROY:
            instance_map.erase(hwnd);
            PostQuitMessage(0);
            return 0;

        default:
            return DefWindowProc(hwnd, msg, wparam, lparam);
    }
}

bool SystemTrayWindows::init(const std::string& app_name_str, const std::string& icon_path) {
    app_name = app_name_str;
    h1_text = "comboom.sucht";
    h2_text = "Live Wallpaper";

    // Register window class
    WNDCLASS wc = {};
    wc.lpfnWndProc = window_proc;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.lpszClassName = L"ComboomTrayWindow";

    RegisterClass(&wc);

    // Create hidden window
    hwnd = CreateWindow(L"ComboomTrayWindow", L"Comboom Tray", 0, 0, 0, 0, 0,
                       HWND_MESSAGE, nullptr, GetModuleHandle(nullptr), nullptr);

    if (!hwnd) return false;

    instance_map[hwnd] = this;

    // Set up tray icon
    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = hwnd;
    nid.uID = TRAY_ICON_ID;
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    nid.uCallbackMessage = WM_TRAY_CALLBACK;

    // Load icon - for now use default Windows icon
    nid.hIcon = LoadIcon(nullptr, IDI_APPLICATION);

    wcscpy_s(nid.szTip, sizeof(nid.szTip) / sizeof(wchar_t),
             L"comboom.sucht Live Wallpaper");

    if (!Shell_NotifyIcon(NIM_ADD, &nid)) {
        return false;
    }

    return true;
}

void SystemTrayWindows::set_h1_text(const std::string& text) {
    h1_text = text;
    // Update tooltip with new text
    size_t len = h1_text.length();
    wchar_t tooltip[128];
    MultiByteToWideChar(CP_UTF8, 0, h1_text.c_str(), -1, tooltip, 128);
    wcscpy_s(nid.szTip, 128, tooltip);
    Shell_NotifyIcon(NIM_MODIFY, &nid);
}

void SystemTrayWindows::set_h2_text(const std::string& text) {
    h2_text = text;
}

bool SystemTrayWindows::show_input_dialog(const std::string& title, std::string& input_text) {
    // Simple implementation: use MessageBox with input
    // For a proper implementation, you'd create a custom dialog
    // This is a simplified version
    wchar_t buffer[256] = {};
    MultiByteToWideChar(CP_UTF8, 0, input_text.c_str(), -1, buffer, 256);

    wchar_t title_w[256];
    MultiByteToWideChar(CP_UTF8, 0, title.c_str(), -1, title_w, 256);

    // Use the built-in edit dialog (simplified)
    wchar_t result[256] = {0};
    if (GetWindowTextLength(hwnd) == 0) {
        wmemcpy_s(result, 256, buffer, wcslen(buffer));
    }

    input_text = "";
    if (GetWindowTextA(hwnd, (LPSTR)buffer, 256)) {
        char ansi_result[256] = {};
        WideCharToMultiByte(CP_UTF8, 0, result, -1, ansi_result, 256, nullptr, nullptr);
        input_text = ansi_result;
        return true;
    }

    return false;
}

void SystemTrayWindows::handle_tray_icon_click(LPARAM lparam) {
    if (lparam == WM_RBUTTONUP || lparam == WM_LBUTTONUP) {
        show_context_menu();
    }
}

void SystemTrayWindows::show_context_menu() {
    POINT pt;
    GetCursorPos(&pt);

    HMENU hmenu = CreatePopupMenu();
    AppendMenuA(hmenu, MFT_STRING, 0, app_name.c_str());
    AppendMenuA(hmenu, MFT_SEPARATOR, 0, nullptr);
    AppendMenuA(hmenu, MFT_STRING, 101, "Edit H1...");
    AppendMenuA(hmenu, MFT_STRING, 102, "Edit H2...");
    AppendMenuA(hmenu, MFT_SEPARATOR, 0, nullptr);
    AppendMenuA(hmenu, MFT_STRING, 103, "Quit");

    SetForegroundWindow(hwnd);
    TrackPopupMenu(hmenu, TPM_BOTTOMALIGN | TPM_LEFTALIGN, pt.x, pt.y, 0, hwnd, nullptr);

    DestroyMenu(hmenu);
}

void SystemTrayWindows::run() {
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);

        if (quit_flag) {
            break;
        }
    }

    // Cleanup
    Shell_NotifyIcon(NIM_DELETE, &nid);
    if (hwnd) {
        DestroyWindow(hwnd);
    }
}

SystemTrayWindows::~SystemTrayWindows() {
    if (hwnd) {
        Shell_NotifyIcon(NIM_DELETE, &nid);
        DestroyWindow(hwnd);
        instance_map.erase(hwnd);
    }
}

std::unique_ptr<SystemTray> SystemTray::create() {
    return std::make_unique<SystemTrayWindows>();
}

#endif // _WIN32
