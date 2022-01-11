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
#define XPADDING (0.1)
#define YPADDING (0.05)

struct line {
	char *data;
	int len;
};

static int canvas_len;
static int canvas_wid;
static struct line *canvas;

WINDOW *handle_resize(WINDOW *w) {
	erase();
	border(0, 0, 0, 0, 0, 0, 0, 0);
	refresh();
	if (w)
		delwin(w);
	return newwin(HEIGHT, WIDTH, 1, 1);
}

void clear_canvas(void) {
	for (int i = 0; i < canvas_len; i++)
		free(canvas[i].data);
	canvas = NULL;
	canvas_len = 0;
	canvas_wid = 0;
}

bool load_file(char *filename) {
	FILE *fp = fopen(filename, "r");
	if (fp == NULL)
		return false;

	clear_canvas();
	size_t read;
	struct line *line = NULL;
	do {
		canvas_len++;
		canvas = realloc(canvas, sizeof *canvas * canvas_len);
		line = canvas + canvas_len - 1;
		*line = (struct line) { NULL, 0 };
		line->len = read = getline(&line->data, &(size_t){0}, fp);
		if (line->data[line->len - 1] == '\n') {
			line->data[line->len - 1] = '\0';
			line->len--;
		}
		if (line->len > canvas_wid)
			canvas_wid = line->len;
	} while (read != -1);

	canvas_len--;
	free(line->data);
	fclose(fp);
	return true;
}

int main(int argc, char **argv) {

	char *active_file = NULL;
	for (int i = 1; i < argc; i++) {
		char *arg = argv[i];
		if (!strcmp(arg, "-h") || !strcmp(arg, "--help") || !strcmp(arg, "-?")) {
			// TODO: print help
			printf("txtart %s - Simple ncurses ASCII art editor\n", TXTART_VERSION);
			return EXIT_SUCCESS;
		} else if (!strcmp(arg, "-v") || !strcmp(arg, "--version")) {
			printf("txtart %s\n", TXTART_VERSION);
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

	if (active_file && !load_file(active_file)) {
		fprintf(stderr, "Unable to load '%s'.\n", active_file);
		return EXIT_FAILURE;
	}

	initscr();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);
	WINDOW *w = handle_resize(NULL);

	bool is_running = true;
	int xview = (canvas_wid - WIDTH) / 2, yview = (canvas_len - HEIGHT) / 2;
	int xcursor = WIDTH / 2 - 1, ycursor = HEIGHT / 2 - 1;
	while (is_running) {

		// Drawing
		werase(w);
		for (int yscreen = 0; yscreen < HEIGHT; yscreen++) {
			for (int xscreen = 0; xscreen < WIDTH; xscreen++) {
				int ycanvas = yscreen + yview;
				if (ycanvas < 0 || ycanvas >= canvas_len)
					continue;
				struct line *line = canvas + ycanvas;
				int xcanvas = xscreen + xview;
				if (xcanvas < 0 || xcanvas >= line->len)
					continue;
				mvwaddch(w, yscreen, xscreen, line->data[xcanvas]);
			}
		}
		wmove(w, ycursor, xcursor);
		wrefresh(w);

		// Input
		int ch = getch();
		if (ch == KEY_RESIZE) {
			w = handle_resize(w);
		} else if (ch == KEY_F(1)) {
			is_running = false;
		} else if (ch == KEY_UP) {
			ycursor--;
		} else if (ch == KEY_DOWN) {
			ycursor++;
		} else if (ch == KEY_LEFT) {
			xcursor--;
		} else if (ch == KEY_RIGHT) {
			xcursor++;
		}

		// Cursor view padding
		if (ycursor < HEIGHT * YPADDING) {
			yview--;
			ycursor++;
		} else if (ycursor >= HEIGHT - HEIGHT * YPADDING - 1) {
			yview++;
			ycursor--;
		}
		if (xcursor < WIDTH * XPADDING) {
			xview--;
			xcursor++;
		} else if (xcursor >= WIDTH - WIDTH * XPADDING - 1) {
			xview++;
			xcursor--;
		}
	}

	delwin(w);
	endwin();
	clear_canvas();
	return EXIT_SUCCESS;
}
