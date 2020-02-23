#define SDL_MAIN_HANDLED
#ifdef FUUGB_SYSTEM_WINDOWS
#define LOAD_DLL 1
#include <windows.h>
#include <stdio.h>
#endif

typedef void (*Dllfunc)();

int main(int argc, char **argv)
{
	if (LOAD_DLL)
	{
		HINSTANCE hDll;
		Dllfunc execute;
		hDll = LoadLibrary("FuuGBcore.dll");

		if (hDll != NULL)
		{
			printf("FuuGBcore.dll Loaded Succesfully.\n");
			execute = (Dllfunc)GetProcAddress(hDll, "execute");

			if (!execute)
			{
				printf("Could not load run() method from dll.\n");
				return 2;
			}
			execute();
		}
		FreeLibrary(hDll);
	}
	return 1;
}
