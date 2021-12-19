#define ProjectId 123
#define PathName  "queue.h" /* any existing, accessible file would do */
#define MsgLen    100

typedef struct { 
  long type; 
  char payload[MsgLen];  /* bytes in the message */  
} queuedMessage;
