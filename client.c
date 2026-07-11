// AdamHStiles
// A custom TUI to recieve and display packets from our x86 Web Server
#include "client.h"

const char *title =
" __   __   _____   ___        _______  _______  __   __    _     _  _______  _______    _______  _______  ______    __   __  _______  ______   \n"
"|  |_|  | |  _  | |   |      |   _   ||       ||  |_|  |  | | _ | ||       ||  _    |  |       ||       ||    _ |  |  | |  ||       ||    _ |  \n"
"|       | | |_| | |   |___   |  |_|  ||  _____||       |  | || || ||    ___|| |_|   |  |  _____||    ___||   | ||  |  |_|  ||    ___||   | ||  \n"
"|       ||   _   ||    _  |  |       || |_____ |       |  |       ||   |___ |       |  | |_____ |   |___ |   |_||_ |       ||   |___ |   |_||_ \n"
" |     | |  | |  ||   | | |  |       ||_____  ||       |  |       ||    ___||  _   |   |_____  ||    ___||    __  ||       ||    ___||    __  |\n"
"|   _   ||  |_|  ||   |_| |  |   _   | _____| || ||_|| |  |   _   ||   |___ | |_|   |   _____| ||   |___ |   |  | | |     | |   |___ |   |  | |\n"
"|__| |__||_______||_______|  |__| |__||_______||_|   |_|  |__| |__||_______||_______|  |_______||_______||___|  |_|  |___|  |_______||___|  |_|\n";

// parameters as defined by curl
size_t curl_callback(void *contents, size_t size, size_t nmeb, void *data){
    size_t total = size * nmeb;
    struct Buffer *buf = (struct Buffer*)data;
    char *ptr = realloc(buf->data, buf->size + total + 1);
    if(!ptr) return 0; // out of memory
    buf->data = ptr;
    memcpy(&(buf->data[buf->size]), contents, total);
    // copies contents into our buffer
    buf->size += total;
    buf->data[buf->size] = '\0';

    return total;
}

int main(){
    const char **frames;
    const char *server_URL = "http://localhost:8888";
    int max_rows, max_cols;
    int f_width, f_height;
    int row_no, col_no;
    f_width = 40; // 160x160 in the future
    f_height = 40;
    int ch, f = 0;

    // *---- curl setup ----*

   // curl_global_init(CURL_GLOBAL_ALL); // starts curl, inits all platforms
   // CURL *easy_handle = curl_easy_init();
   // if(!easy_handle){
   //     printf("CURL failed to initialize!");
   //     return 1;
   // }else{
   //     struct Buffer buf = {malloc(1), 0};
   //     curl_easy_setopt(easy_handle, CURLOPT_URL, server_URL);
   //     curl_easy_setopt(easy_handle, CURLOPT_WRITEFUNCTION, (void*)&buf);
   //     // start sending requests
   // }

    // *---- ncurses setup ----*
    noecho(); 
    raw();
    cbreak();
    initscr();

    getmaxyx(stdscr, max_rows, max_cols);
    
    row_no = max_rows/2 - f_height;
    col_no = max_cols/2 - f_width;
    printw("%s", title);
    mvprintw(max_rows, col_no, "testing");
    nodelay(stdscr, TRUE); // makes getch non-blocking
    // animation loop
    while(getch() != 'q'){
        if(f >= 3) f = 0;
        clear();
        printw("%s", frames[f]);
        refresh();
        napms(500);
        f++;
    }
    // Turns getch blocking on
    nodelay(stdscr, FALSE);
    getch();

    endwin();
    return 0;
}