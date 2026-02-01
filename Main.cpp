#include "windows.h"
#include "MainDialog.h"
#include "gdiplus.h"

using namespace Gdiplus;

ULONG_PTR g_gdiplusToken = NULL;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR nCmdLine, int nCmdShow)
{

	GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&g_gdiplusToken, &gdiplusStartupInput, nullptr);

	if (MessageBoxA(
		NULL,
		"This program was made to destroy the registry of your computer.\n"
		"Using it will likely make your computer unbootable or unusable.\n"
		"The GUI of this program was made intentionally bad, if you don't know what you do, don't continue.\n\n"
		"You have been warned. Clicking Yes will start the program now.",
		"REGFuck - Warning",
		MB_YESNO | MB_ICONWARNING) == IDYES)
	{
		MainDialog mainDlg(hInstance);
		mainDlg.Show(NULL);
	}

	return 0;
}