#include "posix_messages.h"
#include "posix_project_functions.h"


int client_session_id = -1;
mqd_t server_queue_descriptor = -1;
mqd_t client_queue_descriptor = -1;
char client_path[20];


void process_input(struct Message *message, char *command_str);


void stop_and_close_queue() {

    if (client_session_id != -1) {

        Message message;
        message . mtype = STOP;
        message . sender_pid = getpid();

        if (mq_send(server_queue_descriptor, (const char *) &message, MESSAGE_SIZE, 1) == -1)
            fprintf(stderr, "STOP -> failure. Sending message failed!\n");
    }

    if (client_queue_descriptor > -1) {

        if (mq_close(client_queue_descriptor) == -1)
            fprintf(stderr, "Closing client's queue failed!\n");

        if (mq_unlink(client_path) == -1)
            fprintf(stderr, "Deleting client's queue failed!\n");

        else
            printf("Client's queue deleted.\n");

        if (mq_close(server_queue_descriptor) == -1)
            fprintf(stderr, "Closing server's queue failed!\n");
    }
}


void message_init() {

    Message message;
    message . mtype = INIT;
    message . sender_pid = getpid();

    if (mq_send(server_queue_descriptor, (const char *) &message, MESSAGE_SIZE, 1) == -1) {

        fprintf(stderr, "INIT -> failure. Sending message failed!\n");
        exit(1);
    }

    if (mq_receive(client_queue_descriptor, (char *) &message, MESSAGE_SIZE, NULL) == -1) {

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

    printf("INIT -> success. Client session number: \"%d\".\n", client_session_id);
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

    if (mq_send(server_queue_descriptor, (const char *) &message, MESSAGE_SIZE, 1) == -1) {

        fprintf(stderr, "ECHO -> failure. Sending message failed!\n");
        exit(1);
    }

    if (mq_receive(client_queue_descriptor, (char *) &message, MESSAGE_SIZE, NULL) == -1) {

        fprintf(stderr, "ECHO -> failure. Receiving response failed!\n");
        exit(1);
    }

    printf("ECHO -> success. Received response: %s.\n", message . message_text);
}


void message_list(Message message) {

    message . mtype = LIST;

    if (mq_send(server_queue_descriptor, (const char *) &message, MESSAGE_SIZE, 1) == -1) {

        fprintf(stderr, "LIST -> failure. Sending message failed!\n");
        exit(1);
    }

    if (mq_receive(client_queue_descriptor, (char *) &message, MESSAGE_SIZE, NULL) == -1) {

        fprintf(stderr, "LIST -> failure. Receiving response failed!\n");
        exit(1);
    }

    printf("LIST -> success. Received response:%s.\n", message . message_text);
}


void message_friends(Message message, char *command_str) {

    message . mtype = FRIENDS;
    separate_command_argument(&message, command_str);

    if (mq_send(server_queue_descriptor, (const char *) &message, MESSAGE_SIZE, 1) == -1) {

        fprintf(stderr, "FRIENDS -> failure. Sending message failed!\n");
        exit(1);
    }

    printf("FRIENDS -> success.\n");
}


void message_add(Message message, char *command_str) {

    message . mtype = ADD;
    separate_command_argument(&message, command_str);

    if (mq_send(server_queue_descriptor, (const char *) &message, MESSAGE_SIZE, 1) == -1) {

        fprintf(stderr, "ADD -> failure. Sending message failed!\n");
        exit(1);
    }

    printf("ADD -> success.\n");
}


void message_del(Message message, char *command_str) {

    message . mtype = DEL;
    separate_command_argument(&message, command_str);

    if (mq_send(server_queue_descriptor, (const char *) &message, MESSAGE_SIZE, 1) == -1) {

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

    if (mq_send(server_queue_descriptor, (const char *) &message, MESSAGE_SIZE, 1) == -1) {

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

    if (mq_send(server_queue_descriptor, (const char *) &message, MESSAGE_SIZE, 1) == -1) {

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

    if (mq_send(server_queue_descriptor, (const char *) &message, MESSAGE_SIZE, 1) == -1) {

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


int main() {

    if (atexit(stop_and_close_queue) == -1) {

        fprintf(stderr, "Setting up atexit failed!\n");
        exit(1);
    }

    if (signal(SIGINT, sigint_handler) == SIG_ERR) {

        fprintf(stderr, "Setting up SIGINT handler failed!\n");
        exit(1);
    }

    server_queue_descriptor = mq_open(server_path, O_WRONLY);
    if (server_queue_descriptor == -1) {

        fprintf(stderr, "Opening server's queue failed!\n");
        exit(1);
    }

    sprintf(client_path, "/%d", getpid());

    struct mq_attr client_queue_attr;
    client_queue_attr . mq_msgsize = MESSAGE_SIZE;
    client_queue_attr . mq_maxmsg = MAX_MESSAGES;

    client_queue_descriptor = mq_open(client_path, O_CREAT | O_EXCL | O_RDONLY, 0666, &client_queue_attr);
    if (client_queue_descriptor == -1) {

        fprintf(stderr, "Creating client's queue failed!\n");
        exit(1);
    }

    message_init();

    Message message;
    struct mq_attr current_queue_attr;
    char command[64];

    while (1) {

        printf("Enter command: ");
        if (fgets(command, 64, stdin) == NULL) {

            printf("Reading command failed!\n");
            continue;
        }

        mq_getattr(client_queue_descriptor, &current_queue_attr);

        printf("%d\n", (int) current_queue_attr . mq_curmsgs);
        for (; current_queue_attr . mq_curmsgs > 0; mq_getattr(client_queue_descriptor, &current_queue_attr)) {

            if (mq_receive(client_queue_descriptor, (char *) &message, MESSAGE_SIZE, NULL) != -1) {

                process_message(&message);

            } else {

                fprintf(stderr, "Receiving message failed!\n");
                exit(1);
            }
        }

        message . sender_pid = getpid();
        process_input(&message, command);
    }

    return 0;
}