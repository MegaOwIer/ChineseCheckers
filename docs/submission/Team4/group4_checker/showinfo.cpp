#include "showinfo.h"
#include "ui_showinfo.h"
#include "predefine.h"
#include "board.h"

//定义在main.cpp中
extern Board *board;
extern ShowInfo *showInfo;

ShowInfo::ShowInfo(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ShowInfo)
{

    //设置窗口居中
    QPoint globalPos = board->mapToGlobal(QPoint(0, 0)); //父窗口绝对坐标
    int x = globalPos.x() + (board->width() - this->width()) / 2;//x坐标
    int y = globalPos.y() + (board->height() - this->height()) / 2;//y坐标
    this->move(x, y);//窗口移动

    ui->setupUi(this);

    //初始状态下设置两个按钮都不可见，然后根据情况显示不同的按钮
    this->ui->OK->setVisible(false);
    this->ui->reConnect->setVisible(false);

    //当接收到Board::client_receive_ERROR信号时，触发接收信息并分流处理的ShowInfo::receiveInfo函数
    connect(board, &Board::client_receive_ERROR, this, &ShowInfo::receiveERROR);

    //一旦用户点击OK按钮就进行关闭窗口
    connect(ui->OK, &QPushButton::clicked, this, &ShowInfo::close);

    //一旦用户点击reConnect按钮就关闭窗口
    connect(ui->reConnect, &QPushButton::clicked, this, &ShowInfo::close);

    //一旦用户点击reConnect按钮就重新进行连接
    connect(ui->reConnect, &QPushButton::clicked, board, &Board::reConnect);

}

ShowInfo::~ShowInfo()
{
    delete ui;
}

void ShowInfo::receiveERROR(QString info, QString detail)
{

    if (info == "FAIL_TO_CONNECT")
    {
        showFAIL_TO_CONNECT();
    }

    if (info == "INVALID_JOIN")
    {
        showINVALID_JOIN(detail);
    }

    if (info == "INVALID_REQ")
    {
        showINVALID_REQ(detail);
    }

    if (info == "NOT_IN_ROOM")
    {
        showNOT_IN_ROOM(detail);
    }

    if (info == "ROOM_IS_RUNNING")
    {
        showROOM_IS_RUNNING(detail);
    }

    if (info == "ROOM_NOT_RUNNING")
    {
        showROOM_NOT_RUNNING(detail);
    }

    if (info == "OTHER_ERROR")
    {
        showOTHER_ERROR(detail);
    }

    if (info == "INVALID_MOVE")
    {
        showINVALID_MOVE(detail);
    }
}

void ShowInfo::showCONNECTING()
{
    this->ui->info->setText("Plaese waiting");
    this->ui->detail->setText("C++ term project drive me crazy!!!");
    this->ui->shark->setPixmap(QPixmap(SHARK_SLEEP));

    showInfo->show();
}

void ShowInfo::showSUCCESSFULLY_CONNECTE()
{
    //设置一些showInfo框内的基本信息
    this->ui->info->setText("SUCCESSFULLY_CONNECTE");
    this->ui->detail->setText("");
    this->ui->shark->setPixmap(QPixmap(SHARK_HAPPY));

    //将showInfo窗口显示出来等待用户处理
    showInfo->show();
    this->ui->OK->show();
}

void ShowInfo::showFAIL_TO_CONNECT()
{
    this->ui->info->setText("FAIL_TO_CONNECT");
    this->ui->detail->setText("");
    this->ui->shark->setPixmap(QPixmap(SHARK_SLEEP));

    //将showInfo窗口显示出来等待用户处理
    showInfo->show();
    //这种情况下应该显示的是reConnect按钮
    this->ui->reConnect->show();
}

void ShowInfo::showINVALID_JOIN(QString detail)
{
    this->ui->info->setText("INVALID_JOIN");
    this->ui->detail->setText(detail);
    this->ui->shark->setPixmap(QPixmap(SHARK_AMAZED));

    //将showInfo窗口显示出来等待用户处理
    showInfo->show();
    this->ui->OK->show();
}

void ShowInfo::showINVALID_REQ(QString detail)
{
    this->ui->info->setText("INVALID_REQ");
    this->ui->detail->setText(detail);
    this->ui->shark->setPixmap(QPixmap(SHARK_AMAZED));
    this->ui->OK->setText("OK");

    //将showInfo窗口显示出来等待用户处理
    showInfo->show();
    this->ui->OK->show();
}

void ShowInfo::showNOT_IN_ROOM(QString detail)
{
    this->ui->info->setText("NOT_IN_ROOM");
    this->ui->detail->setText(detail);
    this->ui->shark->setPixmap(QPixmap(SHARK_AMAZED));

    //将showInfo窗口显示出来等待用户处理
    showInfo->show();
    this->ui->OK->show();
}

void ShowInfo::showROOM_IS_RUNNING(QString detail)
{
    this->ui->info->setText("ROOM_IS_RUNNING");
    this->ui->detail->setText(detail);
    this->ui->shark->setPixmap(QPixmap(SHARK_AMAZED));

    //将showInfo窗口显示出来等待用户处理
    showInfo->show();
    this->ui->OK->show();
}

void ShowInfo::showROOM_NOT_RUNNING(QString detail)
{
    this->ui->info->setText("ROOM_NOT_RUNNING");
    this->ui->detail->setText(detail);
    this->ui->shark->setPixmap(QPixmap(SHARK_AMAZED));

    //将showInfo窗口显示出来等待用户处理
    showInfo->show();
    this->ui->OK->show();
}

void ShowInfo::showOTHER_ERROR(QString detail)
{
    this->ui->info->setText("OTHER_ERROR");
    this->ui->detail->setText(detail);
    this->ui->shark->setPixmap(QPixmap(SHARK_AMAZED));

    //将showInfo窗口显示出来等待用户处理
    showInfo->show();
    this->ui->OK->show();
}

void ShowInfo::showINVALID_MOVE(QString detail)
{
    qDebug() << "be surppose to show" << detail;

    this->ui->info->setText("INVALID_MOVE");
    this->ui->detail->setText(detail);
    this->ui->shark->setPixmap(QPixmap(SHARK_AMAZED));

    //将showInfo窗口显示出来等待用户处理
    showInfo->show();
    this->ui->OK->show();

    board->movePieceBack();
}

