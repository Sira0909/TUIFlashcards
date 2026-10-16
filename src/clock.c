//template for later mostly
#include <bits/types/sigevent_t.h>
#include <pthread.h>
#include <poll.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
struct clock_data{
    int ms; //interval
    int pipein;
    pthread_t parent;
};
void closer(void* arg){
    struct clock_data *params = arg;
    close((params->pipein));
    free(params);
}
void* Metronome(void* arg){//arg is a clock_data
    struct clock_data params = *((struct clock_data*)arg);
    pthread_cleanup_push(closer, (arg));
        struct pollfd pfds[1];
        pfds[0].fd = params.pipein;
        pfds[0].events = POLLIN;
        while(1==1){
            poll(pfds, 1, params.ms);
            if(pfds[0].revents&POLLIN){
                read(params.pipein,&params.ms,sizeof(params.ms));
            }
            else{
                pthread_kill(params.parent, SIGALRM);
            }
        }
    pthread_cleanup_pop(0);
}

pthread_t setupClock(int ms, int* fd_command){ 
    pthread_t metronome;
    int command_pipe[2];
    pipe(command_pipe);
    *fd_command = command_pipe[1];
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    struct clock_data* data = malloc(sizeof(struct clock_data));
    *data = (struct clock_data){ms, command_pipe[0], pthread_self()};
    pthread_create(&metronome, &attr,&Metronome, data);
    pthread_attr_destroy(&attr);
    return metronome;
}

void changeinterval(int ms, int fd_command){
    write(fd_command, &ms, sizeof(int));
}
void endClock(pthread_t *thread){
    pthread_cancel(*thread);
    *thread=0;
}
