/* Define interface. This gives us a simple way to show how the bank
 * system works.  */

#include <curses.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include "bank_data.h"

/* define string size in the user information and other information. */
#define TMP_STRING_SIZE 80

/* We define our menu options typedefs.*/
typedef enum _menu_options_e {
    mo_invalid,
    mo_new,
    mo_reset,
    mo_loss,
    mo_login,
    mo_logout,
    mo_query,
    mo_store,
    mo_draw,
    mo_renewal,
    mo_change_passwd,
    mo_exit
} menu_options_e;

/* Now the prototypes for the local functions.
 * The prototypes for accessing the database were included in bank_data.h. */
static void announce(void);
static menu_options_e show_menu(const char* account_id);
static int get_confirm(const char *question);
static void create_new_account(void);
static void reset_passwd(void);
static void report_loss(void);
static void login(char *account_id);

static void query(const char *account_id);
static void draw_money(const char *account_id);
static void store_money(const char *account_id);
static void renewal(const char *account_id);
static void change_passwd(const char *account_id);
static void logout(char *account_id);

static void strip_return(char *string);

int main(int argc, char *argv[])
{
    char account_id[ACCOUNT_LEN+1];
    menu_options_e current_option;

    /* Initian account_id. */
    memset(account_id, '\0', sizeof(account_id));
    current_option = mo_login; /* make sure current_option not equal mo_exit. */
    announce();
    //database_initialize();

    /* Now we wait for user to input command. */
    while (current_option != mo_exit) {
        current_option = show_menu(account_id);

        switch(current_option) {
            case mo_new:
                create_new_account();
                break;
            case mo_reset:
                reset_passwd();
                break;
            case mo_loss:
                report_loss();
                break;
            case mo_login:
                login(account_id);
                break;
            case mo_query:
                query(account_id);
                break;
            case mo_store:
                store_money(account_id);
                break;
            case mo_draw:
                draw_money(account_id);
                break;
            case mo_renewal:
                renewal(account_id);
                break;
            case mo_change_passwd:
                change_passwd(account_id);
                break;
            case mo_logout:
                logout(account_id);
                break;
            case mo_exit:
            case mo_invalid:
            default:
                break;
        } // switch
    } // while

    //database_close();
    exit(EXIT_SUCCESS);
} // main

static void announce(void)
{
    printf("\n\n\t\tWelcome to the demonstration bank management system \
            program\n");
    return;
} // announce

static menu_options_e show_menu(const char *account_id)
{
    char tmp_str[TMP_STRING_SIZE + 1];
    menu_options_e option_chosen = mo_invalid;

    while (option_chosen == mo_invalid) {
        if (account_id[0] == '\0') {
            printf("\n\nMenus:\n");
            printf("\t1 - create new account\n");
            printf("\t2 - reset your password while you can remember your password\n");
            printf("\t3 - report loss card\n");
            printf("\t4 - login with your account\n");
            printf("\t5 - exit\n");
            printf("Your choise:");
            fgets(tmp_str, TMP_STRING_SIZE, stdin);
            switch(tmp_str[0]) {
                case '1':option_chosen = mo_new; break;
                case '2':option_chosen = mo_reset; break;
                case '3':option_chosen = mo_loss; break;
                case '4':option_chosen = mo_login; break;
                case '5':option_chosen = mo_exit; break;
            }
        } else {
            printf("\nMenus:\n");
            printf("\t1 - query money left\n");
            printf("\t2 - get some money\n");
            printf("\t3 - store some into accont\n");
            printf("\t4 - transfer money to another account\n");
            printf("\t5 - change password\n");
            printf("\t6 - logout\n");
            printf("Your choice:");
            fgets(tmp_str, TMP_STRING_SIZE, stdin);
            switch(tmp_str[0]) {
                case '1':option_chosen = mo_query; break;
                case '2':option_chosen = mo_draw; break;
                case '3':option_chosen = mo_store; break;
                case '4':option_chosen = mo_renewal; break;
                case '5':option_chosen = mo_change_passwd; break;
                case '6':option_chosen = mo_logout; break;
            }
        }
    }

    return option_chosen;

}

/* confirm your choices.
 */
static int get_confirm(const char *question)
{
    char tmp_str[TMP_STRING_SIZE + 1];

    printf("%s", question);
    fgets(tmp_str, TMP_STRING_SIZE, stdin);
    if (tmp_str[0] == 'Y' || tmp_str[0] == 'y') {
        return 1;
    }

    return 0;
}

/* Strip return.
 */
static void strip_return(char *str)
{
    if (str[strlen(str) - 1] == '\n')
        str[strlen(str)-1] = '\0';

    return;
}

/* clear stdin.
 * Stdin may retain some word before input.
 */
static void clear_input()
{
    while (getc(stdin) != '\n');

    return;
}

static int check_name(char *name)
{
    return 0;
}

static int check_sex(char sex)
{
    return 0;
}

static int check_id(char *id)
{
    return 0;
}

static int check_phone(char *phone)
{
    return 0;
}

static int check_address(char *address)
{
    return 0;
}

static int check_account(char *account)
{
    return 0;
}

/* create a new account.
 * You must input your information(id num, name, phone number, sex, address
 * at least). Then the system will generate your accont numbers.
 */
static void create_new_account(void)
{
    person_infor person;
    int invalid = 1;

    memset(person.name, '\0', sizeof(person.name));
    memset(person.id, '\0', sizeof(person.id));
    memset(person.phone, '\0', sizeof(person.phone));
    memset(person.address, '\0', sizeof(person.address));

    /* MARK: muliple repeat. Can be improved in future.
     */
    while (invalid) {
        printf("Name: ");
        //clear_input(); /* clear stdin */
        fgets(person.name, NAME_LEN, stdin);
        strip_return(person.name);
        invalid = check_name(person.name);
    }

    invalid = 1;
    while (invalid) {
        printf("Sex: ");
        person.sex = getchar();
        clear_input();
        invalid = check_sex(person.sex);
    }

    invalid = 1;
    while (invalid) {
        printf("ID: ");
        fgets(person.id, ID_LEN+2, stdin); /* one more for '\n', one for '\0' */
        strip_return(person.id);
        invalid = check_id(person.id);
    }

    invalid = 1;
    while (invalid) {
        printf("Phone: ");
        fgets(person.phone, PHONE_LEN+2, stdin);
        strip_return(person.phone);
        invalid = check_phone(person.phone);
    }
    
    invalid = 1;
    while (invalid) {
        printf("Address: ");
        fgets(person.address, ADDRESS_LEN, stdin);
        strip_return(person.address);
        /* Don't check address, it can be non. */
        invalid = 0;
    }

    
    printf("\nName: %s\n", person.name);
    printf("Sex: %c\n", person.sex);
    printf("ID: %s\n", person.id);
    printf("Phone: %s\n", person.phone);
    printf("Address: %s\n", person.address);
    if (get_confirm("Create this account ?")) 
        //ask_new_account(person);
    return;
}

/* Reset your password when you can't remember it.
 * You must input your name, id and account id.
 * Then system will check them. If they are correct then your account 
 * password will be setted to '000000'. Later, the system require you
 * to change your password imme*****.
 */
static void reset_passwd(void)
{
    person_infor person;
    account_data account;

    printf("Account: ");
    fgets(account.account_id, ACCOUNT_LEN+2, stdin);
    strip_return(account.account_id);

    printf("Name: ");
    fgets(person.name, NAME_LEN+2, stdin);
    strip_return(person.name);

    printf("ID: ");
    fgets(person.id, ID_LEN+1, stdin);

    printf("\nAccount: %s\n", account.account_id);
    printf("Name: %s\n", person.name);
    printf("ID: %s\n", person.id);
    if (get_confirm("Reset this account's password ?"))
        ask_reset_passwd(person, account);
          

    return;
}

/* Report your lossing.
 */
static void report_loss(void)
{
    person_infor person;
    account_data account;

    printf("Account: ");
    fgets(account.account_id, ACCOUNT_LEN+2, stdin);
    strip_return(account.account_id);

    printf("Name: ");
    fgets(person.name, NAME_LEN+2, stdin);
    strip_return(person.name);

    printf("ID: ");
    fgets(person.id, ID_LEN+2, stdin);
    strip_return(person.id);

    printf("\nAccount: %s\n", account.account_id);
    printf("Name: %s\n", person.name);
    printf("ID: %s\n", person.id);
    if (get_confirm("Report lossing?"))
        //ask_report_loss(person, account);

    return;
}

/* Login your account.
 * Input your account num and pasword.
 */
static void login(char *account_id)
{
    account_data account;

    printf("Account: ");
    fgets(account.account_id, ACCOUNT_LEN+2, stdin);
    strip_return(account.account_id);

    /* Visually password input.
     * No time to correct it, just let it goes.
     */
    printf("Passwd: ");
    fgets(account.passwd, STRING_LEN, stdin); 
    strip_return(account.passwd);
    
    /*
    if (ask_login(account)) {
        strncpy(account_id, account.account_id, ACCOUNT_LEN+1);
    }
    */

    return;
}


/* Query money left in your account.
 */
static void query(const char *account_id)
{
    int money = 0;
    if ((money = ask_query(account_id)) >= 0) {
        printf("\n\tMoney left: %d", money);
    }
        
    return;
}

/* Draw moeny from your account.
 * Enter number you want to get out.
 */
static void draw_money(const char *account_id)
{
    account_data account;
    
    strcpy(account.account_id, account_id);
    printf("Take money: ");
    fscanf(stdin, "%d", account.money);

    if (ask_draw_money(account)) {
        printf("Success!\n");
    }

    return;
}

/* Store money into your account.
 * Enter numbers.
 */
static void store_money(const char *account_id)
{
    account_data account;

    strcpy(account.account_id, account_id);
    printf("Store money: ");
    fscanf(stdin, "%d", &account.money);
    clear_input();

    if (ask_store_money(account)) {
        printf("Success!");
    }

    return;
}

/* Transfer your money to another account.
 * Input account number and money.
 */
static void renewal(const char *account_id)
{
    account_data account;

    strcpy(account.account_id, account_id);
    
    printf("Transfer account: ");
    fgets(account.renew_account, ACCOUNT_LEN+2, stdin);
    strip_return(account.renew_account);

    fprintf("Transfer money: ");
    fscanf(stdin, "%d", &account.money);

    printf("\nTransfer account: %s\n", account.renew_account);
    printf("Transfer money: %d\n", account.money);
    if (get_confirm("Are you sure?")) {
        if (ask_renewal(account)) 
            printf("Success!\n");
    }

    return;
}

/* Change password.
 * Your must provide your previous passwd and new ones.
 */
static void change_passwd(const char *account_id)
{
    account_data account;
    int valid = 1;
    char passwd[STRING_LEN];

    strcpy(account.account_id, account_id);
    printf("Old password: ");
    fgets(account.passwd, STRING_LEN, stdin);
    strip_return(account.passwd);
    while (valid) {
        printf("New password: ");
        fgets(account.new_passwd, STRING_LEN, stdin);
        strip_return(account.new_passwd);

        printf("Again: ");
        fgets(passwd, STRING_LEN, stdin);
        strip_return(passwd);

        valid = 0;
        if (strcmp(passwd, account.new_passwd) != 0) {
            printf("New password don't match.\n");
            valid = 1;
        }
    }

    if (ask_change_passwd(account)) {
        printf("Password changed!\n");
    }

    return;
}

/* Logout.
 */
static void logout(char *account_id)
{
    if (ask_logou(account_id))
        account_id[0] = '\0';

    return;
}
