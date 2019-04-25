//
//  WinApplication.h
//  FuuGBcore
//
//  Created by Derek Boucher on 2019-04-20.
//

#ifndef WinApplication_h
#define WinApplication_h

#include "Application.h"
#include "Utilities.h"

namespace FuuGB
{
	class FUUGB_API WinApplication : public Application
	{
	public:
		void run();
		wchar_t* open_file(SDL_Window*);
	};
}
#endif /* WinApplication_h */

#define FUUGB_WINDOW_CONFIG(...) FuuGB::ActivateMenu(FuuGB::getSDLWinHandle(__VA_ARGS__))
#define FUUGB_WIN_EVENT FUUGB_EVENT.syswm.msg->msg.win.wParam
#define FUUGB_WIN_HANDLE(...) FuuGB::getSDLWinHandle(__VA_ARGS__)