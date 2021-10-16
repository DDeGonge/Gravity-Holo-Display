#include <string>
#include <vector>

#define MAX_MSG_LEN 1000

bool respondToSerial(char (&serial_data) [MAX_MSG_LEN]);
void clear_data(char (&serial_data) [MAX_MSG_LEN]);
