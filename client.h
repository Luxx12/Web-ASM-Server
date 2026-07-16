#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include <curl/curl.h>

struct Buffer{
    char *data;
    size_t size;
};

size_t curl_callback(void *contents, size_t size, size_t nmeb, void *data);

char *get_frame(CURL *easy_handle, double theta, double phi, double distance);

char **get_frames(CURL *easy_handle);

void start_menu(int center_row, int center_col);

void animation_loop(char **frames, int num_frames);
