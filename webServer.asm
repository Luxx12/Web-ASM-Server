section .data
req_get: db "GET "
get_len: equ $ - req_get
err_response: db "HTTP/1.0 404 Not Found", 13, 10, 13, 10
err_len: equ $ - err_response
status_ok: db "HTTP/1.0 200 OK", 13, 10, 13, 10
status_ok_len: equ $ - status_ok
default_file: db "index.html", 0

section .text
    global _start

_start:
    mov rax, 41                  ; # socket
    mov rdi, 2                   ; # AF_INET = IPv4
    mov rsi, 1                   ; # SOCK_STREAM = TCP
    mov rdx, 0                   ; # default protocol
    syscall


    mov rbx, rax                 ; # save socket fb

    sub rsp, 16                  ; # reserve 16 bytes for sockaddr_in
    mov word [rsp], 2            ; # using IPv4 aswell
    mov word [rsp + 2], 0xB822   ; # port #: 8888
    mov dword [rsp + 4], 0       ; # listen on all
    mov qword [rsp + 8], 0       ; # no padding


    mov rax, 49                  ; # bind(fd, &sock_adrr, 16)
    mov rdi, rbx                 ;
    lea rsi, [rsp]               ;
    mov rdx, 16                  ;
    syscall

    mov rax, 50                  ; # listen(fd, backlog = 5)
    mov rsi, 5                   ;
    mov rdi, rbx                 ;
    syscall


accept_loop:
    mov rax, 43                  ; # accept(fd, client address = 0, struct length = 0)
    mov rdi, rbx                 ;
    mov rsi, 0                   ;
    mov rdx, 0                   ;
    syscall

    mov r12, rax                 ;

    mov rax, 57                  ;
    syscall
    cmp rax, 0                   ;
    jz child                     ;
    jg parent                    ;

    jmp accept_loop              ; # reloop if fork failed

parent:
    mov rax, 3                   ; # close client fd
    mov rdi, r12                 ;
    syscall

    jmp accept_loop              ;

child:
    mov rax, 3                   ; # close listening fd
    mov rdi, rbx                 ;
    syscall

    sub rsp, 2048                ; # getting 2048 bytes from stack for buffer to recieve incoming requests
    mov rax, 0                   ; # read(client fd, &buffer, bufferSize);
    mov rdi, r12                 ;
    lea rsi, [rsp]               ;
    mov rdx, 2048                ;
    syscall

    
    lea rsi, [req_get]           ; # checks if GET was recieved, sends a 400 error if not, basically doing a string comparison here
    lea rdi, [rsp]               ;
    mov rcx, get_len             ;
    cld                          ; # checks if their pointing at the same place in memory
    repe cmpsb                   ; # actually compares both strings in rsi & rdi byte by byte repeatedly using repe
    jne not_get

    mov r13, rdi                 ; # saving start of file path

path_scan:
    cmp byte [rdi], 0x20         ; # checking for space char to see if we've hit boundary between the path and http::/1.1
    je path_end                  ;
    inc rdi                      ;
    jmp path_scan                ;

path_end:
    mov byte [rdi], 0            ;
    cmp byte [r13], '/'          ;
    jne skip_root                ;
    cmp byte [r13 + 1], 0        ;
    jne skip_root                ;
    lea r13, [default_file]      ;
    jmp open_file                ;

skip_root:
    inc r13                      ;

open_file:
    mov rax, 2                   ;
    mov rdi, r13                 ;
    mov rsi, 0                   ;
    syscall

    cmp rax, 0                   ;
    js file_not_found            ;

    mov r14, rax                 ; # saving opened files fd

    sub rsp, 8192                ;
    mov rax, 0                   ;
    mov rdi, r14                 ;
    mov rsi, [rsp]               ;
    mov rdx, 8192                ;
    syscall

    mov r15, rax                 ;

    mov rax, 3                   ;
    mov rdi, r14                 ;
    syscall



not_get:
    mov rax, 1                   ;
    mov rdi, r12                 ;
    lea rsi, [err_response]      ;
    mov rdx, err_len             ;
    syscall

    mov rax, 3                   ;
    mov rdi, r12                 ;
    syscall

    mov rax, 60                  ;
    mov rdi, 1                   ;
    syscall
