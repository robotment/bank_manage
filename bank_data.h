/* user information */

#define NAME_LEN    30
#define ID_LEN      18
#define PHONE_LEN   11
#define ADDRESS_LEN 80
#define ACCOUNT_LEN 19

typedef struct _person_infor {
    char name[NAME_LEN + 1];  /* person name, one more size for '\0' */
    char sex;
    char id[ID_LEN + 1];
    char phone[PHONE_LEN + 1];
    char address[ADDRESS_LEN + 1];
} person_infor;

#define STRING_LEN 80
typedef struct _account_data {
    char account_id[ACCOUNT_LEN + 1]; /* account num */
    char passwd[STRING_LEN + 1];
    int money;
    char renew_account[ACCOUNT_LEN+ 1]; /* renew account num */
    char new_passwd[STRING_LEN + 1];
} account_data;

/* Now we can define some functions that operate the database vitually,
 * which is seemed to be real for app_ui. They just repack the fuctions,
 * then send them to server. */

/* Actually they intialize the fifo,but seemed to intialize database for 
 * client. */
int database_initialize(void);
void database_close(void);

/* Functions that ask server to operate.*/
int ask_new_account(const person_infor user);
int ask_reset_passwd(const person_infor user, const account_data data);
int ask_report_loss(const person_infor user, const account_data data);
int ask_login(const account_data data);

/* return money in account */
int ask_query(const account_data data);
int ask_draw_money(const account_data data);
int ask_store_money(const account_data data);
int ask_renewal(const account_data data);
int ask_change_passwd(const account_data data);
int ask_logout(const char *account_id);
