#include <iostream>
#include <windows.h>

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
                break;
        }
    }

    if (replacekey == false)
    {
        return CallNextHookEx(NULL, nCode, wParam, lParam);
    }
    return 1;
}

int main() 
{
    HHOOK hook_keyboard = SetWindowsHookEx( WH_KEYBOARD_LL, ll_keyboardproc, 0, 0);

    MSG msg;
    while (!GetMessage(&msg, NULL, NULL, NULL))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    UnhookWindowsHookEx(hook_keyboard);
}
