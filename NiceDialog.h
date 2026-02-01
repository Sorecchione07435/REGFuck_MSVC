#pragma once
#include <windows.h>

#include <gdiplus.h>
using namespace Gdiplus;

class NiceDialog
{
public:
    NiceDialog(HINSTANCE hInstance);

    INT_PTR Show(HWND hParent);

    static INT_PTR CALLBACK NiceDialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    HINSTANCE m_hInstance;
    HWND m_hWnd;
protected:

    BOOL OnDialogLoad();
public:
    Bitmap* pImage = nullptr;

    void LoadGIF();

};
