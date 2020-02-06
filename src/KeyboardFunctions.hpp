#ifndef KEYBOARDFUNCTIONS_HPP
#define KEYBOARDFUNCTIONS_HPP

#ifdef __unix__

#include <stdio.h>
#include <sys/select.h>
#include <termios.h>
#include <stropts.h>
#include <unistd.h>
#include <sys/ioctl.h>

#elif defined(_WIN32) || defined(WIN32)     /* _Win32 is usually defined by compilers targeting 32 or   64 bit Windows systems */
#include <conio.h>
#include <stdio.h>
#endif

#define KB_ENTER 1
#define KB_ESCAPE 2
#define KB_CONTINUE 3
#define KB_PAUSE 4
#define KB_UNKNOWN 5

/**
Keyboard function class \n

A implementation of the _kbhit() function for Linux. \n
In windows _kbhit() is suppose to implement non-blocking user input checking function. If the function returns a nonzero value, a keystroke is waiting in the buffer. The program can then call _getch or _getche to get the keystroke. \n
Non-blocking means that inputs can interrupt the current ongoing process, but not let the program wont wait for user input indefinitely like scanf

In Windows , conio.h implements the _kbhit function for us. \n
In Linux, we have to implement our own, because it lacks a _kbhit function.
@see http://www.flipcode.com/archives/_kbhit_for_Linux.shtml

@note within rpi, when using this function, we need to allow the user to run the internal script "/sys/class/gpio/export" in sudo mode. Hence, there is a need to elevate permission when running the code, or change permission of the export file. \n
1) elevate permission when running code: $ sudo ./bin/ARM/Debug/RPi_VS_CrossplatformBuild.out. \n
2) change permission of export script: $ sudo chmod 777 /sys/class/gpio/export

@author Er Jie Kai (EJK)

Edited from the source written by the following
@author Morgan McGuire, morgan@cs.brown.edu
*/
class KeyboardFunctions
{
private:
#ifdef __unix__
	int _kbhit();
	char _getch();
#endif
public:
	KeyboardFunctions()
	{
	}
	~KeyboardFunctions()
	{
	}
	char getNonBlockingTriggers();
	char getBlockingTriggers();
	void demo();
};



#endif  // KEYBOARDFUNCTIONS_HPP

