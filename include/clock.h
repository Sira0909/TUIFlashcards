//i dont wanna make this work for windows....
#ifndef _WIN32
#ifndef clocking
#define clocking
#include <pthread.h>

pthread_t setupClock(int ms, int* fd_command);

void changeinterval(int ms, int fd_command);
void endClock(pthread_t *thread);
#endif
#endif
