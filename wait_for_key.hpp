
#ifdef _WIN32
	#include <conio.h>
#else
	#include <termios.h>
	#include <unistd.h>
	#include <cstdio>
#endif

inline static void wait_for_key() {
#ifdef _WIN32
	_getch();
#else
	termios oldt, newt;
	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);
	getchar();
	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
#endif
}
