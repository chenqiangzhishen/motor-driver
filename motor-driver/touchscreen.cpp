//#include <WinUser.h>
#include "touchscreen.h"

POINT getLeftButtonPosition(void)
{
	POINT pt;
	GetCursorPos(&pt);
	PostMessage(GetConsoleWindow(), WM_LBUTTONDOWN, MK_LBUTTON, MAKELONG(pt.x, pt.y));
	PostMessage(GetConsoleWindow(), WM_LBUTTONUP, MK_LBUTTON, MAKELONG(pt.x, pt.y));
	printf("x=%d,y=%d\n", pt.x, pt.y);
	return pt;
}

int touchTest(void) {

	getLeftButtonPosition();
	while (1)
	{
		getLeftButtonPosition();
		// Press  ESC on keyboard to  exit
		Sleep(1000);
		char c = (char)cv::waitKey(1);
		if (c == 27)
			break;
	}
	return 0;
}