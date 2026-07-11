# x86 Assembly Web Server

Renders a from scratch C++ ASCII spinning cube that is recieved by the clinet. The client is a custom terminal user interface (TUI) 

## Dependencies 
We are running everything inside of WSL for simplicity's sake, to run we recommend using a linux environment as well.

ncurses and libcurl are C libraries used by every application that has a TUI or touches a netowrk. They are usually preinstalled but sometimes the dev headers aren't so you may have to install them. 

#### update your packages

`sudo apt update`
`sudo apt upgrade`

#### installing ncurses

`sudo apt install libncurses-dev`

#### installing libcurl

`sudo apt install libcurl4-openssl-dev`
