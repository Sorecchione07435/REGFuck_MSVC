#pragma once
#include <windows.h>

#include <gdiplus.h>

using namespace Gdiplus;

class SettingzDialog
{
public:
    SettingzDialog(HINSTANCE hInstance);

    INT_PTR Show(HWND hParent);

    static INT_PTR CALLBACK SettingzDialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    HINSTANCE m_hInstance;
    HWND m_hWnd;
protected:

    BOOL OnDialogLoad();
public:
    static double hkcr, hku, hklm, del;

    static int seed;

    void OnTrackBar1Scroll();
    void OnTrackBar2Scroll();
    void OnTrackBar3Scroll();
    void OnTrackBar4Scroll();

    void OnSeedEditTextChanged();
    void OnRandomButtonClicked();

    void OnCloseButtonClicked();

    HFONT m_WindowFont;
    HFONT m_ButtonsFont;

    Bitmap* m_pGif = nullptr;

    GUID m_frameGuid;
    UINT m_frameCount = 0;
    int m_currentFrame = 0;

};
