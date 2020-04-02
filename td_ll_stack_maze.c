#include <stdio.h>
#include <wchar.h>
#include <stddef.h>
#include <stdlib.h>
#include <time.h>

#include "td_ll_stack.h"

static const char *USAGE_TEXT =
    "\tusage: %s size\n"
    "\t       %s width height\n";

static const char *INVALID_NUMBER_OF_ARGUMENTS_ERROR_TEXT =
    "error: invalid number of arguments.\n";

static const char *INVALID_WIDTH_ERROR_TEXT =
    "error: invalid width '%zu'.\n";
static const char *INVALID_HEIGHT_ERROR_TEXT =
    "error: invalid height '%zu'.\n";
static const char *OUT_OF_MEMORY_ERROR_TEXT =
    "error: out of memory.\n";

static const wchar_t MAZE_EMPTY_NOT_VISITED_CELL_CHARACTER =
    L'#';
static const wchar_t MAZE_EMPTY_NOT_BACKTRACKED_CELL_CHARACTER =
    L'+';
static const wchar_t MAZE_EMPTY_CELL_CHARACTER =
    L' ';
static const wchar_t MAZE_VERTICAL_WALL_CHARACTER =
    L'|';
//    L'#';
static const wchar_t MAZE_HORIZONTAL_WALL_CHARACTER =
    L'_';
//  L'#';
static const wchar_t MAZE_WALL_JUNCTION_CHARACTER =
    L'.';
//  L'#';

static const unsigned int RANDOM_SEED =
    42;
static const long UPDATE_DELAY_NSEC =
    25000000L;

typedef enum _td_directions
{
    TD_DIRECTION_NOWHERE    = 0,
    TD_DIRECTION_UP         = 1 << 0,
    TD_DIRECTION_DOWN       = 1 << 1,
    TD_DIRECTION_LEFT       = 1 << 2,
    TD_DIRECTION_RIGHT      = 1 << 3,
    TD_DIRECTION_EVERYWHERE = 0xF
} td_directions;

typedef struct _td_position
{
    int x;
    int y;
} td_position;

typedef struct _td_path_state
{
    td_position position;
    td_directions pending_directions;
} td_path_state;

td_directions td_random_direction_from_directions(
                  td_directions directions
              );

td_directions td_opposite_directions(
                  td_directions direction
              );

int main(int argc, char **argv)
{
    int exit_status;

#define TD_MAIN_EXIT_WITH_FAILURE \
    do {                          \
        exit_status =             \
            EXIT_FAILURE;         \
        goto cleanup;             \
    } while(0)

    char *program_name = NULL;

    size_t width  = 0,
           height = 0;

    size_t total_width  = 0,
           total_height = 0;

    td_directions **open_directions = NULL;
    wchar_t **maze = NULL;

    struct timespec sleep_time;
    sleep_time.tv_sec  = 0;
    sleep_time.tv_nsec = UPDATE_DELAY_NSEC;

    td_path_state *path_state = NULL;
    td_ll_stack *path = NULL;

    if (argc < 2 || argc > 3) {
        if (argc > 0) {
            program_name =
                argv[0];

            fputs(
                INVALID_NUMBER_OF_ARGUMENTS_ERROR_TEXT,
                stderr
            );

            fprintf(
                stderr,
                USAGE_TEXT,
                program_name,
                program_name
            );
        }

        TD_MAIN_EXIT_WITH_FAILURE;
    }

    width =
        strtol(argv[1], NULL, 10);

    if (argc == 2) {
        height =
            width;
    } else if (argc == 3) {
        height =
            strtol(argv[2], NULL, 10);
    }

    if (width == 0) {
        fprintf(
            stderr,
            INVALID_WIDTH_ERROR_TEXT,
            width
        );
    }

    if (height == 0) {
        fprintf(
            stderr,
            INVALID_HEIGHT_ERROR_TEXT,
            height
        );
    }

    if (width == 0 || height == 0) {
        TD_MAIN_EXIT_WITH_FAILURE;
    }

    open_directions =
        (td_directions **) malloc(sizeof(open_directions) * height);

    if (!open_directions) {
        fputs(OUT_OF_MEMORY_ERROR_TEXT, stderr);
        TD_MAIN_EXIT_WITH_FAILURE;
    }

    for (size_t y = 0; y < height; ++y) {
        open_directions[y] =
            (td_directions *) malloc(sizeof(*open_directions) * width);

        if (!open_directions[y]) {
            fputs(OUT_OF_MEMORY_ERROR_TEXT, stderr);
            TD_MAIN_EXIT_WITH_FAILURE;
        }

        for (size_t x = 0; x < width; ++x) {
            open_directions[y][x] =
                TD_DIRECTION_NOWHERE;
        }
    }

    total_width =
        width * 2 + 1;
    total_height =
        height * 2 + 1;

    maze =
        (wchar_t **) malloc(sizeof(maze) * total_height);

    if (!maze) {
        fputs(OUT_OF_MEMORY_ERROR_TEXT, stderr);
        TD_MAIN_EXIT_WITH_FAILURE;
    }

    for (size_t y = 0; y < total_height; ++y) {
        maze[y] =
            (wchar_t *) malloc(sizeof(*maze) * total_width);

        if (!maze[y]) {
            fputs(OUT_OF_MEMORY_ERROR_TEXT, stderr);
            TD_MAIN_EXIT_WITH_FAILURE;
        }

        for (size_t x = 0; x < total_width; ++x) {
            wchar_t cell_character;
            if (y % 2 == 0) {
                if (x % 2 == 0) {
                    cell_character =
                        MAZE_WALL_JUNCTION_CHARACTER;
                } else {
                    cell_character =
                        MAZE_HORIZONTAL_WALL_CHARACTER;
                }
            } else {
                if (x % 2 == 0) {
                    cell_character =
                        MAZE_VERTICAL_WALL_CHARACTER;
                } else {
                    cell_character =
                        MAZE_EMPTY_NOT_VISITED_CELL_CHARACTER;
                }
            }

            maze[y][x] =
                cell_character;

            putwchar(cell_character);
        }
        printf("\n");
    }

    path_state =
        (td_path_state *) malloc(sizeof(*path_state));

    if (!path_state) {
        fputs(OUT_OF_MEMORY_ERROR_TEXT, stderr);
        TD_MAIN_EXIT_WITH_FAILURE;
    }

    *path_state = (td_path_state) {
        { 0, 0 }, TD_DIRECTION_EVERYWHERE
    };

    path =
        td_ll_stack_init(td_ll_stack_alloc());

    if (!path) {
        fputs(OUT_OF_MEMORY_ERROR_TEXT, stderr);
        TD_MAIN_EXIT_WITH_FAILURE;
    }

    td_ll_stack_push(path, path_state);

    srand(RANDOM_SEED);

    for (;;) {
        while (!td_ll_stack_is_empty(path) &&
                    ((td_path_state *) td_ll_stack_top(path))->
                                           pending_directions ==
                                               TD_DIRECTION_NOWHERE) {
            td_path_state *path_state =
                td_ll_stack_pop(path);

            if (path_state) {
                td_position position =
                    path_state->position;

                td_position maze_position = {
                    position.x * 2 + 1,
                    position.y * 2 + 1
                };

                maze[maze_position.y]
                    [maze_position.x] =
                        MAZE_EMPTY_CELL_CHARACTER;

                free(path_state);

                if (!td_ll_stack_is_empty(path)) {
                    path_state =
                        td_ll_stack_top(path);

                    if (path_state) {
                        td_position another_position =
                            path_state->position;

                        td_position another_maze_position = {
                            another_position.x * 2 + 1,
                            another_position.y * 2 + 1
                        };

                        td_position difference = {
                            (another_maze_position.x -
                                 maze_position.x),
                            (another_maze_position.y -
                                 maze_position.y)
                        };

                        if (difference.x != 0) {
                            if (difference.x > 0) {
                                difference.x = 1;
                            } else if (difference.x < 0) {
                                difference.x = -1;
                            }
                        }

                        if (difference.y != 0) {
                            if (difference.y > 0) {
                                difference.y = 1;
                            } else if (difference.y < 0) {
                                difference.y = -1;
                            }
                        }

                        td_position wall_position = {
                            maze_position.x +
                                difference.x,
                            maze_position.y +
                                difference.y
                        };

                        maze[wall_position.y]
                            [wall_position.x] =
                                MAZE_EMPTY_CELL_CHARACTER;
                    }
                }

                printf(
                    "\033[%zuA\033[%zuD",
                    total_height,
                    total_width
                );

                for (size_t y = 0; y < total_height; ++y) {
                    for (size_t x = 0; x < total_width; ++x) {
                        putwchar(maze[y][x]);
                    }
                    printf("\n");
                }

                nanosleep(&sleep_time, NULL);
            }
        }

        if (!td_ll_stack_is_empty(path)) {
            td_path_state *current_path_state =
                td_ll_stack_top(path);

            td_directions random_direction =
                td_random_direction_from_directions(
                    current_path_state->pending_directions
                );

            current_path_state->pending_directions &=
                ~random_direction;

            td_position current_position =
                current_path_state->position;
            td_position next_position =
                current_position;
            td_position wall_position = {
                next_position.x * 2 + 1,
                next_position.y * 2 + 1
            };

            maze[wall_position.y]
                [wall_position.x] =
                    MAZE_EMPTY_NOT_BACKTRACKED_CELL_CHARACTER;

            if (random_direction == TD_DIRECTION_UP) {
                --next_position.y;
                --wall_position.y;
            } else if (random_direction == TD_DIRECTION_DOWN) {
                ++next_position.y;
                ++wall_position.y;
            } else if (random_direction == TD_DIRECTION_LEFT) {
                --next_position.x;
                --wall_position.x;
            } else if (random_direction == TD_DIRECTION_RIGHT) {
                ++next_position.x;
                ++wall_position.x;
            }

            if (next_position.y >= 0 && next_position.y < height &&
                next_position.x >= 0 && next_position.x < width  &&
                    open_directions
                        [next_position.y]
                        [next_position.x] ==
                            TD_DIRECTION_NOWHERE) {
                open_directions
                    [current_position.y]
                    [current_position.x] |=
                        random_direction;
                open_directions
                    [next_position.y]
                    [next_position.x] |=
                        td_opposite_directions(random_direction);

                maze[wall_position.y]
                    [wall_position.x] =
                        MAZE_EMPTY_NOT_BACKTRACKED_CELL_CHARACTER;

                printf(
                    "\033[%zuA\033[%zuD",
                    total_height,
                    total_width
                );

                for (size_t y = 0; y < total_height; ++y) {
                    for (size_t x = 0; x < total_width; ++x) {
                        putwchar(maze[y][x]);
                    }
                    printf("\n");
                }

                nanosleep(&sleep_time, NULL);

                path_state =
                    (td_path_state *) malloc(sizeof(*path_state));

                if (!path_state) {
                    fputs(OUT_OF_MEMORY_ERROR_TEXT, stderr);
                    TD_MAIN_EXIT_WITH_FAILURE;
                }

                *path_state = (td_path_state) {
                    next_position, TD_DIRECTION_EVERYWHERE
                };

                td_ll_stack_push(path, path_state);
            }
        } else {
            break;
        }
    }

    exit_status =
        EXIT_SUCCESS;

cleanup:
    if (open_directions) {
        for (size_t y = 0; y < height; ++y) {
            if (open_directions[y]) {
                free(open_directions[y]);
                open_directions[y] = NULL;
            }
        }

        free(open_directions);
        open_directions = NULL;
    }

    if (maze) {
        for (size_t y = 0; y < height; ++y) {
            if (maze[y]) {
                free(maze[y]);
                maze[y] = NULL;
            }
        }

        free(maze);
        maze = NULL;
    }

    td_ll_stack_free_with_elements(path);
    path = NULL;

    return exit_status;

#undef TD_MAIN_EXIT_WITH_FAILURE
}

td_directions td_random_direction_from_directions(
                 td_directions directions
              ) {
    td_directions random_direction =
        TD_DIRECTION_NOWHERE;

    int direction_count = 0;

    if (directions & TD_DIRECTION_UP) {
        ++direction_count;
    }

    if (directions & TD_DIRECTION_DOWN) {
        ++direction_count;
    }

    if (directions & TD_DIRECTION_LEFT) {
        ++direction_count;
    }

    if (directions & TD_DIRECTION_RIGHT) {
        ++direction_count;
    }

    if (direction_count > 0) {
        int selection =
            rand() % direction_count + 1;

        int found = 0;

        direction_count = 0;

        if (!found && directions & TD_DIRECTION_UP) {
            ++direction_count;

            if (direction_count == selection) {
                found = !found;
                random_direction =
                    TD_DIRECTION_UP;
            }
        }

        if (!found && directions & TD_DIRECTION_DOWN) {
            ++direction_count;

            if (direction_count == selection) {
                found = !found;
                random_direction =
                    TD_DIRECTION_DOWN;
            }
        }

        if (!found && directions & TD_DIRECTION_LEFT) {
            ++direction_count;

            if (direction_count == selection) {
                found = !found;
                random_direction =
                    TD_DIRECTION_LEFT;
            }
        }

        if (!found && directions & TD_DIRECTION_RIGHT) {
            ++direction_count;

            if (direction_count == selection) {
                found = !found;
                random_direction =
                    TD_DIRECTION_RIGHT;
            }
        }
    }

    return random_direction;
}

td_directions td_opposite_directions(
                  td_directions direction
              ) {
    td_directions opposite_directions =
        TD_DIRECTION_NOWHERE;

    if (direction & TD_DIRECTION_UP) {
        opposite_directions |=
            TD_DIRECTION_DOWN;
    }

    if (direction & TD_DIRECTION_DOWN) {
        opposite_directions |=
            TD_DIRECTION_UP;
    }

    if (direction & TD_DIRECTION_LEFT) {
        opposite_directions |=
            TD_DIRECTION_RIGHT;
    }

    if (direction & TD_DIRECTION_RIGHT) {
        opposite_directions |=
            TD_DIRECTION_LEFT;
    }

    return opposite_directions;
}

