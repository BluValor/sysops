#include <unistd.h>
#include <time.h>
#include "systemv_messages.h"
#include "systemv_project_functions.h"


int server_queue_descriptor = -1;
int loop_on = 1;
int clients_pids[MAX_CLIENT_NR];
int clients_queue_descriptors[MAX_CLIENT_NR];
int clients_friends[MAX_CLIENT_NR][MAX_CLIENT_NR];
int client_count = 0;


int find_pid_position(int pid) {

    for (int i = 0; i < MAX_CLIENT_NR; i++)
        if (clients_pids[i] == pid)
            return i;

    return -1;
}


void close_queue() {

    if (server_queue_descriptor > -1) {

        if (msgctl(server_queue_descriptor, IPC_RMID, NULL) == -1)
            fprintf(stderr, "Deleting server's queue failed!\n");

        else
            printf("Server's queue deleted.\n");
    }
}


void create_server_queue(char *path, int project_id) {

    int key = ftok(path, project_id);
    if (key == -1) {

        fprintf(stderr, "Server's queue key generating failed!\n");
        exit(1);
    }

//    server_queue_descriptor = msgget(key, 0);
//    msgctl(server_queue_descriptor, IPC_RMID, NULL);

    server_queue_descriptor = msgget(key, IPC_CREAT | IPC_EXCL | 0666);
    if (server_queue_descriptor == -1)  {

        fprintf(stderr, "Opening server's queue failed!\n");
        exit(1);
    }
}


void append_date_to_message(struct Message *message) {

    struct tm *current_time;
    time_t now = time(NULL);
    current_time = localtime(&now);
    char date_buffer[21];
    strftime(date_buffer, sizeof(date_buffer) + 1, " %Y-%m-%d_%H:%M:%S", current_time);
    date_buffer[20] = '\0';

    int length = (int) strlen(message -> message_text);
    if (length + sizeof(date_buffer) > MAX_CONT_SIZE) {

        printf("WARNING. Couldn't append date at the end of the text due to it's length!\n");
        return;
    }

    memcpy(message -> message_text + length, &date_buffer, sizeof(date_buffer));
}


void append_id_to_message(struct Message *message, int id) {

    char pid_buffer[32];

    int length = (int) strlen(message -> message_text);
    if (length + sizeof(pid_buffer) > MAX_CONT_SIZE) {

        printf("WARNING. Couldn't append client id at the end of the text due to its length!\n");
        return;
    }

    sprintf(pid_buffer, " from client: \"%d\"", id);
    memcpy(message -> message_text + length, &pid_buffer, sizeof(pid_buffer));
}


void process_init(struct Message *message) {

    key_t client_queue_key;
    if (sscanf(message -> message_text, "%d", &client_queue_key) <= 0) {

        fprintf(stderr, "INIT -> failure. Bad message text value!\n");
        exit(1);
    }

    int client_queue_descriptor = msgget(client_queue_key, 0);
    if (client_queue_key == -1) {

        fprintf(stderr, "INIT -> failure. Opening client's queue faile!\n");
        exit(1);
    }

    int set_up = 1;

    if (client_count >= MAX_CLIENT_NR) {

        fprintf(stderr, "INIT -> failure. MAX_CLIENT_NR exceeded!\n");
        sprintf(message -> message_text, "%d", -1);
        set_up = 0;

    } else {

        clients_pids[client_count] = message -> sender_pid;
        clients_queue_descriptors[client_count] = client_queue_descriptor;
        sprintf(message -> message_text, "%d", client_count);
    }

    message -> sender_pid = getpid();

    if (msgsnd(client_queue_descriptor, message, MESSAGE_SIZE, 0) == -1) {

        fprintf(stderr, "INIT -> failure. Sending message failed!\n");
        exit(1);
    }

    if (set_up) {

        memset(clients_friends[client_count], 0, sizeof(clients_friends[client_count]));

        printf("INIT -> success. Client session nr. %d started.\n", client_count);
        client_count++;
    }
}


void process_stop(struct Message *message) {

    int client_session_id = find_pid_position(message -> sender_pid);
    if (client_session_id == -1) {

        fprintf(stderr, "STOP -> failure. Invalid sender pid.");
        exit(1);
    }

    clients_pids[client_session_id] = 0;
    clients_queue_descriptors[client_session_id] = 0;

    printf("STOP -> success. Client session nr. %d stopped.\n", client_session_id);
}


void process_echo(struct Message *message) {

    int client_session_id = find_pid_position(message -> sender_pid);
    if (client_session_id == -1) {

        fprintf(stderr, "ECHO -> failure. Invalid sender pid!\n");
        exit(1);
    }

    append_date_to_message(message);
    message -> sender_pid = getpid();

    if (msgsnd(clients_queue_descriptors[client_session_id], message, MESSAGE_SIZE, 0) == -1) {

        fprintf(stderr, "ECHO -> failure. Sending message failed!\n");
        exit(1);
    }

    printf("ECHO -> success.\n");
}


void process_list(struct Message *message) {

    int client_session_id = find_pid_position(message -> sender_pid);
    if (client_session_id == -1) {

        fprintf(stderr, "ECHO -> failure. Invalid sender pid!\n");
        exit(1);
    }

    message -> sender_pid = getpid();

    int offset = 0;
    for (int i = 0; i < client_count; i++) {

        if (clients_pids[i]) {

            sprintf(message -> message_text + offset, "%3d", i);
            offset += 3;
        }
    }
    message -> message_text[offset] = '\0';

    if (msgsnd(clients_queue_descriptors[client_session_id], message, MESSAGE_SIZE, 0) == -1) {

        fprintf(stderr, "LIST -> failure. Sending message failed!\n");
        exit(1);
    }

    printf("LIST -> success.\n");
}


void process_friends(struct Message *message) {

    int client_session_id = find_pid_position(message -> sender_pid);
    if (client_session_id == -1) {

        fprintf(stderr, "FRIENDS -> failure. Invalid sender pid!\n");
        exit(1);
    }

    if (message -> message_text[0] == '\0') {

        for (int i = 0; i < MAX_CLIENT_NR; i++)
            clients_friends[client_session_id][i] = 0;

        printf("FRIENDS -> success. Friends list cleaned.\n");
        return;
    }

    for (int i = 0; i < MAX_CLIENT_NR; i++)
        clients_friends[client_session_id][i] = 0;

    char *saveptr;
    for (char *arg = strtok_r(message -> message_text, " ", &saveptr); arg != NULL; arg = strtok_r(NULL, " ", &saveptr)) {

        int friend_session_id = (int) strtol(arg, NULL, 0);

        if (friend_session_id < 0 || friend_session_id >= client_count || clients_pids[friend_session_id] == 0) {

            printf("WARNING! Invalid client id \"%d\"!\n", friend_session_id);
            continue;
        }

        if (friend_session_id == client_session_id) {

            printf("WARNING! Client can be its own friend!\n");
            continue;
        }

        clients_friends[client_session_id][friend_session_id] = 1;
    }

    printf("FRIENDS -> success. New friends list set.\n");
}


void process_add(struct Message *message) {

    int client_session_id = find_pid_position(message -> sender_pid);
    if (client_session_id == -1) {

        fprintf(stderr, "ADD -> failure. Invalid sender pid!\n");
        exit(1);
    }

    char *saveptr;
    for (char *arg = strtok_r(message -> message_text, " ", &saveptr); arg != NULL; arg = strtok_r(NULL, " ", &saveptr)) {

        int friend_session_id = (int) strtol(arg, NULL, 0);

        if (friend_session_id < 0 || friend_session_id >= client_count || clients_pids[friend_session_id] == 0) {

            printf("WARNING! Invalid client id \"%d\"!\n", friend_session_id);
            continue;
        }

        if (friend_session_id == client_session_id) {

            printf("WARNING! Client can be its own friend!\n");
            continue;
        }

        clients_friends[client_session_id][friend_session_id] = 1;
    }

    printf("ADD -> success. Friends added to list.\n");
}


void process_del(struct Message *message) {

    int client_session_id = find_pid_position(message -> sender_pid);
    if (client_session_id == -1) {

        fprintf(stderr, "DEL -> failure. Invalid sender pid!\n");
        exit(1);
    }

    char *saveptr;
    for (char *arg = strtok_r(message -> message_text, " ", &saveptr); arg != NULL; arg = strtok_r(NULL, " ", &saveptr)) {

        int friend_session_id = (int) strtol(arg, NULL, 0);

        if (friend_session_id < 0 || friend_session_id >= client_count || clients_pids[friend_session_id] == 0) {

            printf("WARNING! Invalid client id \"%d\"!\n", friend_session_id);
            continue;
        }

        clients_friends[client_session_id][friend_session_id] = 0;
    }

    printf("DEL -> success. Friends deleted from list.\n");
}


void process_toall(struct Message *message) {

    int client_session_id = find_pid_position(message -> sender_pid);
    if (client_session_id == -1) {

        fprintf(stderr, "TOALL -> failure. Invalid sender pid!\n");
        exit(1);
    }

    append_id_to_message(message, client_session_id);
    append_date_to_message(message);
    message -> sender_pid = getpid();

    for (int i = 0; i < client_count; i++) {

        if (clients_pids[i] != 0 && i != client_session_id) {

            if (msgsnd(clients_queue_descriptors[i], message, MESSAGE_SIZE, 0) == -1) {

                fprintf(stderr, "TOALL -> failure. Sending message to client \"%d\" failed!\n", i);
                exit(1);
            }

        } else if (clients_pids[i] == 0) {

            printf("WARNING! Invalid client id!\n");
        }
    }

    printf("TOALL -> success.\n");
}


void process_tofriends(struct Message *message) {

    int client_session_id = find_pid_position(message -> sender_pid);
    if (client_session_id == -1) {

        fprintf(stderr, "TOFRIENDS -> failure. Invalid sender pid!\n");
        exit(1);
    }

    append_id_to_message(message, client_session_id);
    append_date_to_message(message);
    message -> sender_pid = getpid();

    for (int i = 0; i < client_count; i++) {

        if (clients_pids[i] != 0 && i != client_session_id && clients_friends[client_session_id][i]) {

            if (msgsnd(clients_queue_descriptors[i], message, MESSAGE_SIZE, 0) == -1) {

                fprintf(stderr, "TOFRIENDS -> failure. Sending message to friend \"%d\" failed!\n", i);
                exit(1);
            }

        }
    }

    printf("TOFRIENDS -> success.\n");
}


void process_toone(struct Message *message) {

    int client_session_id = find_pid_position(message -> sender_pid);
    if (client_session_id == -1) {

        fprintf(stderr, "TOONE -> failure. Invalid sender pid!\n");
        exit(1);
    }

    char *saveptr;
    char *arg = strtok_r(message -> message_text, " ", &saveptr);
    int receiver_id = (int) strtol(arg, NULL, 0);
    sprintf(message -> message_text, "%s", saveptr);
    append_id_to_message(message, client_session_id);
    append_date_to_message(message);
    message -> sender_pid = getpid();

    if (clients_pids[receiver_id] != 0) {

        if (msgsnd(clients_queue_descriptors[receiver_id], message, MESSAGE_SIZE, 0) == -1) {

            fprintf(stderr, "TOONE -> failure. Sending message to client \"%d\" failed!\n", receiver_id);
            exit(1);
        }

    } else {

        printf("TOONE -> failure! Invalid client id: \"%d\"!\n", receiver_id);
    }

    printf("TOONE -> success.\n");
}


void process_command(struct Message *message) {

    if (message == NULL)
        return;

    switch (message -> mtype) {

        case INIT:
            process_init(message);
            break;

        case ECHO:
            process_echo(message);
            break;

        case LIST:
            process_list(message);
            break;

        case FRIENDS:
            process_friends(message);
            break;

        case ADD:
            process_add(message);
            break;

        case DEL:
            process_del(message);
            break;

        case TOALL:
            process_toall(message);
            break;

        case TOFRIENDS:
            process_tofriends(message);
            break;

        case TOONE:
            process_toone(message);
            break;

        case STOP:
            process_stop(message);
            break;

        default:
            printf("Incorrect command. Available commands: ECHO, LIST, FRIENDS, ADD, DEL, TOALL, TOFRIENDS, TOONE, STOP, READ.\n");
            break;
    }
}


int main() {

    if (atexit(close_queue) == -1) {

        fprintf(stderr, "Setting up atexit failed!\n");
        exit(1);
    }

    if (signal(SIGINT, sigint_handler) == SIG_ERR) {

        fprintf(stderr, "Setting up SIGINT handler failed!\n");
        exit(1);
    }

    char* path = getenv("HOME");
    if (path == NULL) {

        fprintf(stderr, "Obtaining path failed!\n");
        exit(1);
    }

    create_server_queue(path, PROJECT_ID);

    struct msqid_ds server_queue_specs;
    Message message;

    while (1) {

        if (loop_on == 0) {

            if (msgctl(server_queue_descriptor, IPC_STAT, &server_queue_specs) == -1) {

                fprintf(stderr, "Getting server's queue specifications failed!\n");
                exit(1);
            }

            if (server_queue_specs.msg_qnum == 0)
                break;
        }

        if (msgrcv(server_queue_descriptor, &message, MESSAGE_SIZE, 0, 0) == -1) {

            fprintf(stderr, "Receiving message from server's queue failed!\n");
            exit(1);
        }

        process_command(&message);

    }

    return 0;
}