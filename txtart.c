#include <stdlib.h>
#include <stdbool.h>
#include <ncurses.h>

int main(int argc, char **argv) {
	initscr();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);

	bool is_running = true;
	while (is_running) {
		int ch = getch();
		if (ch == KEY_F(1))
			is_running = false;
	}

	endwin();
	return EXIT_SUCCESS;
}
