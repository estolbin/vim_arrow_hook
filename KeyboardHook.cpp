#include <windows.h>

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
                    case 'Q':
                        keybd_event('Z', 0, keyup, 0);
                        replacekey = true;
                        break;
                    case 'Z':
                        keybd_event('Q', 0, keyup, 0);
                        replacekey = true;
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
