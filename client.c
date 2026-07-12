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

char *read_output_file(const char *path){
    FILE *fp = fopen(path, "rb");
    if(!fp) return NULL;

    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    rewind(fp);

    char *buf = malloc(size + 1);
    fread(buf, 1, size, fp);
    buf[size] = '\0';

    fclose(fp);
    return buf;
}

char *GET_FRAME(CURL *easy_handle, double theta, double phi, double distance){
    char url[128];
    snprintf(url, sizeof(url), "http://localhost:8888/?t=%f&p=%f&d=%f", theta, phi, distance);

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

int main(){
    const char *server_URL = "http://localhost:8888";
    int max_rows, max_cols;
    int f_width, f_height;
    int row_no, col_no;
    f_width = 160;
    f_height = 160;
    int ch;
    double theta = 0.0, phi = 0.5, distance = 10.0;

    // *---- curl setup ----*
    curl_global_init(CURL_GLOBAL_ALL);
    CURL *easy_handle = curl_easy_init();
    if(!easy_handle){
        printf("CURL failed to initialize!");
        return 1;
    }

    // *---- ncurses setup ----*
    initscr();
    noecho();
    raw();
    cbreak();

    getmaxyx(stdscr, max_rows, max_cols);

    row_no = (max_rows > f_height) ? (max_rows/2 - f_height/2) : 0;
    col_no = (max_cols > f_width) ? (max_cols/2 - f_width/2) : 0;

    // splash screen -- shown once before the animation loop starts
    printw("%s", title);
    refresh();
    napms(1500);

    nodelay(stdscr, TRUE); // makes getch non-blocking

    // animation loop
    while(getch() != 'q'){
        char *frame = GET_FRAME(easy_handle, theta, phi, distance);
        clear();
        mvprintw(row_no, col_no, "%s", frame);
        free(frame);
        refresh();
        napms(500);
        theta += 0.1; // rotation step per frame
    }

    // Turns getch blocking on
    nodelay(stdscr, FALSE);
    getch();

    endwin();
    curl_easy_cleanup(easy_handle);
    curl_global_cleanup();
    return 0;
}
