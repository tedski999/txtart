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

#ifndef TXTART_VERSION
#define TXTART_VERSION ""
#endif

#define WIDTH (COLS - 2)
#define HEIGHT (LINES - 2)
#define XPADDING (0.1)
#define YPADDING (0.05)

struct line {
	int offset;
	int data_len;
	char *data;
};

struct canvas {
	int offset;
	int max_width;
	int lines_len;
	struct line *lines;
};

static WINDOW *handle_resize(WINDOW *w) {
	erase();
	border(0, 0, 0, 0, 0, 0, 0, 0);
	refresh();
	if (w)
		delwin(w);
	return newwin(HEIGHT, WIDTH, 1, 1);
}

static void write_char_to_canvas(struct canvas *c, char ch, int x, int y) {
	y += c->offset;
	if (y < 0) {
		int len = -y;
		c->lines = realloc(c->lines, sizeof *c->lines * (c->lines_len + len));
		memmove(c->lines + len, c->lines, sizeof *c->lines * c->lines_len);
		memset(c->lines, 0, sizeof *c->lines * len);
		c->lines_len += len;
		c->offset += len;
		y = 0;
	} else if (y >= c->lines_len) {
		int len = y - c->lines_len + 1;
		c->lines = realloc(c->lines, sizeof *c->lines * (c->lines_len + len));
		memset(c->lines + c->lines_len, 0, sizeof *c->lines * len);
		c->lines_len += len;
	}

	struct line *line = c->lines + y;
	x += line->offset;
	if (x < 0) {
		int len = -x;
		line->data = realloc(line->data, line->data_len + len);
		memmove(line->data + len, line->data, line->data_len);
		memset(line->data, ' ', len);
		line->data_len += len;
		line->offset += len;
		x = 0;
	} else if (x >= line->data_len) {
		int len = x - line->data_len + 1;
		line->data = realloc(line->data, line->data_len + len);
		memset(line->data + line->data_len, ' ', len);
		line->data_len += len;
	}

	line->data[x] = ch;
}

static void clear_canvas(struct canvas *c) {
	for (int i = 0; i < c->lines_len; i++)
		free(c->lines[i].data);
	*c = (struct canvas) {0};
}

static bool load_file(struct canvas *c, char *filename) {
	FILE *fp = fopen(filename, "r");
	if (fp == NULL)
		return false;

	size_t read;
	struct line *line = NULL;
	clear_canvas(c);
	do {
		c->lines_len++;
		c->lines = realloc(c->lines, sizeof *c->lines * c->lines_len);
		line = c->lines + c->lines_len - 1;
		// TODO: replace leading whitespace with offset
		*line = (struct line) {0};
		line->data_len = read = getline(&line->data, &(size_t){0}, fp);
		if (line->data[line->data_len - 1] == '\n')
			line->data_len--;
		if (line->data_len > c->max_width)
			c->max_width = line->data_len;
	} while (read != -1);

	c->lines_len--;
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

	struct canvas *c = &(struct canvas) {0};
	if (active_file && !load_file(c, active_file)) {
		fprintf(stderr, "Unable to load '%s'.\n", active_file);
		return EXIT_FAILURE;
	}

	initscr();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);
	WINDOW *w = handle_resize(NULL);

	bool is_running = true;
	int xview = (c->max_width - WIDTH) / 2;
	int yview = (c->lines_len - HEIGHT) / 2;
	int xcursor = WIDTH / 2 - 1;
	int ycursor = HEIGHT / 2 - 1;

	while (is_running) {

		// Drawing
		werase(w);
		for (int yscreen = 0; yscreen < HEIGHT; yscreen++) {
			for (int xscreen = 0; xscreen < WIDTH; xscreen++) {
				int ycanvas = yscreen + yview + c->offset;
				if (ycanvas < 0 || ycanvas >= c->lines_len)
					continue;
				struct line *line = c->lines + ycanvas;
				int xcanvas = xscreen + xview + line->offset;
				if (xcanvas < 0 || xcanvas >= line->data_len)
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
		} else if (ch >= ' ' && ch <= '~') {
			write_char_to_canvas(c, ch, xview + xcursor, yview + ycursor);
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
	clear_canvas(c);
	return EXIT_SUCCESS;
}
