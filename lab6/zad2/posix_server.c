#include <time.h>
#include "posix_messages.h"
#include "posix_project_functions.h"


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

    for (int i = 0; i < client_count; i++)
        if (clients_pids[i] != 0 && mq_close(clients_queue_descriptors[i]) == -1)
            fprintf(stderr, "Closing client's nr. %d queue failed!\n", i);

    if (server_queue_descriptor > -1) {

        if (mq_close(server_queue_descriptor) == -1)
            fprintf(stderr, "Closing server's queue failed!\n");

        if (mq_unlink(server_path) == -1)
            fprintf(stderr, "Deleting server's queue failed!\n");

        else
            printf("Server's queue deleted.\n");
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

    int client_pid = message -> sender_pid;
    char client_path[20];
    sprintf(client_path, "/%d", client_pid);

    int client_queue_descriptor = mq_open(client_path, O_WRONLY);
    if (client_queue_descriptor == -1) {

        fprintf(stderr, "Opening client's queue failed!\n");
        exit(1);
    }

    int close_queue = 0;

    if (client_count >= MAX_CLIENT_NR) {

        fprintf(stderr, "INIT -> failure. MAX_CLIENT_NR exceeded!\n");
        sprintf(message -> message_text, "%d", -1);
        close_queue = 1;

    } else {

        clients_pids[client_count] = message -> sender_pid;
        clients_queue_descriptors[client_count] = client_queue_descriptor;
        sprintf(message -> message_text, "%d", client_count);
    }

    message -> sender_pid = getpid();

    if (mq_send(client_queue_descriptor, (const char *) message, MESSAGE_SIZE, 1) == -1) {

        fprintf(stderr, "INIT -> failure. Sending message failed!\n");
        exit(1);
    }

    if (close_queue) {

        if (mq_close(client_queue_descriptor) == -1) {

            fprintf(stderr, "Deleting client's queue failed!\n");
            exit(1);
        }

    } else {

        memset(clients_friends[client_count], 0, sizeof(clients_friends[client_count]));

        printf("INIT -> success. Client session nr. %d started.\n", client_count);
        client_count++;
    }
}


void process_stop(struct Message *message) {

    int client_session_id = find_pid_position(message->sender_pid);
    if (client_session_id == -1) {

        fprintf(stderr, "STOP -> failure. Invalid sender pid.");
        exit(1);
    }

    clients_pids[client_session_id] = 0;

    if (mq_close(clients_queue_descriptors[client_session_id]) == -1) {

        fprintf(stderr, "Closing client's nr. %d queue failed!\n", client_session_id);
        exit(1);
    }

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

    if (mq_send(clients_queue_descriptors[client_session_id], (const char *) message, MESSAGE_SIZE, 1) == -1) {

        fprintf(stderr, "INIT -> failure. Sending message failed!\n");
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

    if (mq_send(clients_queue_descriptors[client_session_id], (const char *) message, MESSAGE_SIZE, 1) == -1) {

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

            if (mq_send(clients_queue_descriptors[i], (const char *) message, MESSAGE_SIZE, 1) == -1) {

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

            if (mq_send(clients_queue_descriptors[i], (const char *) message, MESSAGE_SIZE, 1) == -1) {

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

        if (mq_send(clients_queue_descriptors[receiver_id], (const char *) message, MESSAGE_SIZE, 1) == -1) {

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

    struct mq_attr server_queue_attr;
    server_queue_attr . mq_msgsize = MESSAGE_SIZE;
    server_queue_attr . mq_maxmsg = MAX_MESSAGES;

    server_queue_descriptor = mq_open(server_path, O_CREAT | O_EXCL | O_RDONLY, 0666, &server_queue_attr);
    if (server_queue_descriptor == -1) {

        fprintf(stderr, "Creating server's queue failed!\n");
        exit(1);
    }

    struct mq_attr current_server_attr;
    Message message;

    while (1) {

        if (loop_on == 0) {

            if (mq_getattr(server_queue_descriptor, &current_server_attr) == -1) {

                fprintf(stderr, "Getting server's queue specifications failed!\n");
                exit(1);
            }

            if (current_server_attr . mq_curmsgs == 0)
                break;
        }

        if (mq_receive(server_queue_descriptor, (char *) &message, MESSAGE_SIZE, NULL) == -1) {

            fprintf(stderr, "Receiving message from server's queue failed!\n");
            exit(1);
        }

        process_command(&message);
    }

    return 0;
}