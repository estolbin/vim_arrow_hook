#include <windows.h>
#include "KeyboardHook.h"

const UINT WM_TRAY = WM_USER + 1;
HICON g_hIcon = NULL;
HICON g_hIcon_ON = NULL;
HMENU g_menu;
HHOOK hook_keyboard;
NOTIFYICONDATA stData;
bool IconChangedToOn = false;

int capital_active()
{
    return (GetKeyState(VK_CAPITAL) & 1) == 1;
}

void ChangeIcon()
{
    bool mode_on = (bool)capital_active();
    if (mode_on && !IconChangedToOn)
    {
        IconChangedToOn = true;
        //MessageBox(NULL, "Caps on!","VIM",MB_OK);
        stData.hIcon = g_hIcon_ON;
        Shell_NotifyIcon(NIM_MODIFY, &stData);
        DestroyIcon(stData.hIcon);
    } else if (!mode_on && IconChangedToOn)
    {
        IconChangedToOn = false;
        stData.hIcon = g_hIcon;
        Shell_NotifyIcon(NIM_MODIFY, &stData);
        DestroyIcon(stData.hIcon);
    }

}

LRESULT CALLBACK ll_keyboardproc (int nCode, WPARAM wParam, LPARAM lParam)
{
    int keyup = 0;
    bool replacekey = false;

    if (nCode == HC_ACTION)
    {
        switch(wParam)
        {
        case WM_KEYUP:
        case WM_SYSKEYUP:
            keyup = KEYEVENTF_KEYUP;
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
            PKBDLLHOOKSTRUCT p = (PKBDLLHOOKSTRUCT) lParam;
            if (p->vkCode == VK_CAPITAL)
            {
                ChangeIcon();
            }

            if (capital_active())
            {
                switch(p->vkCode)
                {
                case 'H':
                    keybd_event( VK_LEFT, 0, keyup, 0 );
                    replacekey = capital_active();
                    break;
                case 'J':
                    keybd_event(VK_DOWN, 0, keyup, 0);
                    replacekey = capital_active();
                    break;
                case 'K':
                    keybd_event(VK_UP, 0, keyup, 0);
                    replacekey = capital_active();
                    break;
                case 'L':
                    keybd_event(VK_RIGHT, 0, keyup, 0);
                    replacekey = capital_active();
                    break;
                case 'D':
                    keybd_event(VK_NEXT, 0, keyup, 0);
                    replacekey = capital_active();
                    break;
                case 'U':
                    keybd_event(VK_PRIOR, 0, keyup, 0);
                    replacekey = capital_active();
                    break;
                case 'X':
                    keybd_event(VK_DELETE, 0, keyup, 0);
                    replacekey = capital_active();
                    break;
                case 'A':
                    keybd_event(VK_HOME, 0, keyup, 0);
                    replacekey = capital_active();
                    break;
                case 'E':
                    keybd_event(VK_END, 0, keyup, 0);
                    replacekey = capital_active();
                    break;
                case VK_OEM_2:
                    keybd_event(VK_CONTROL, 0, KEYEVENTF_EXTENDEDKEY, 0);
                    keybd_event('F', 0, keyup, 0);
                    keybd_event(VK_CONTROL, 0, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
                    keybd_event(VK_CAPITAL, 0, keyup, 0);
                    break;
                }
            }
            break;
        }
    }

    if (replacekey == false)
    {
        return CallNextHookEx(NULL, nCode, wParam, lParam);
    }
    return 1;
}

LRESULT CALLBACK HiddenWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
    case WM_CREATE:
    {
        g_menu = CreatePopupMenu();
        AppendMenu(g_menu, MF_STRING, ID_QUIT, "Quit");

        hook_keyboard = SetWindowsHookEx( WH_KEYBOARD_LL, ll_keyboardproc, 0, 0);
    }
    return 0;
    case WM_DESTROY:
    {
        //NOTIFYICONDATA stData;
        ZeroMemory(&stData, sizeof(stData));
        stData.cbSize = sizeof(stData);
        stData.hWnd = hWnd;
        Shell_NotifyIcon(NIM_DELETE, &stData);

        UnhookWindowsHookEx(hook_keyboard);

    }
    return 0;

    case WM_TRAY:
        switch(lParam)
        {
        case WM_RBUTTONDOWN:
        {
            POINT curPoint;
            GetCursorPos(&curPoint);

            UINT clicked = TrackPopupMenu(
                               g_menu,
                               TPM_RETURNCMD | TPM_NONOTIFY,
                               curPoint.x,
                               curPoint.y,
                               0,
                               hWnd,
                               NULL
                           );
            if(clicked == ID_QUIT)
            {
                PostQuitMessage(0);
            }
        }
        }
        break;
    case WM_COMMAND:
        switch(LOWORD(wParam))
        {
        case ID_QUIT:
            PostQuitMessage(0);
            return 0;
        }
        break;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR args, int nCmdShow)
{
    WNDCLASSEX wc;
    ZeroMemory(&wc, sizeof(wc));
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.lpszClassName = "Vim arrow global";
    wc.hInstance = hInstance;
    wc.lpfnWndProc = HiddenWndProc;

    if (!RegisterClassEx(&wc))
    {
        MessageBox(NULL, "Could not register class", "Error", MB_OK | MB_ICONERROR);
    }


    HWND hwnd = CreateWindow("Vim arrow global","", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT,0,CW_USEDEFAULT,0, NULL, NULL, hInstance, NULL);

    g_hIcon_ON = LoadIcon(hInstance, MAKEINTRESOURCE(ID_ICON_ON));

    //NOTIFYICONDATA stData;
    ZeroMemory(&stData, sizeof(stData));
    stData.cbSize = sizeof(stData);
    stData.hWnd = hwnd;
    stData.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    stData.uCallbackMessage = WM_TRAY;
    stData.hIcon = g_hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(ID_ICON));
    strcpy(stData.szTip, "Vim like keys... global");
    Shell_NotifyIcon(NIM_ADD, &stData);
    DestroyIcon(stData.hIcon);

    //ShowWindow(FindWindow("ConsoleWindowClass", NULL), false);

    MSG msg;
    while(GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;

}
