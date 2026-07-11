section .data
response: db "HTTP/1.0 200 OK", 13, 10, "Content-Length: 13", 13, 10, 13, 10, "Hello, world!"
response_len: equ $ - response

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
    mov word [rsp + 2], 0xB822     ; # port #: 8888
    mov dword [rsp + 4], 0        ; # listen on all
    mov qword [rsp + 8], 0        ; # no padding


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

    mov rax, 1                   ; # write(client fd, &response, response length)
    mov rdi, r12                 ;
    lea rsi, [response]          ;
    mov rdx, response_len        ;
    syscall

    mov rax, 3                   ; # close client fd
    mov rdi, r12                 ;
    syscall                      ;

    mov rdi, rax                 ;
    mov rax, 60                  ;
    syscall
