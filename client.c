// AdamHStiles
// A custom TUI to recieve and display packets from our x86 Web Server
#include "client.h"

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
void start_menu(int center_row, int center_col){
    const char *title =
    "     ___ ___    _____ _____ _____    _ _ _     _      _____                     \n"
    " _ _| . |  _|  |  _  |   __|     |  | | | |___| |_   |   __|___ ___ _ _ ___ ___ \n"
    "|_'_| . | . |  |     |__   | | | |  | | | | -_| . |  |__   | -_|  _| | | -_|  _|\n"
    "|_,_|___|___|  |__|__|_____|_|_|_|  |_____|___|___|  |_____|___|_|  \\_/|___|_|  \n";
    printw("%s", title);
    const char *start = "press any key to start.";
    mvprintw(center_row, center_col-strlen(start), "%s", start);
    getch();
    clear();
    return;
}

char *get_frame(CURL *easy_handle, double theta, double phi, double distance){
    char url[128];
    snprintf(url, sizeof(url), "http://localhost:8888/%f,%f,%f", theta, phi, distance);

    struct Buffer buf = {malloc(1), 0};

    curl_easy_setopt(easy_handle, CURLOPT_URL, url);
    curl_easy_setopt(easy_handle, CURLOPT_WRITEFUNCTION, curl_callback);
    curl_easy_setopt(easy_handle, CURLOPT_WRITEDATA, (void*)&buf);

    CURLcode res = curl_easy_perform(easy_handle);
    if(res != CURLE_OK){
        fprintf(stderr, "GET_FRAME failed: %s\n", curl_easy_strerror(res));
        free(buf.data);
        return NULL;
    }
    return buf.data;
}

char **get_frames(CURL *easy_handle){
    char *frame, **frames;
    double theta, phi, distance; // theta (0-360) phi (0-360)
    int frame_size = 25600;
    int num_frames = 360;
    distance = 2;
    frames = (char**)calloc(num_frames, sizeof(char)*frame_size + 1);
    int i;
    theta = 0;
    phi = 0;
    for(i = 0; i < num_frames; ++i){
        frame = get_frame(easy_handle, theta, phi, distance);
        if(frame == NULL) return NULL;
        theta++;
        phi++;
        frames[i] = frame;
    }
    return frames;
}

void animation_loop(char **frames, int num_frames){
    int f;
    nodelay(stdscr, true);
    while(getch() != 'q'){
        if(f >= num_frames) f = 0;
        clear();
        printw("%s", frames[f]);
        refresh();
        napms(500);
        f++;
    }
    nodelay(stdscr, false);
    return;
}
int main(){
    char **frames;
    const char *server_URL = "http://localhost:8888";
    int max_rows, max_cols;
    int f_width, f_height;
    int row_no, col_no;
    f_width = 160;
    f_height = 160;
    int ch, f = 0;

    // *---- curl setup ----*
    curl_global_init(CURL_GLOBAL_ALL);
    CURL *easy_handle = curl_easy_init();
    if(!easy_handle){
        printf("CURL failed to initialize!");
        return 1;
    }

    // *---- ncurses setup ----*
    noecho(); 
    raw();
    cbreak();
    initscr();
    getmaxyx(stdscr, max_rows, max_cols);

    start_menu(row_no, col_no); 
    clear();
    mvprintw(row_no, col_no-strlen("Loading..."), "Loading...");
    frames = get_frames(easy_handle);
    
    animation_loop(frames, 360);

    endwin();
    return 0;
}