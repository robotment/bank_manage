create table if not exists userinfo (
    id char(18) primary key not null,
    name varchar(30) not null,
    sex varchar(1) not null check(sex in ('m', 'f', 'F', 'M')),
    phoneNum    char(11) not null,
    address     varchar(40)
);

create table account (
    num bigint auto_increment primary key,
    id  char(18),
    password    char(33) not null,
    money   numeric(9,2) not null check(money >= 0),
    is_login    boolean not null,
    is_locked   boolean not null,
    create_time date not null,
    foreign key (id) references userinfo(id)
) auto_increment=6222023200000000000;
