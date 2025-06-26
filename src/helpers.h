#ifndef HELPERSH
#define HELPERSH 1
#define sign(x) \
    ({ __typeof__ (x) _x = (x); \
        _x>0 ? 1 : _x<0 ? -1 : 0; })
#define max(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })
#define ctrl(x)           ((x) & 0x1f)
static char* trim_whitespaces(char *str); // trims whitespace from a string. defined in main.c
                                          //

#endif
