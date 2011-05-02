#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "bank_data.h"
#include "cliser.h"

/* Eliminate duplicated code. */
static int read_one_response(message_t *rec_ptr);

/* the database_initialize initialize the client-side of the pipes. */
int database_initialize(void)
{
    if (!client_starting())
        return 0;
    else
        return 1;
}

void database_close(void)
{
    client_ending();
}

int ask_create_account(const person_infor user)
{
    int res;
    message_t mess_send;
    message_t mess_ret;

    mess_send.client_pid = getpid();
    mess_send.request = s_create_new_account;
    
    /* set mess_send information */
    memcpy(&mess_send.user_infor, &user, sizeof(user));
    
    res = 0;
    if (send_mess_to_server(mess_send)) {
        if (read_one_response(&mess_ret)) {
            if (mess_ret.response == r_success) {
                res = 1;
                printf("Your card number:%d\nPassword:%s\n", mess_ret.data.account_id,
                        mess_ret.data.passwd);
                printf("Please change your password as soon as posible!\n");
            } else {
                fprintf(stderr, "%s", mess_ret.error_text);
            }
        } else {
            fprintf(stderr, "Server failed to respond!\n");
        }
    } else {
        fprintf(stderr, "Server not accepting requests!\n");
    }

    return res;
}

/* The function read_one_response is used to avoid duplicating code. */
static int read_one_response(message_t *rec_ptr)
{
    int res = 0;
    
    if (!rec_ptr) 
        return 0;

    if (start_resp_from_server()) {
        if (read_resp_from_server(rec_ptr)) {
            res = 1;
        }
        end_resp_from_server();
    }

    return res;
}

int ask_reset_passwd(const person_infor user, const account_data data)
{
    int res = 0;
    message_t mess_send;
    message_t mess_ret;

    mess_send.client_pid = getpid();
    mess_send.request = s_reset_passwd;

    /* set mess_send */
    memcpy(&mess_send.user_infor, &user, sizeof(user));
    memcpy(&mess_send.data, &data, sizeof(data));
    
    if (send_mess_to_server(mess_send)) {
        if (read_one_response(&mess_ret)) {
            if (mess_ret.response == r_success) {
                res = 1;
            } else {
                fprintf(stderr, "%s\n", mess_ret.error_text);
            }
        } else {
            fprintf(stderr, "Server failed to respond!\n");
        }
    } else {
        fprintf(stderr, "Server not accepting requests!\n");
    }

    return res;
}

int ask_report_loss(const person_infor user, const account_data data)
{
    int res = 0;
    message_t mess_send;
    message_t mess_ret;

    mess_send.client_pid = getpid();
    mess_send.request = s_report_loss;
    memcpy(&mess_send, &user, sizeof(user));
    memcpy(&mess_send, &data, sizeof(data));

    if (send_mess_to_server(mess_send)) {
        if (read_one_response(&mess_ret)) {
            if (mess_ret.response == r_success) {
                res = 1;
            } else {
                fprintf(stderr, "%s\n", mess_ret.error_text);
            }
        } else {
            frpintf(stderr, "Server failed to response!\n");
        }
    } else {
        fprintf(stderr, "Server not accepting requests!\n");
    }

    return res;
}

int ask_login(const account_data data)
{
    int res = 0;
    message_t mess_send;
    message_t mess_ret;

    mess_send.client_pid = getpid();
    mess_send.request = s_login;
    memcpy(&mess_send.data, &data, sizeof(data));
    
    if (send_mess_to_server(mess_send)) {
        if (read_one_response(&mess_ret)) {
            if (mess_ret.response = r_success) {
                res = 1;
            } else {
                fprintf(stderr, "%s\n", mess_ret.error_text);
            }
        } else {
            fprintf(stderr, "Server failed to response!\n");
        }
    } else {
        fprintf(stderr, "Server not accepting request!\n");
    }

    return res;
}

int ask_query(const account_data data)
{
    int money = -1;
    message_t mess_send;
    message_t mess_ret;

    mess_send.client_pid = getpid();
    mess_send.request = s_query;
    memcpy(&mess_send.data, &data, sizeof(data));

    if (send_mess_to_server(mess_send)) {
        if (read_one_response(&mess_ret)) {
            if (mess_ret.response == r_success) {
                money = mess_ret.data.money;
            } else {
                fprinf(stderr, "%s\n", mess_ret.error_text);
            }
        } else {
            fprintf(stderr, "Server failed to response!\n");
        }
    } else {
        fprintf(stderr, "Server not accepting request!\n");
    }

    return money;
}

int ask_draw_money(const account_data data)
{
    int res = 0;
    message_t mess_send;
    message_t mess_ret;

    mess_send.client_pid = getpid();
    mess_send.request = s_draw_money;
    memcpy(&mess_send.data, &data, sizeof(data));
    
    if (send_mess_to_server(mess_send)) {
        if (read_one_response(&mess_ret)) {
            if (mess_ret.response == r_success) {
                res = 1;
            } else {
                fprintf(stderr, "%s\n", mess_ret.error_text);
            }
        } else {
            fprintf(stderr, "Server failed to response!\n");
        }
    } else {
        fprintf(stderr, "Server not accepting request!\n");
    }

    return res;
}

int ask_store_money(const account_data data)
{
    int res = 0;
    message_t mess_send;
    message_t mess_ret;

    mess_send.client_pid = getpid();
    mess_send.request = s_store_money;
    memcpy(&mess_send.data, &data, sizeof(data));

    if (send_mess_to_server(mess_send)) {
        if (read_one_response(&mess_ret)) {
            if (mess_ret.response == r_success) {
                res = 1;
            } else {
                fprintf(stderr, "%s\n", mess_ret.error_text);
            }
        } else {
            fprintf(stderr, "Server failed to response!\n");
        }
    } else {
        fprintf(stderr, "Server not accepting request!\n");
    }

    return res;
}

int ask_renewal(const account_data data)
{
    int res;
    message_t mess_send;
    message_t mess_ret;

    mess_send.client_pid = getpid();
    mess_send.request = s_renewal;
    memcpy(&mess_send.data, &data, sizeof(data));
 
    if (send_mess_to_server(mess_send)) {
        if (read_one_response(&mess_ret)) {
            if (mess_ret.response == r_success) {
                res = 1;
            } else {
                fprintf(stderr, "%s\n", mess_ret.error_text);
            }
        } else {
            fprintf(stderr, "Server failed to response!\n");
        }
    } else {
        fprintf(stderr, "Server not accepting request!\n");
    }

    return res;
}

int ask_change_passwd(const account_data data)
{
    int res;
    message_t mess_send;
    message_t mess_ret;

    mess_send.client_pid = getpid();
    mess_send.request = s_change_passwd;
    memcpy(&mess_send.data, &data, sizeof(data));
 
    if (send_mess_to_server(mess_send)) {
        if (read_one_response(&mess_ret)) {
            if (mess_ret.response == r_success) {
                res = 1;
            } else {
                fprintf(stderr, "%s\n", mess_ret.error_text);
            }
        } else {
            fprintf(stderr, "Server failed to response!\n");
        }
    } else {
        fprintf(stderr, "Server not accepting request!\n");
    }

    return res;
}

int ask_logout(const char *account_id)
{
    int res;
    message_t mess_send;
    message_t mess_ret;

    mess_send.client_pid = getpid();
    mess_send.request = s_logout;
    strcpy(mess_send.data.account_id, account_id);

    if (send_mess_to_server(mess_send)) {
        if (read_one_response(&mess_ret)) {
            if (mess_ret.response == r_success) {
                res = 1;
            } else {
                fprintf(stderr, "%s\n", mess_ret.error_text);
            }
        } else {
            fprintf(stderr, "Server failed to response!\n");
        }
    } else {
        fprintf(stderr, "Server not accepting request!\n");
    }

    return res;
}
