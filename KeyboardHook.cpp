#include <windows.h>

#define ID_ICON 100
#define ID_QUIT 200


const UINT WM_TRAY = WM_USER + 1;
HINSTANCE g_hInstance = NULL;
HICON g_hIcon = NULL;
HMENU g_menu;
HHOOK hook_keyboard;

int capital_active()
{
    return (GetKeyState(VK_CAPITAL) & 1) == 1;
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

                        default:
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


                NOTIFYICONDATA stData;
                ZeroMemory(&stData, sizeof(stData));
                stData.cbSize = sizeof(stData);
                stData.hWnd = hWnd;
                stData.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
                stData.uCallbackMessage = WM_TRAY;
                stData.hIcon = g_hIcon = (HICON)LoadImage(NULL, "vim.ico", IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
                strcpy(stData.szTip, "Vim like keys... global");
                if(!Shell_NotifyIcon(NIM_ADD, &stData))
                    return -1;


            }
            return 0;
        case WM_DESTROY:
            {
                NOTIFYICONDATA stData;
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
    WNDCLASS wc;
    ZeroMemory(&wc, sizeof(wc));
    wc.lpszClassName = "Vim arrow global";

    HWND hHiddenWnd = FindWindow(wc.lpszClassName, NULL);
    if (hHiddenWnd)
        PostMessage(hHiddenWnd, WM_TRAY, 0, WM_LBUTTONDBLCLK);
    else
    {
        wc.hInstance = hInstance;
        wc.lpfnWndProc = HiddenWndProc;

        ATOM aClass = RegisterClass(&wc);
        if (aClass)
        {
            g_hInstance = hInstance;
            if (hHiddenWnd = CreateWindow((LPCSTR) aClass, "", 0, 0, 0, 0, 0, NULL, NULL, hInstance, NULL))
            {
                MSG msg;
                while (GetMessage(&msg, NULL, 0, 0))
                {
                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                }

                if (IsWindow(hHiddenWnd))
                    DestroyWindow(hHiddenWnd);
            }
            UnregisterClass((LPCSTR) aClass, g_hInstance);
        }
    }
}
