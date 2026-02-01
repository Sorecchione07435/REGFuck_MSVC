#pragma once
#include <windows.h>

class MainDialog
{
public:
    MainDialog(HINSTANCE hInstance);

    INT_PTR Show(HWND hParent);

    static INT_PTR CALLBACK regfuckDialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    HINSTANCE m_hInstance;
    HWND m_hWnd;
protected:

    BOOL OnDialogLoad();
public:

    HFONT m_DoItBtnFont;
    HFONT m_SettingzBtnFont;

    void OnTrackBarScroll();

    void OnDoItButtonClicked();
    void OnSettingzButtonClicked();
};
