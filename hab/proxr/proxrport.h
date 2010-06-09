// proxrport.h

int open_port(void);
int write_port(char cmd);
int read_port(char *result);
void close_port(void);
