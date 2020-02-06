#include "KeyboardFunctions.hpp"

#ifdef __unix__

/** @brief A Linux implementation of Windows _kbhit non-blocking function to wait for user's key pressed
*
* @return number of bytes waiting in the buffer.
*/
int KeyboardFunctions::_kbhit() {
	static const int STDIN = 0;
	static bool initialized = false;

	if (!initialized) {
		// Use termios to turn off line buffering
		termios term;
		tcgetattr(STDIN, &term);
		term.c_lflag &= ~ICANON;
		tcsetattr(STDIN, TCSANOW, &term);
		setbuf(stdin, NULL);
		initialized = true;
	}

	int bytesWaiting;
	ioctl(STDIN, FIONREAD, &bytesWaiting);
	return bytesWaiting;
}

/** @brief A Linux implementation of Windows _getch to obtain the keypressed from the buffer
*
* @return the ascii keystroke
*/
char KeyboardFunctions::_getch()
{
	char key;
	scanf("%c", &key);
	return key;
}

#endif  // KEYPRESSED_HPP

/** @brief Get non-blocking keyboard triggers (skip until triggered)
 *
 * Calling this function will NOT block the program \n
 * return the triggered code
 *
 * Trigger Code: \n
 * -look at the define pre-compiled macros defined in header file
 *
 * @return returns the trigger code
 */
char KeyboardFunctions::getNonBlockingTriggers()
{
	char ch = 0;
	if (_kbhit())
	{
		ch = _getch();
		if (ch == 27)
			ch = KB_ESCAPE;
		else if (ch == 10)
			ch = KB_ENTER;
		else if (ch == 99)
			ch = KB_CONTINUE;
		else if (ch == 112)
			ch = KB_PAUSE;
		else
			ch = KB_UNKNOWN;
	}
	return ch;
}

/** @brief Get blocking keyboard triggers (wait until triggered)
 *
 * Calling this function will block the program using a while(1) loop. \n
 * Will only return when a keyboard is pressed/triggered
 *
 * Trigger Code: \n
 * -look at the define pre-compiled macros defined in header file
 *
 * @return returns the trigger code
 */
char KeyboardFunctions::getBlockingTriggers()
{
	fflush(stdin);
	char ch = 0;
	while (1)
	{
		ch = getNonBlockingTriggers();
		if (ch == KB_UNKNOWN)
			printf("Unknown keypressed. Please Re-enter\n");
		else if (ch != 0)
			break;
	}
	return ch;
}

/** @brief A test/demo of the linux kbhit function
 *
 * Calling this function will run a non-blocking keyboard detection loop.\n
 * Once the 'E' key is pressed, the program will stop.
 *
 * @note this demo should run on both linux and windows.
 *
 * @return returns nothing
 */
void KeyboardFunctions::demo()
{
	printf("please press e key to exit \n ");

	char key = 0;
	fflush(stdout);
	while (1)
	{
		if (_kbhit())
		{
			key = _getch();
			printf("%c\n", key);
			if (key == 'e')
				break;
		}
	}

}

