#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ncurses.h>

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

	bool is_running = true;
	while (is_running) {
		int ch = getch();
		if (ch == KEY_F(1))
			is_running = false;
	}

	endwin();
	return EXIT_SUCCESS;
}
