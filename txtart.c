/**
 * txtart - Simple ncurses ASCII art editor
 * Copyright (C) 2022 Ted Johnson <tedjohnsonjs@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ncurses.h>

#define WIDTH (COLS - 2)
#define HEIGHT (LINES - 2)

WINDOW *handle_resize(WINDOW *w) {
	clear();
	border(0, 0, 0, 0, 0, 0, 0, 0);
	refresh();
	if (w)
		delwin(w);
	return newwin(HEIGHT, WIDTH, 1, 1);
}

int main(int argc, char **argv) {

	char *active_file = NULL;
	for (int i = 1; i < argc; i++) {
		char *arg = argv[i];
		if (!strcmp(arg, "-h") || !strcmp(arg, "--help") || !strcmp(arg, "-?")) {
			// TODO: print help
			printf("txtart v%s - Simple ncurses ASCII art editor\n", TXTART_VERSION);
			return EXIT_SUCCESS;
		} else if (!strcmp(arg, "-v") || !strcmp(arg, "--version")) {
			printf("txtart v%s\n", TXTART_VERSION);
			return EXIT_SUCCESS;
		} else if (!strcmp(arg, "-l") || !strcmp(arg, "--load")) {
			if (++i >= argc) {
				fprintf(stderr, "%s requires an argument.\n", arg);
				return EXIT_FAILURE;
			}
			active_file = argv[i];
		} else {
			fprintf(stderr, "Unknown argument '%s'.\n", arg);
			return EXIT_FAILURE;
		}
	}

	initscr();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);
	WINDOW *w = handle_resize(NULL);

	bool is_running = true;
	while (is_running) {

		// Drawing
		wclear(w);
		wrefresh(w);

		// Input
		int ch = getch();
		if (ch == KEY_RESIZE) {
			w = handle_resize(w);
		} else if (ch == KEY_F(1)) {
			is_running = false;
		}
	}

	delwin(w);
	endwin();
	return EXIT_SUCCESS;
}
