#ifndef ZAD1_MESSAGES_H
#define ZAD1_MESSAGES_H


#include <signal.h>
#include <string.h>


#define MAX_CLIENT_NR  10
#define PROJECT_ID 0x099
#define MAX_CONT_SIZE 4096


typedef enum mtype {
    INIT = 1,
    ECHO = 2,
    LIST = 3,
    FRIENDS = 4,
    ADD = 5,
    DEL = 6,
    TOALL = 7,
    TOFRIENDS = 8,
    TOONE = 9,
    STOP = 10,
    READ = 11,
    PASS = 12,
    UNKNOWN = 13
} mtype;


mtype get_mtype_for_str(char *str) {

    if (strncmp("INIT", str, 4) == 0) {
        return INIT;
    } else if (strncmp("ECHO", str, 4) == 0) {
        return ECHO;
    } else if (strncmp("LIST", str, 4) == 0) {
        return LIST;
    } else if (strncmp("FRIENDS", str, 7) == 0) {
        return FRIENDS;
    } else if (strncmp("ADD", str, 3) == 0) {
        return ADD;
    } else if (strncmp("DEL", str, 3) == 0) {
        return DEL;
    } else if (strncmp("TOALL", str, 5) == 0) {
        return TOALL;
    } else if (strncmp("TOFRIENDS", str, 9) == 0) {
        return TOFRIENDS;
    } else if (strncmp("TOONE", str, 5) == 0) {
        return TOONE;
    } else if (strncmp("STOP", str, 4) == 0) {
        return STOP;
    } else if (strncmp("READ", str, 4) == 0) {
        return READ;
    } else if (strncmp("PASS", str, 4) == 0) {
        return PASS;
    } else return UNKNOWN;
}


typedef struct Message {

    long mtype;
    pid_t sender_pid;
    char message_text[MAX_CONT_SIZE];

} Message;


const size_t MESSAGE_SIZE = sizeof(Message) - sizeof(long);


#endif
