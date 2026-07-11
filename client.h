#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include <curl/curl.h>

struct Buffer{
    char *data;
    size_t size;
};

struct menu_button{
    int row;
    int col;
    char *txt;
};

size_t curl_callback(void *contents, size_t size, size_t nmeb, void *data){