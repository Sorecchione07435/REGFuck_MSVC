#include "NiceDialog.h"
#include "resource.h"
#include "Main.h"
#include "SettingzDialog.h"

#include <iostream>
#include <string>

using namespace std;

NiceDialog::NiceDialog(HINSTANCE hInstance)
{

}

INT_PTR NiceDialog::Show(HWND hParent)
{
	return DialogBoxParam(m_hInstance, MAKEINTRESOURCE(IDD_NICE), hParent, NiceDialogProc, (LPARAM)this);
}

INT_PTR NiceDialog::NiceDialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	NiceDialog* pThis = nullptr;

	if (uMsg == WM_INITDIALOG)
	{
		pThis = (NiceDialog*)lParam;

		SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pThis);

		pThis->m_hWnd = hWnd;

		return pThis->OnDialogLoad();
	}
	else
	{
		pThis = (NiceDialog*)GetWindowLongPtr(hWnd, GWLP_USERDATA);

		if (!pThis) return false;
	}

	switch (uMsg)
	{
		case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{

				case IDCANCEL:
				{
					EndDialog(hWnd, HIWORD(wParam));
					break;
				}
			}

			return true;
		}

		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);

			RECT rc;
			GetClientRect(hWnd, &rc);

			Graphics graphics(hdc);

			if (pThis->pImage)
			{
				// Disegna l’immagine scalata alla dimensione della finestra
				graphics.DrawImage(pThis->pImage, 0, 0, rc.right, rc.bottom);
			}

			EndPaint(hWnd, &ps);
			return 0;
		}

		case WM_DESTROY:
		{
			GdiplusShutdown(g_gdiplusToken);

			if (pThis->pImage)
			{
				delete pThis->pImage;
			}

			break;
		}
	}

	return false;
}

void NiceDialog::LoadGIF()
{
	HRSRC hJPG = FindResource(m_hInstance, MAKEINTRESOURCE(IDR_NICE), "JPG");
	HGLOBAL hJPGData = LoadResource(m_hInstance, hJPG);

	DWORD hJPGSize = SizeofResource(m_hInstance, hJPG);
	void* pJPGData = LockResource(hJPGData);

	IStream* pStream = nullptr;
	CreateStreamOnHGlobal(NULL, TRUE, &pStream);

	ULONG written;

	pStream->Write(pJPGData, hJPGSize, &written);

	LARGE_INTEGER liZero = {};

	pStream->Seek(liZero, STREAM_SEEK_SET, nullptr);

	pImage = new Bitmap(pStream);

	pStream->Release();

}

BOOL NiceDialog::OnDialogLoad()
{
	LONG exStyle = GetWindowLong(m_hWnd, GWL_EXSTYLE);
	SetWindowLong(m_hWnd, GWL_EXSTYLE, exStyle | WS_EX_COMPOSITED);

	LoadGIF();

	string seedStr;

	seedStr = "Seed: " + to_string(SettingzDialog::seed);

	SetDlgItemText(m_hWnd, IDC_FinalSeed, seedStr.c_str());

	return true;
}