#include "ask_server_or_client.h"
#include "ui_ask_server_or_client.h"
#include "ask_join_room.h"
#include "board.h"
#include "showinfo.h"
//定义在main函数中
extern Board *board;
extern int networkMode;
extern ask_join_room *ask2;
extern ShowInfo *showInfo;

ask_server_or_client::ask_server_or_client(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ask_server_or_client)
{
    //设置窗口居中
    // QPoint globalPos = board->mapToGlobal(QPoint(0, 0)); //父窗口绝对坐标
    // int x = globalPos.x() + (board->width() - this->width()) / 2;//x坐标
    // int y = globalPos.y() + (board->height() - this->height()) / 2;//y坐标
    this->move(270, 150);//窗口移动

    ui->setupUi(this);

    //点击按钮设置模式
    connect(ui->Client, &QPushButton::clicked, this, &ask_server_or_client::setClientMode);
    connect(ui->Server, &QPushButton::clicked, this, &ask_server_or_client::setServerMode);

    //设置按钮的点击导致窗口的关闭
    connect(ui->Client, &QPushButton::clicked, this, &ask_server_or_client::close);
    connect(ui->Server, &QPushButton::clicked, this, &ask_server_or_client::close);
}

ask_server_or_client::~ask_server_or_client()
{
    delete ui;
}

void ask_server_or_client::setClientMode()
{
    networkMode = 0;    //执行客户端模式

    board->isServer = networkMode;

    delete board->server;   //如果是客户端模式，删除server指针

    //ask2->show();  只有在连接成功后才显示

    showInfo->showCONNECTING();  //显示正在连接的信息

    board->socket->hello("10.46.54.135", 16666);   //一旦点击ClientMode就发送连接请求
}

void ask_server_or_client::setServerMode()
{
    networkMode = 1;    //执行服务端模式

    board->isServer = networkMode;

    delete board->socket;  //如果是服务端模式，删除socket指针
}

