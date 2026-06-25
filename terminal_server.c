#include "terminal_server.h"

void rc(int r, int c)
{
    printf("\033[%d;%df",r,c); 
    fflush(stdout);
}

void clr(void)
{
    printf("\033[2J \n");
}
/**********************************Term Server and Term Client common data */
#define BUFF_LEN 1024
char SOCK_PATH[2][104] ={{"/tmp/term_server_temp_socket_file_0"},          // socket file 0 is input terminal
                               {"/tmp/term_server_temp_socket_file_1"}};   //  '    '     1 is output terminal

/************************************Terminal Server "Class"********************** */

void term_server_s::setup(int IO)
{

    printf("input output type IO=(%d)\n",IO);
    memset(&ts_server_sockaddr,0,sizeof(ts_server_sockaddr));
    ts_server_sockaddr.sun_family = AF_LOCAL; 
    strncpy(ts_server_sockaddr.sun_path,&(SOCK_PATH[IO][0]),sizeof(ts_server_sockaddr.sun_path));
    if ((term_server_fd = socket(AF_LOCAL, SOCK_STREAM, 0)) == -1)
    {
        perror("socket");
        fprintf(stderr,"error on line %d in file: %s\n",__LINE__,__FILE__);
        exit(1);
    }
    unlink(ts_server_sockaddr.sun_path);
    {   /* limit "len"'s scope, lenght of address not includeing null termination byte */
        int len = strlen(ts_server_sockaddr.sun_path) + sizeof(ts_server_sockaddr.sun_family);
        int len2 = (offsetof (struct sockaddr_un,sun_path) + strlen(ts_server_sockaddr.sun_path));
        int len3 = SUN_LEN(&ts_server_sockaddr);
        int test = len==len2 && len2 == len3;
        //printf("sizeof(sockaddr) is following: test(%d), len(%d), len2(%d), len3(%d)\n",test,len,len2,len3);
        if (bind(term_server_fd, (struct sockaddr*)&ts_server_sockaddr, len) == -1)
        {
            perror("bind"); 
            fprintf(stderr,"error on line %d in file: %s\n",__LINE__,__FILE__);
            exit(1);
        }
    }
    if (listen(term_server_fd, 5) == -1)  //non-blocking, marks socket as "passive" socket,["stack up some clients"]
    {
        perror("listen");
        fprintf(stderr,"error on line %d in file: %s\n",__LINE__,__FILE__);
        exit(1);
    }
}

/***************************Terminal Server Program, Input terminal and Output terminal********** */
                  /**** Designed for a client(below) to have two terminals, an input and output ****/
#ifdef TERM_SERVER_MAIN
int
main(int argc,char **argv) // usage: argv[0] [0|1]  -- 0 makes it input terminal, 1 makes it output terminal
{
    term_server_s term_server;
    int IO=-1;             // IO argument to class setup() makes it input or output terminal
    if(argc < 2)
    {
        fprintf(stderr,"usage: %s (0|1) // term 0 input term 1 output\nexiting.",argv[0]);
        exit(0);
    }
    if(0==strncmp(argv[1],"0",1))
    {
        IO=0;
    }else if(0 == strncmp(argv[1],"1",1))
    {
        IO=1;
    }
    term_server.setup(IO);  //IO selects socket filename
    char bufRecv[BUFF_LEN]={0};
    char bufStdin[BUFF_LEN];
    int quit=0;
    for (;0==quit;)    // outer loop runs accept() again after losing client
    {
        { //accept or re-accept (scope limiting parenthesis)
            socklen_t slen = sizeof(term_server.ts_client_addr); // slen is both input to and output from accept()
            if ((term_server.term_client_fd =
                   accept(term_server.term_server_fd, (struct sockaddr*)&term_server.ts_client_addr, &slen)) == -1)
            {
                perror("accept");
                fprintf(stderr,"Error line(%d), file(%s)\n",__LINE__,__FILE__);
                exit(1);
            }
        }
        clr();
        rc(0,25);
        if(0==IO) 
            printf("INPUT Screen\n");
        else
            printf("OUTPUT Screen\n");
        int done = 0,cnt=0;
        do
        {
            /* terminal 0 reads Keyboard input and sends to term 0's client*/
            if(0 == IO)  // IO is 0 if this is the input terminal blocking on fgets
            {
                ++cnt;
                bufStdin[0]=0;
                void* p_void = fgets(bufStdin, BUFF_LEN, stdin);  // term1 blocks, term2 non-blocking because of fcntl above
                if ((0 == strncmp(bufStdin, "quit", 4)))// || (0==strncmp(bufRecv,"quit",4)))
                {
                    done = 1;
                    quit = 1;
                }
                if(0 == strncmp(bufStdin,"*clrscr*",8))
                {
                    printf("\033[2J\n");
                }
                if(bufStdin[0] != 0)
                {
                    if (send(term_server.term_client_fd, bufStdin, strnlen(bufStdin,BUFF_LEN)+1, 0) < 0)
                    {
                        perror("send");
                        fprintf(stderr,"Error: line(%d), file(%s)\n",__LINE__,__FILE__);
                        done = 1; 
                    } 
                }
            }
            /* term 1 Receives from client and writes to it's screen*/
            else if(1 == IO)
            {
                int bytes_recved = recv(term_server.term_client_fd, bufRecv, sizeof(bufRecv), 0);
                /* Display Teriminal's client receipt           Lines 6 to 15 */
                if(bytes_recved>0)
                printf("%s",bufRecv);
                if(0 == strncmp(bufRecv,"quit",4))
                {
                    done = 1;
                    quit = 1;
                }
                if(0 == strncmp(bufRecv,"*clrscr*",8))
                {
                    clr();
                }
                if(0 == strncmp(bufRecv,"*goto*",6))
                {
                    int x=0,y=0;
                    int cnt = sscanf(bufRecv,"*goto* %d %d",&y,&x);
                    if(2 == cnt)
                    {
                        rc(y,x);
                    }
                }
                /* end Display Terminal's client receipt */
                if(bytes_recved ==0)
                usleep(10000);
            }
            else
            {
                fprintf(stderr,"Logic error.\n");
                fprintf(stderr,"Line(%d) , File(%s)\n",__LINE__,__FILE__);
                exit(0);
            }
        } while (!done);

        //close(term_server.term_client_fd);   Todo, clean up files after usage
        sleep(1);
    }
    return 0; 
}
#endif // TERM_SERVER_MAIN




/******************************************Terminal Client "Class"******************************/

void
TerminalClient_s::setup(int IO)
{
    printf("client #%d\n",IO);
    tc_fd = socket(AF_LOCAL, SOCK_STREAM, 0);
    if (-1 == tc_fd)
    {
        perror("socket");
        fprintf(stderr, "Line(%d) FILE(%s)\n", __LINE__, __FILE__);
        exit(1);
    }
    tc_server_sockaddr.sun_family = AF_LOCAL;
    strncpy(tc_server_sockaddr.sun_path, &(SOCK_PATH[IO][0]),
            sizeof(tc_server_sockaddr.sun_path));
    {
        int len = sizeof(sockaddr_un);
        int connected = 0;
        while (0 == connected)
        {
            int connectResult =
              connect(tc_fd, (struct sockaddr*)&tc_server_sockaddr, len);
            if (0 == connectResult)
            {
                connected = 1;
            }
            else // -1 == connectResult, eg not connected
            {
                perror("connect");
                fprintf(stderr, "connectResult(%d), LINE(%d) , FILE(%s) client#%d\n",
                        connectResult, __LINE__, __FILE__,IO);
                fprintf(stderr, "path(%s)\n", tc_server_sockaddr.sun_path); 
                sleep(1); //keep trying again
            }
        }
    }
}

int
TerminalClient_s::tc_send(const char* buff)
{
    int cnt_sent = send(tc_fd, buff, strlen(buff) + 1, MSG_DONTWAIT);   // Don't Block!
    if (cnt_sent == -1)
    {
        perror("send");
        fprintf(stderr, "LINE(%d), FILE(%s)\n", __LINE__, __FILE__);
        exit(1);
    }
    return cnt_sent;
}
int
TerminalClient_s::tc_recv(char* buff, int len)
{
    int len_recv = recv(tc_fd, buff, len - 1, MSG_DONTWAIT);  // Don't Block!
    if (len_recv >= 0)
    {
        buff[len_recv] = '\0';
    }
    return len_recv;
}






/***************************Framework for terminal client to have two terminals: input and output */
/*******************  Client only needs term_input.tc_recv, term_output.tc_send and to pass a 'quit' to output to close gnome-terminals*/
#ifdef TEST_CLIENT_TERMINAL_MAIN
int
main()
{ 
    system("gnome-terminal -- ./terminal_server.exe 0");
    sleep(5);
    system("gnome-terminal -- ./terminal_server.exe 1");
    sleep(5);
    TerminalClient_s term_input,term_output;
    term_input.setup(0);
    term_output.setup(1);
    char buff[BUFF_LEN] = { 0 };
    int done = 0;
    while (0 == done)
    {
        buff[0]='\0';
        /*************************************  Receive  ********************* */
        term_input.tc_recv(buff, BUFF_LEN);  // Note: this is from input terminal
        int len = strnlen(buff,BUFF_LEN);
        if(len>0)
        {
        /**************************************  Send  ************************* */
            term_output.tc_send(buff);        // Note: this is to output terminal
            if(0 == strncmp(buff,"quit",4))
            {
                done = 1;
            } 
        }
        /************ If nothing happend, len==0, Output dots on this terminal to show life and sleep  *********/
        if(len == 0)
        { 
            static int k = 1000;
            --k;
            if(k<0) 
            {
                printf("."); 
                fflush(stdout);
                k=1000;
            }
            usleep(10000);
        }
    }
    return 0;
}
#endif // TEST_TERMINAL_CLIENT













#ifdef clipboard2  //Dead code
void rc(int r, int c)
{
    printf("\033[%d;%df",r,c);
}
void fl(void){fflush(stdout);}
void dl(void){printf("\033[K");}
typedef struct mat_s{
    int m,n;
    const char *pname;
    double p[];
} mat_t;
mat_t T{
    4,4,"T",
    {0,1.0,0,0,
    0.6,0,-0.8,0,
    -0.8,0,-0.6,5.0,
    0,0,0,1,4*4}};
void mat_print_string(mat_t *m, char *buff)
{
    char stick[100]={0};
    printf("mn(%d,%d) %f\n",m->m,m->n,m->p[16]);
    buff[0]=0;
    sprintf(buff,"\033[2;0f\n%s mn(%d,%d) %f\n",m->pname,m->m,m->n,m->p[16]);
    for(int i=0;i<m->m;i++)
    {
        for(int j=0;j<m->n;j++) 
        { 
            printf("%10.5f,",m->p[i*m->n+j]);
            sprintf(stick,"%10.5f,",m->p[i*m->n+j]);
            strncat(buff,stick,strnlen(stick,100));
        }
        strncat(buff,"\n",strnlen("\n",2)) ; 
        puts("");
    }
}
    if (0)
    {
        int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
        fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
    }
        if(0)
        {
            void* p_void = fgets(buff, BUFF_LEN, stdin);
            if (NULL != p_void)
            { 
                int cnt_sent = term_input.tc_send(buff); 
                if (0 == strncmp(buff, "quit", 4)) 
                { 
                    done = 1; 
                    break; 
                }
            }
            if(0)//NULL == p_void)
            {
                if (ferror(stdin))
                {
                    //perror("stdin");
                    //fprintf(stderr,"ferror: line(%d) file(%s)\n",__LINE__,__FILE__);
                    //clearerr(stdin);
                }
            }
        }
            if (0)// < len_recv)
            {
                printf("x%s", buff);
                fflush(stdout);
            }
    {
        int flags = fcntl(STDIN_FILENO, F_GETFL);
        if(flags==-1)
        {
            perror("fcntl stdin");
        }
        if(0) // term two never reads keyboard if(2 == term_server.This_term_number) 
        // also term one blocks on input so nonblock not used
        { 
            fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK); 
        }
    }
                if (NULL == p_void)
                {
                    if (ferror(stdin))
                    {
                        //puts("clear error");
                        clearerr(stdin);
                    }
                }
                if (bytes_recved <= 0)
                {
                    if (bytes_recved < 0)
                        perror("recv");
                }
    else if(0)
    {
        if (len_recv < 0)
            perror("recv");
        else
            printf("Server closed connection\n");
        //exit(1);
    }
#endif