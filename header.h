#ifndef HEADER_H    
// To make sure you don't declare the function more than once by including the header multiple times.
#define HEADER_H

int serverinit();
int clientinit();
char* serverfn(int, int, int, int, int);
char* clientfn(int, int, int, int, int);

#endif