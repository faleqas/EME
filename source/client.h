/* date = July 10th 2024 10:51 am */

#ifndef CLIENT_H
#define CLIENT_H

/*
PLATFORM DEPENDENT ABSTRACTION
Functions that should be implemented for every supported platform
*/


#include <stdbool.h>


void create_dialog();


//a client_rect is the drawable area of the window
void get_client_rect(int* w, int* h);

//opens the OS specific file common dialog
//returns the file path
char* get_file_from_user(const char* title, bool open_only);

#endif //CLIENT_H
