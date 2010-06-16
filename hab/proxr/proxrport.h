// proxrport.h

int proxr_connect(void);
int write_port(char cmd);
int read_port(char *result);
void proxr_disconnect(void);
