#include <windows.h>
#include <bits/stdc++.h>
#include <commctrl.h>
#include "main.h"
using namespace std;
HWND hwnd;
string codealltmp = "";
int wordsizepos = 4;
int wsizes[13] = {4,8,12,16,20,22,24,30,36,48,60,72,96};
string fontname = "Inconsolata";

BOOL runprocess(char szCommandLine[], int fwait, int fshow) {
	BOOL ret = system(szCommandLine);
	
	/*
	if (fshow && (!fwait)) {
		::ShellExecute(hwnd, "open", szCommandLine, NULL, NULL, SW_SHOWMAXIMIZED);
		return 0;
	}
	STARTUPINFO si = { sizeof(si) };
	PROCESS_INFORMATION  pi; 
	memset(&si, 0, sizeof(si));
	memset(&pi, 0, sizeof(pi));
	si.wShowWindow = fshow?TRUE:FALSE;
	si.dwFlags = STARTF_USESHOWWINDOW;
	MessageBox(0, szCommandLine, "Caption", 0);
	BOOL ret = ::CreateProcess(NULL, szCommandLine, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
	//if (fwait) {
		BOOL wret2 = ::WaitForSingleObject(pi.hProcess, INFINITE);
	//}
	char errorname[1000];
	sprintf(errorname, "ERROR:ret=%d; LastError=%d; WaitRet=%d", ret, GetLastError(), wret2);
	MessageBox(0, errorname, "", 0);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	*/
	return ret;
}

bool fsaved=0, fopend=0, fcompiled=0;
bool programmeexiterrorstatusflag = 1;
unsigned long long variMsgCnt = 0;
HINSTANCE g_hInst;
char szFileName[MAX_PATH]="Untitled";
HWND g_hStatusBar;

string output_time() {
	time_t rawtime;
   	time(&rawtime);   
   	char pblgtime[20];
	strftime(pblgtime, 20, "%Y-%m-%d %H-%M", localtime(&rawtime));
	string tttmps="";
	tttmps.insert(0, pblgtime);
	return tttmps;
}

BOOL LoadFile(HWND hEdit, LPSTR pszFileName) {
	HANDLE hFile;
	BOOL bSuccess = FALSE;

	hFile = CreateFile(pszFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, 0);
	if(hFile != INVALID_HANDLE_VALUE) {
		DWORD dwFileSize;
		dwFileSize = GetFileSize(hFile, NULL);
		if(dwFileSize != 0xFFFFFFFF) {
			LPSTR pszFileText;
			pszFileText = (LPSTR)GlobalAlloc(GPTR, dwFileSize + 1);
			if(pszFileText != NULL) {
				DWORD dwRead;
				if(ReadFile(hFile, pszFileText, dwFileSize, &dwRead, NULL)) {
					pszFileText[dwFileSize] = 0; // Null terminator
					if(SetWindowText(hEdit, pszFileText)) {
						bSuccess = TRUE; // It worked!
						fopend=1;
						SendMessage(g_hStatusBar, SB_SETTEXT, 4, (LPARAM)szFileName); 
					}
				}
				GlobalFree(pszFileText);
			}
		}
		CloseHandle(hFile);
	}
	return bSuccess;
}

BOOL SaveFile(HWND hEdit, LPSTR pszFileName) {
	HANDLE hFile;
	BOOL bSuccess = FALSE;

	hFile = CreateFile(pszFileName, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if(hFile != INVALID_HANDLE_VALUE) {
		DWORD dwTextLength;
		dwTextLength = GetWindowTextLength(hEdit);
		if(dwTextLength > 0) {
			LPSTR pszText;
			pszText = (LPSTR)GlobalAlloc(GPTR, dwTextLength + 1);
			if(pszText != NULL) {
				if(GetWindowText(hEdit, pszText, dwTextLength + 1)) {
					DWORD dwWritten;
					if(WriteFile(hFile, pszText, dwTextLength, &dwWritten, NULL)) {
						bSuccess = TRUE;
						fsaved=1;
					}
				}
				GlobalFree(pszText);
			}
		}
		CloseHandle(hFile);
	}
	return bSuccess;
}

BOOL DoFileOpenSave(HWND hwnd, BOOL bSave) {
	OPENFILENAME ofn;

	ZeroMemory(&ofn, sizeof(ofn));
	szFileName[0] = 0;

	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFilter = (bSave ? "C++ Files (*.cpp; *.c++; *.cxx)\0*.cpp;*.c++;*.cxx\0C++ Header Files (*.hpp)\0*.hpp\0Pascal Files (*.pp)\0*.pp\0Windows Batching Files (*.bat; *.com; *.cmd)\0*.bat;*.com;*.cmd\0All Files (*.*)\0*.*\0\0" : "C++ Files (*.cpp; *.c++; *.cxx)\0*.cpp;*.c++;*.cxx\0C++ Header Files (*.hpp)\0*.hpp\0Pascal Files (*.pp)\0*.pp\0Windows Batching Files (*.bat; *.com; *.cmd)\0*.bat;*.com;*.cmd\0ClickIDE Temporary Compilation Logs\0*_compile_tmp.log\0All Files (*.*)\0*.*\0\0");
	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrDefExt = "";

	if(bSave) {
		ofn.Flags = OFN_EXPLORER|OFN_PATHMUSTEXIST|OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT;
		if(GetSaveFileName(&ofn)) {
			if(!SaveFile(GetDlgItem(hwnd, IDC_MAIN_TEXT), szFileName)) {
				MessageBox(hwnd, "Save file failed.\n(Or this is an empty file.)", "Error",MB_OK|MB_ICONEXCLAMATION);
				fsaved=0;
				return FALSE;
			}
		}
	} else {
		ofn.Flags = OFN_EXPLORER|OFN_FILEMUSTEXIST|OFN_HIDEREADONLY;
		if(GetOpenFileName(&ofn)) {
			if(!LoadFile(GetDlgItem(hwnd, IDC_MAIN_TEXT), szFileName)) {
				MessageBox(hwnd, "Load of file failed.", "Error",MB_OK|MB_ICONEXCLAMATION);
				fopend=0;
				return FALSE;
			}
		}
	}
	return TRUE;
}

string getpasfn (char yufn[]) {
	string rtttmp = "";
	for (int i = 0; i < strlen(yufn) - 3; i++) {
		rtttmp += yufn[i];
	}
	return rtttmp;
}
string getcppfn (char yufn[]) {
	string rtttmp = "";
	for (int i = 0; i < strlen(yufn) - 4; i++) {
		rtttmp += yufn[i];
	}
	return rtttmp;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	HMENU hMenu, hFileMenu, hCompileMenu;
	ifstream wndfin;
	bool GHELPEXITFLAG = 0;
	string errreporti="";
	string titlestr01="Click 4.6 ";
	int errreportcnt = 0;
	char cmdbuf1[MAX_PATH+40];
	char cmdbuf2[MAX_PATH+40];
	char cmdbuf3[MAX_PATH+40];
	char cmdbuf4[MAX_PATH+40];
	char cmdbuf5[MAX_PATH+40];
	int iStatusWidths[] = {100, 230, 300, 320, -1};
	RECT rectStatus;
	bool isycl = 0;
	bool iszfc = 0;
	bool islfst = 1;
	bool nlycl = 0;
	bool issgzs = 0; /*//*/ //a
	bool ismtzs = 0;
	bool dontout = 0;
	ofstream fout;
	HFONT hFont;
	switch(Message) {
		case WM_CREATE:
			CreateWindow("EDIT", "",WS_CHILD|WS_VISIBLE|WS_HSCROLL|WS_VSCROLL|ES_MULTILINE|ES_WANTRETURN,CW_USEDEFAULT, CW_USEDEFAULT, 1000/*CW_USEDEFAULT*/, CW_USEDEFAULT,hwnd, (HMENU)IDC_MAIN_TEXT, GetModuleHandle(NULL), NULL);
			/*4.7*/hFont = CreateFont(wsizes[wordsizepos],0,0,0,0,FALSE,FALSE,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH|FF_SWISS,fontname.c_str());//��������
			    
			/*4.7*/SendDlgItemMessage(hwnd, IDC_MAIN_TEXT, WM_SETFONT,(WPARAM)hFont/*GetStockObject(DEFAULT_GUI_FONT)*/, MAKELPARAM(TRUE,0));
			
			/*3.10*/
			g_hStatusBar = CreateWindowEx(0, STATUSCLASSNAME, NULL,
			WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP, 0, 0, 0, 0,
			hwnd, (HMENU)ID_STATUSBAR, g_hInst, NULL);
			
			SendMessage(g_hStatusBar, SB_SETPARTS, 5, (LPARAM)iStatusWidths);
			SendMessage(g_hStatusBar, SB_SETTEXT, 0, (LPARAM)"Click 4.6 IDE"); 
			SendMessage(g_hStatusBar, SB_SETTEXT, 1, (LPARAM)"..."); 
			SendMessage(g_hStatusBar, SB_SETTEXT, 2, (LPARAM)(fcompiled ? "�ѱ���" : "δ����")); 
			SendMessage(g_hStatusBar, SB_SETTEXT, 3, (LPARAM)""); 
			SendMessage(g_hStatusBar, SB_SETTEXT, 4, (LPARAM)szFileName); 
			/*--3.10*/
			break;
		case WM_SIZE:
			if(wParam != SIZE_MINIMIZED) {
				MoveWindow(GetDlgItem(hwnd, IDC_MAIN_TEXT), 0, 0, LOWORD(lParam),HIWORD(lParam), TRUE);
		    }
			SendMessage(g_hStatusBar, WM_SIZE, 0, 0);
			GetWindowRect(g_hStatusBar, &rectStatus);
			break;
		case WM_SETFOCUS:
			SetFocus(GetDlgItem(hwnd, IDC_MAIN_TEXT));
			break;
		case WM_COMMAND:
			switch(LOWORD(wParam)) {
				case CM_FILE_OPEN:
					if (MessageBox (0, " If you open a new file, the unsaved contents will be lost!\n Sure to continue?", "Warning!", MB_YESNO | MB_ICONWARNING) != IDYES) {
						break;
					}
					/*settitle*/ 
					titlestr01="Click 4.6 [ Opening... ]";
					SetWindowText (hwnd, titlestr01.c_str());
					SendMessage(g_hStatusBar, SB_SETTEXT, 1, (LPARAM)"Opening..."); 
					/*end:settitle*/ 
					DoFileOpenSave(hwnd, FALSE);
					fcompiled=0;
					/*settitle*/ 
					titlestr01="Click 4.6 [ ";
					titlestr01+=szFileName;
					titlestr01+=" ]";
					SetWindowText (hwnd, titlestr01.c_str());
					SendMessage(g_hStatusBar, SB_SETTEXT, 2, (LPARAM)(fcompiled ? "�ѱ���" : "δ����")); 
					SendMessage(g_hStatusBar, SB_SETTEXT, 1, (LPARAM)"..."); 
					/*end:settitle*/ 
					break;
				case CM_WLARGE: {
					if (wordsizepos >= 11) {
						MessageBox(hwnd, "�Ѿ���������壡", "", MB_OK);
						break;
					}
					/*4.7*/hFont = CreateFont(wsizes[++wordsizepos],0,0,0,0,FALSE,FALSE,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH|FF_SWISS,fontname.c_str());//��������
					/*4.7*/SendDlgItemMessage(hwnd, IDC_MAIN_TEXT, WM_SETFONT,(WPARAM)hFont/*GetStockObject(DEFAULT_GUI_FONT)*/, MAKELPARAM(TRUE,0));
					break;
				}
				case CM_WSMALL: {
					if (wordsizepos <= 0) {
						MessageBox(hwnd, "�Ѿ�����С���壡", "", MB_OK);
						break;
					}
					/*4.7*/hFont = CreateFont(wsizes[--wordsizepos],0,0,0,0,FALSE,FALSE,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH|FF_SWISS,fontname.c_str());//��������
					/*4.7*/SendDlgItemMessage(hwnd, IDC_MAIN_TEXT, WM_SETFONT,(WPARAM)hFont/*GetStockObject(DEFAULT_GUI_FONT)*/, MAKELPARAM(TRUE,0));
					break;
				}
				case CM_CFONT: {
					if (fontname == "Inconsolata") {
						fontname = "Consolas";
					} else {
						fontname = "Inconsolata";
					}
					/*4.7*/hFont = CreateFont(wsizes[wordsizepos],0,0,0,0,FALSE,FALSE,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH|FF_SWISS,fontname.c_str());//��������
					/*4.7*/SendDlgItemMessage(hwnd, IDC_MAIN_TEXT, WM_SETFONT,(WPARAM)hFont/*GetStockObject(DEFAULT_GUI_FONT)*/, MAKELPARAM(TRUE,0));
					break;
				}
				case CM_FILE_SAVEAS:
					/*settitle*/ 
					titlestr01="Click 4.6 [ Saving... ]";
					SetWindowText (hwnd, titlestr01.c_str());
					SendMessage(g_hStatusBar, SB_SETTEXT, 1, (LPARAM)"Saving..."); 
					/*end:settitle*/ 
					DoFileOpenSave(hwnd, TRUE);
					/*settitle*/ 
					titlestr01="Click 4.6 [ ";
					titlestr01+=szFileName;
					titlestr01+=" ]";
					SetWindowText (hwnd, titlestr01.c_str());
					SendMessage(g_hStatusBar, SB_SETTEXT, 1, (LPARAM)"..."); 
					/*end:settitle*/ 
					break;
				case CM_FILE_EXIT:
					PostMessage(hwnd, WM_CLOSE, 0, 0);
					break;
				case CM_FILE_SAVE:
					/*settitle*/ 
					titlestr01="Click 4.6 [ Saving... ]";
					SetWindowText (hwnd, titlestr01.c_str());
					SendMessage(g_hStatusBar, SB_SETTEXT, 1, (LPARAM)"Saving..."); 
					/*end:settitle*/ 
					if ((!fsaved && !fopend) || strcmp(szFileName, "Untitled") == 0) {
						DoFileOpenSave(hwnd, TRUE);
					} else {
						if(!SaveFile(GetDlgItem(hwnd, IDC_MAIN_TEXT), szFileName)) {
							MessageBox(hwnd, "Save file failed.\n(Or this is an empty file.)", "Error",MB_OK|MB_ICONEXCLAMATION);
							fsaved=0;
						}
					}
					/*settitle*/ 
					titlestr01="Click 4.6 [ ";
					titlestr01+=szFileName;
					titlestr01+=" ]";
					SendMessage(g_hStatusBar, SB_SETTEXT, 1, (LPARAM)"..."); 
					SetWindowText (hwnd, titlestr01.c_str());
					/*end:settitle*/ 
					break;
				case CM_ABOUT:
					/*settitle*/ 
					SetWindowText (hwnd, "Click 4.6 [ About... ]");
					SendMessage(g_hStatusBar, SB_SETTEXT, 1, (LPARAM)"About..."); 
					/*end:settitle*/ 
					MessageBox (NULL, "Click IDE: 2020.4\nVersion: 4.6.0-Insider\nBy: ������ѧ Eric �����.\nHelp: Win32 API.\nIntroduction: Click is an light, open-source, convenient C++/Pascal/Bat IDE which based on GNU MinGW/ Free Pascal/ Windows.\nOnly for: Windows.\nLicense: Apache License, Version 2.0\nTo learn more or get updates, please visit our official website: ericnth.cn/clickide" , "About...", 0);
					/*settitle*/ 
					titlestr01="Click 4.6 [ ";
					titlestr01+=szFileName;
					titlestr01+=" ]";
					SendMessage(g_hStatusBar, SB_SETTEXT, 1, (LPARAM)"..."); 
					SetWindowText (hwnd, titlestr01.c_str());
					/*end:settitle*/ 
					break;
				case CM_RUN:
					if (fcompiled) {
					/*settitle*/ 
					titlestr01="Click 4.6 [ Running ] [ ";
					titlestr01+=szFileName;
					titlestr01+=" ]";
					SendMessage(g_hStatusBar, SB_SETTEXT, 1, (LPARAM)"Running..."); 
					SetWindowText (hwnd, titlestr01.c_str());
					/*end:settitle*/ 
					sprintf (cmdbuf2, "start \"Click4.6-Executing [%s.exe]\" /max %s.exe",getcppfn(szFileName).c_str(),getcppfn(szFileName).c_str());
						runprocess (cmdbuf2, 0, 1);
					} else {
						MessageBox (NULL, "You haven't compiled this file yet (or have failed in it),\nPlease compile it first!", "Can't Run!", MB_OK | MB_ICONERROR);
					}
					/*settitle*/ 
					titlestr01="Click 4.6 [ ";
					titlestr01+=szFileName;
					titlestr01+=" ]";
					SendMessage(g_hStatusBar, SB_SETTEXT, 1, (LPARAM)"..."); 
					SetWindowText (hwnd, titlestr01.c_str());
					/*end:settitle*/ 
					break;
				case CM_RUNPAS:
					if (fcompiled) {
					/*settitle*/ 
					titlestr01="Click 4.6 [ Running ] [ ";
					titlestr01+=szFileName;
					titlestr01+=" ]";
					SendMessage(g_hStatusBar, SB_SETTEXT, 1, (LPARAM)"Running..."); 
					SetWindowText (hwnd, titlestr01.c_str());
					/*end:settitle*/ 
						sprintf (cmdbuf2, "start \"Click4.6-Executing [%s.exe]\" /max %s.exe",getpasfn(szFileName).c_str(),getpasfn(szFileName).c_str());
						runprocess (cmdbuf2, 0, 1);
					} else {
						MessageBox (NULL, "You haven't compiled this file yet (or have failed in it),\nPlease compile it first!", "Can't Run!", MB_OK | MB_ICONERROR);
					}
					/*settitle*/ 
					titlestr01="Click 4.6 [ ";
					titlestr01+=szFileName;
					titlestr01+=" ]";
					SendMessage(g_hStatusBar, SB_SETTEXT, 1, (LPARAM)"..."); 
					SetWindowText (hwnd, titlestr01.c_str());
					/*end:settitle*/ 
					break;
				case CM_COMPILE:
					SendMessage(g_hStatusBar, SB_SETTEXT, 1, (LPARAM)"Compiling..."); 
					if ((!fsaved && !fopend) || strcmp(szFileName, "") == 0) {
						DoFileOpenSave(hwnd, TRUE);
					} else {
						if(!SaveFile(GetDlgItem(hwnd, IDC_MAIN_TEXT), szFileName)) {
							MessageBox(hwnd, "Save file failed.", "Error",MB_OK|MB_ICONEXCLAMATION);
							fsaved=0;
						}
					}
					/*settitle*/ 
					titlestr01="Click 4.6 [ Compiling ] [ ";
					titlestr01+=szFileName;
					titlestr01+=" ]";
					SetWindowText (hwnd, titlestr01.c_str());
					/*end:settitle*/ 
					sprintf (cmdbuf1, "g++.exe \"%s\" -o \"%s.exe\" 2> %s_compile_tmp.log",szFileName,getcppfn(szFileName).c_str(),szFileName);
					sprintf (cmdbuf2, "start \"Click4.6-Executing [%s.exe]\" /max %s.exe",getcppfn(szFileName).c_str(),getcppfn(szFileName).c_str());
					sprintf (cmdbuf3, "del \"%s.exe\"",getcppfn(szFileName).c_str());
					sprintf (cmdbuf4, "del \"%s_compile_tmp.log\"",szFileName);
					sprintf (cmdbuf5, "%s_compile_tmp.log",szFileName);
					runprocess (cmdbuf3, 1, 0);
					runprocess (cmdbuf1, 1, 0);
					wndfin.open (cmdbuf5);
					while (wndfin) {
						errreportcnt++;
						errreporti += wndfin.get();
					}
					wndfin.close();
					if (errreportcnt>1) {
						MessageBox (NULL, errreporti.c_str(), "Click 4.6: Compile Error", MB_OK);
						break;
						fcompiled=0; 
					} else {
						fcompiled=1;
					}
					runprocess (cmdbuf4, 1, 0);
					/*settitle*/ 
					titlestr01="Click 4.6 [ ";
					titlestr01+=szFileName;
					titlestr01+=" ]";
					SendMessage(g_hStatusBar, SB_SETTEXT, 1, (LPARAM)"..."); 
					SetWindowText (hwnd, titlestr01.c_str());
					SendMessage(g_hStatusBar, SB_SETTEXT, 2, (LPARAM)(fcompiled ? "�ѱ���" : "δ����")); 
					/*end:settitle*/ 
					break;
				case CM_COMPILERUN:
					SendMessage(g_hStatusBar, SB_SETTEXT, 1, (LPARAM)"Compiling..."); 
					if ((!fsaved && !fopend) || strcmp(szFileName, "") == 0) {
						DoFileOpenSave(hwnd, TRUE);
					} else {
						if(!SaveFile(GetDlgItem(hwnd, IDC_MAIN_TEXT), szFileName)) {
							MessageBox(hwnd, "Save file failed.", "Error",MB_OK|MB_ICONEXCLAMATION);
							fsaved=0;
						}
					}
					/*settitle*/ 
					titlestr01="Click 4.6 [ Compiling ] [ ";
					titlestr01+=szFileName;
					titlestr01+=" ]";
					SetWindowText (hwnd, titlestr01.c_str());
					/*end:settitle*/ 
					sprintf (cmdbuf1, "g++ \"%s\" -o \"%s.exe\" 2> %s_compile_tmp.log",szFileName,getcppfn(szFileName).c_str(),szFileName);
					sprintf (cmdbuf2, "start \"Click4.6-Executing [%s.exe]\" /max %s.exe",getcppfn(szFileName).c_str(),getcppfn(szFileName).c_str());
					sprintf (cmdbuf3, "del \"%s.exe\"",getcppfn(szFileName).c_str());
					sprintf (cmdbuf4, "del \"%s_compile_tmp.log\"",szFileName);
					sprintf (cmdbuf5, "%s_compile_tmp.log",szFileName);
					runprocess (cmdbuf3, 1, 0);
					errreportcnt = 0;
					runprocess (cmdbuf1, 1, 0);
					wndfin.open (cmdbuf5);
					while (wndfin) {
						errreportcnt++;
						errreporti += wndfin.get();
					}
					wndfin.close();
					if (errreportcnt>1) {
						MessageBox (NULL, errreporti.c_str(), "Click 4.6: Compile Error", MB_OK);
						fcompiled=0;
						break;
					} else {
						fcompiled=1;
					}
					runprocess (cmdbuf4, 1, 0);
					/*settitle*/ 
					titlestr01="Click 4.6 [ Running ] [ ";
					titlestr01+=szFileName;
					titlestr01+=" ]";
					SendMessage(g_hStatusBar, SB_SETTEXT, 1, (LPARAM)"Running..."); 
					SetWindowText (hwnd, titlestr01.c_str());
					/*end:settitle*/ 
					runprocess (cmdbuf2, 0, 1);
					/*settitle*/ 
					titlestr01="Click 4.6 [ ";
					titlestr01+=szFileName;
					titlestr01+=" ]";
					SetWindowText (hwnd, titlestr01.c_str());
					SendMessage(g_hStatusBar, SB_SETTEXT, 2, (LPARAM)(fcompiled ? "�ѱ���" : "δ����")); 
					SendMessage(g_hStatusBar, SB_SETTEXT, 1, (LPARAM)"..."); 
					/*end:settitle*/ 
					break;
					
				case CM_COMPILPAS:
					SendMessage(g_hStatusBar, SB_SETTEXT, 1, (LPARAM)"Compiling..."); 
					if ((!fsaved && !fopend) || strcmp(szFileName, "") == 0) {
						DoFileOpenSave(hwnd, TRUE);
					} else {
						if(!SaveFile(GetDlgItem(hwnd, IDC_MAIN_TEXT), szFileName)) {
							MessageBox(hwnd, "Save file failed.", "Error",MB_OK|MB_ICONEXCLAMATION);
							fsaved=0;
						}
					}
					/*settitle*/ 
					titlestr01="Click 4.6 [ Compiling ] [ ";
					titlestr01+=szFileName;
					titlestr01+=" ]";
					SetWindowText (hwnd, titlestr01.c_str());
					/*end:settitle*/ 
					sprintf (cmdbuf1, "fpc.exe %s > %s_compile_tmp.log",szFileName,szFileName);
					sprintf (cmdbuf2, "start /max %s.exe",getpasfn(szFileName).c_str());
					sprintf (cmdbuf3, "del \"%s\"",getpasfn(szFileName).c_str());
					sprintf (cmdbuf4, "del \"%s_compile_tmp.log\"",szFileName);
					sprintf (cmdbuf5, "%s_compile_tmp.log",szFileName);
					//runprocess (cmdbuf3);
					runprocess (cmdbuf1, 1, 0);
					errreportcnt = 0;
					wndfin.open (cmdbuf5);
					while (wndfin) {
						errreportcnt++;
						errreporti += wndfin.get();
					}
					wndfin.close();
					if (errreportcnt>1) {
						MessageBox (NULL, errreporti.c_str(), "Click 4.6: Compile Message", MB_OK | MB_ICONINFORMATION);
						break;
						fcompiled=1;
					} else {
						fcompiled=1;
					}
					runprocess (cmdbuf4, 1, 0);
					/*settitle*/ 
					titlestr01="Click 4.6 [ ";
					titlestr01+=szFileName;
					titlestr01+=" ]";
					SendMessage(g_hStatusBar, SB_SETTEXT, 1, (LPARAM)"..."); 
					SendMessage(g_hStatusBar, SB_SETTEXT, 2, (LPARAM)(fcompiled ? "�ѱ���" : "δ����")); 
					SetWindowText (hwnd, titlestr01.c_str());
					/*end:settitle*/ 
					break;
				case CM_COMPILERUPAS:
					SendMessage(g_hStatusBar, SB_SETTEXT, 1, (LPARAM)"Compiling..."); 
					if ((!fsaved && !fopend) || strcmp(szFileName, "") == 0) {
						DoFileOpenSave(hwnd, TRUE);
					} else {
						if(!SaveFile(GetDlgItem(hwnd, IDC_MAIN_TEXT), szFileName)) {
							MessageBox(hwnd, "Save file failed.", "Error",MB_OK|MB_ICONEXCLAMATION);
							fsaved=0;
						}
					}
					/*settitle*/ 
					titlestr01="Click 4.6 [ Compiling ] [ ";
					titlestr01+=szFileName;
					titlestr01+=" ]";
					SetWindowText (hwnd, titlestr01.c_str());
					/*end:settitle*/ 
					sprintf (cmdbuf1, "fpc.exe %s > %s_compile_tmp.log",szFileName,szFileName);
					sprintf (cmdbuf2, "start /max %s.exe",getpasfn(szFileName).c_str());
					sprintf (cmdbuf3, "del \"%s\"",getpasfn(szFileName).c_str());
					sprintf (cmdbuf4, "del \"%s_compile_tmp.log\"",szFileName);
					sprintf (cmdbuf5, "%s_compile_tmp.log",szFileName);
					//runprocess (cmdbuf3);
					runprocess (cmdbuf1, 1, 0);
					wndfin.open (cmdbuf5);
					errreportcnt = 0;
					while (wndfin) {
						errreportcnt++;
						errreporti += wndfin.get();
					}
					wndfin.close();
					if (errreportcnt>1) {
						/*
						if (errreporti.find("Fatal:")&&errreporti.find("Error:")) {
							fcompiled=0;
							MessageBox (NULL, errreporti.c_str(), "Click 4.6: Compile Error", MB_OK | MB_ICONERROR);
							break;
						} else {
						*/
							fcompiled=1;
							MessageBox (NULL, errreporti.c_str(), "Click 4.6: Compile Message", MB_OK | MB_ICONINFORMATION);
						/*
						}
						*/
					} else {
						fcompiled=1;
					}
					runprocess (cmdbuf4, 1, 0);
					/*settitle*/ 
					titlestr01="Click 4.6 [ Running ] [ ";
					titlestr01+=szFileName;
					titlestr01+=" ]";
					SendMessage(g_hStatusBar, SB_SETTEXT, 1, (LPARAM)"Running..."); 
					SetWindowText (hwnd, titlestr01.c_str());
					/*end:settitle*/ 
					runprocess (cmdbuf2, 0, 1);
					/*settitle*/ 
					titlestr01="Click 4.6 [ ";
					titlestr01+=szFileName;
					titlestr01+=" ]";
					SendMessage(g_hStatusBar, SB_SETTEXT, 1, (LPARAM)"..."); 
					SendMessage(g_hStatusBar, SB_SETTEXT, 2, (LPARAM)(fcompiled ? "�ѱ���" : "δ����")); 
					SetWindowText (hwnd, titlestr01.c_str());
					/*end:settitle*/ 
					break;
				case CM_STARTCMD:
					runprocess ("start /max \"Click 4.6 [Command]\"", 0, 1);
					break;
				case CM_RUNBAT:
					/*settitle*/ 
					titlestr01="Click 4.6 [ Running (Bat) ] [ ";
					titlestr01+=szFileName;
					titlestr01+=" ]";
					SendMessage(g_hStatusBar, SB_SETTEXT, 1, (LPARAM)"Running Bat..."); 
					SetWindowText (hwnd, titlestr01.c_str());
					/*end:settitle*/ 
					if ((!fsaved && !fopend) || strcmp(szFileName, "") == 0) {
						DoFileOpenSave(hwnd, TRUE);
					} else {
						if(!SaveFile(GetDlgItem(hwnd, IDC_MAIN_TEXT), szFileName)) {
							MessageBox(hwnd, "Save file failed.\n(Or this is an empty file.)", "Error",MB_OK|MB_ICONEXCLAMATION);
							fsaved=0;
						}
					}
					sprintf (cmdbuf2, "start \"Click4.6-Executing [%s]\" /max %s", szFileName, szFileName);
					runprocess (cmdbuf2, 0, 1);
					/*settitle*/ 
					titlestr01="Click 4.6 [ ";
					titlestr01+=szFileName;
					titlestr01+=" ]";
					SendMessage(g_hStatusBar, SB_SETTEXT, 1, (LPARAM)"..."); 
					SetWindowText (hwnd, titlestr01.c_str());
					/*end:settitle*/ 
					break;
				case CM_DEBUG: 
					SendMessage(g_hStatusBar, SB_SETTEXT, 1, (LPARAM)"Debugging..."); 
					if ((!fsaved && !fopend) || strcmp(szFileName, "") == 0) {
						DoFileOpenSave(hwnd, TRUE);
					} else {
						if(!SaveFile(GetDlgItem(hwnd, IDC_MAIN_TEXT), szFileName)) {
							MessageBox(hwnd, "Save file failed.", "Error",MB_OK|MB_ICONEXCLAMATION);
							fsaved=0;
						}
					}
					sprintf (cmdbuf2, "start /max \"Click4.6-Debugging [%s]\" gdb %s.exe", getcppfn(szFileName).c_str(), getcppfn(szFileName).c_str());
					runprocess (cmdbuf2, 0, 1);
					SendMessage(g_hStatusBar, SB_SETTEXT, 1, (LPARAM)"..."); 
					break;
				case CM_VVARI:
					/*settitle*/ 
					titlestr01="Click 4.6 [ Viewing Variables... ]";
					SendMessage(g_hStatusBar, SB_SETTEXT, 1, (LPARAM)"Viewing Variables..."); 
					SetWindowText (hwnd, titlestr01.c_str());
					/*end:settitle*/ 
					sprintf (cmdbuf1, "szFileName\t= %s\nfsaved         \t= %s\nfopened      \t= %s\nfcompiled   \t= %s\nCurrentTime\t= %s\nCurrentMessage\t= %d/%d\nMessageCount\t= %lld", szFileName, (fsaved ? "True" : "False"), (fopend ? "True" : "False"), (fcompiled ? "True" : "False"), output_time().c_str(), WM_COMMAND, CM_VVARI, variMsgCnt);
					MessageBox (0, cmdbuf1, "Variables...", MB_OK | MB_ICONINFORMATION);
					/*settitle*/ 
					titlestr01="Click 4.6 [ ";
					titlestr01+=szFileName;
					titlestr01+=" ]";
					SendMessage(g_hStatusBar, SB_SETTEXT, 1, (LPARAM)"..."); 
					SetWindowText (hwnd, titlestr01.c_str());
					/*end:settitle*/ 
					break;
				case CM_GHELP:
					SendMessage(g_hStatusBar, SB_SETTEXT, 1, (LPARAM)"Helps..."); 
					GHELPSTARTPLACE:
					
					switch (MessageBox (0, "����ʹ�ø��������б�������ǰ����ȷ�����Ѿ�������g++������binĿ¼��fpc��������bin\\i386-win32\\Ŀ¼���ӵ���������PATH���������������÷������һ����˵��ԡ�->���ԣ������ࡰ�߼�ϵͳ���á����ڡ��߼�����ǩ�µ�������������(N)...����˫����ϵͳ�������е�PATH����б༭���ں������ӡ�XXX\\FPC\\2.2.2\\bin\\i386-win32\\���͡�XXX\\MinGW\\bin\\������XXX�滻Ϊ��İ�װλ�ã���Ҫ�����ţ�����Ȼ��ȫ�������ȷ�������ɡ�", "Help 01", MB_CANCELTRYCONTINUE | MB_ICONINFORMATION | MB_DEFBUTTON3)) {
						case IDCANCEL: GHELPEXITFLAG = 1;break;
						case IDCONTINUE:break;
						case IDTRYAGAIN: goto GHELPSTARTPLACE;break;
						default: GHELPEXITFLAG = 1;break;
					}
					if (GHELPEXITFLAG) {SendMessage(g_hStatusBar, SB_SETTEXT, 1, (LPARAM)"..."); break;}
					
					switch (MessageBox (0, "������һ���ļ��󣬿��Զ��������κβ��������ǲ�û�н�ֹ���ƴ�һ��C++�ļ�����\"Compile Pascal File...\"�����б���ȵĲ����������ⲻ�ԣ����������ʹ�ñ���/������Щѡ��ʱ�������ȷ���Ƿ�ѡ������ȷ�ı�����ԣ�", "Help 02", MB_CANCELTRYCONTINUE | MB_ICONINFORMATION | MB_DEFBUTTON3)) {
						case IDCANCEL: GHELPEXITFLAG = 1;break;
						case IDCONTINUE:break;
						case IDTRYAGAIN: goto GHELPSTARTPLACE;break;
						default: GHELPEXITFLAG = 1;break;
					}
					if (GHELPEXITFLAG) {SendMessage(g_hStatusBar, SB_SETTEXT, 1, (LPARAM)"..."); break;}
					
					switch (MessageBox (0, "�����������������Լ���������C++��ƫ���ԣ�����Pascal��������޷���ȷ����/���У������½⡣��Ҳ��ѡ��ʹ������Pascal��������ֻҪ������Ŀ¼���ӵ���������PATH������������װʱ�Դ���FPCĿ¼�ӻ�������PATH���Ƴ����ɡ�", "Help 03", MB_CANCELTRYCONTINUE | MB_ICONINFORMATION | MB_DEFBUTTON3)) {
						case IDCANCEL: GHELPEXITFLAG = 1;break;
						case IDCONTINUE:break;
						case IDTRYAGAIN: goto GHELPSTARTPLACE;break;
						default: GHELPEXITFLAG = 1;break;
					}
					if (GHELPEXITFLAG) {SendMessage(g_hStatusBar, SB_SETTEXT, 1, (LPARAM)"..."); break;}
					
					switch (MessageBox (0, "���ڱ���������ʱ��϶̣������ʹ�ù��������������ƣ�\n  1.������Windows����ϵͳ��֧��Win32API�İ汾��\n  2.C++�ļ���֧��.cpp, .c++, .cxx��չ����Pascal�ļ���֧��.pp��չ����C++ͷ�ļ���֧��.hpp��չ�����������ļ���֧��.bat, .com, .cmd��չ�������½⡣����ʹ�������ĺ�׺�����ַ����������������ܵ��±�������ʧ�ܡ�", "Help 04", MB_CANCELTRYCONTINUE | MB_ICONINFORMATION | MB_DEFBUTTON3)) {
						case IDCANCEL: GHELPEXITFLAG = 1;break;
						case IDCONTINUE:break;
						case IDTRYAGAIN: goto GHELPSTARTPLACE;break;
						default: GHELPEXITFLAG = 1;break;
					}
					if (GHELPEXITFLAG) {SendMessage(g_hStatusBar, SB_SETTEXT, 1, (LPARAM)"..."); break;}
					
					switch (MessageBox (0, "������Ҫֱ������/����һ������ʱ�������ȱ��档���ң��������Ҫ����/���Ե�ǰ��д�ĳ������ȱ��룬��������/���Ե�������һ�α�������ɵĳ���", "Help 05", MB_CANCELTRYCONTINUE | MB_ICONINFORMATION | MB_DEFBUTTON3)) {
						case IDCANCEL: GHELPEXITFLAG = 1;break;
						case IDCONTINUE:break;
						case IDTRYAGAIN: goto GHELPSTARTPLACE;break;
						default: GHELPEXITFLAG = 1;break;
					}
					if (GHELPEXITFLAG) {SendMessage(g_hStatusBar, SB_SETTEXT, 1, (LPARAM)"..."); break;}
					
					switch (MessageBox (0, "������״̬�����ڵ���������Ҫ�鿴������ѡ��Help > Flush StatusBar����ˢ�¡�", "Help 06", MB_CANCELTRYCONTINUE | MB_ICONINFORMATION | MB_DEFBUTTON3)) {
						case IDCANCEL: GHELPEXITFLAG = 1;break;
						case IDCONTINUE:break;
						case IDTRYAGAIN: goto GHELPSTARTPLACE;break;
						default: GHELPEXITFLAG = 1;break;
					}
					if (GHELPEXITFLAG) {SendMessage(g_hStatusBar, SB_SETTEXT, 1, (LPARAM)"..."); break;}
					
					switch (MessageBox (0, "���ļ�����Click4.6.exe�ǿ��Ե���ʹ�õģ����������԰����exe�ļ����Ƶ�����λ�ã���������Windows���ԣ�����ʹ�á�������Ҫ�Լ�����MinGW��FPC�⣬�ڱ����������Ѿ��Դ�����MinGW��FPC�ļ��У���������Ȼ����ʹ���Լ��Ŀ⡣ֻҪ�������ӵ������������ɡ�", "Help 07", MB_CANCELTRYCONTINUE | MB_ICONINFORMATION | MB_DEFBUTTON3)) {
						case IDCANCEL: GHELPEXITFLAG = 1;break;
						case IDCONTINUE:break;
						case IDTRYAGAIN: goto GHELPSTARTPLACE;break;
						default: GHELPEXITFLAG = 1;break;
					}
					if (GHELPEXITFLAG) {SendMessage(g_hStatusBar, SB_SETTEXT, 1, (LPARAM)"..."); break;}
					
					switch (MessageBox (0, "��Σ����������ƽ�βΪ_compile_tmp.log���ļ����������ڱ�������У����ܻᣩ���ɵ���ʱ��־�ļ�������ȫ����ֱ��ɾ������ClickIDE����������������û���κ�Ӱ�졣", "Help 08", MB_CANCELTRYCONTINUE | MB_ICONINFORMATION | MB_DEFBUTTON3)) {
						case IDCANCEL: GHELPEXITFLAG = 1;break;
						case IDCONTINUE:break;
						case IDTRYAGAIN: goto GHELPSTARTPLACE;break;
						default: GHELPEXITFLAG = 1;break;
					}
					if (GHELPEXITFLAG) {SendMessage(g_hStatusBar, SB_SETTEXT, 1, (LPARAM)"..."); break;}
					
					switch (MessageBox (0, "��������ʱ������C++���򣬽�����������/����ʱ�Żᷢ����ʾ������ֱ�ӱ�����ɣ�����Pascal�����κ�����¶��ᷢ��������ʾ������������������ʾ���Ƿ��������\"Fatal\"��\"Error\"֮������ۣ����У�����������������֮�����������ͨ����", "Help 09", MB_CANCELTRYCONTINUE | MB_ICONINFORMATION | MB_DEFBUTTON3)) {
						case IDCANCEL: GHELPEXITFLAG = 1;break;
						case IDCONTINUE:break;
						case IDTRYAGAIN: goto GHELPSTARTPLACE;break;
						default: GHELPEXITFLAG = 1;break;
					}
					if (GHELPEXITFLAG) {SendMessage(g_hStatusBar, SB_SETTEXT, 1, (LPARAM)"..."); break;}
					
					switch (MessageBox (0, "�����������ѣ����⣬������߽��飬����һ��Ҫ��ʱ��ϵ��������eric_ni2008@163.com�����QQȺ��1019034208��������ѯ��Ͷ�ߣ��Ա����ǽ���ClickIDE���ø������ƣ�", "Help 10", MB_CANCELTRYCONTINUE | MB_ICONINFORMATION | MB_DEFBUTTON3)) {
						case IDCANCEL: GHELPEXITFLAG = 1;break;
						case IDCONTINUE:break;
						case IDTRYAGAIN: goto GHELPSTARTPLACE;break;
						default: GHELPEXITFLAG = 1;break;
					}
					if (GHELPEXITFLAG) {SendMessage(g_hStatusBar, SB_SETTEXT, 1, (LPARAM)"..."); break;}
					
					MessageBox (0, "û�и�����ʾ��......", "Message", MB_OK | MB_ICONINFORMATION);
					SendMessage(g_hStatusBar, SB_SETTEXT, 1, (LPARAM)"..."); 
					
					break;
				case CM_EDIT_UNDO:
					SendDlgItemMessage(hwnd, IDC_MAIN_TEXT, EM_UNDO, 0, 0);
					break;
				case CM_EDIT_CUT:
					SendDlgItemMessage(hwnd, IDC_MAIN_TEXT, WM_CUT, 0, 0);
					break;
				case CM_EDIT_COPY:
					SendDlgItemMessage(hwnd, IDC_MAIN_TEXT, WM_COPY, 0, 0);
					break;
				case CM_EDIT_PASTE:
					SendDlgItemMessage(hwnd, IDC_MAIN_TEXT, WM_PASTE, 0, 0);
					break;
				case CM_FLSTB:
					SendMessage(g_hStatusBar, SB_SETTEXT, 0, (LPARAM)"Click 4.6 IDE"); 
					SendMessage(g_hStatusBar, SB_SETTEXT, 1, (LPARAM)"..."); 
					SendMessage(g_hStatusBar, SB_SETTEXT, 2, (LPARAM)(fcompiled ? "�ѱ���" : "δ����")); 
					SendMessage(g_hStatusBar, SB_SETTEXT, 3, (LPARAM)""); 
					SendMessage(g_hStatusBar, SB_SETTEXT, 4, (LPARAM)szFileName); 
					break;
				case CM_GHTML: {
					SendMessage(g_hStatusBar, SB_SETTEXT, 1, (LPARAM)"GeneratingHTML..."); 
					char getallcodetmpstr[200000];
					GetDlgItemText(hwnd, IDC_MAIN_TEXT, getallcodetmpstr, 200000);
					codealltmp.clear();
					codealltmp+=getallcodetmpstr;
					codealltmp+="\n     ";
					//MessageBox(hwnd, codealltmp.c_str(), "", MB_OK);
					char hlfilename[MAX_PATH*6];
					strcpy(hlfilename, szFileName);
					strcat(hlfilename, (char*)"_highlight.html");
					char titlefilename[MAX_PATH*6];
					strcpy(titlefilename, "Click4.6-");
					strcat(titlefilename, hlfilename);
					fout.open(hlfilename);
					fout << "<!DOCTYLE html>" << endl;
					fout << "<html><head><meta charset=\"utf-8\"/><title>";
					fout << titlefilename;
					fout << "</title><style>body{font-family:Consolas,Arial,Helvetica,sans-serif;}h3{font-style:italic}.operator{color:#ff2c0f;}.zfc{color:#0000ff;}.ycl{color:#80ab66}.zs{color:#0078d7;}.gjz{font-weight:bold;}.vari{font-weight:bold;color:#8e37a7;}</style></head>" << endl;
					fout << "<body><h3>Exported by ClickIDE 4.6</h3><p>";
					for (int i = 0; i < codealltmp.size(); i++) {
						if (dontout) {
							dontout = 0;
							continue;
						}
						char tch = codealltmp[i];
						if (tch == '\"') {
							islfst=0;
							if (codealltmp[i-1] == '\\') {
								fout << "quot;";continue;
							}
							if (!iszfc) {
								fout << "<span class=\"zfc\">&quot;";
								iszfc = true;continue;
							}
							if (iszfc) {
								fout << "&quot;</span>";
								iszfc = false;continue;
							}
						}
						if (tch == '#' && islfst) {
							islfst=0;
							isycl = 1;
							fout << "<span class=\"ycl\">#";continue; 
						}
						if (tch == '/' && codealltmp[i+1] == '*') {
							ismtzs = 1;
							fout << "<span class=\"zs\">/";continue; 
						}
						if (tch == '*' && codealltmp[i+1] == '/') {
							ismtzs = 0;dontout=1;
							fout << "*/</span>";continue; 
						}
						if (tch == '/' && codealltmp[i+1] == '/') {
							issgzs = 1;dontout=1;
							fout << "<span class=\"zs\">//";continue; 
						}
						if (tch == ' ') {
							fout << "&nbsp;";continue;
						}
						if (issgzs || ismtzs) {
							if (tch == '<') {fout << "&lt;";continue;}
							if (tch == '>') {fout << "&gt;";continue;}
							if (tch == '&') {fout << "&amp;";continue;}
							if (tch == '\"') {fout << "&quot;";continue;}
							if (tch == '\t') {fout << "&nbsp;&nbsp;&nbsp;&nbsp;";continue;}
							if (tch == ' ') {fout << "&nbsp;";continue;}
							if (tch == '\n') {
								if (isycl && !nlycl) {
									isycl = 0;
									fout << "</span>"; 
								}
								if (issgzs) {
									fout << "</span>";
									issgzs = 0;
								}
								islfst = 1;fout << "<br/>";continue;
							}
							fout << tch;
							continue;
						}
						if (tch == '<') {islfst=0;if (isycl || iszfc) {fout << "&lt";} else {fout << "<span class=\"operator\">&lt</span>";}continue;}
						if (tch == '>') {islfst=0;if (isycl || iszfc) {fout << "&gt";} else {fout << "<span class=\"operator\">&gt</span>";}continue;}
						if (tch == '&') {islfst=0;if (isycl || iszfc) {fout << "&amp";} else {fout << "<span class=\"operator\">&amp;</span>";}continue;}
						if (tch == '{') {islfst=0;if (isycl || iszfc) {fout << "{";} else {fout << "<span class=\"operator\">{</span>";}continue;}
						if (tch == '}') {islfst=0;if (isycl || iszfc) {fout << "}";} else {fout << "<span class=\"operator\">}</span>";}continue;}
						if (tch == '(') {islfst=0;if (isycl || iszfc) {fout << "(";} else {fout << "<span class=\"operator\">(</span>";}continue;}
						if (tch == ')') {islfst=0;if (isycl || iszfc) {fout << ")";} else {fout << "<span class=\"operator\">)</span>";}continue;}
						if (tch == ':') {islfst=0;if (isycl || iszfc) {fout << ":";} else {fout << "<span class=\"operator\">:</span>";}continue;}
						if (tch == ';') {islfst=0;if (isycl || iszfc) {fout << ";";} else {fout << "<span class=\"operator\">;</span>";}continue;}
						if (tch == '+') {islfst=0;if (isycl || iszfc) {fout << "+";} else {fout << "<span class=\"operator\">+</span>";}continue;}
						if (tch == '-') {islfst=0;if (isycl || iszfc) {fout << "-";} else {fout << "<span class=\"operator\">-</span>";}continue;}
						if (tch == '*') {islfst=0;if (isycl || iszfc) {fout << "*";} else {fout << "<span class=\"operator\">*</span>";}continue;}
						if (tch == '/') {islfst=0;if (isycl || iszfc) {fout << "/";} else {fout << "<span class=\"operator\">/</span>";}continue;}
						if (tch == '%') {islfst=0;if (isycl || iszfc) {fout << "%";} else {fout << "<span class=\"operator\">%</span>";}continue;}
						if (tch == '~') {islfst=0;if (isycl || iszfc) {fout << "~";} else {fout << "<span class=\"operator\">~</span>";}continue;}
						if (tch == '?') {islfst=0;if (isycl || iszfc) {fout << "?";} else {fout << "<span class=\"operator\">?</span>";}continue;}
						if (tch == '^') {islfst=0;if (isycl || iszfc) {fout << "^";} else {fout << "<span class=\"operator\">^</span>";}continue;}
						if (tch == '[') {islfst=0;if (isycl || iszfc) {fout << "[";} else {fout << "<span class=\"operator\">[</span>";}continue;}
						if (tch == ']') {islfst=0;if (isycl || iszfc) {fout << "]";} else {fout << "<span class=\"operator\">]</span>";}continue;}
						if (tch == '=') {islfst=0;if (isycl || iszfc) {fout << "=";} else {fout << "<span class=\"operator\">=</span>";}continue;}
						if (tch == ',') {islfst=0;if (isycl || iszfc) {fout << ",";} else {fout << "<span class=\"operator\">,</span>";}continue;}
						if (tch == '\n') {
							if (isycl && !nlycl) {
								isycl = 0;
								fout << "</span>"; 
							}
							if (issgzs) {
								fout << "</span>";
								issgzs = 0;
							}
							islfst = 1;
							fout << "<br/>";continue;
						}
						if (tch == '\\') {
							if (codealltmp[i+1] == '\n' && isycl) {
								nlycl = 1;
							}
						}
						if (tch == '\t') {
							fout << "&nbsp;&nbsp;&nbsp;&nbsp;";continue;
						}
						fout << tch;
						islfst = 0;
					}
					fout << "</p></body></html>" << endl;
					fout.close();
					Sleep(200);
					ShellExecute(NULL,TEXT("open"), hlfilename, TEXT(""),NULL,SW_SHOWNORMAL);
					SendMessage(g_hStatusBar, SB_SETTEXT, 1, (LPARAM)"..."); 
					break;
				}
				case CM_GITHUB: {
					ShellExecute(NULL,TEXT("open"), TEXT("https://github.com/EricNTH080103/ClickIDE"), TEXT(""),NULL,SW_SHOWNORMAL);
					break;
				}
				case CM_WEBSITE: {
					ShellExecute(NULL,TEXT("open"), TEXT("https://ericnth.cn/clickide/"), TEXT(""),NULL,SW_SHOWNORMAL);
					break;
				}
				case CM_ASTYLE: {
					MessageBox(NULL, "���汾��4.6-Stable����֧�ָù��ܡ����볢�ʣ�����ϵ���߻�ȡ�ڲ�汾����һ����ʽ�汾��4.8-stable����������˹��ܡ�\n", "Ah oh~", MB_OK);
					
					PostMessage(hwnd, WM_COMMAND, CM_FILE_SAVE, (LPARAM)"");
					char astylestr[MAX_PATH*6];
					sprintf(astylestr, "--recursive --style=bsd --convert-tabs --indent=spaces=4 --attach-closing-while --indent-switches --indent-namespaces --indent-continuation=4 --indent-preproc-block --indent-preproc-define --indent-preproc-cond --indent-col1-comments --pad-oper --pad-paren-in --unpad-paren --delete-empty-lines --align-pointer=name --align-reference=name --break-elseifs --add-braces >%s.astyle.log %s*", szFileName, szFileName);
					//system(astylestr)
					MessageBox(NULL, astylestr, "", MB_OK);
					ShellExecute(NULL,TEXT("open"), "AStyle.exe", TEXT(astylestr),NULL,SW_HIDE);
					sprintf(astylestr, "del %s.orig", szFileName);
					system(astylestr);
					
					break;
				}
			}
			hMenu = GetMenu(hwnd);
			hFileMenu = GetSubMenu(hMenu, 0);
			hCompileMenu = GetSubMenu(hMenu, 1);
			EnableMenuItem(hFileMenu, CM_FILE_SAVE, MF_BYCOMMAND | (!(!fsaved && !fopend) || strcmp(szFileName, "Untitled") ? MF_ENABLED : MF_GRAYED));
			EnableMenuItem(hCompileMenu, CM_RUN, MF_BYCOMMAND | ((fcompiled) ? MF_ENABLED : MF_GRAYED));
			EnableMenuItem(hCompileMenu, CM_RUNPAS, MF_BYCOMMAND | ((fcompiled) ? MF_ENABLED : MF_GRAYED));
			EnableMenuItem(hCompileMenu, CM_DEBUG, MF_BYCOMMAND | ((fcompiled) ? MF_ENABLED : MF_GRAYED));
			break;
		case WM_CLOSE:
					SendMessage(g_hStatusBar, SB_SETTEXT, 1, (LPARAM)"Exitting..."); 
					/*settitle*/ 
					titlestr01="Click 4.6 [ Exiting... ]";
					SetWindowText (hwnd, titlestr01.c_str());
					/*end:settitle*/ 
			if (MessageBox (NULL, "Are you sure to quit? \nThings which are not saved will be lost!", "Exiting...", MB_OKCANCEL | MB_ICONQUESTION) != IDOK) {
				/*settitle*/ 
				titlestr01="Click 4.6 [ ";
				titlestr01+=szFileName;
				titlestr01+=" ]";
				SendMessage(g_hStatusBar, SB_SETTEXT, 1, (LPARAM)"..."); 
				SetWindowText (hwnd, titlestr01.c_str());
				/*end:settitle*/ 
				break;
			}
			programmeexiterrorstatusflag = 0;
			DestroyWindow(hwnd);
			break;
		case WM_DESTROY:
			/*
			if (programmeexiterrorstatusflag) {
				if (MessageBox (0, "Abnormal exit!\nDo you want to try to recover?", "Click 4.6", MB_YESNO | MB_ICONQUESTION) == IDYES) {
					break;
				}
			}
			*/
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hwnd, Message, wParam, lParam);
	}
	return 0;
}



int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,LPSTR lpCmdLine, int nCmdShow) {
	WNDCLASSEX wc;
	//HWND hwnd;
	MSG Msg;

	wc.cbSize        = sizeof(WNDCLASSEX);
	wc.style         = 0;
	wc.lpfnWndProc   = WndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = hInstance;
	wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wc.lpszMenuName  = "MAINMENU";
	wc.lpszClassName = "WindowClass";
	wc.hIconSm       = LoadIcon(hInstance,"A"); /* A is name used by project icons */

	if(!RegisterClassEx(&wc)) {
		MessageBox(0,"Window Registration Failed!","Error!",MB_ICONEXCLAMATION|MB_OK|MB_SYSTEMMODAL);
		return 0;
	}
	
	sprintf (szFileName, "%s", "Untitled"); 
	
	hwnd = CreateWindowEx(WS_EX_CLIENTEDGE,"WindowClass","Click 4.6",WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		1000,600,
		NULL, NULL, hInstance, NULL);

	if(hwnd == NULL) {
		MessageBox(0, "Window Creation Failed!", "Error!",MB_ICONEXCLAMATION|MB_OK|MB_SYSTEMMODAL);
		return 0;
	}

	ShowWindow(hwnd,1);
	UpdateWindow(hwnd);
	

	while(GetMessage(&Msg, NULL, 0, 0) > 0) {
		variMsgCnt++;
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}
	return Msg.wParam;
}