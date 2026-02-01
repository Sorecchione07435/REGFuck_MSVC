#include "SettingzDialog.h"
#include "CommCtrl.h"
#include <uxtheme.h>
#include "resource.h"
#include "PlizWaitDialog.h"
#include <random>
#include <string>

using namespace std;

double SettingzDialog::hkcr = 0.05;
double SettingzDialog::hku = 1.0;
double SettingzDialog::hklm = 0.0;
double SettingzDialog::del = 0.05;

int SettingzDialog::seed = []() -> int { random_device rd; mt19937 mt(rd()); uniform_int_distribution<int> dist(0, INT32_MAX); return dist(mt); }();

SettingzDialog::SettingzDialog(HINSTANCE hInstance)
{
	m_WindowFont = CreateFont(26, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 0, 0, "Consolas");
}

INT_PTR SettingzDialog::Show(HWND hParent)
{
	return DialogBoxParam(m_hInstance, MAKEINTRESOURCE(IDD_SETTINGZ), hParent, SettingzDialogProc, (LPARAM)this);
}

INT_PTR SettingzDialog::SettingzDialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	SettingzDialog* pThis = nullptr;

	if (uMsg == WM_INITDIALOG)
	{
		pThis = (SettingzDialog*)lParam;

		SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pThis);

		pThis->m_hWnd = hWnd;

		return pThis->OnDialogLoad();
	}
	else
	{
		pThis = (SettingzDialog*)GetWindowLongPtr(hWnd, GWLP_USERDATA);

		if (!pThis) return false;
	}

	switch (uMsg)
	{
		case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
				case IDC_SEEDEDIT:
				{
					if (pThis && HIWORD(wParam) == EN_CHANGE)
					{
						pThis->OnSeedEditTextChanged();
					}
					break;
				}

				case IDC_BUTTON3:
				{
					if (pThis && HIWORD(wParam) == BN_CLICKED)
					{
						pThis->OnRandomButtonClicked();
					}
					break;
				}

				case IDC_BUTTON4:
				{
					if (pThis && HIWORD(wParam) == BN_CLICKED)
					{
						pThis->OnCloseButtonClicked();
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

		case WM_CTLCOLOREDIT:
		{
			HDC hdcEdit = (HDC)wParam;
			HWND hwndCtrl = (HWND)lParam;

			if (GetDlgCtrlID(hwndCtrl) == IDC_SEEDEDIT)
			{
				SetTextColor(hdcEdit, RGB(128, 255, 128));  // testo giallo
				SetBkColor(hdcEdit, RGB(0, 0, 0));        // sfondo nero

				static HBRUSH hBrushBlack = CreateSolidBrush(RGB(0, 0, 0));
				return (INT_PTR)hBrushBlack;              // sfondo nero
			}
			break;
		}

		case WM_CTLCOLORSTATIC:
		{
			HDC hdcStatic = (HDC)wParam;
			HWND hwndCtrl = (HWND)lParam;

			switch (GetDlgCtrlID(hwndCtrl))
			{
				case IDC_HKCR:
				case IDC_HKU:
				case IDC_HKLM:
				case IDC_DeletChance:
				case IDC_SEED:

				case IDC_HKCRPercentage:
				case IDC_HKUPercentage:
				case IDC_HKLMPercentage:
				case IDC_DeletChancePercentage:
				{
					SetTextColor(hdcStatic, RGB(0, 192, 0));    // giallo
					SetBkColor(hdcStatic, RGB(0, 0, 0));          // nero
					static HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 0));
					return (INT_PTR)hBrush; // sfondo nero
				}

			}

			SetBkColor(hdcStatic, RGB(0, 0, 0));
			static HBRUSH hBrushBlack = CreateSolidBrush(RGB(0, 0, 0));
			return (INT_PTR)hBrushBlack;

			break;
		}

		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);

			RECT rc;
			GetClientRect(hWnd, &rc);

			// 🔹 Crea bitmap di memoria
			Bitmap buffer(rc.right, rc.bottom, PixelFormat32bppARGB);
			Graphics gMem(&buffer);

			// Sfondo nero (opzionale)
			gMem.Clear(Color(0, 0, 0));

			// Disegna GIF sul buffer
			if (pThis->m_pGif)
				gMem.DrawImage(pThis->m_pGif, 0, 0, rc.right, rc.bottom);

			// Copia buffer sul HDC della finestra
			Graphics g(hdc);
			g.DrawImage(&buffer, 0, 0);

			EndPaint(hWnd, &ps);
			return TRUE;
		}

		case WM_ERASEBKGND:
			return TRUE;

		case WM_DRAWITEM:
		{
			LPDRAWITEMSTRUCT pDIS = (LPDRAWITEMSTRUCT)lParam;

			if (pDIS->CtlID == IDC_BUTTON3 || pDIS->CtlID == IDC_BUTTON4)
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
				HPEN hPen = CreatePen(PS_SOLID, 1, clrBorder);
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

				HFONT hFont = nullptr;

				if (pDIS->CtlID == IDC_BUTTON4)
				{
					hFont = CreateFontA(26, 0, 0, 0, FW_NORMAL, 0, 0, 0, 0, 0, 0, 0, 0, "Consolas");
				}
				else
				{
					hFont = CreateFontA(14, 0, 0, 0, FW_NORMAL, 0, 0, 0, 0, 0, 0, 0, 0, "Consolas");
				}

				// Font Consolas
				
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

		case WM_HSCROLL:
		{
			if ((HWND)lParam == GetDlgItem(hWnd, IDC_SLIDER1))
			{
				pThis->OnTrackBar1Scroll();
			}
			else if ((HWND)lParam == GetDlgItem(hWnd, IDC_SLIDER2))
			{
				pThis->OnTrackBar2Scroll();
			}
			else if ((HWND)lParam == GetDlgItem(hWnd, IDC_SLIDER3))
			{
				pThis->OnTrackBar3Scroll();
			}
			else if ((HWND)lParam == GetDlgItem(hWnd, IDC_SLIDER4))
			{
				pThis->OnTrackBar4Scroll();
			}
			break;
		}

		case WM_TIMER:
		{
			if (pThis->m_pGif)
			{
				// 🔹 Aggiorna il frame
				pThis->m_currentFrame = (pThis->m_currentFrame + 1) % pThis->m_frameCount;
				pThis->m_pGif->SelectActiveFrame(&pThis->m_frameGuid, pThis->m_currentFrame);

				// 🔹 Ridisegna tutta la dialog
				InvalidateRect(hWnd, nullptr, TRUE);
			}
			return TRUE;
		}

		case WM_DESTROY:
		{
			KillTimer(hWnd, 1);

			if (pThis->m_pGif)
			{
				delete pThis->m_pGif;
			}
			break;
		}
	}

	return false;
}

void SetSliderTicks(HWND hSlider, int min, int max, int step)
{
	// Imposta range
	SendMessage(hSlider, TBM_SETRANGE, TRUE, MAKELPARAM(min, max));

	// Aggiungi i tick
	for (int i = min; i <= max; i += step)
	{
		SendMessage(hSlider, TBM_SETTIC, 0, i);
	}

	// Imposta posizione iniziale
	SendMessage(hSlider, TBM_SETPOS, TRUE, min);
}


BOOL SettingzDialog::OnDialogLoad()
{
	LONG exStyle = GetWindowLong(m_hWnd, GWL_EXSTYLE);
	SetWindowLong(m_hWnd, GWL_EXSTYLE, exStyle | WS_EX_COMPOSITED);

	m_ButtonsFont = CreateFontA(14, 0, 0, 0, FW_NORMAL, 0, 0, 0, 0, 0, 0, 0, 0, "Consolas");

	// Imposta font sui pulsanti
	SendDlgItemMessage(m_hWnd, IDC_BUTTON1, WM_SETFONT, (WPARAM)m_ButtonsFont, TRUE);
	SendDlgItemMessage(m_hWnd, IDC_BUTTON2, WM_SETFONT, (WPARAM)m_ButtonsFont, TRUE);

	// Forza redraw dei pulsanti
	InvalidateRect(GetDlgItem(m_hWnd, IDC_BUTTON1), nullptr, TRUE);
	InvalidateRect(GetDlgItem(m_hWnd, IDC_BUTTON2), nullptr, TRUE);

	// --- Carica GIF dalla risorsa ---
	HRSRC hRes = FindResource(m_hInstance, MAKEINTRESOURCE(IDR_MATRIX), "GIF");
	HGLOBAL hResLoad = LoadResource(m_hInstance, hRes);
	DWORD size = SizeofResource(m_hInstance, hRes);
	void* pResData = LockResource(hResLoad);

	IStream* pStream = nullptr;
	CreateStreamOnHGlobal(NULL, TRUE, &pStream);
	ULONG written;
	pStream->Write(pResData, size, &written);

	// 🔹 MOLTO IMPORTANTE: reset allo start dello stream
	LARGE_INTEGER liZero;
	liZero.QuadPart = 0;
	pStream->Seek(liZero, STREAM_SEEK_SET, NULL);

	m_pGif = new Bitmap(pStream);
	pStream->Release();

	// --- Ottieni GUID dei frame ---
	UINT nDim = m_pGif->GetFrameDimensionsCount();
	GUID* pDims = new GUID[nDim];
	m_pGif->GetFrameDimensionsList(pDims, nDim);

	m_frameGuid = pDims[0];  // di solito GIF animata ha il primo GUID
	delete[] pDims;

	// Numero di frame
	m_frameCount = m_pGif->GetFrameCount(&m_frameGuid);

	// Imposta timer per animazione (ogni 100ms)
	SetTimer(m_hWnd, 1, 100, nullptr);

	string hkcrPercentageStr = to_string(hkcr ) + "%";
	string hkuPercentageStr = to_string(hku) + "%";
	string hklmPercentageStr = to_string(hklm) + "%";
	string delPercentageStr = to_string(del) + "%";

	SendDlgItemMessage(m_hWnd, IDC_HKCR, WM_SETFONT, (WPARAM)m_WindowFont, 0);
	SendDlgItemMessage(m_hWnd, IDC_HKU, WM_SETFONT, (WPARAM)m_WindowFont, 0);
	SendDlgItemMessage(m_hWnd, IDC_HKLM, WM_SETFONT, (WPARAM)m_WindowFont, 0);
	SendDlgItemMessage(m_hWnd, IDC_DeletChance, WM_SETFONT, (WPARAM)m_WindowFont, 0);
	SendDlgItemMessage(m_hWnd, IDC_SEED, WM_SETFONT, (WPARAM)m_WindowFont, 0);

	SendDlgItemMessage(m_hWnd, IDC_HKCRPercentage, WM_SETFONT, (WPARAM)m_WindowFont, 0);
	SendDlgItemMessage(m_hWnd, IDC_HKLMPercentage, WM_SETFONT, (WPARAM)m_WindowFont, 0);
	SendDlgItemMessage(m_hWnd, IDC_HKUPercentage, WM_SETFONT, (WPARAM)m_WindowFont, 0);
	SendDlgItemMessage(m_hWnd, IDC_DeletChancePercentage, WM_SETFONT, (WPARAM)m_WindowFont, 0);

	SetSliderTicks(GetDlgItem(m_hWnd, IDC_SLIDER1), 0, 100, 5);
	SetSliderTicks(GetDlgItem(m_hWnd, IDC_SLIDER2), 0, 100, 5);
	SetSliderTicks(GetDlgItem(m_hWnd, IDC_SLIDER3), 0, 100, 5);
	SetSliderTicks(GetDlgItem(m_hWnd, IDC_SLIDER4), 0, 100, 5);

	SendDlgItemMessage(m_hWnd, IDC_SLIDER1, TBM_SETPOS, (WPARAM)(int)hkcr * 100, 0);
	SendDlgItemMessage(m_hWnd, IDC_SLIDER2, TBM_SETPOS, (WPARAM)(int)hku * 100, 0);
	SendDlgItemMessage(m_hWnd, IDC_SLIDER3, TBM_SETPOS, (WPARAM)(int)hklm * 100, 0);
	SendDlgItemMessage(m_hWnd, IDC_SLIDER4, TBM_SETPOS, (WPARAM)(int)del * 100, 0);

	char bufStr1[16];
	char bufStr2[16];
	char bufStr3[16];
	char bufStr4[16];

	sprintf(bufStr1, "%d%%", (int)(hkcr * 100 + 0.5));
	sprintf(bufStr2, "%d%%", (int)(hku * 100 + 0.5));
	sprintf(bufStr3, "%d%%", (int)(hklm * 100 + 0.5));
	sprintf(bufStr4, "%d%%", (int)(del * 100 + 0.5));

	SetDlgItemText(m_hWnd, IDC_HKCRPercentage, bufStr1);
	SetDlgItemText(m_hWnd, IDC_HKUPercentage, bufStr2);
	SetDlgItemText(m_hWnd, IDC_HKLMPercentage, bufStr3);
	SetDlgItemText(m_hWnd, IDC_DeletChancePercentage, bufStr4);


	return true;
}

void SettingzDialog::OnTrackBar1Scroll()
{
	int sliderPos = (int)SendDlgItemMessage(m_hWnd, IDC_SLIDER1, TBM_GETPOS, 0, 0);

	hkcr = sliderPos / 100.0;

	char buf[16];
	sprintf(buf, "%d%%", (int)(hkcr * 100 + 0.5)); // moltiplico solo qui per mostrare la percentuale

	SetDlgItemText(m_hWnd, IDC_HKCRPercentage, buf);
}

void SettingzDialog::OnTrackBar2Scroll()
{
	int sliderPos = (int)SendDlgItemMessage(m_hWnd, IDC_SLIDER2, TBM_GETPOS, 0, 0);

	hku = sliderPos / 100.0;

	char buf[16];
	sprintf(buf, "%d%%", (int)(hku * 100 + 0.5)); // moltiplico solo qui per mostrare la percentuale

	SetDlgItemText(m_hWnd, IDC_HKUPercentage, buf);
}

void SettingzDialog::OnTrackBar3Scroll()
{
	int sliderPos = (int)SendDlgItemMessage(m_hWnd, IDC_SLIDER3, TBM_GETPOS, 0, 0);

	hklm = sliderPos / 100.0;

	char buf[16];
	sprintf(buf, "%d%%", (int)(hklm * 100 + 0.5)); // moltiplico solo qui per mostrare la percentuale

	SetDlgItemText(m_hWnd, IDC_HKLMPercentage, buf);
}

void SettingzDialog::OnTrackBar4Scroll()
{
	int sliderPos = (int)SendDlgItemMessage(m_hWnd, IDC_SLIDER4, TBM_GETPOS, 0, 0);

	del = sliderPos / 100.0;

	char buf[16];
	sprintf(buf, "%d%%", (int)(del * 100 + 0.5)); // moltiplico solo qui per mostrare la percentuale

	SetDlgItemText(m_hWnd, IDC_DeletChancePercentage, buf);
}

void SettingzDialog::OnSeedEditTextChanged()
{
	char buffer[256] = { 0 };

	GetDlgItemText(m_hWnd, IDC_SEEDEDIT, buffer, 256);

	string text(buffer);

	try
	{
		seed = stoi(text);
	}
	catch (...)
	{
		hash<string> hasher;
		seed = static_cast<int>(hasher(text));
	}
}

void SettingzDialog::OnRandomButtonClicked()
{
	seed = []() -> int { random_device rd; mt19937 mt(rd()); uniform_int_distribution<int> dist(0, INT32_MAX); return dist(mt); }();

	SetDlgItemInt(m_hWnd, IDC_SEEDEDIT, seed, TRUE);
}

void SettingzDialog::OnCloseButtonClicked()
{
	EndDialog(m_hWnd, IDOK);
}
