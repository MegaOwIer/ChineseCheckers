#include "ask_join_room.h"
#include "ui_ask_join_room.h"
#include "board.h"
#include "predefine.h"

//定义在main.cpp中
extern Board *board;
extern int userCntInRoom;
extern int userCnt;
extern QString roomNumber;
extern QString myName;

//定义在user.cpp中
extern User users[7];
extern Player playerList[7];

ask_join_room::ask_join_room(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ask_join_room)
{
    ui->setupUi(this);

    //设置窗口居中
    // QPoint globalPos = board->mapToGlobal(QPoint(0, 0)); //父窗口绝对坐标
    // int x = globalPos.x() + (board->width() - this->width()) / 2;//x坐标
    // int y = globalPos.y() + (board->height() - this->height()) / 2;//y坐标
    this->move(200, 60);//窗口移动

    //初始状态下，用户还没有加入房间，ready按键不允许使用
    ui->Ready->setEnabled(false);

    //初始状态下，还没有加入房间，设置房间相关信息不可见
    ui->ranking->setVisible(false);
    ui->AName->setVisible(false);
    ui->BName->setVisible(false);
    ui->CName->setVisible(false);
    ui->DName->setVisible(false);
    ui->EName->setVisible(false);
    ui->FName->setVisible(false);
    ui->_1->setVisible(false);
    ui->_2->setVisible(false);
    ui->_3->setVisible(false);
    ui->_4->setVisible(false);
    ui->_5->setVisible(false);
    ui->_6->setVisible(false);
    ui->ready1->setVisible(false);
    ui->ready2->setVisible(false);
    ui->ready3->setVisible(false);
    ui->ready4->setVisible(false);
    ui->ready5->setVisible(false);
    ui->ready6->setVisible(false);

    ui->roomNum_show->setVisible(false);
    ui->Leave->setVisible(false);

    //点击Enter按钮后直接调用board对象的client_send_JOIN_ROOM函数，应该在接受到成功加入的信息的时候才禁止，因为如果成功加入应该重新输入
    connect(ui->Enter, &QPushButton::clicked, this, &ask_join_room::clickEnter);

    connect(board, &Board::forbidButton, this, &ask_join_room::forbidButton);

    //当用户接受到已经成功进入房间的信号时，激活Ready按键
    connect(board, &Board::Ready_is_enabled, ui->Ready, &QPushButton::setEnabled);

    // 在得到确定后才更新列表（receive中实现），在得到信号后才更新用户准备状态（Board::client_receive_PLAYER_READY中是实现）
    // 由setReady內部发送sendReady信号，触发更新列表的函数
    // connect(this, &ask_join_room::sendReady, this, &ask_join_room::updateTable);


    //由setReady內部发送sendReady信号，触发client_send_PLAYER_READY函数
    connect(ui->Ready, &QPushButton::clicked, this, &ask_join_room::clickReady);

    //clickReady->sendReady->Board::client_send_PLAYER_READY
    connect(this, &ask_join_room::sendReady, board, &Board::client_send_PLAYER_READY);

    //一旦用户点击leave就离开房间
    connect(ui->Leave, &QPushButton::clicked, board, &Board::client_send_LEAVE_ROOM);
    connect(ui->Leave, &QPushButton::clicked, this, &ask_join_room::init);
}

ask_join_room::~ask_join_room()
{
    delete ui;
}

void ask_join_room::init()
{
    //清空playerList
    for (int i = 1; i <= 6; i++)
    {
        playerList[i].userName = "";
        playerList[i].isReady = false;
    }

    ui->NameEdit->setVisible(true);
    ui->NameEdit->setText("Please enter your name");
    ui->roomNum->setVisible(true);
    ui->roomNum->setText("Please enter your room number");
    ui->name->setVisible(true);
    ui->room->setVisible(true);
    ui->ready1->setVisible(true);
    ui->ready2->setVisible(true);
    ui->ready3->setVisible(true);
    ui->ready4->setVisible(true);
    ui->ready5->setVisible(true);
    ui->ready6->setVisible(true);
    ui->Enter->setVisible(true);
    ui->Ready->setVisible(true);

    ui->_1->setVisible(true);
    ui->_2->setVisible(true);
    ui->_3->setVisible(true);
    ui->_4->setVisible(true);
    ui->_5->setVisible(true);
    ui->_6->setVisible(true);

    ui->AName->setVisible(true);
    ui->BName->setVisible(true);
    ui->CName->setVisible(true);
    ui->DName->setVisible(true);
    ui->EName->setVisible(true);
    ui->FName->setVisible(true);

    ui->AName->setText("");
    ui->BName->setText("");
    ui->CName->setText("");
    ui->DName->setText("");
    ui->EName->setText("");
    ui->FName->setText("");

    // ui->ranking->setVisible(true);
    // ui->ranking->setText("RANKING");
    // updateTable();

    this->show();
}

/**
 * @brief setRoomNumber
 */
void ask_join_room::setRoomNumber()
{
    //将roomNum框中的字符串存到全局的roomNumber中
    roomNumber = ui->roomNum->text();

    ui->roomNum_show->setText(roomNumber);
}

/**
 * @brief setUserName 设置自己的名字,直接在users数组中修改
 */
void ask_join_room::setUserName()
{
    //由于还不知道用户是第几个玩家，所以在myName中先暂时进行储存
    myName = ui->NameEdit->text();
}

/**
 * @brief forbidButton 如果点击了Enter按钮，就直接禁用Enter,OK1,OK2三个按钮
 */
void ask_join_room::forbidButton()
{
    ui->Enter->setEnabled(false);
}

/**
 * @brief setReady 设置自己的准备状态
 */
void ask_join_room::clickReady()
{
    //发送信号，触发Board的sendReady函数
    emit sendReady();
}

void ask_join_room::clickEnter()
{
    //设置房间号和名字
    setRoomNumber();
    setUserName();

    //调用board的client_send_JOIN_ROOM函数
    board->client_send_JOIN_ROOM();
}

void ask_join_room::updateTable()
{
    //将房间号展示标签设置为可见
    ui->roomNum_show->show();

    //将Leave按钮设置为可见
    ui->Leave->show();

    //将所有相关部件设置为可见
    ui->AName->setVisible(true);
    ui->BName->setVisible(true);
    ui->CName->setVisible(true);
    ui->DName->setVisible(true);
    ui->EName->setVisible(true);
    ui->FName->setVisible(true);
    ui->_1->setVisible(true);
    ui->_2->setVisible(true);
    ui->_3->setVisible(true);
    ui->_4->setVisible(true);
    ui->_5->setVisible(true);
    ui->_6->setVisible(true);
    ui->ready1->setVisible(true);
    ui->ready2->setVisible(true);
    ui->ready3->setVisible(true);
    ui->ready4->setVisible(true);
    ui->ready5->setVisible(true);
    ui->ready6->setVisible(true);

    //显示用户的名字
    for (int i = 1; i <= 6; i++)
    {
        //如果用户的名字不为空就进行显示
        if (!playerList[i].userName.isEmpty())
        {
            if (i == 1)
            {
                ui->AName->setText(playerList[i].userName);
            }

            if (i == 2)
            {
                ui->BName->setText(playerList[i].userName);
            }

            if (i == 3)
            {
                ui->CName->setText(playerList[i].userName);
            }

            if (i == 4)
            {
                ui->DName->setText(playerList[i].userName);
            }

            if (i == 5)
            {
                ui->EName->setText(playerList[i].userName);
            }

            if (i == 6)
            {
                ui->FName->setText(playerList[i].userName);
            }
        }
    }

    //显示用户的准备状态
    for (int i = 1; i <= 6; i++)
    {
        //如果用户已经准备好了，就为这名用户打勾
        if (playerList[i].isReady)
        {
            if (i == 1)
            {
                ui->ready1->setPixmap(QPixmap(READY));
            }

            if (i == 2)
            {
                ui->ready2->setPixmap(QPixmap(READY));
            }

            if (i == 3)
            {
                ui->ready3->setPixmap(QPixmap(READY));
            }

            if (i == 4)
            {
                ui->ready4->setPixmap(QPixmap(READY));
            }

            if (i == 5)
            {
                ui->ready5->setPixmap(QPixmap(READY));
            }

            if (i == 6)
            {
                ui->ready6->setPixmap(QPixmap(READY));
            }
        }
    }

}

void ask_join_room::showWINNER_LIST(QString detail)
{
    //清洗playerList数组
    for (int i  = 1; i <= 6; i++)
    {
        playerList[i].userName = "";
    }
    //将名字写到playerList中
    for (int i = 1; i <= userCnt; i++)
    {
        playerList[i].userName = detail.section(" ", i - 1, i - 1);
    }

    ui->NameEdit->setVisible(false);
    ui->roomNum->setVisible(false);
    ui->name->setVisible(false);
    ui->room->setVisible(false);
    ui->ready1->setVisible(false);
    ui->ready2->setVisible(false);
    ui->ready3->setVisible(false);
    ui->ready4->setVisible(false);
    ui->ready5->setVisible(false);
    ui->ready6->setVisible(false);
    ui->Enter->setVisible(false);
    ui->Ready->setVisible(false);

    ui->_1->setVisible(true);
    ui->_2->setVisible(true);
    ui->_3->setVisible(true);
    ui->_4->setVisible(true);
    ui->_5->setVisible(true);
    ui->_6->setVisible(true);

    ui->AName->setVisible(true);
    ui->BName->setVisible(true);
    ui->CName->setVisible(true);
    ui->DName->setVisible(true);
    ui->EName->setVisible(true);
    ui->FName->setVisible(true);

    ui->AName->setText(playerList[1].userName);
    ui->BName->setText(playerList[2].userName);
    ui->CName->setText(playerList[3].userName);
    ui->DName->setText(playerList[4].userName);
    ui->EName->setText(playerList[5].userName);
    ui->FName->setText(playerList[6].userName);

    ui->ranking->setVisible(true);
    ui->ranking->setText("RANKING");

    this->show();
}
