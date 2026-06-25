#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <fcntl.h>
#include <unistd.h>
void rc(int r, int c);
void clr(void);
struct term_server_s
{
    //enum {SETUP,CHECKING_ACCEPT,RUNNING,LOST_CLIENT,DONE} STATE = SETUP;
    int term_server_fd, term_client_fd; 
    struct sockaddr_un ts_client_addr, ts_server_sockaddr;
    void setup(int IO);

};


struct TerminalClient_s
{
    int tc_fd;
    struct sockaddr_un tc_server_sockaddr{};
    void setup(int IO);
    int tc_send(const char* buff);
    int tc_recv(char* buff, int len);
};