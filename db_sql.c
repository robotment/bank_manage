#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "mysql.h"
#include "bank_data.h"

enum {
    SQL_SIZE = 300
};

static MYSQL *conn_ptr = NULL;

int database_initialize(void)
{
    conn_ptr = mysql_init(NULL);
    if (conn_ptr == NULL) {
        fprintf(stderr, "mysql_init failed\n");
        return 0;
    }

    /* connect to my database. Named by peng, and password 19891221.
     * If you want to use it, you must change it or create this one.
     */
    conn_ptr = mysql_real_connect(conn_ptr, "localhost", "peng", "19891221",
            "bank", 0, NULL, 0);
    if (conn_ptr == NULL) {
        fprintf(stderr, "Can't connect to bank database\n");
        return 0;
    }

    return 1;
}

void database_close(void)
{
    if (conn_ptr != NULL) {
        mysql_close(conn_ptr);
    }

    return;
}

/* INSERT user into database.
 * If user id exists, then update it.
 */
int ask_new_account(const person_infor user, account_data *data)
{
    int res;
    char sql[SQL_SIZE];
    MYSQL_RES *result;
    MYSQL_ROW sqlrow;

    sprintf(sql, "INSERT INTO userinfo(id, name, sex, phoneNum, address)\
            VALUES(\'%s\', \'%s\', \'%c\', \'%s\', \'%s\')",
            user.id, user.name, user.sex, user.phone, user.address);
    res = mysql_query(conn_ptr, sql);
    /* ID already exists, then update infor. */
    if (res && mysql_errno(conn_ptr) == 1062) {
        sprintf(sql, "UPDATE userinfo set name = '%s', sex = '%c',\
                phoneNum = '%s', address = '%s' WHERE id =\
                '%s'", user.name, user.sex, user.phone, user.address,
                user.id);
        res = mysql_query(conn_ptr, sql);
    }

    sprintf(sql, "INSERT INTO account(id, password, money, is_login,\
        is_locked, create_time) VALUES('%s', md5('%s'), %d, %d,\
            %d, curdate())", user.id, "000000", 0, 0, 0);
    res = mysql_query(conn_ptr, sql);

    res = mysql_query(conn_ptr, "SELECT LAST_INSERT_ID()");
    if (!res) {
        result = mysql_store_result(conn_ptr);
        sqlrow = mysql_fetch_row(result);
        strncpy(data->account_id, sqlrow[0], sizeof(data->account_id));
        strcpy(data->passwd, "000000");
        mysql_free_result(result);
    }

    return res != 0? 0: 1;
}

int ask_reset_passwd(const person_infor user, const account_data data)
{
    MYSQL_RES *result;
    MYSQL_ROW sqlrow;
    int res;
    char sql[SQL_SIZE];

    sprintf(sql, "SELECT '%s' = (SELECT id from account where num =\
        '%s') && (SELECT '%s' = (SELECT name from userinfo where id =\
                '%s'))", user.id, data.account_id, user.name,
            user.id);
    res = mysql_query(conn_ptr, sql);
    if (!res) {
        result = mysql_store_result(conn_ptr);
        sqlrow = mysql_fetch_row(result);
        if (atoi(sqlrow[0])) {
            sprintf(sql, "UPDATE account SET password = md5('000000')\
                    where num = '%s'", data.account_id);
            mysql_query(conn_ptr, sql);

            mysql_free_result(result);
            return 1;
        }
    }
    
    return 0;
}
int ask_report_loss(const person_infor user, const account_data data)
{
    MYSQL_RES *result;
    MYSQL_ROW sqlrow;
    int res;
    char sql[SQL_SIZE];

    sprintf(sql, "SELECT '%s' = (SELECT id from account where num =\
        '%s') && (SELECT '%s' = (SELECT name from userinfo where id =\
                '%s'))", user.id, data.account_id, user.name,
            user.id);
    res = mysql_query(conn_ptr, sql);
    if (!res) {
        result = mysql_store_result(conn_ptr);
        sqlrow = mysql_fetch_row(result);
        if (atoi(sqlrow[0])) {
            sprintf(sql, "UPDATE account SET is_locked = 1\
                    where num = '%s'", data.account_id);
            mysql_query(conn_ptr, sql);

            mysql_free_result(result);
            return 1;
        }
    }
    
    return 0;
}
int ask_login(const account_data data)
{
    MYSQL_RES *result = NULL;
    MYSQL_ROW sqlrow;
    int res;
    char sql[SQL_SIZE];

    res = 0;
    sprintf(sql, "SELECT password = md5('%s'), is_login = 0, is_locked = 0\
            from account where num='%s'", data.passwd, data.account_id);
    mysql_query(conn_ptr, sql);
    result = mysql_store_result(conn_ptr);
    sqlrow = mysql_fetch_row(result);
    if (sqlrow) {
        if (atoi(sqlrow[0]) && atoi(sqlrow[1]) &&
                    atoi(sqlrow[0])) {
            sprintf(sql, "UPDATE account SET is_login = 1\
                    where num='%s'", data.account_id);
            mysql_query(conn_ptr, sql);
            res = 1;
        }
    }

    mysql_free_result(result);  
    return res;
}

int ask_query(const account_data data)
{
    MYSQL_RES *result;
    MYSQL_ROW sqlrow;
    int money = -1;
    char sql[SQL_SIZE];

    sprintf(sql, "SELECT money from account where \
            num = '%s'", data.account_id);
    if (!mysql_query(conn_ptr, sql)) {
        result = mysql_store_result(conn_ptr);
        sqlrow = mysql_fetch_row(result);
        money = atoi(sqlrow[0]);
        mysql_free_result(result);
    }

    return money;
}
int ask_draw_money(const account_data data)
{
    MYSQL_RES *result;
    MYSQL_ROW sqlrow;
    char sql[SQL_SIZE];

    sprintf(sql, "SELECT money >= %d from account\
            where num = '%s'", data.money, data.account_id);
    if (!mysql_query(conn_ptr, sql)) {
        result = mysql_store_result(conn_ptr);
        sqlrow = mysql_fetch_row(result);
        if (atoi(sqlrow[0])) {
            sprintf(sql, "UPDATE account SET money = money - %d\
                    where num = '%s'", data.money, data.account_id);
            mysql_query(conn_ptr, sql);
            mysql_free_result(result);
            return 1;
        }
    } 
    return 0;
}
int ask_store_money(const account_data data)
{
    char sql[SQL_SIZE];

    sprintf(sql, "UPDATE account SET money = money + %d\
            where num = '%s'", data.money, data.account_id);
    mysql_query(conn_ptr, sql);

    return 1;
}
int ask_renewal(const account_data data)
{
    MYSQL_RES *result = NULL;
    MYSQL_ROW sqlrow;
    char sql[SQL_SIZE];
    int res;

    sprintf(sql, "SELECT money > %d FROM account\
            where num = '%s'", data.money, data.account_id);
    if (!mysql_query(conn_ptr, sql)) {
        result = mysql_store_result(conn_ptr);
        sqlrow = mysql_fetch_row(result);
        res = atoi(sqlrow[0]);
        sprintf(sql, "select count(num) FROM account\
                where num = '%s'", data.renew_account);
        mysql_query(conn_ptr, sql);
        result = mysql_store_result(conn_ptr);
        sqlrow = mysql_fetch_row(result);
        if (res && (atoi(sqlrow[0]) != 0)) {
            sprintf(sql, "UPDATE account SET money =\
                    money - %d where num = '%s'",
                    data.money, data.account_id);
            mysql_query(conn_ptr, sql);
            sprintf(sql, "UPDATE account SET money =\
                    money + %d where num = '%s'",
                    data.money, data.renew_account);
            mysql_query(conn_ptr, sql);
            mysql_free_result(result);

            return 1;
        }
    }
    
    return 0;
}
int ask_change_passwd(const account_data data)
{
    char sql[SQL_SIZE];
    MYSQL_RES *result;
    MYSQL_ROW sqlrow;

    sprintf(sql, "SELECT password = md5('%s') from account\
            where num = '%s'", data.passwd, data.account_id);
    if (!mysql_query(conn_ptr, sql)) {
        result = mysql_store_result(conn_ptr);
        sqlrow = mysql_fetch_row(result);
        if (atoi(sqlrow[0])) {
            sprintf(sql, "UPDATE account SET password = md5('%s')\
                    where num = '%s'", data.new_passwd, 
                    data.account_id);
            mysql_query(conn_ptr, sql);

            return 1;
        }
    }
    return 0;
}
int ask_logout(const char *account_id)
{
    char sql[SQL_SIZE];

    sprintf(sql, "UPDATE account SET\
            is_login = 0 where num = '%s'", account_id);
    mysql_query(conn_ptr, sql);

    return 1;
}
