## Notes

HTTP requests get a stream of packets at a time so I created a buffer struct to store the returned data from the server as the packets arrive.

the parameters of curl_callback come directly from the HTTP request.

I will assume that get_frame() gets a full frame and work on creating the rest of the UI elements.

Currently the animation loop is done. The curl stuff is almost ready to be tested-- not sure if it works.

## Deliverables

Send GET requests to the server.
return full frames to me, make a get_frame() function.
setup may be correct, not sure feel free to make changes to the initialization.

## Context

My conversation: https://claude.ai/share/0771c348-fea9-42c3-9288-1f03df2bdb36
This isn't that useful but helpful: https://curl.se/libcurl/c/libcurl-tutorial.html
API reference: https://curl.se/libcurl/c/allfuncs.html
This is a different project with the same stack as us: https://github.com/kurealnum/todoist-terminal/blob/main/main.c
