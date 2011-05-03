#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <limits.h>
#include <signal.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>

#include "bank_data.h"
#include "cliser.h"

int save_errno;
static int server_running = 1;

static void process_command(const message_t mess_command);

void catch_signals()
{
    server_running = 0;
}

int main()
{
    struct sigaction new_action, old_action;
    message_t mess_command;
    
    new_action.sa_handler = catch_signals;
    sigemptyset(&new_action.sa_mask);
    new_action.sa_flags = 0;
    if ((sigaction(SIGINT, &new_action, &old_action) != 0) ||
        (sigaction(SIGHUP, &new_action, &old_action) != 0) ||
        (sigaction(SIGTERM, &new_action, &old_action) != 0)) {
        fprintf(stderr, "Sever starup error, signal catching failed.\n");
        exit(EXIT_FAILURE);
    }

    if (!database_initialize()) {
        fprintf(stderr, "Server error:-\
                could not initialize database\n");
        exit(EXIT_FAILURE);
    }

    if (!server_starting()) exit(EXIT_FAILURE);

    while (server_running) {
        if (read_request_from_client(&mess_command)) {
            process_command(mess_command);
        } else {
            if (server_running) 
                fprintf(stderr, "Server ended - can not\
                        read pipe\n");
            server_running = 0;
        }
    }

    server_ending();
    database_close();
    exit(EXIT_SUCCESS);
}

static void process_command(const message_t comm)
{
    message_t resp;

    resp = comm; /* copy command back, then change as needed. */

    if (!start_resp_to_client(resp)) {
        fprintf(stderr, "Server Warning:-\
                star_resp_to_client %d failed\n", resp.client_pid);
        return;
    }
    
    resp.response = r_success;
    memset(resp.error_text, '\0', sizeof(resp.error_text));
    save_errno = 0;

    switch (resp.request) {
        case s_create_new_account:
            if (!ask_new_account(comm.user_infor, &resp.data))
                resp.response = r_failure;
            break;
        case s_reset_passwd:
            if (!ask_reset_passwd(comm.user_infor, comm.data))
                resp.response = r_failure;
            break;
        case s_report_loss:
            if (!ask_report_loss(comm.user_infor, comm.data))
                resp.response = r_failure;
            break;
        case s_login:
            if (!ask_login(comm.data))
                resp.response = r_failure;
            break;
        case s_query:
            if ((resp.data.money = ask_query(comm.data)) < 0)
                resp.response = r_failure;
            break;
        case s_draw_money:
            if (!ask_draw_money(comm.data))
                resp.response = r_failure;
            break;
        case s_store_money:
            if (!ask_store_money(comm.data))
                resp.response = r_failure;
            break;
        case s_renewal:
            if (!ask_renewal(comm.data))
                resp.response = r_failure;
            break;
        case s_change_passwd:
            if (!ask_change_passwd(comm.data))
                resp.response = r_failure;
            break;
        case s_logout:
            if (!ask_logout(comm.data.account_id))
                resp.response = r_failure;
            break;
        default:
            resp.response = r_failure;
            break;
    } /* switch */

    if (!send_resp_to_client(resp)) {
        fprintf(stderr, "Server Warning:-\
                failed to respond to %d\n", resp.client_pid);
    }

    end_resp_to_client();
    return;
}
