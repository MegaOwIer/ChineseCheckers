#include "hello.h"
#include "board.h"
#include "ui_hello.h"
#include "ask_server_or_client.h"

extern int  userCnt;
extern int  mode;
extern int  thisIsServer;
extern bool INIT_GAME;
extern bool USER_HAS_BEEN_INIT;
extern ask_server_or_client *ask1;
extern Board* board;

Hello::Hello(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Hello)
{

    //设置窗口居中
    QPoint globalPos = board->mapToGlobal(QPoint(0, 0)); //父窗口绝对坐标
    int x = globalPos.x() + (board->width() - this->width()) / 2;//x坐标
    // int y = globalPos.y() + (board->height() - this->height()) / 2;//y坐标
    this->move(x, 60);//窗口移动

    ui->setupUi(this);
    connect(ui->a_single_mode,
            &QPushButton::clicked,
            this,
            &Hello::set_a_single_mode);
    connect(ui->AI_mode, &QPushButton::clicked, this, &Hello::set_AI_mode);
    connect(ui->network_mode,
            &QPushButton::clicked,
            this,
            &Hello::set_network_mode);
}

Hello::~Hello()
{
    delete ui;
}

void Hello::get_userCnt()
{
    QString suserCnt = ui->lineEdit->text();
    qDebug() << "suserCnt is " << suserCnt;
    QString str2("2");
    QString str3("3");
    QString str6("6");

    if (suserCnt == str2)
    {
        userCnt = 2;
    }

    if (suserCnt == str3)
    {
        userCnt = 3;
    }

    if (suserCnt == str6)
    {
        userCnt = 6;
    }

    if (suserCnt == nullptr) // 什么都不写，默认为6
    {
        userCnt = 6;
    }
}

void Hello::set_a_single_mode()
{
    mode = 1;
    get_userCnt();
    INIT_GAME = true;
    initUsers();
    USER_HAS_BEEN_INIT = true;
    this->close();
}

void Hello::set_AI_mode()
{
    mode = 2;
    get_userCnt();
    INIT_GAME = true;
    initUsers();
    USER_HAS_BEEN_INIT = true;
    this->close();
}

void Hello::set_network_mode()
{
    mode = 3;
    // get_userCnt();
    // INIT_GAME = true;
    // initUsers();
    // USER_HAS_BEEN_INIT = true;
    this->close();

    //显示ask1窗口
    ask1->show();
}


