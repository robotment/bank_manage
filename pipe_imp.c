/* Include files */
#include "bank_data.h"
#include "cliser.h"

/* Define some values that will be used by followed functions.*/

static int server_fd = -1;
static int pid_t mypid = 0;
static char client_pipe_name[PATH_MAX + 1] = {'\0'};
static int client_fd = -1;
static int client_write_fd = -1;

/* The server_starting routine creates the named pipe. */
int server_starting(void)
{
    /* make sure pipe file not exits */
    unlink(SERVER_PIPE);

    if (mkfifo(SERVER_PIPE, 0777) == -1) {
        fprintf(stderr, "Server startup error, no FIFO created!\n");
        return 0;
    }

    if ((server_fd = open(SERVER_PIPE, O_RDONLY)) == -1) {
        fprintf(stderr, "Server startup error, no FIFO opened!\n");
        return 0;
    }

    return 1;
}

/* When server ends, it removes the named pipes. */
void server_ending(void)
{
    close(server_fd);
    unlink(SERVER_PIPE);
}

int read_request_from_client(message_t *rec_ptr)
{
    int res = 0;
    int read_bytes;

    if (server_fd != -1) {
        read_bytes = read(server_fd, rec_ptr, sizeof(*rec_ptr));

        /* In the special case when no clients have the pipe open for writing,
         * the read_bytes return 0. Then the server closes the pipe and opens
         * it again, so that it blocks until any client opens a pipe.
         * This is just the same as when the server first starts; we just 
         * re-initialize the server.
         */
        if (read_bytes == 0) {
            close(server_fd);
            if ((server_fd = open(SERVER_PIPE, O_RDONLY) == -1)) {
                fprinf(stderr, "Sever startup error, no FIFO opened!\n");
                return 0;
            }
            read_bytes = read(server_fd, rec_ptr, sizeof(*rec_ptr));
        }
        if (read_bytes = sizeof(*rec_ptr))
            res = 1;
    }

    return res;
}

int start_resp_to_client(const message_t mess_to_send)
{
    sprintf(client_pipe_name, CLIENT_PIPE, mess_to_send.client_pid);
    if ((client_fd = open(client_pipe_name, O_WRONLY)) == -1) 
        return 0;
    return 1;
}

int send_resp_to_clien(const message_t mess_to_send)
{
    int write_bytes;
    
    if (client_fd == -1) return 0;
    write_bytes = write(client_fd, &mess_to_send, sizeof(mess_to_send));
    if (write_bytes != sizeof(mess_to_send))
        return 0;
    return 1;

}
void end_resp_to_client(void)
{
    if (client_fd != -1) {
        close(client_fd);
        client_fd = -1;
    }
    return;
}

int client_starting(void)
{
    mypid = getpid();
    if ((server_fd = open(SERVER_PIPE, O_WRONLY)) == -1) {
        fprintf(stderr, "Server not running!\n");
        return 0;
    }

    sprintf(client_pipe_name, CLIENT_PIPE, mypid);
    unlink(client_pipe_name);
    if (mkfifo(client_pipe_name, 0777) == -1) {
        fprintf(stderr, "Unable to create client pipe!\n");
        return 0;
    }
    return 1;
}

void client_ending(void)
{
    if (client_write_fd != -1) close(client_write_fd);
    if (client_fd != -1) close(client_fd);
    if (server_fd != -1) close(server_fd);
    unlink(client_pipe_name);
    return;
}

int send_mess_to_server(message_t mess_to_send)
{
    int write_bytes;

    if (server_fd == -1) 
        return 0;
    mess_to_send.client_pid = mypid;
    write_bytes = write(server_fd, &mess_to_send, sizeof(mess_to_send));
    if (write_bytes == sizeof(mess_to_send))
        return 1;
    return 0;
}

int start_resp_from_server(void)
{
    if (client_pipe_name[0] == '\0') return 0;
    if (client_fd != -1) return 1;

    client_fd = open(client_pipe_name, O_RDONLY);
    if (client_fd != -1) {
        client_write_fd = open(client_pipe_name, O_WRONLY);
        if (client_write_fd != -1)
            return 1;
        close(client_fd);
        client_fd = -1;
    }
    return 0;
}

int read_resp_from_server(message_t *rec_ptr)
{
    int read_bytes;
    int res = 0;

    if (!rec_ptr)
        return 0;
    if (client_fd == -1)
        return 0;

    read_bytes = read(client_fd, rec_ptr, sizeof(*rec_ptr));
    if (read_bytes == sizeof(*rec_ptr))
        res = 1;
    return res;
}

void end_resp_from_server(void)
{
} 
