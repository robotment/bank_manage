/* Requied #include header files. */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifndef CLISER_H
#define CLISER_H

/* Define the pipes which used between clients and server. */
#define SERVER_PIPE "/tmp/server_pipe"
#define CLIENT_PIPE "/tmp/client_%d_pipe"

#define STRING_SIZE 256
#define ERR_TEXT_SIZE 128

/* Define client request type and server response type. */

typedef enum _client_request_e {
    s_create_new_account = 0,
    s_login,
    s_logout,
    s_query,
    s_store_money,
    s_draw_money,
    s_renewal,
    s_change_passwd,
    s_reset_passwd,
    s_report_loss
} client_request_e;

typedef enum _server_response_e {
    r_success = 0,
    r_failure
} server_response_e;

typedef struct _message_t {
    pid_t               client_pid;
    client_request_e    request;
    server_response_e   response;
    person_infor        user_infor;
    account_data        data;
    char                error_text[ERR_TEXT_SIZE + 1];
} message_t;

int server_starting(void);
void server_ending(void);
int read_request_from_client(message_t *rec_ptr);
int start_resp_to_client(const message_t mess_to_send);
int send_resp_to_client(const message_t mess_to_send);
void end_resp_to_client(void);

int client_starting(void);
void client_ending(void);
int send_message_to_server(message_t mess_to_send);
int start_resp_from_server(void);
int send_resp_from_server(message_t *rec_ptr);
void end_resp_from_server(void);

#endif /*  cliser.h  end */
