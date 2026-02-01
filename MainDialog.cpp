#include "MainDialog.h"
#include "resource.h"
#include "gdiplus.h"
#include "CommCtrl.h"
#include <cmath>
#include <sstream>
#include <iomanip>

#include "PlizWaitDialog.h"
#include "SettingzDialog.h"

#include "cstdio"


using namespace Gdiplus;
using namespace std;


#define WM_Z_ORDER_REPAIR (WM_APP + 1)

MainDialog::MainDialog(HINSTANCE hInstance)
{
	m_DoItBtnFont = CreateFontA(32, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 0, 0, "Comic Sans MS");
	m_SettingzBtnFont = CreateFontA(14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Consolas");
}

INT_PTR MainDialog::Show(HWND hParent)
{
	return DialogBoxParam(m_hInstance, MAKEINTRESOURCE(IDD_Main), hParent, regfuckDialogProc, (LPARAM)this);
}

INT_PTR MainDialog::regfuckDialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	MainDialog* pThis = nullptr;

	if (uMsg == WM_INITDIALOG)
	{
		pThis = (MainDialog*)lParam;

		SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pThis);

		pThis->m_hWnd = hWnd;

		return pThis->OnDialogLoad();
	}
	else
	{
		pThis = (MainDialog*)GetWindowLongPtr(hWnd, GWLP_USERDATA);

		if (!pThis) return false;
	}

	switch (uMsg)
	{
		case WM_COMMAND:
		{
			DWORD event = HIWORD(wParam);

			switch (LOWORD(wParam))
			{
				case IDC_BUTTON1:
				{
					if (pThis && event == BN_CLICKED)
					{
						pThis->OnDoItButtonClicked();
					}
					break;
				}

				case IDC_BUTTON2:
				{
					if (pThis && event == BN_CLICKED)
					{
						pThis->OnSettingzButtonClicked();
					}
					break;
				}

				case IDCANCEL:
				{
					EndDialog(hWnd, HIWORD(wParam));
					break;
				}
			}

			return true;
		}

		case WM_HSCROLL:
		{
			if ((HWND)lParam == GetDlgItem(hWnd, IDC_SLIDER1))
			{
				pThis->OnTrackBarScroll();
			}
			break;
		}

		case WM_PAINT:
		{
			if (pThis)
			{
				PAINTSTRUCT ps;

				HDC hdc = BeginPaint(hWnd, &ps);

				RECT rc;
				GetClientRect(hWnd, &rc);

				HRSRC hPNGRes = FindResource(pThis->m_hInstance, MAKEINTRESOURCE(IDB_BACKGROUND), "PNG");
				HGLOBAL hPNGMem = LoadResource(pThis->m_hInstance, hPNGRes);

				void* pPNGData = LockResource(hPNGMem);
				DWORD pPNGSize = SizeofResource(pThis->m_hInstance, hPNGRes);

				IStream* pStream = nullptr;
				CreateStreamOnHGlobal(NULL, TRUE, &pStream);

				ULONG written;
				pStream->Write(pPNGData, pPNGSize, &written);

				Bitmap bitmap(pStream);
				Graphics graphics(hdc);

				graphics.DrawImage(&bitmap, 0, 0, rc.right, rc.bottom);
				pStream->Release();

				EndPaint(hWnd, &ps);

				return 0;
			}

			break;
		}

		case WM_DRAWITEM:
		{
			LPDRAWITEMSTRUCT pDIS = (LPDRAWITEMSTRUCT)lParam;

			if (pDIS->CtlID == IDC_BUTTON2)
			{
				// Colori
				COLORREF clrText = RGB(0, 255, 0);      // Verde
				COLORREF clrBg = RGB(0, 0, 0);          // Nero
				COLORREF clrBorder = RGB(0, 192, 0);    // Verde scuro

				// Riempi sfondo
				HBRUSH hBrush = CreateSolidBrush(clrBg);
				FillRect(pDIS->hDC, &pDIS->rcItem, hBrush);
				DeleteObject(hBrush);

				// Disegna bordo
				HPEN hPen = CreatePen(PS_SOLID, 3, clrBorder);
				HPEN hOldPen = (HPEN)SelectObject(pDIS->hDC, hPen);
				SelectObject(pDIS->hDC, GetStockObject(NULL_BRUSH));
				Rectangle(pDIS->hDC, pDIS->rcItem.left, pDIS->rcItem.top,
					pDIS->rcItem.right, pDIS->rcItem.bottom);
				SelectObject(pDIS->hDC, hOldPen);
				DeleteObject(hPen);

				// Disegna testo
				wchar_t szText[256];
				GetWindowTextW(pDIS->hwndItem, szText, 256);

				SetTextColor(pDIS->hDC, clrText);
				SetBkMode(pDIS->hDC, TRANSPARENT);

				// Font Consolas
				HFONT hFont = CreateFontA(14, 0, 0, 0, FW_NORMAL, 0, 0, 0,
					0, 0, 0, 0, 0, "Consolas");
				HFONT hOldFont = (HFONT)SelectObject(pDIS->hDC, hFont);

				RECT rc = pDIS->rcItem;
				RECT calc = rc;

				DrawTextW(
					pDIS->hDC,
					szText,
					-1,
					&calc,
					DT_CENTER | DT_WORDBREAK | DT_CALCRECT
				);

				// sposta il rettangolo in basso
				int textHeight = calc.bottom - calc.top;
				int boxHeight = rc.bottom - rc.top;

				// formula per centratura verticale
				rc.top += (boxHeight - textHeight) / 2;
				rc.bottom = rc.top + textHeight;

				DrawTextW(
					pDIS->hDC,
					szText,
					-1,
					&rc,
					DT_CENTER | DT_WORDBREAK
				);

				SelectObject(pDIS->hDC, hOldFont);
				DeleteObject(hFont);

				// Se premuto, disegna effetto
				if (pDIS->itemState & ODS_SELECTED)
				{
					InvertRect(pDIS->hDC, &pDIS->rcItem);
				}

				// Se ha il focus, disegna rettangolo tratteggiato
				if (pDIS->itemState & ODS_FOCUS)
				{
					RECT rcFocus = pDIS->rcItem;
					InflateRect(&rcFocus, -3, -3);
					DrawFocusRect(pDIS->hDC, &rcFocus);
				}

				return TRUE;
			}
			break;
		}

		case WM_MOUSEACTIVATE:
		case WM_SETFOCUS:
		{
			if (pThis)
			{
				PostMessage(hWnd, WM_Z_ORDER_REPAIR, 0, 0);
			}
			
			break;
		}

		case WM_Z_ORDER_REPAIR:
		{
			if (pThis)
			{
				SetWindowPos(GetDlgItem(hWnd, IDC_SLIDER1), HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

				SetWindowPos(GetDlgItem(hWnd, IDC_PUSSY), HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
				SetWindowPos(GetDlgItem(hWnd, IDC_RESTINPISS), HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
				SetWindowPos(GetDlgItem(hWnd, IDC_PERCENTAGE), HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
			}

			break;
		}
	}

	return false;
}

BOOL MainDialog::OnDialogLoad()
{
	LONG exStyle = GetWindowLong(m_hWnd, GWL_EXSTYLE);
	SetWindowLong(m_hWnd, GWL_EXSTYLE, exStyle | WS_EX_COMPOSITED);


	SendDlgItemMessage(m_hWnd, IDC_BUTTON1, WM_SETFONT, (WPARAM)m_DoItBtnFont, 0);
	SendDlgItemMessage(m_hWnd, IDC_BUTTON2, WM_SETFONT, (WPARAM)m_SettingzBtnFont, 0);

	SendDlgItemMessage(m_hWnd, IDC_SLIDER1, TBM_SETRANGE, TRUE, MAKELPARAM(0, 100));


	SendDlgItemMessage(m_hWnd, IDC_SLIDER1, TBM_SETPOS, (WPARAM)80,0 );

	SetFocus(GetDlgItem(m_hWnd, IDC_BUTTON1));

	PostMessage(m_hWnd, WM_Z_ORDER_REPAIR, 0, 0);

	int sliderValue = (int)SendDlgItemMessage(m_hWnd, IDC_SLIDER1, TBM_GETPOS, 0, 0);


	double value = std::pow(2.0, (sliderValue / (100.0 / 12.0)) - 12.0) * 100.0;

	wostringstream oss;  // usa wostringstream invece di ostringstream
	oss << std::fixed << std::setprecision(2) << value << L"%";
	SetDlgItemTextW(m_hWnd, IDC_PERCENTAGE, oss.str().c_str());

	return true;
}

void MainDialog::OnTrackBarScroll()
{
	int sliderValue = SendDlgItemMessage(m_hWnd, IDC_SLIDER1, TBM_GETPOS, 0, 0);

	double value = std::pow(2.0, (sliderValue / (100.0 / 12.0)) - 12.0) * 100.0;

	wostringstream oss;  // usa wostringstream invece di ostringstream
	oss << std::fixed << std::setprecision(2) << value << L"%";
	SetDlgItemTextW(m_hWnd, IDC_PERCENTAGE, oss.str().c_str());
}

void MainDialog::OnDoItButtonClicked()
{
	int sliderValue = (int)SendDlgItemMessage(m_hWnd, IDC_SLIDER1, TBM_GETPOS, 0, 0);

	EndDialog(m_hWnd, IDOK); 
	PlizWaitDialog plizWaitDlg(m_hInstance, pow(2, (sliderValue / (100.0 / 12)) - 12));
	plizWaitDlg.Show(m_hWnd);
}

void MainDialog::OnSettingzButtonClicked()
{

	SettingzDialog settingzDlg(m_hInstance);
	settingzDlg.Show(m_hWnd);
}
