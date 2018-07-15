#include <Windows.h>
#include <cstring>
#include<processthreadsapi.h>
int WINAPI WinMain(HINSTANCE hInstance,
            HINSTANCE hPrevInstance,
            LPSTR    lpCmdLine,
            int       cmdShow) {

	if(strstr(lpCmdLine,"/s")) {
		HANDLE hfile = ::OpenFileMapping(FILE_MAP_READ,FALSE,TEXT("My_Screen_decorator_95819"));
		if(NULL==hfile) {
		    wchar_t tmp[2048];
		    DWORD len=sizeof(tmp);
			if(ERROR_SUCCESS==RegGetValueW(HKEY_CURRENT_USER, L"Software\\ScreenDecorator",L"Exe",RRF_RT_REG_SZ,NULL,tmp,&len)){
				STARTUPINFOW si;
				PROCESS_INFORMATION pi;
				wchar_t Arg[6]=L"-s";
                ShellExecuteW(0,L"open",tmp,L"-s",NULL,SW_SHOWNORMAL);

			}
			
			return 0;
		}


		PVOID pview = NULL;
		pview = ::MapViewOfFile(hfile,FILE_MAP_READ,0,0,sizeof(HWND));
		if (NULL==pview) {
			return 0;
		}
		HWND window;
		memcpy(&window,pview,sizeof(HWND));
		WPARAM wparam=(WPARAM)0x5747;
		LPARAM lparam=(LPARAM)0x0683;
		PostMessage(window,WM_APP+0x0518,wparam,lparam);
		return 0;
	} else {
		return 0;
	}
}
