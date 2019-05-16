#include <unistd.h>
#include "systemv_messages.h"
#include "systemv_project_functions.h"


int client_session_id = -1;
int server_queue_descriptor = -1;
int client_queue_descriptor = -1;


void process_input(struct Message *message, char *command_str);


void stop_and_close_queue() {

    if (client_session_id != -1) {

        Message message;
        message . mtype = STOP;
        message . sender_pid = getpid();

        if (msgsnd(server_queue_descriptor, &message, MESSAGE_SIZE, 0) == -1)
            fprintf(stderr, "STOP -> failure. Sending message failed!\n");
    }

    if (client_queue_descriptor > -1) {

        if (msgctl(client_queue_descriptor, IPC_RMID, NULL) == -1)
            fprintf(stderr, "Deleting client's queue failed!\n");

        else
            printf("Client's queue deleted.\n");
    }
}


void message_init(key_t client_queue_key) {

    Message message;
    message . mtype = INIT;
    message . sender_pid = getpid();
    sprintf(message . message_text, "%d", client_queue_key);

    if (msgsnd(server_queue_descriptor, &message, MESSAGE_SIZE, 0) == -1) {

        fprintf(stderr, "INIT -> failure. Sending message failed!\n");
        exit(1);
    }

    if (msgrcv(client_queue_descriptor, &message, MESSAGE_SIZE, INIT, 0) == -1) {

        fprintf(stderr, "INIT -> failure. Receiving response failed!\n");
        exit(1);
    }

    if (sscanf(message.message_text, "%d", &client_session_id) <= 0) {

        fprintf(stderr, "INIT -> failure. Bad response!\n");
        exit(1);
    }

    if (client_session_id < 0) {

        fprintf(stderr, "INIT -> failure. MAX_CLIENT_NR exceeded!\n");
        exit(1);
    }

    printf("INIT -> success. Client sessions number: %d.\n", client_session_id);
}


int separate_command_argument(Message *message, char *command_str) {

    char *absolute_whitespace_position = strchr(command_str, ' ');
    if (absolute_whitespace_position == NULL) {

        message -> message_text[0] = '\0';
        return -1;
    }

    int whitespace = (int) (absolute_whitespace_position - command_str);
    int length = (int) strlen(command_str);

    memcpy(message -> message_text, &command_str[whitespace + 1], (size_t) (length - whitespace - 1));
    message -> message_text[length - whitespace - 2] = '\0';
    return 0;
}


void message_echo(Message message, char *command_str) {

    message . mtype = ECHO;
    int argument_status = separate_command_argument(&message, command_str);
    if (argument_status == -1) {

        fprintf(stderr, "ECHO -> failure. Missing argument.\n");
        return;
    }

    if (msgsnd(server_queue_descriptor, &message, MESSAGE_SIZE, 0) == -1) {

        fprintf(stderr, "ECHO -> failure. Sending message failed!\n");
        exit(1);
    }

    if (msgrcv(client_queue_descriptor, &message, MESSAGE_SIZE, ECHO, 0) == -1) {

        fprintf(stderr, "ECHO -> failure. Receiving response failed!\n");
        exit(1);
    }

    printf("ECHO -> success. Received response: %s.\n", message . message_text);
}


void message_list(Message message) {

    message . mtype = LIST;

    if (msgsnd(server_queue_descriptor, &message, MESSAGE_SIZE, 0) == -1) {

        fprintf(stderr, "LIST -> failure. Sending message failed!\n");
        exit(1);
    }

    if (msgrcv(client_queue_descriptor, &message, MESSAGE_SIZE, LIST, 0) == -1) {

        fprintf(stderr, "LIST -> failure. Receiving response failed!\n");
        exit(1);
    }

    printf("LIST -> success. Received response:%s.\n", message . message_text);
}


void message_friends(Message message, char *command_str) {

    message . mtype = FRIENDS;
    separate_command_argument(&message, command_str);

    if (msgsnd(server_queue_descriptor, &message, MESSAGE_SIZE, 0) == -1) {

        fprintf(stderr, "FRIENDS -> failure. Sending message failed!\n");
        exit(1);
    }

    printf("FRIENDS -> success.\n");
}


void message_add(Message message, char *command_str) {

    message . mtype = ADD;
    separate_command_argument(&message, command_str);

    if (msgsnd(server_queue_descriptor, &message, MESSAGE_SIZE, 0) == -1) {

        fprintf(stderr, "ADD -> failure. Sending message failed!\n");
        exit(1);
    }

    printf("ADD -> success.\n");
}


void message_del(Message message, char *command_str) {

    message . mtype = DEL;
    separate_command_argument(&message, command_str);

    if (msgsnd(server_queue_descriptor, &message, MESSAGE_SIZE, 0) == -1) {

        fprintf(stderr, "DEL -> failure. Sending message failed!\n");
        exit(1);
    }

    printf("DEL -> success.\n");
}


void message_toall(Message message, char *command_str) {

    message . mtype = TOALL;
    int argument_status = separate_command_argument(&message, command_str);
    if (argument_status == -1) {

        fprintf(stderr, "TOALL -> failure. Missing argument.\n");
        return;
    }

    if (msgsnd(server_queue_descriptor, &message, MESSAGE_SIZE, 0) == -1) {

        fprintf(stderr, "TOALL -> failure. Sending message failed!\n");
        exit(1);
    }

    printf("TOALL -> success.\n");
}


void message_tofriends(Message message, char *command_str) {

    message . mtype = TOFRIENDS;
    int argument_status = separate_command_argument(&message, command_str);
    if (argument_status == -1) {

        fprintf(stderr, "TOFRIENDS -> failure. Missing argument.\n");
        return;
    }

    if (msgsnd(server_queue_descriptor, &message, MESSAGE_SIZE, 0) == -1) {

        fprintf(stderr, "TOFRIENDS -> failure. Sending message failed!\n");
        exit(1);
    }

    printf("TOFRIENDS -> success.\n");
}


void message_toone(Message message, char *command_str) {

    message . mtype = TOONE;
    int argument_status = separate_command_argument(&message, command_str);
    if (argument_status == -1) {

        fprintf(stderr, "TOONE -> failure. Missing argument.\n");
        return;
    }

    if (msgsnd(server_queue_descriptor, &message, MESSAGE_SIZE, 0) == -1) {

        fprintf(stderr, "TOONE -> failure. Sending message failed!\n");
        exit(1);
    }

    printf("TOONE -> success.\n");
}


void message_read(Message message, char *command_str) {

    int argument_status = separate_command_argument(&message, command_str);
    if (argument_status == -1) {

        fprintf(stderr, "READ -> failure. Missing argument.\n");
        return;
    }

    char *path = message .message_text;

    FILE * file;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    file = fopen(path, "r");
    if (file == NULL) {

        fprintf(stderr, "READ -> failure. Couldn't open given file.\n");
        return;
    }

    while ((read = getline(&line, &len, file)) != -1) {

        message . sender_pid = getpid();
        process_input(&message, line);
    }

    fclose(file);
    if (line)
        free(line);

    printf("READ -> success.\n");
}


void process_input(struct Message *message, char *command_str) {

    mtype command = get_mtype_for_str(command_str);

    switch (command) {

        case ECHO:
            message_echo(*message, command_str);
            break;

        case LIST:
            message_list(*message);
            break;

        case FRIENDS:
            message_friends(*message, command_str);
            break;

        case ADD:
            message_add(*message, command_str);
            break;

        case DEL:
            message_del(*message, command_str);
            break;

        case TOALL:
            message_toall(*message, command_str);
            break;

        case TOFRIENDS:
            message_tofriends(*message, command_str);
            break;

        case TOONE:
            message_toone(*message, command_str);
            break;

        case STOP:
            exit(0);

        case READ:
            message_read(*message, command_str);
            break;

        case PASS:
            break;

        default:
            printf("Incorrect command. Available commands: ECHO, LIST, FRIENDS, ADD, DEL, TOALL, TOFRIENDS, TOONE, STOP, READ.\n");
            break;
    }
}


void process_message(struct Message *message) {

    switch (message -> mtype) {

        case TOALL:
            printf("TOALL -> received. Message text: \"%s\"\n", message -> message_text);
            break;

        case TOFRIENDS:
            printf("TOFRIENDS -> received. Message text: \"%s\"\n", message -> message_text);
            break;

        case TOONE:
            printf("TOONE -> received. Message text: \"%s\"\n", message -> message_text);
            break;

        default:
            printf("Incorrect command. Available commands: ECHO, LIST, FRIENDS, ADD, DEL, TOALL, TOFRIENDS, TOONE, STOP, READ.\n");
            break;
    }
}


int open_server_queue(char *path, int project_id) {

    int key = ftok(path, project_id);
    if (key == -1) {

        fprintf(stderr, "Queue key generating failed!\n");
        exit(1);
    }

    int queue_id = msgget(key, 0);
    if (queue_id == -1)  {

        fprintf(stderr, "Opening queue failed!\n");
        exit(1);
    }

    return queue_id;
}


int main() {

    if (atexit(stop_and_close_queue) == -1) {

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

    server_queue_descriptor = open_server_queue(path, PROJECT_ID);

    key_t client_queue_key = ftok(path, getpid());
    if (client_queue_key == -1) {

        fprintf(stderr, "Client's key generating failed!\n");
        exit(1);
    }

    client_queue_descriptor = msgget(client_queue_key, IPC_CREAT | IPC_EXCL | 0666);
    if (client_queue_descriptor == -1) {

        fprintf(stderr, "Creating client's queue failed!\n");
    }

    message_init(client_queue_key);

    Message message;
    char command[64];

    while (1) {

        printf("Enter command: ");
        if (fgets(command, 64, stdin) == NULL) {

            printf("Reading command failed!\n");
            continue;
        }

        while (msgrcv(client_queue_descriptor, &message, MESSAGE_SIZE, 0, IPC_NOWAIT) != -1)
            process_message(&message);

        message . sender_pid = getpid();
        process_input(&message, command);
    }

    return 0;
}