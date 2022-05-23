#include "board.h"
#include "ask_join_room.h"
#include "qwhtimimg.h"
#include "predefine.h"
#include "showinfo.h"
#include <cmath>

//定义在user.cpp中
extern User *curUser;
extern User *me;
extern User  users[7];
extern ask_join_room *ask2;
//定义在coord.cpp中
extern Coord center;
extern Coord nullCoord;
//定义在main.cpp中
extern Player playerList[7];
extern int mode;
extern int userCnt;
extern int userCntInRoom;
extern bool networkMode;
extern QString roomNumber;
extern QWHTimimg* countdown;
extern QString myName;
extern ShowInfo *showInfo;
extern Player winnerList[7];

Coord coords[25][17];


Coord NOTCHOSEN(-100, -100);

Coord curChosenCoord = NOTCHOSEN;

/* 这是决定是否初始化游戏的变量，整个游戏进程中只需要进行一次，所以采用以下特殊的方式进行设置
 * ywt:2022\4\16 虽然我不知道下面前天的这两行代码具体是写来干嘛的，现在看起来第二个bool变量似乎没什么用
 * 但是既然程序运行流畅，建议就不要再乱改了......
 * hello出现的时候是false，hello关闭以后设置为true，画完以后再改为false
 */
bool INIT_GAME = false;
bool USER_HAS_BEEN_INIT = false;
bool START_GAME = false;

// int entertime = 0;

// coord的计算原理:id和Coord的转换

// 数组坐标和系统坐标的转换函数
void initCoords()
{
    for (int i = 0; i <= 24; i++)
    {
        for (int j = 0; j <= 16; j++)
        {
            coords[i][j].coord_x = i - 12;
            coords[i][j].coord_y = j - 8;
            coords[i][j].setHaveSlot(false);
        }
    }
    qDebug() << "Coords have been init";
}

//初始化用户(这个初始化操作不包括对于用户名的设置（这是在joinRoom以后进行的）)
void initUsers()
{
    users[0].zoom = "O";
    users[1].zoom = "A";
    users[2].zoom = "B";
    users[3].zoom = "C";
    users[4].zoom = "D";
    users[5].zoom = "E";
    users[6].zoom = "F";

    for (int i = 0; i <= 6; i++)
    {
        users[i].id = i;            //设置初始化用户的id
        users[i].initMoveTrack();   //初始化用户的行棋轨迹
    }

    switch (userCnt)
    {
        case 2:
            users[1].setUserColor(uyellow);
            users[2].setUserColor(unull);
            users[3].setUserColor(unull);
            users[4].setUserColor(ublue);
            users[5].setUserColor(unull);
            users[6].setUserColor(unull);
            break;

        case 3:
            users[1].setUserColor(uyellow);
            users[2].setUserColor(unull);
            users[3].setUserColor(upurple);
            users[4].setUserColor(unull);
            users[5].setUserColor(ucyan);
            users[6].setUserColor(unull);
            break;

        case 6:
            users[1].setUserColor(uyellow);
            users[2].setUserColor(ured);
            users[3].setUserColor(upurple);
            users[4].setUserColor(ublue);
            users[5].setUserColor(ucyan);
            users[6].setUserColor(ugreen);
            break;
    }

    curUser = &users[1];
}

// board的初始化构造函数
Board::Board(QWidget *parent)
    : QMainWindow{parent}
{
    //在窗口类的构造函数中加入下列代码来完成初始化和信号与槽的连接
    this->server = new NetworkServer(this);
    connect(this->server, &NetworkServer::receive, this, &Board::receiveData);
    /*TODO solved(ywt:2022/4/28)：
     * 自己实现一个槽函数Board::receiveData(server)
     * 这个槽函数用来处理服务端收到的信息
     */

    //初始化socket
    this->socket = new NetworkSocket(new QTcpSocket(), this);

    //初始化信号和槽的连接的操作
    //当socket发出receive信号时
    //客户端主窗口的槽函数做出相应
    connect(socket, &NetworkSocket::receive, this, &Board::receive);

    /*TODO unsolved(ywt:2022/4/28)
     * 完成Board类里面接收信息的receive函数
     */

    //初始化信号和槽的断开的操作
    connect(socket->base(), &QAbstractSocket::disconnected, [ = ]()
    {
        QMessageBox::critical(this, tr("Connection lost"), tr("Connection to server has closed"));
    });

    //连接失败
    connect(socket->base(), &QAbstractSocket::errorOccurred, this, &Board::displayError);
    /*TODO unsolved(ywt:2022\4\28)
     * 完成displayError槽函数：用于报告连接失败的信息，并处理这一失败造成的相应结果
     */

    //连接成功
    connect(socket->base(), &QAbstractSocket::connected, this, &Board::connected);
    /*TODO unsolved(ywt:2022\4\28)
     * 完成connected槽函数：用于报告连接成功的信息，并在处理后直接消失
     */
}

// -------------------------------------------鼠标点击相关----------------------------------------------------
// 鼠标点击事件函数
int realeseTime;

void Board::mouseReleaseEvent(QMouseEvent *event)
{
    if (mode == 3 && !START_GAME)  //如果是网络模式，并且还没有初始化一局游戏，直接return（避免点击到还没初始化的坐标）
    {
        return;
    }

    if (mode == 3 && this->isServer == true)  //服务端不设置鼠标点击功能
    {
        return;
    }

    if (mode == 1)
    {
        realeseTime++;
        qDebug() << "you click " << realeseTime << " times";

        // 获取当前点击的位置
        QPoint curPoint = event->pos();


        {
            // 将当前的点击位置对应到具体的坐标(枚举法或二分查找，后续请进行优化)
            // Coord curCoord = getCoord(curPoint);
            // {
            // do语句的作用在于实现可取消行棋的操作
            // do
            // {
            // 重走一次就设置为false,如果又取消，在updateChosenState里面会进行重新设值
            // RECHOOSE = false;
            // if语句获取当前点击的位置上棋子的颜色并进行校对
            // 规定只能使用自己颜色的棋子
            // if (curCoord.getColor() == curUser.getUserColor())
            // {
            // 如果校对成功被选中棋子作出响应:
            //            // 响应一：修改自身的信息
            //            connect(&curCoord,
            //                    &Coord::updateChosenState,
            //                    this,
            //                    &Board::updateChosenState);
            //            // 响应二：使其它棋子处于不可选状态
            //            // 响应三：绘制选中效果（利用qpainter的重绘效果）
            //            connect(&curCoord,
            //                    &Coord::updateChosenState,
            //                    this,
            //                    &Board::updateChosenStateImag);
            //            // 将自身作为参数发送信号
            //            emit curCoord.updateChosenState();
            // 事实上，feasibility已经为我们此步打下了很好的铺垫
            // this->updateChosenState(curCoord);
            // 进行可行性分析，并进行可移动位置的标注（调用painter）
            // 初始设置跳动次数为0
            // this->updateFeasibility(curCoord, 0);
            // 再次获取选中的区域
            // 判断该区域是不是被标注过的区域
            // 清除标注，绘制棋子的移动，循环可行性分析的步骤
            // }
            // }while (RECHOOSE);
        }

        Coord curCoord = getCoord(curPoint);

        // 以下是用户自己点击的next
        // 是否点击next一定在是否点击棋槽之前
        // 如果当前是用户的第一次跳跃,并且试图点击 next，直接忽略其想要点击的操作，进行下一个if判断
        if (curUser->jumpTime != 0)
        {
            // qDebug() << "curUser's jumpTime != 0--------------------------------------------------------";
            qDebug() << "curUser'jumpTime is" << curUser->jumpTime;


            // 判断当前点是否在next按键的指定范围内，如果没有，退出这个if判断（不能直接return，因为用户有可能是在点棋槽）
            if ((curPoint.x() >= Coord(16, 6).coord2QPoint().x())
                    && (curPoint.x() <= Coord(28, 6).coord2QPoint().x())
                    && (curPoint.y() >= Coord(16, 6).coord2QPoint().y())
                    && (curPoint.y() <= Coord(16, 4).coord2QPoint().y()))
                // 如果点在区域内,注意:这里有一个非常易错的地方，在我们的坐标系内，QPoint的纵坐标越大，Coord的纵坐标越小
            {
                //以下是点击next的操作

                qDebug() << "user click next legally";

                // 撤销所有的可行性设置
                this->cancelFeasibility();

                // 撤销用户所有的选择
                this->cancelChoose();

                // 要将这位用户的跳动次数置为0
                curUser->jumpTime = 0;

                next();
                // next函数里面已经包含了updata

                return;
            }
        }

        if (curCoord.coord_x == -100) // 如果返回的是空坐标，说明没点到棋子或棋槽，直接return
        {
            qDebug() << "nullPlace";
            return;
        }

        // 点到了我们定义的coords
        // 如果当前界面没有选中的棋子
        if (curChosenCoord.coord_x == -100)
        {
            qDebug() << "have not Chosen piece-----------------------------------------------------";

            //qDebug() << "clicked piece is " << curCoord.getColor() << "my color is " << curUser->getUserColor();

            //(你要先确保你点到的是棋子)
            if (curCoord.getColor() != null)
            {
                // 如果点到的是别人的棋子，说明点错了，直接return
                if (int(curCoord.getColor()) != int(curUser->getUserColor()))
                {
                    qDebug() << "not my piece";
                    return;
                }
                // 如果点到的是自己的棋子，将其设为选中，并展示可行性
                else if (int(curCoord.getColor()) == int(curUser->getUserColor()))
                {
                    qDebug() << "choose my piece--------------------------------------------------------------";

                    this->choose(curCoord);
                    this->search();  // search 依赖于curChosenUser指针，不用传参
                    update();        // 更新
                    return;
                }
            }
            else
            {
                return ;
            }
        }

        // 如果当前界面已经有选中的棋子了
        else
        {
            qDebug() << "have piece chosen-----------------------------------------------------------";

            qDebug() << "curCoord is " << curCoord.coord_x << " " << curCoord.coord_y;

            //如果点到的是自己的刚刚选中的棋子，执行取消操作,如果用户已经操作过了，则禁止这种行为
            if ((curCoord.coord_x == curChosenCoord.coord_x)
                    && (curCoord.coord_y == curChosenCoord.coord_y)
                    && curUser->jumpTime == 0)
            {
                qDebug() << "cancle the piece-------------------------------------------------------";
                // 注意这三行代码的顺序先后
                // 清空原来棋子的选中状态，指针指向空地址，调用绘图函数更新图象

                cancelFeasibility();
                cancelChoose();
                update();
                return;
            }

            // 如果点到的不是自己刚刚选中的棋子

            //如果点到的是自己的其它棋子,并且这个用户当前还没有跳过棋（如果这个用户已经操作过了，则禁止这种行为）
            if (int(curCoord.getColor()) == int(curUser->getUserColor()) && curUser->jumpTime == 0)
            {
                cancelFeasibility();
                cancelChoose();
                choose(curCoord);
                search();
                update();
                return;
            }

            // 如果点到的是不能到达的位置，直接return
            if (curCoord.getFeasibility() == false)
            {
                return;
            }

            // 如果点到的是能到达的位置,绘制新的棋子布局图像,然后返回
            if (curCoord.getFeasibility() == true)
            {
                // qDebug() << "move piece";

                //这个位置虽然能够到达，但是这个位置在自己的历史轨迹上，我们应该阻止这种行为
                for (int i = 0; i <= 20; i++)
                {
                    //如果当前点击的棋子已经成为过某次跳跃的终点或者自己第一次跳跃的起点（这里不会和取消操作冲突，因为对取消操作的判断已经在前面进行过了），直接return
                    if (curCoord == curUser->moveTrack[i][1] || curCoord == curUser->moveTrack[1][0])
                    {
                        return;
                    }
                }

                /*TODO(ywt:2022\4\29)
                 * 测试一下新增加的这个阻止返回的功能
                 */

                move(curChosenCoord, curCoord);  //移动（我们允许在客户端先显示移动以后的画面，如果服务端检测到异常再进行强制遣返）

                //如果进行的是间隔跳跃
                if (abs(curChosenCoord.coord_x - curCoord.coord_x) >= 4
                        || abs(curChosenCoord.coord_y - curCoord.coord_y) >= 2)
                {
                    curUser->jumpTime++;  //跳跃次数+1

                    //记录轨迹
                    curUser->moveTrack[curUser->jumpTime][0] = curChosenCoord;
                    curUser->moveTrack[curUser->jumpTime][1] = curCoord;

                    //在用户点击next时才将jumpTime置为0（这里不置零）

                    this->cancelChoose();  //撤销已经选择的棋子
                    this->cancelFeasibility();  //撤销所有的可行性标注
                    choose(curCoord);      //选择跳到的位置作为当前选择的位置
                    search();              //搜索可行性
                    update();
                    return;
                }


                //如果进行的是相邻跳跃
                else
                {
                    curUser->jumpTime++;  //跳跃次数++
                    curUser->moveTrack[curUser->jumpTime][0] = curChosenCoord; //记录轨迹
                    curUser->moveTrack[curUser->jumpTime][1] = curCoord;
                    this->cancelChoose();  //撤销已经选择的棋子
                    this->cancelFeasibility();  //撤销所有的可行性标注
                    next();
                    update();
                    return;
                }
            }
        }
    }

    // 如果当前模式为3并且是客户端

    // 事实上并不需要在客户端将那些各种限制条件都删除，但是服务端上要有必要的判断功能

    // 事实上feasibility，chosenState，moveTrack, jumpTime等等，都是在服务端的辅助功能，并不需要我们进行信息传递，也不需要我们进行服务端的检测

    // 所以事实上我们要检测的内容只有：用户是不是点了next，用户的移动轨迹是什么

    if (mode == 3 && this->isServer == false)
    {
        //debug语句：鼠标点击次数
        realeseTime++;
        //qDebug() << "you click " << realeseTime << " times";

        // 获取鼠标当前点击的位置
        QPoint curPoint = event->pos();

        // 将鼠标当前点击的位置转换成鼠标当前点击的点
        Coord curCoord = getCoord(curPoint);

        // 以下是用户自己点击的next
        // 如果当前是用户的第一次跳跃,并且试图点击 next，直接忽略其想要点击的操作，进行下一个if判断

        qDebug() << "My jumpTime is" << me->jumpTime;

        // TODO：服务端在这边应该能够识别用户是第几次跳跃，并对第一次跳跃就试图点击next（MOVE中必然传递空字符串）的行为进行报错

        if (curUser != me)  // 客户端应该增加的新功能:如果curUser和me指针指向的不是users数组中的同一个玩家对象，那么直接return，无视用户的所有操作
        {
            return;
        }


        if (me->jumpTime != 0)    //如果当前用户不是第一次跳跃
        {
            // 判断当前点是否在next按键的指定范围内，如果没有，退出这个if判断（不能直接return，因为用户有可能是在点棋槽）
            if ((curPoint.x() >= Coord(16, 6).coord2QPoint().x())
                    && (curPoint.x() <= Coord(28, 6).coord2QPoint().x())
                    && (curPoint.y() >= Coord(16, 6).coord2QPoint().y())
                    && (curPoint.y() <= Coord(16, 4).coord2QPoint().y()))  // 如果点在区域内,注意:这里有一个非常易错的地方，在我们的坐标系内，QPoint的纵坐标越大，Coord的纵坐标越小

            {

                qDebug() << "user click next legally";

                // 以下是网络模式下点击next后的操作
                // next被封装到了Board::client_receive_MOVE内
                // 当收到来自客户端的回复时，才调用next功能

                this->cancelFeasibility();   // 撤销所有的可行性设置

                this->cancelChoose();   // 撤销用户所有的选择

                /*todo：
                 *当收到来自客户端的错误信息时
                 *将棋子退回本回合的起始位置，并且前面已经将jumpTime置零，让玩家重新开始操作
                 */

                //重置轨迹的操作不能写在这里，要等收到客户端信息确认可以重置才进行重置

                client_send_MOVE();     //发送信息

                me->jumpTime = 0;  // 要将自己的跳动次数置为0

                update();               //这里允许现在客户端先显示移动的图像，如果出错再进行撤回

                return;
            }
        }

        if (curCoord.coord_x == -100) // 如果返回的是空坐标，说明没点到棋子或棋槽，直接return，就算是在联网状态下也无需传递信号
        {
            qDebug() << "nullPlace";
            return;
        }

        if (curChosenCoord.coord_x == -100)  //如果当前界面没有被选中的棋子
        {
            qDebug() << "have not Chosen piece-----------------------------------------------------";

            qDebug() << "clicked piece is " << curCoord.getColor() << "my color is " << me->getUserColor();

            if (curCoord.getColor() != null)  //(你要先确保你点到的是棋子而不是棋槽)
            {
                if (int(curCoord.getColor()) != int(me->getUserColor()))   // 如果点到的是别人的棋子，说明点错了，直接return
                {
                    qDebug() << "not my piece";
                    return;
                }

                else if (int(curCoord.getColor()) == int(me->getUserColor()))  // 如果点到的是自己的棋子，将其设为选中，并展示可行性，选中棋子这一步不需要服务端的介入，也不需要接收服务端的反馈
                {
                    qDebug() << "choose my piece--------------------------------------------------------------";

                    this->choose(curCoord);
                    this->search();   // search 依赖于curChosenUser指针，不用传参
                    update();     // 更新
                    return;
                }
            }
            else
            {
                return ;         //点到空棋槽的话直接return
            }
        }
        else       // 如果当前界面已经有选中的棋子了
        {
            qDebug() << "have piece chosen-----------------------------------------------------------";

            qDebug() << "curCoord is " << curCoord.coord_x << " " << curCoord.coord_y;

            //如果点到的是自己的刚刚选中的棋子，执行取消操作,如果用户已经操作过了，则禁止这种行为，取消操作是纯客户端的行为
            if ((curCoord.coord_x == curChosenCoord.coord_x)
                    && (curCoord.coord_y == curChosenCoord.coord_y)
                    && me->jumpTime == 0)
            {
                qDebug() << "cancle the piece-------------------------------------------------------";
                // 注意这三行代码的顺序先后
                // 清空原来棋子的选中状态，指针指向空地址，调用绘图函数更新图象

                cancelFeasibility();
                cancelChoose();
                update();
                return;
            }

            // 以下表示如果点到的不是自己刚刚选中的棋子

            if (int(curCoord.getColor()) == int(me->getUserColor()) && me->jumpTime == 0)   //如果点到的是自己的其它棋子,并且这个用户当前还没有跳过棋，就执行换棋（如果这个用户已经操作过了，则禁止这种行为），中途换棋也是纯客户端的行为
            {
                cancelFeasibility();
                cancelChoose();
                choose(curCoord);
                search();
                update();
                return;
            }

            if (curCoord.getFeasibility() == false)  // 如果点到的是不能到达的位置，直接return
            {
                return;
            }

            if (curCoord.getFeasibility() == true) // 如果点到的是能到达的位置,绘制新的棋子布局图像,记录到轨迹中
            {
                // qDebug() << "move piece";

                for (int i = 0; i <= 20; i++)   //这个位置虽然能够到达，但是这个位置在自己的历史轨迹上，我们应该阻止这种行为，判定历史轨迹也是纯客户端的行为
                {
                    //如果当前点击的棋子已经成为过某次跳跃的终点或者自己第一次跳跃的起点，直接return
                    //（这里不会和取消操作冲突，因为对取消操作的判断已经在前面进行过了，不会和点击棋盘外的其它位置冲突，因为这个已经判断过了）
                    if (curCoord == me->moveTrack[i][1] || curCoord == me->moveTrack[1][0])
                    {
                        return;
                    }
                }
                move(curChosenCoord, curCoord);  //允许自己的客户端先进行移动

                if (abs(curChosenCoord.coord_x - curCoord.coord_x) >= 4
                        || abs(curChosenCoord.coord_y - curCoord.coord_y) >= 2)  //如果进行的是间隔跳跃
                {
                    me->jumpTime++;   //当点击next时会把jumpTime置为0

                    //记录轨迹（网络模式下，应该将move记录到自己的moveTrack里面）
                    me->moveTrack[me->jumpTime][0] = curChosenCoord;
                    me->moveTrack[me->jumpTime][1] = curCoord;
                    //moveTrack应该在得到服务端反馈以后才清空，因为如果接收到的是错误信息，我们还应该通过moveTrack进行遣返

                    this->cancelChoose();  //撤销已经选择的棋子
                    this->cancelFeasibility();  //撤销所有的可行性标注
                    choose(curCoord);      //选择跳到的位置作为当前选择的位置
                    search();              //搜索可行性
                    update();              //重绘
                    return;
                }

                //如果进行的是相邻跳跃
                else
                {
                    me->jumpTime++;
                    me->moveTrack[me->jumpTime][0] = curChosenCoord;
                    me->moveTrack[me->jumpTime][1] = curCoord;

                    //me->jumpTime = 0;  在next中已经实现
                    this->cancelChoose();  //撤销已经选择的棋子
                    this->cancelFeasibility();  //撤销所有的可行性标志
                    update();              //重绘（允许在自己的客户端重绘）
                    client_send_MOVE();    //发送信号（里面包含了next函数）
                    return;
                }
            }
        }
    }
}

// 根据鼠标点击的位置得到当前点击的棋子对象
Coord& Board::getCoord(QPoint& curPoint)
{
    for (int i = 0; i <= 24; i++)     // 枚举行
    {
        for (int j = 0; j <= 16; j++)   // 枚举列
        {
            // 如果这个位置是有棋槽的
            if (coords[i][j].getHaveSlot() == true)
            {
                // distance是用户点击点和当前枚举的棋槽点的距离
                int distance =
                    (curPoint.x() - coords[i][j].coord2QPoint().x()) *
                    (curPoint.x() - coords[i][j].coord2QPoint().x()) +
                    (curPoint.y() - coords[i][j].coord2QPoint().y()) *
                    (curPoint.y() - coords[i][j].coord2QPoint().y());

                // 距离小于棋槽半径则返回引用
                if (distance <= SLOTR * SLOTR)
                {
                    qDebug() << "you click coords:" << coords[i][j].coord_x << " " <<
                             coords[i][j].coord_y << " " << "\n";
                    return coords[i][j];
                }
            }
        }
    }

    // 如果用户点击的位置在界面外，则返回nullCoord，届时通过比较地址进行校对
    return nullCoord;
}

// --------------------------------------------更新棋盘信息的函数-----------------------------------------------------------

void deserted()
{
// 更新棋盘信息的子函数(更新棋子的颜色)(信息设置):包含行棋操作

// 这里给出警示：不要过度封装函数！！不要封装边沿细节！！不要封装边沿细节！！不要封装边沿细节！！


// {
// void Board::updatePieceColor(Coord& beginCoord, Coord& endCoord)
// {
//    // 将终点色设置为起点色


//    endCoord.setColor(beginCoord.getColor());

//    // 将起点色置为空
//    beginCoord.setColor(null);

//    // 将为了这一步而进行的前置操作清空
//    // 这里的顺序需要注意:应该先调用updateChosenState（因为函数这个里面会设置curChosenCoord的值）
//    // 再将的指向beginCoord的curChosenCoord置为NULL，才能满足清除可行性标志的条件

//    // 将起点处的选择状态置为空
//    this->updateChosenState(beginCoord);

//    // 撤销原本那个被选中的棋子引发的所有可行性状态都置为0
//    this->updateFeasibility(curChosenCoord);

//    // 重新执行painteEvent
//    // update();
// }

// 更新棋盘信息的子函数（更新棋盘的可行棋性）(信息设置)
// chosenCoord是初始位置
// int Board::updateFeasibility(Coord *chosenCoord)
// {
//    // 如果有被选中的棋子，则更新可行棋性
//    if (this->curChosenCoord != NULL)
//    {
//        //

//        { //        // 储存当前的搜索结果总数
//          //        int cur_result = 0;

//            //        for (int i = 0; i <= 16; i++)
//            //        {
//            //            for (int j = 0; j <= 24; j++)
//            //            {
//            //                // 当jumpTime为0的时候可以跳到周围的位置
//            //                if (jumpTime == 0)
//            //                {
//            //                    if ((j - 1 >= 0) && (i + 1 <= 16))
//            //                        if ((coords[j - 1][i +
// 1].getHaveSlot() ==
//            // true) &&
//            //                            (coords[j - 1][i + 1].getColor()
// ==
//            //                             null)) coords[j - 1][i +
//            // 1].setFeasibility(true);

//            //                    if ((j + 1 <= 24) && (i + 1 <= 16) && (j
// - 1
//            // >=
//            // 0))
//            //                        if ((coords[j + 1][i +
// 1].getHaveSlot() ==
//            //                             true) &&
//            //                            (coords[j + 1][i + 1].getColor()
// ==
//            //                             null)) coords[j - 1][i +
//            // 1].setFeasibility(true);

//            //                    if (j - 2 >= 0)
//            //                        if ((coords[j - 2][i].getHaveSlot()
// ==
//            //                             true) &&
//            //                            (coords[j - 2][i].getColor() ==
//            //                             null)) coords[j -
//            // 2][i].setFeasibility(true);

//            //                    if (j + 2 <= 24)
//            //                        if ((coords[j + 2][i].getHaveSlot()
// ==
//            //                             true) &&
//            //                            (coords[j + 2][i].getColor() ==
//            //                             null)) coords[j +
//            // 2][i].setFeasibility(true);

//            //                    if ((j - 1 >= 0) && (i - 1 >= 0))
//            //                        if ((coords[j - 1][i -
// 1].getHaveSlot() ==
//            //                             true) &&
//            //                            (coords[j - 1][i - 1].getColor()
// ==
//            //                             null)) coords[j - 1][i -
//            // 1].setHaveSlot(true);

//            //                    if ((j + 1 <= 24) && (i - 1 >= 0))
//            //                        if ((coords[j + 1][i -
// 1].getHaveSlot() ==
//            //                             true) &&
//            //                            (coords[j + 1][i - 1].getColor()
// ==
//            //                             null)) coords[j + 1][i -
//            // 1].setHaveSlot(true);
//            //                }

//            //                // 如果jumpTime不为0
//            //                //
//            // 如果在当前位置和某个位置之间有一个棋子,并且这条直线上没有其它棋子，并且这个位置是棋槽,并且这个位置没有棋子
//            //                if ((coords
//            //                     [(coords[j][i].coord_x +
//            // chosenCoord->coord_x) /
//            // 2]
//            //                     [(coords[j][i].coord_y +
//            // chosenCoord->coord_y) /
//            // 2]
//            //                     .getColor() != null)
//            //                    && ((coords[j][i].getHaveSlot() == true)
//            //                        && (coords[j][i].getColor() ==
// null)))
//            //                {
//            //                    cur_result++; // 当前结果总数+1

//            //                    // 就将这个位置的可行棋性设置为true
//            //                    coords[j][i].setFeasibility(true);

//            //                    // 通知调用绘制可行棋标志的函数
//            //                    //
//                    connect(&chosenCoord,
//            //                    //
//            //                            &Coord::updateFeasibility,
//            //                    //                            this,
//            //                    //
//            //                    //
//            //                          &Board::updateFeasibilityImag);
//            //                    //                    emit
//            // chosenCoord.updateFeasibility();

//            //                    // 储存进一步进行搜索的结果总数
//            //                    int next_result =
//            //                        updateFeasibility(&coords[j][i],
// jumpTime
//            // +
//            // 1);

//            //                    if (next_result == 0) continue;
//            //                }
//            //            }
//            //        }


//            // 等全部标记完毕再通知绘图函数
//            // UPDATE_FEASIBILITY = true;

//            // 等待深搜算法完成后，重新执行painteEvent
//            // update();
//            // return cur_result;
//        }

//        //

//        // remake:

//        for (int i = 0; i <= 24; i++)
//        {
//            for (int j = 0; j <= 16; j++)
//            {
//                // 设置边界条件：如果当前状况下
//            }
//        }
//    }

//    // 如果没有被选中的棋子，则抹除可行棋性
//    else
//    {
//        for (int i = 0; i <= 16; i++)
//        {
//            for (int j = 0; j <= 24; j++)
//            {
//                coords[j][i].setFeasibility(false);
//            }
//        }

//        // update();
//        return 0;
//    }
// }

// // 更新棋盘信息的子函数（更新棋盘的选中对象）(信息设置)
// // 用来设置该棋子是否被选中(如果原本是true,就改成false，如果原本是false，就改成true)
// void Board::updateChosenState(Coord& chosenCoord)
// {

//    // 这里一定要通过重新获取坐标的形式来进行设置，确保设置的是数组中的坐标
//    // 如果该位置的ChosenState的值为true，则改为false，如果为false，则改为true
//    coords[chosenCoord.coord_x + 12][chosenCoord.coord_y + 8]
//    .setChosenState
//        (!coords[chosenCoord.coord_x + 12][chosenCoord.coord_y + 8]
//        .getChosenState());

//    // 如果当前没有被选中的棋子，就让curdChosenCoord指向要被选中的棋子
//    if (curChosenCoord == NULL) curChosenCoord = &chosenCoord;

//    // 如果当前有被选中的棋子，就让curdChosenCoord指向空区域
//    else curChosenCoord = NULL;
// }
}

void Board::move(Coord& beginCoord,
                 Coord& endCoord)
{
    qDebug() << "enter move--------------------------------------------------------------------------";

    coords[endCoord.coord_x + 12][endCoord.coord_y + 8].setColor(coords[beginCoord.coord_x + 12][beginCoord.coord_y + 8].getColor());
    coords[beginCoord.coord_x + 12][beginCoord.coord_y + 8].setColor(null);
}

//next函数用来转移操作权和当前操作用户
void Board::next()
{
    curUser->jumpTime = 0;

    qDebug() << "Enter next";

    // 要将当前用户的轨迹置为全部是nullCoord
    curUser->initMoveTrack();

    if (curUser->id == 6)
    {
        curUser = &users[1];    // 防越界处理（移到第一个成员）
    }
    else
    {
        curUser++;                              // curUser指向下一个User
    }

    // qDebug() << "the current user is" << curUser->id;

    // 如果当前指向的颜色为unull（空）,则再调用一次该函数(系统自动调用next)或者这名用户已经被判胜或判负
    if (curUser->getUserColor() == unull || curUser->isFail == true || curUser->isWin == true)
    {
        next();
        // qDebug() << "Because null next";
    }

    // 等到所有的next执行完毕后执行update
    update();
}

void Board::choose(Coord& chosenCoord)
{
    qDebug() << "enter choose!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!";
    curChosenCoord = chosenCoord;
    coords[curChosenCoord.coord_x + 12][curChosenCoord.coord_y + 8].setChosenState(true);
}

void Board::search()
{
    // 如果是当前用户的第一次跳跃
    if (curUser->jumpTime == 0)
    {
        // qDebug() << "enter jumpTime = 0 jump";

        //相邻跳跃可行性
        //向六个方向相邻位置搜索可行位置，如果上面没有棋子并且有棋槽，就可以跳往
        //-1,+1
        if (curChosenCoord.coord_x >= -11 && curChosenCoord.coord_y <= 7 && (coords[curChosenCoord.coord_x + 12 - 1][curChosenCoord.coord_y + 8 + 1].getColor() == null)
                && (coords[curChosenCoord.coord_x + 12 - 1][curChosenCoord.coord_y + 8 + 1].getHaveSlot() == true))
        {
            coords[curChosenCoord.coord_x + 12 - 1][curChosenCoord.coord_y + 8 + 1].setFeasibility(true);
//            qDebug() << "feasibility: " << coords[curChosenCoord.coord_x + 12 - 1][curChosenCoord.coord_y + 8 + 1].coord_x
//                     << " " << coords[curChosenCoord.coord_x + 12 - 1][curChosenCoord.coord_y + 8 + 1].coord_y;
        }

        //-2 0
        if (curChosenCoord.coord_x >= -10 && (coords[curChosenCoord.coord_x + 12 - 2][curChosenCoord.coord_y + 8].getColor() == null)
                && (coords[curChosenCoord.coord_x - 2 + 12][curChosenCoord.coord_y + 8].getHaveSlot() == true))
        {
            coords[curChosenCoord.coord_x + 12 - 2][curChosenCoord.coord_y + 8].setFeasibility(true);
//            qDebug() << "feasibility: " << coords[curChosenCoord.coord_x + 12 - 2][curChosenCoord.coord_y + 8].coord_x
//                     << " " << coords[curChosenCoord.coord_x + 12 - 2][curChosenCoord.coord_y + 8].coord_y;
        }

        //-1 -1
        if (curChosenCoord.coord_x >= -11 && curChosenCoord.coord_y >= -7 && (coords[curChosenCoord.coord_x + 12 - 1][curChosenCoord.coord_y + 8 - 1].getColor() == null)
                && (coords[curChosenCoord.coord_x + 12 - 1][curChosenCoord.coord_y + 8 - 1].getHaveSlot() == true))
        {
            coords[curChosenCoord.coord_x + 12 - 1][curChosenCoord.coord_y + 8 - 1].setFeasibility(true);
//            qDebug() << "feasibility: " << coords[curChosenCoord.coord_x + 12 - 1][curChosenCoord.coord_y + 8 - 1].coord_x
//                     << " " << coords[curChosenCoord.coord_x + 12 - 1][curChosenCoord.coord_y + 8 - 1].coord_y;
        }

        //1 1
        if (curChosenCoord.coord_x <= 11 && curChosenCoord.coord_y <= 7 && (coords[curChosenCoord.coord_x + 12 + 1][curChosenCoord.coord_y + 8 + 1].getColor() == null)
                && (coords[curChosenCoord.coord_x + 12 + 1][curChosenCoord.coord_y + 8 + 1].getHaveSlot() == true))
        {
            coords[curChosenCoord.coord_x + 12 + 1][curChosenCoord.coord_y + 8 + 1].setFeasibility(true);
//            qDebug() << "feasibility: " << coords[curChosenCoord.coord_x + 12 + 1][curChosenCoord.coord_y + 8 + 1].coord_x
//                     << " " << coords[curChosenCoord.coord_x + 12 + 1][curChosenCoord.coord_y + 8 + 1].coord_y;
        }

        //2 0
        if (curChosenCoord.coord_x <= 10 && (coords[curChosenCoord.coord_x + 2 + 12][curChosenCoord.coord_y + 8].getColor() == null)
                && (coords[curChosenCoord.coord_x + 2 + 12][curChosenCoord.coord_y + 8].getHaveSlot() == true))
        {
            coords[curChosenCoord.coord_x + 2 + 12][curChosenCoord.coord_y + 8].setFeasibility(true);
//            qDebug() << "feasibility: " << coords[curChosenCoord.coord_x + 2 + 12][curChosenCoord.coord_y + 8].coord_x
//                     << " " << coords[curChosenCoord.coord_x + 2 + 12][curChosenCoord.coord_y + 8].coord_y;
        }

        // 1 -1
        if (curChosenCoord.coord_x <= 11 && curChosenCoord.coord_y >= -7 && (coords[curChosenCoord.coord_x + 1 + 12][curChosenCoord.coord_y - 1 + 8].getColor() == null)
                && (coords[curChosenCoord.coord_x + 1 + 12][curChosenCoord.coord_y - 1 + 8].getHaveSlot() == true))
        {
            coords[curChosenCoord.coord_x + 1 + 12][curChosenCoord.coord_y - 1 + 8].setFeasibility(true);
//            qDebug() << "feasibility: " << coords[curChosenCoord.coord_x + 1 + 12][curChosenCoord.coord_y - 1 + 8].coord_x
//                     << " " << coords[curChosenCoord.coord_x + 1 + 12][curChosenCoord.coord_y - 1 + 8].coord_y;
        }

        //间隔跳跃可行性
        //-1 1
        if (curChosenCoord.coord_x >= -10 && curChosenCoord.coord_y <= 6 &&
                (coords[curChosenCoord.coord_x - 1 + 12][curChosenCoord.coord_y + 1 + 8].getColor() != null)
                && (coords[curChosenCoord.coord_x - 1 + 12][curChosenCoord.coord_y + 1 + 8].getHaveSlot() == true))
        {
            if ( (coords[curChosenCoord.coord_x - 2 + 12][curChosenCoord.coord_y + 2 + 8].getColor() == null)
                    && (coords[curChosenCoord.coord_x - 2 + 12][curChosenCoord.coord_y + 2 + 8].getHaveSlot() == true))
            {
                coords[curChosenCoord.coord_x - 2 + 12][curChosenCoord.coord_y + 2 + 8].setFeasibility(true);
            }
        }

        if (curChosenCoord.coord_x >= -8 && (coords[curChosenCoord.coord_x - 2 + 12][curChosenCoord.coord_y + 8].getColor() != null)
                && (coords[curChosenCoord.coord_x - 2 + 12][curChosenCoord.coord_y + 8].getHaveSlot() == true))
        {
            if ( (coords[curChosenCoord.coord_x - 4 + 12][curChosenCoord.coord_y + 8].getColor() == null)
                    && (coords[curChosenCoord.coord_x - 4 + 12][curChosenCoord.coord_y + 8].getHaveSlot() == true))
            {
                coords[curChosenCoord.coord_x - 4 + 12][curChosenCoord.coord_y + 8].setFeasibility(true);
            }
        }

        if (curChosenCoord.coord_x >= -10 && curChosenCoord.coord_y >= -6 &&
                (coords[curChosenCoord.coord_x - 1 + 12][curChosenCoord.coord_y - 1 + 8].getColor() != null)
                && (coords[curChosenCoord.coord_x - 1 + 12][curChosenCoord.coord_y - 1 + 8].getHaveSlot() == true))
        {
            if ( (coords[curChosenCoord.coord_x - 2 + 12][curChosenCoord.coord_y - 2 + 8].getColor() == null)
                    && (coords[curChosenCoord.coord_x - 2 + 12][curChosenCoord.coord_y - 2 + 8].getHaveSlot() == true))
            {
                coords[curChosenCoord.coord_x - 2 + 12][curChosenCoord.coord_y - 2 + 8].setFeasibility(true);
            }
        }

        if (curChosenCoord.coord_x <= 10 && curChosenCoord.coord_y <= 6 &&
                (coords[curChosenCoord.coord_x + 1 + 12][curChosenCoord.coord_y + 1 + 8].getColor() != null)
                && (coords[curChosenCoord.coord_x + 1 + 12][curChosenCoord.coord_y + 1 + 8].getHaveSlot() == true))
        {
            if ((coords[curChosenCoord.coord_x + 2 + 12][curChosenCoord.coord_y + 2 + 8].getColor() == null)
                    && (coords[curChosenCoord.coord_x + 2 + 12][curChosenCoord.coord_y + 2 + 8].getHaveSlot() == true))
            {
                coords[curChosenCoord.coord_x + 2 + 12][curChosenCoord.coord_y + 2 + 8].setFeasibility(true);
            }
        }


        if (curChosenCoord.coord_x <= 8 && (coords[curChosenCoord.coord_x + 2 + 12][curChosenCoord.coord_y + 8].getColor() != null)
                && (coords[curChosenCoord.coord_x + 2 + 12][curChosenCoord.coord_y + 8].getHaveSlot() == true))
        {
            if ( (coords[curChosenCoord.coord_x + 4 + 12][curChosenCoord.coord_y + 8].getColor() == null)
                    && (coords[curChosenCoord.coord_x + 4 + 12][curChosenCoord.coord_y + 8].getHaveSlot() == true))
            {
                coords[curChosenCoord.coord_x + 4 + 12][curChosenCoord.coord_y + 8].setFeasibility(true);
            }
        }


        if (curChosenCoord.coord_x <= 10 && curChosenCoord.coord_y >= -6 &&
                (coords[curChosenCoord.coord_x + 1 + 12][curChosenCoord.coord_y - 1 + 8].getColor() != null)
                && (coords[curChosenCoord.coord_x + 1 + 12][curChosenCoord.coord_y - 1 + 8].getHaveSlot() == true))
        {
            if ( (coords[curChosenCoord.coord_x + 2 + 12][curChosenCoord.coord_y - 2 + 8].getColor() == null)
                    && (coords[curChosenCoord.coord_x + 2 + 12][curChosenCoord.coord_y - 2 + 8].getHaveSlot() == true))
            {
                coords[curChosenCoord.coord_x + 2 + 12][curChosenCoord.coord_y - 2 + 8].setFeasibility(true);
            }
        }
    }

    //用户在此步搜索之前已经完成了一次跳跃
    if (curUser -> jumpTime != 0)
    {
        // qDebug() << "enter jumpTime != 0 senarch";

        //间隔跳跃可行性
        //-1 1
        if (curChosenCoord.coord_x >= -10 && curChosenCoord.coord_y <= 6 &&
                (coords[curChosenCoord.coord_x - 1 + 12][curChosenCoord.coord_y + 1 + 8].getColor() != null)
                && (coords[curChosenCoord.coord_x - 1 + 12][curChosenCoord.coord_y + 1 + 8].getHaveSlot() == true))
        {
            if ( (coords[curChosenCoord.coord_x - 2 + 12][curChosenCoord.coord_y + 2 + 8].getColor() == null)
                    && (coords[curChosenCoord.coord_x - 2 + 12][curChosenCoord.coord_y + 2 + 8].getHaveSlot() == true))
            {
                coords[curChosenCoord.coord_x - 2 + 12][curChosenCoord.coord_y + 2 + 8].setFeasibility(true);
            }
        }

        if (curChosenCoord.coord_x >= -8 && (coords[curChosenCoord.coord_x - 2 + 12][curChosenCoord.coord_y + 8].getColor() != null)
                && (coords[curChosenCoord.coord_x - 2 + 12][curChosenCoord.coord_y + 8].getHaveSlot() == true))
        {
            if ( (coords[curChosenCoord.coord_x - 4 + 12][curChosenCoord.coord_y + 8].getColor() == null)
                    && (coords[curChosenCoord.coord_x - 4 + 12][curChosenCoord.coord_y + 8].getHaveSlot() == true))
            {
                coords[curChosenCoord.coord_x - 4 + 12][curChosenCoord.coord_y + 8].setFeasibility(true);
            }
        }

        if (curChosenCoord.coord_x >= -10 && curChosenCoord.coord_y >= -6 &&
                (coords[curChosenCoord.coord_x - 1 + 12][curChosenCoord.coord_y - 1 + 8].getColor() != null)
                && (coords[curChosenCoord.coord_x - 1 + 12][curChosenCoord.coord_y - 1 + 8].getHaveSlot() == true))
        {
            if ( (coords[curChosenCoord.coord_x - 2 + 12][curChosenCoord.coord_y - 2 + 8].getColor() == null)
                    && (coords[curChosenCoord.coord_x - 2 + 12][curChosenCoord.coord_y - 2 + 8].getHaveSlot() == true))
            {
                coords[curChosenCoord.coord_x - 2 + 12][curChosenCoord.coord_y - 2 + 8].setFeasibility(true);
            }
        }

        if (curChosenCoord.coord_x <= 10 && curChosenCoord.coord_y <= 6 &&
                (coords[curChosenCoord.coord_x + 1 + 12][curChosenCoord.coord_y + 1 + 8].getColor() != null)
                && (coords[curChosenCoord.coord_x + 1 + 12][curChosenCoord.coord_y + 1 + 8].getHaveSlot() == true))
        {
            if ((coords[curChosenCoord.coord_x + 2 + 12][curChosenCoord.coord_y + 2 + 8].getColor() == null)
                    && (coords[curChosenCoord.coord_x + 2 + 12][curChosenCoord.coord_y + 2 + 8].getHaveSlot() == true))
            {
                coords[curChosenCoord.coord_x + 2 + 12][curChosenCoord.coord_y + 2 + 8].setFeasibility(true);
            }
        }


        if (curChosenCoord.coord_x <= 8 && (coords[curChosenCoord.coord_x + 2 + 12][curChosenCoord.coord_y + 8].getColor() != null)
                && (coords[curChosenCoord.coord_x + 2 + 12][curChosenCoord.coord_y + 8].getHaveSlot() == true))
        {
            if ( (coords[curChosenCoord.coord_x + 4 + 12][curChosenCoord.coord_y + 8].getColor() == null)
                    && (coords[curChosenCoord.coord_x + 4 + 12][curChosenCoord.coord_y + 8].getHaveSlot() == true))
            {
                coords[curChosenCoord.coord_x + 4 + 12][curChosenCoord.coord_y + 8].setFeasibility(true);
            }
        }


        if (curChosenCoord.coord_x <= 10 && curChosenCoord.coord_y >= -6 &&
                (coords[curChosenCoord.coord_x + 1 + 12][curChosenCoord.coord_y - 1 + 8].getColor() != null)
                && (coords[curChosenCoord.coord_x + 1 + 12][curChosenCoord.coord_y - 1 + 8].getHaveSlot() == true))
        {
            if ( (coords[curChosenCoord.coord_x + 2 + 12][curChosenCoord.coord_y - 2 + 8].getColor() == null)
                    && (coords[curChosenCoord.coord_x + 2 + 12][curChosenCoord.coord_y - 2 + 8].getHaveSlot() == true))
            {
                coords[curChosenCoord.coord_x + 2 + 12][curChosenCoord.coord_y - 2 + 8].setFeasibility(true);
            }
        }
    }
}

void Board::cancelChoose()
{
    // qDebug() << "cancel!!!!!!!!!!!!!!!!";
    coords[curChosenCoord.coord_x + 12][curChosenCoord.coord_y + 8].setChosenState(false);
    curChosenCoord = NOTCHOSEN;
}

void Board::cancelFeasibility()
{
    for (int i = 0; i <= 24; i++)
    {
        for (int j = 0; j <= 16; j++)
        {
            coords[i][j].setFeasibility(false);
        }
    }
}

// -------------------------------------------棋盘绘画相关------------------------------------------------------
void Board::paintEvent(QPaintEvent *event)
{
    // entertime++;
    // qDebug() << "enter paintEvent " << entertime;
    // 为类Board指定一个画家
    // 注：该对象只能在paintEvent中定义
    QPainter painter(this);
    // 调用函数设置背景
    setDeskBackground(painter);

    // 绘制棋盘布局
    drawBoard(painter);

    // 初始化一局游戏
    if (INIT_GAME)
    {
        // qDebug() << "Enter initGame";

        this->initGame(painter);
    }

    if (USER_HAS_BEEN_INIT)
    {
        //qDebug() << "hhhhhh";
        nextImag(painter);
    }

    // 只在第一次进行一个初始的next，其余根据curUser的情况由nextImag自行判断处理

    // 绘制当前棋盘的状态
    // updateBoardImag(painter);

    this->updateChosenStateImag(painter);
    //qDebug() << "enter UpdateChosenStateImag";

    this->updatePieceColorImag(painter);
    //qDebug() << "D6enter UpdateChosenImag";

    this->updateFeasibilityImag(painter);
    //qDebug() << "enter updatePieceColorImag";
}

void Board::setDeskBackground(QPainter& painter)
{
    // 引入图像，从（0 ，0）处铺开，并且宽度和高度都设置为窗口大小
    // 在参数中构造并引入desk图片
    painter.drawPixmap(0, 0,
                       this->width(),
                       this->height(),
                       QPixmap(DESK));
}

void initMode() {}

// 初始化一局游戏的所有棋子图像
void Board::initGame(QPainter& painter)
{
    // 重新绘制所有的棋子,并确定所有坐标的目标颜色
    qDebug() << "enter initGame";
    //qDebug() << "2\n";
    for (int i = 0; i <= 3; i++)
    {
        for (int j = 0; j <= 3 - i; j++)
        {
            // 黄色(一定有)
            painter.setBrush(Qt::yellow);
            painter.drawEllipse(Coord(-3 + i + 2 * j, 5 + i).coord2QPoint(),
                                PIECER,
                                PIECER);
            coords[-3 + i + 2 * j + 12][5 + i + 8].setColor(yellow);
            coords[-3 + i + 2 * j + 12][5 + i + 8].targetColor = yellow;

            // 测试代码
            //            painter.drawText(
            //                coords[-3 + i + 2 * j + 12][5 + i +
            // 8].coord2QPoint(),
            //                "yellow");
            // 蓝色（3人模式没有）
            if ((userCnt == 6) || (userCnt == 2))
            {
                painter.setBrush(Qt::blue);
                painter.drawEllipse(Coord(-3  + i + 2 * j, -5 - i).coord2QPoint(),
                                    PIECER,
                                    PIECER);
                coords[-3  + i + 2 * j + 12][-5 - i + 8].setColor(blue);
                coords[-3  + i + 2 * j + 12][-5 - i + 8].targetColor = blue;
            }

            // 测试代码
            //            painter.drawText(
            //                coords[-3  + i + 2 * j + 12][-5 - i +
            // 8].coord2QPoint(),
            //                "blue");
            // 红色(2人模式和3人模式都没有)
            if (userCnt == 6)
            {
                painter.setBrush(Qt::red);
                painter.drawEllipse(Coord(6 + 2 * i + j, 4 - j).coord2QPoint(),
                                    PIECER,
                                    PIECER);
                coords[6 + 2 * i + j + 12][4 - j + 8].setColor(red);
                coords[6 + 2 * i + j + 12][4 - j + 8].targetColor = red;
            }

            // 测试代码
            //            painter.drawText(coords[6 + 2 * i + j + 12][4 - j +
            // 8].coord2QPoint(),
            //                             "red");
            // 青色(2人模式没有)
            if ((userCnt == 3) || (userCnt == 6))
            {
                painter.setBrush(Qt::cyan);
                painter.drawEllipse(Coord(-6 - 2 * i - j, -4 + j).coord2QPoint(),
                                    PIECER,
                                    PIECER);
                coords[-6 - 2 * i - j + 12][-4 + j + 8].setColor(cyan);
                coords[-6 - 2 * i - j + 12][-4 + j + 8].targetColor = cyan;
            }

            // 测试代码
            //            painter.drawText(
            //                coords[-6 - 2 * i - j + 12][-4 + j +
            // 8].coord2QPoint(),
            //                "cyan");
            // 紫色(2人模式没有)
            if ((userCnt == 3) || (userCnt == 6))
            {
                painter.setBrush(Qt::magenta);
                painter.drawEllipse(Coord(6 + 2 * i + j,  -4 + j).coord2QPoint(),
                                    PIECER,
                                    PIECER);
                coords[6 + 2 * i + j + 12][-4 + j + 8].setColor(purple);
                coords[6 + 2 * i + j + 12][-4 + j + 8].targetColor = purple;
            }

            // 测试代码
            //            painter.drawText(
            //                coords[6 + 2 * i + j + 12][-4 + j +
            // 8].coord2QPoint(),
            //                "purple");
            // 绿色(2人和3人模式没有)
            if (userCnt == 6)
            {
                painter.setBrush(Qt::green);
                painter.drawEllipse(Coord(-6 - 2 * i - j, 4 - j).coord2QPoint(),
                                    PIECER,
                                    PIECER);
                coords[-6 - 2 * i - j + 12][4 - j + 8].setColor(green);
                coords[-6 - 2 * i - j + 12][4 - j + 8].targetColor = green;
            }

            // 测试代码
            //            painter.drawText(
            //                coords[-6 - 2 * i - j + 12][4 - j +
            // 8].coord2QPoint(),
            //                "green");
        }
    }

    INIT_GAME = false;
}

// 绘制棋盘布局的函数
void Board::drawBoard(QPainter& painter)
{
    // qDebug() << "enter drawBoard";
    // 设置棋盘的背景--------------------------------------------------------------------
    // 反走样
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(Qt::white);

    // 给这个六边形插入背景
    painter.setBrush(QBrush(QPixmap(BOARD)));

    // 设置六边形的各个点的坐标
    static const QPoint points1[6] =
    {
        QPoint(366, 35),   QPoint(60,   215),
        QPoint(60,  535),  QPoint(366,  715),
        QPoint(672, 535),  QPoint(672,  215)
    };

    // 让画家完成这个六边形的绘画
    painter.drawPolygon(points1, 6);

    // 绘制棋盘上的其它图形区域，并配置以不一样的颜色---------------------------------------------------
    painter.setPen(Qt::black);

    // 设置黄色三角形的三个点
    static const QPoint points2[3] =
    {
        Coord(0, 8).coord2QPoint(), Coord(-3, 5).coord2QPoint(),
        Coord(3, 5).coord2QPoint()
    };
    //设置蓝色三角形的三个点
    static const QPoint points3[3] =
    {
        Coord(0, -8).coord2QPoint(), Coord(-3, -5).coord2QPoint(),
        Coord(3, -5).coord2QPoint()
    };

    // 完成对黄色三角形的绘画
    QColor color1(Qt::yellow);
    color1.setAlpha(ALPHA);
    painter.setBrush(color1);
    painter.drawPolygon(points2, 3);

    //完成对蓝色三角形的绘画
    QColor color2(Qt::blue);
    color2.setAlpha(4);
    painter.setBrush(Qt::blue);
    painter.drawPolygon(points3, 3);

    // 设置红色三角形的三个点
    static const QPoint points4[3] =
    {
        Coord(6, 4).coord2QPoint(), Coord(12, 4).coord2QPoint(),
        Coord(9, 1).coord2QPoint()
    };

    //设置青色三角形的三个点
    static const QPoint points5[3] =
    {
        Coord(-6, -4).coord2QPoint(), Coord(-12, -4).coord2QPoint(),
        Coord(-9, -1).coord2QPoint()
    };

    // 完成对红色三角形的绘画
    QColor color3(Qt::red);
    color3.setAlpha(ALPHA);
    painter.setBrush(color3);
    painter.drawPolygon(points4, 3);

    //完成对青色三角形的绘画
    QColor color4(Qt::cyan);
    color4.setAlpha(ALPHA);
    painter.setBrush(color4);
    painter.drawPolygon(points5, 3);

    // 设置紫色三角形的三个点
    static const QPoint points6[3] =
    {
        Coord(6, -4).coord2QPoint(), Coord(12, -4).coord2QPoint(),
        Coord(9, -1).coord2QPoint()
    };

    //设置绿色三角形的三个点
    static const QPoint points7[3] =
    {
        Coord(-6, 4).coord2QPoint(), Coord(-12, 4).coord2QPoint(),
        Coord(-9, 1).coord2QPoint()
    };

    // 完成对紫色三角形的绘画
    QColor color5(Qt::magenta);
    color5.setAlpha(ALPHA);
    painter.setBrush(color5);
    painter.drawPolygon(points6, 3);

    //完成对绿色三角形的绘画
    QColor color6(Qt::green);
    color6.setAlpha(ALPHA);
    painter.setBrush(color6);
    painter.drawPolygon(points7, 3);

    // 完成对六个白色梯形的绘画
    painter.setBrush(Qt::white);
    static const QPoint points8[4] =
    {
        Coord(-4, 4).coord2QPoint(),  Coord(-3,  5).coord2QPoint(),
        Coord(3,  5).coord2QPoint(),  Coord(4,   4).coord2QPoint()
    };
    static const QPoint points9[4] =
    {
        Coord(4, 4).coord2QPoint(), Coord(6, 4).coord2QPoint(),
        Coord(9, 1).coord2QPoint(), Coord(8, 0).coord2QPoint()
    };
    static const QPoint points10[4] =
    {
        Coord(4, -4).coord2QPoint(), Coord(6, -4).coord2QPoint(),
        Coord(9, -1).coord2QPoint(), Coord(8, 0).coord2QPoint()
    };
    static const QPoint points11[4] =
    {
        Coord(4,  -4).coord2QPoint(),  Coord(3,   -5).coord2QPoint(),
        Coord(-3, -5).coord2QPoint(),  Coord(-4,  -4).coord2QPoint()
    };
    static const QPoint points12[4] =
    {
        Coord(-8, 0).coord2QPoint(),  Coord(-4,  -4).coord2QPoint(),
        Coord(-6, -4).coord2QPoint(), Coord(-9,  -1).coord2QPoint()
    };
    static const QPoint points13[4] =
    {
        Coord(-8, 0).coord2QPoint(), Coord(-4, 4).coord2QPoint(),
        Coord(-6, 4).coord2QPoint(), Coord(-9, 1).coord2QPoint()
    };
    painter.drawPolygon(points8,  4);
    painter.drawPolygon(points9,  4);
    painter.drawPolygon(points10, 4);
    painter.drawPolygon(points11, 4);
    painter.drawPolygon(points12, 4);
    painter.drawPolygon(points13, 4);

    //完成中间部分的绘制(根据角色的不同显示不同效果)
    QPoint points14[6] =
    {
        Coord(-4, 4).coord2QPoint(),
        Coord(-8, 0).coord2QPoint(),
        Coord(-4, -4).coord2QPoint(),
        Coord(4, -4).coord2QPoint(),
        Coord(8, 0).coord2QPoint(),
        Coord(4, 4).coord2QPoint()
    };

    if (curUser != NULL)
    {
        switch (curUser->getUserColor())
        {
            case uyellow:
                painter.setBrush(QBrush(QPixmap(YELLOW)));
                break;

            case ured:
                painter.setBrush(QBrush(QPixmap(RED)));
                break;

            case ublue:
                painter.setBrush(QBrush(QPixmap(BLUE)));
                break;

            case ugreen:
                painter.setBrush(QBrush(QPixmap(GREEN)));
                break;

            case ucyan:
                painter.setBrush(QBrush(QPixmap(CYAN)));
                break;

            case upurple:
                painter.setBrush(QBrush(QPixmap(PURPLE)));
                break;

            case unull:
                painter.setBrush(Qt::NoBrush);
        }
    }
    else
    {
        painter.setBrush(Qt::NoBrush);
    }

    painter.drawPolygon(points14, 6);

    // 完成线段的绘画
    QPen pen(Qt::black);

    for (int i = 0; i <= 4; i++)
    {
        // 中间部分水平线
        painter.drawLine(Coord(-8 - i, i).coord2QPoint(),
                         Coord(8 + i, i).coord2QPoint());
        painter.drawLine(Coord(-8 - i, -i).coord2QPoint(),
                         Coord(8 + i, -i).coord2QPoint());
        // 下方三角形水平线
        painter.drawLine(Coord(-4 + i, -4 - i).coord2QPoint(),
                         Coord(4 - i, -4 - i).coord2QPoint());
        // 上方三角形水平线
        painter.drawLine(Coord(-4 + i, 4 + i).coord2QPoint(),
                         Coord(4 - i, 4 + i).coord2QPoint());
        // 中间部分斜线 +x 方向
        painter.drawLine(Coord(-4 - 2 * i, -4).coord2QPoint(),
                         Coord(4 - i, 4 + i).coord2QPoint());
        painter.drawLine(Coord(-4 + i, -4 - i).coord2QPoint(),
                         Coord(4 + 2 * i, 4).coord2QPoint());
        // 中间部分斜线 -x方向
        painter.drawLine(Coord(-4 - 2 * i, 4).coord2QPoint(),
                         Coord(4 - i, -4 - i).coord2QPoint());
        painter.drawLine(Coord(-4 + i, 4 + i).coord2QPoint(),
                         Coord(4 + 2 * i, -4).coord2QPoint());
        // 右下三角形斜线 +x方向
        painter.drawLine(Coord(4 + 2 * i, -4).coord2QPoint(),
                         Coord(8 + i, 0 - i).coord2QPoint());
        // 左下三角形斜线 -x方向
        painter.drawLine(Coord(-4 - 2 * i, -4).coord2QPoint(),
                         Coord(-8 - i, 0 - i).coord2QPoint());
        // 右上三角形斜线 +x方向
        painter.drawLine(Coord(-8 - i, 0 + i).coord2QPoint(),
                         Coord(-4 - 2 * i, 4).coord2QPoint());
        // 左上三角形斜线 -x方向
        painter.drawLine(Coord(4 + 2 * i, 4).coord2QPoint(),
                         Coord(8 + i, 0 + i).coord2QPoint());
    }

    // 完成棋槽的绘制
    // 绘制棋槽要用特殊的线
    painter.setPen(pen);
    // 背景镂空处理
    painter.setBrush(QBrush(QPixmap(BOARD)));

    // 绘制所有的棋槽并设置当前位置是否有棋槽
    for (int i = 0; i <= 4; i++)
    {
        for (int j = -8 - i; j <= 8 + i; j += 2)
        {
            painter.drawEllipse(Coord(j, i).coord2QPoint(),
                                SLOTR, SLOTR);
            painter.drawEllipse(Coord(j, -i).coord2QPoint(),
                                SLOTR, SLOTR);
            coords[j + 12][i + 8].setHaveSlot(true);
            // 测试代码
            // painter.drawText(coords[j + 12][i + 8].coord2QPoint(),
            //                "slot");
            coords[j + 12][-i + 8].setHaveSlot(true);
            // 测试代码
            // painter.drawText(coords[j + 12][-i + 8].coord2QPoint(),
            //                "slot");
            // debug += 2;
            // qDebug() << debug;
        }

        for (int j = -i; j <= i; j += 2)
        {
            painter.drawEllipse(Coord(j, 8 - i).coord2QPoint(),  SLOTR,
                                SLOTR);
            painter.drawEllipse(Coord(j, -8 + i).coord2QPoint(), SLOTR,
                                SLOTR);
            coords[j + 12][8 - i + 8].setHaveSlot(true);
            // 测试代码
            // painter.drawText(coords[j + 12][8 - i + 8].coord2QPoint(),
            //                 "slot");
            coords[j + 12][-8 + i + 8].setHaveSlot(true);
            // 测试代码
            // painter.drawText(coords[j + 12][-8 + i + 8].coord2QPoint(),
            //                "slot");
            // debug += 2;
            // qDebug() << debug;
        }
    }
}

// 更新棋盘状态的子函数(更新棋子的颜色)
void Board::updatePieceColorImag(QPainter& painter)
{
    // 更新颜色信息,重绘所有棋子
    painter.setPen(Qt::black);

    for (int i = 0; i <= 16; i++)
    {
        for (int j = 0; j <= 24; j++)
        {
            if (coords[j][i].getHaveSlot() == true)
            {
                switch (coords[j][i].getColor())
                {
                    case red:
                        painter.setBrush(Qt::red);
                        painter.drawEllipse(coords[j][i].coord2QPoint(),
                                            PIECER,
                                            PIECER);
                        break;

                    case yellow:
                        painter.setBrush(Qt::yellow);
                        painter.drawEllipse(coords[j][i].coord2QPoint(),
                                            PIECER,
                                            PIECER);
                        break;

                    case blue:
                        painter.setBrush(Qt::blue);
                        painter.drawEllipse(coords[j][i].coord2QPoint(),
                                            PIECER,
                                            PIECER);
                        break;

                    case green:
                        painter.setBrush(Qt::green);
                        painter.drawEllipse(coords[j][i].coord2QPoint(),
                                            PIECER,
                                            PIECER);
                        break;

                    case cyan:
                        painter.setBrush(Qt::cyan);
                        painter.drawEllipse(coords[j][i].coord2QPoint(),
                                            PIECER,
                                            PIECER);
                        break;

                    case purple:
                        painter.setBrush(Qt::magenta);
                        painter.drawEllipse(coords[j][i].coord2QPoint(),
                                            PIECER,
                                            PIECER);
                        break;

                    case null:
                        painter.setBrush(QPixmap(BOARD));
                        painter.drawEllipse(coords[j][i].coord2QPoint(),
                                            SLOTR,
                                            SLOTR);
                        break;
                }
            }
        }
    }

    //UPDATE_PIECECOLOR = false;
}

// 更新棋盘绘图的子函数（更新棋盘的可执行性）
void Board::updateFeasibilityImag(QPainter& painter)
{
    // 遍历所有的坐标，如果一个坐标具有行棋的可行性，就使用特殊标记进行标识
    for (int i = 0; i <= 16; i++)
    {
        for (int j = 0; j <= 24; j++)
        {
            if (coords[j][i].getFeasibility() == true)
            {
                // qDebug() << "draw updateFeasibility" << coords[j][i].coord_x << " " << coords[j][i].coord_y;
                // qDebug() << "draw updateFeasibility point" << coords[j][i].coord2QPoint().x() << " " << coords[j][i].coord2QPoint().y();
                painter.setPen(QColor(169, 169, 169));
                painter.setBrush(QColor(169, 169, 169));
                painter.drawEllipse(coords[j][i].coord2QPoint(),
                                    FEASIBILITYR,
                                    FEASIBILITYR);
            }
        }
    }

    //UPDATE_FEASIBILITY = false;
}

// 更新棋盘绘图的子函数（更新棋盘的选中对象）
void Board::updateChosenStateImag(QPainter& painter)
{
    for (int i = 0; i <= 16; i++)
    {
        for (int j = 0; j <= 24; j++)
        {
            if (coords[j][i].getChosenState() == true)
            {
                // qDebug() << "update this Coord's Chosen:" <<
                // coords[j][i].coord_x << " " << coords[j][i].coord_y;
                // qDebug() << "draw updateChosenStateImag!";
                painter.setBrush(Qt::NoBrush);
                QPen pen(Qt::black, Qt::DashLine);
                painter.setPen(pen);
                // This F bug spends me an whole afternoon!!!
                painter.drawEllipse(coords[j][i].coord2QPoint(),
                                    CHOSENSTATER,
                                    CHOSENSTATER);
            }
        }
    }

    //UPDATE_CHOSENSTATE = false;
}

// 更新下一步按钮的画面
void Board::nextImag(QPainter& painter)
{
    // 将画笔设置为白色
    //qDebug() << "Enter nextImag";

    painter.setPen(Qt::black);

    // qDebug() << "2the User color is" << curUser->getUserColor();

    if (curUser->getUserColor() == uyellow)
    {
        // qDebug() << "Enter nextImag,the User color is yellow";
        painter.setBrush(QBrush(QPixmap(YELLOW)));
    }
    else if (curUser->getUserColor() == ured)
    {
        // qDebug() << "Enter nextImag,the User color is red";
        painter.setBrush(QBrush(QPixmap(RED)));
    }
    else if (curUser->getUserColor() == upurple)
    {
        // qDebug() << "Enter nextImag,the User color is purple";
        painter.setBrush(QBrush(QPixmap(PURPLE)));
    }
    else if (curUser->getUserColor() == ublue)
    {
        // qDebug() << "Enter nextImag,the User color is blue";
        painter.setBrush(QBrush(QPixmap(BLUE)));
    }
    else if (curUser->getUserColor() == ucyan)
    {
        // qDebug() << "Enter nextImag,the User color is cyan";
        painter.setBrush(QBrush(QPixmap(CYAN)));
    }
    else if (curUser->getUserColor() == ugreen)
    {
        // qDebug() << "Enter nextImag,the User color is green";
        painter.setBrush(QBrush(QPixmap(GREEN)));
    }
    else if (curUser->getUserColor() == unull)
    {
        // qDebug() << "Enter nextImag,the User color is white";
        painter.setBrush(QBrush(QPixmap(DESK)));    // 理论上这个白色是看不见的，因为next函数在遇到unull的情况就会立即跳过
    }

    QPoint points1[4] =
    {
        Coord(16, 6).coord2QPoint(), Coord(16, 4).coord2QPoint(),
        Coord(28, 4).coord2QPoint(), Coord(28, 6).coord2QPoint()
    };

    painter.drawPolygon(points1, 4);

    if (curUser->jumpTime == 0)
    {
        painter.setBrush(QBrush(QPixmap(DESK)));
        QPoint points2[3] =
        {
            Coord(16, 5).coord2QPoint(), Coord(16, 4).coord2QPoint(),
            Coord(18, 4).coord2QPoint()
        };

        painter.drawPolygon(points2, 3);
    }
}

// 更新全部内容：完全更新
void Board::updateBoardImag(QPainter& painter)
{
    this->updateBoardImag(painter);
    this->updateChosenStateImag(painter);
    this->updateFeasibilityImag(painter);
}

// 更新玩家列表图像
void Board::updataPlayerImag()
{
    /*TODO unsolves：
     * 绘制棋盘玩家列表
     */
}

//-----------------------------------------信息发送相关子函数------------------------------------------------

/**
 * @brief setExistedUserName 获取并设置在自己进入房间之前其它已经进入房间的用户的名称
 * 在使用这个函数之前一定要先使用setExistedUserReady函数，目的在于先行获取当前房间中的总人数（包括自己）
 */
void Board::setExistedUserName(QString data1)
{
    for (int i = 1; i <= userCntInRoom - 1; i++)  //userCntInRoom - 1的原因是发送过来的用户列表没有包括用户自己
    {
        playerList[i].userName = data1.section(" ", i - 1, i - 1);   //房间里有userCntInRoom个人，data1必然会被空格分成userCntInRoom段
    }
}

/**
 * @brief setExistedUserReady 设置其它用户的准备状态
 * 这里面包含了重要的功能：获取当前的总人数
 */
void Board::setExistedUserReady(QString data2)
{
    //data2中的字符数就是在自己进入房间之前的玩家总数
    //得到当前房间内的玩家总数
    userCntInRoom += data2.size();

    //获取当前房间里的总人数的准备状态
    for (int i = 1; i <= userCntInRoom - 1; i++)  //userCntInRoom - 1的原因是发送过来的用户列表没有包括用户自己
    {
        if (data2.mid(i - 1, 1) == "1")
        {
            playerList[i].isReady = true;
        }
    }
}

//-----------------------------------------------------------网络连接相关--------------------------------------------------

/**
 * @brief connected 输出连接成功信息
 */
void Board::connected()
{
    qDebug() << "successfully connect!";

    ask2->show();          //连接成功则直接进入房间

    showInfo->showSUCCESSFULLY_CONNECTE();  //显示连接成功的信息，用户点击OK后就关闭
}
/*
 * TODO solved(ywt:2022\4\30)
 * 完成临时显示成功信息的窗口的设计
 */

/**
 * @brief displayError 输出连接失败信息，并设计一个临时窗口展示给用户
 */
void Board::displayError()
{
    qDebug() << "oh NO, fail to connect...";
    emit client_receive_ERROR("FAIL_TO_CONNECT", "You can examine your network then click reconnect to try again.");
}
/*
 * TODO solved(ywt:2022\4\)
 * 完成临时显示错误信息的窗口的设计
 */

void Board::reConnect()
{
    showInfo->showCONNECTING();

    this->socket->hello("10.46.54.135", 16666);
}


//------------------------------------------信息发送相关-------------------------------------------------------

//槽函数receive，用来实现对NetworkData的接收和解析,这是一个非常长的函数,整合了各种函数和操作
void Board::receive(NetworkData data)
{
    // 错误处理操作符：在整个进程中随时监听
    if (data.op == OPCODE::ERROR_OP)
    {
        //将错误码发送给showinfo对象进行处理
        emit client_receive_ERROR(data.data1, data.data2);
    }

    // 处理服务端发送的加入房间结果的反馈
    // 收到这个消息代表已经加入成功
    if (data.op == OPCODE::JOIN_ROOM_REPLY_OP)
    {
        client_receive_JOIN_ROOM_REPLY(data);
    }

    if (data.op == OPCODE::JOIN_ROOM_OP)
    {
        client_receive_JOIN_ROOM(data);
    }

    if (data.op == OPCODE::LEAVE_ROOM_OP)
    {
        client_receive_LEAVE_ROOM(data);
    }

    if (data.op == OPCODE::PLAYER_READY_OP)
    {
        client_receive_PLAYER_READY(data);
    }

    if (data.op == OPCODE::START_GAME_OP)
    {
        client_receive_START_GAME(data);
    }

    if (data.op == OPCODE::START_TURN_OP)
    {
        client_receive_START_TURN(data);
    }

    if (data.op == OPCODE::MOVE_OP)
    {
        client_receive_MOVE(data);
    }

    if (data.op == OPCODE::END_TURN_OP)
    {
        client_receive_END_TURN(data);
    }

    if (data.op == OPCODE::END_GAME_OP)
    {
        client_receive_END_GAME(data);
    }
}

/**
 * @brief client_send_JOIN_ROOM 用来包装一些加入房间的相关操作
 */
void Board::client_send_JOIN_ROOM()
{
    //data1是用户名，data2是房间号
    QString data1 = roomNumber;    //此时me指向的是users[0]
    QString data2 = myName;

    NetworkData data(OPCODE::JOIN_ROOM_OP, data1, data2);

    this->socket->send(data);

    qDebug() << "send";
}


/**
 * @brief client_send_PLAYER_READY 当收到来自ask_join_room的getReady信号时，触发sendReady请求,发送用户名
 */
void Board::client_send_PLAYER_READY()
{
    //构造一个NetworkData类型的对象temp，data1表示当前玩家的用户名
    QString data1 = myName;
    //data2置空
    QString data2;
    NetworkData temp(OPCODE::PLAYER_READY_OP, data1, data2);

    //向服务器发送temp信息
    this->socket->send(temp);
}

/**
 * @brief client_receive_JOIN_ROOM_REPLY 获取并设置在自己进入房间之前其它已经进入房间的用户的名称和准备状态
 * 封装了下面两个函数setExistedUserName和setExistedUserReady
 * 获取了总人数，获取了自己在玩家列表中的位置（me不再是野指针）
 * 通知重绘ask2并允许ask2中Ready键的点击
 */
void Board::client_receive_JOIN_ROOM_REPLY(NetworkData data)
{
    //用户成功加入游戏，总人数+1
    userCntInRoom++;

    //调用setExistedUserName之前一定要先调用setExistedUserReady
    setExistedUserReady(data.data2);
    setExistedUserName(data.data1);

    //更改用户的信息
    playerList[userCntInRoom].userName = myName;
    playerList[userCntInRoom].isReady = false;

    //通知ask_join_room类的对象ask2根据信息重绘表格
    ask2->updateTable();

    //加入房间成功，允许玩家点击Ready键，并禁用其它三个键
    emit Ready_is_enabled(true);
    emit forbidButton();
}

void Board::client_receive_JOIN_ROOM(NetworkData data)
{
    //将新进入的玩家计入总人数
    userCntInRoom++;

    //让这名用户加到用户列表的最后
    playerList[userCntInRoom].userName = data.data1;

    playerList[userCntInRoom].isReady = false;

    //更新玩家列表
    ask2->updateTable();
}

void Board::client_receive_PLAYER_READY(NetworkData data)
{
    for (int i = 1; i <= 6; i++)
    {
        if (playerList[i].userName == data.data1)    //包括自己
        {
            playerList[i].isReady = true;
            break;  //因为没有重名者
        }
    }

    ask2->updateTable();
}

void Board::client_receive_START_GAME(NetworkData data)
{
    qDebug() << "client_receive_START_GAME";

    START_GAME = 1;                //鼠标事件生效

    ask2->setVisible(false);       //一旦开启游戏就将ask2窗口关闭，以免影响视角

    userCnt = userCntInRoom;       //玩家总人数就是当前房间中的人数
    initUsers();                   //确定了玩家人数就可以开始分配颜色了

    INIT_GAME = true;              //确定了玩家人数之后就可以开始绘制棋盘了
    USER_HAS_BEEN_INIT = true;     //nextImag图像也可以开始绘制了
    update();

    //玩家对号入座是客户端的安排，这里需要根据data的值来分配区域
    for (int i = 1; i <= userCntInRoom; i++)
    {
        if (data.data2.mid(i - 1, 1) == "A")
        {
            users[1].userName = data.data1.section(" ", i - 1, i - 1);
        }

        if (data.data2.mid(i - 1, 1) == "B")
        {
            users[2].userName = data.data1.section(" ", i - 1, i - 1);
        }

        if (data.data2.mid(i - 1, 1) == "C")
        {
            users[3].userName = data.data1.section(" ", i - 1, i - 1);
        }

        if (data.data2.mid(i - 1, 1) == "D")
        {
            users[4].userName = data.data1.section(" ", i - 1, i - 1);
        }

        if (data.data2.mid(i - 1, 1) == "E")
        {
            users[5].userName = data.data1.section(" ", i - 1, i - 1);
        }

        if (data.data2.mid(i - 1, 1) == "F")
        {
            users[6].userName = data.data1.section(" ", i - 1, i - 1);
        }
    }

    for (int i = 1; i <= 6; i++)
    {
        if (users[i].userName != "")
        {
            qDebug() << "Users[" << i << "]" << " is " << users[i].userName;
        }
    }

    // 初始化me指针
    for (int i = 1; i <= 6; i++)
    {
        if (users[i].userName == myName)
        {
            me = &users[i];
        }
    }

    //通知绘图函数paintEvent在桌面上重绘信息（重绘玩家列表）
    this->update();
}

void Board::client_receive_START_TURN(NetworkData data)
{
    qDebug() << "client_receive_START_TURN";

    //将倒计时设置为三十秒
    countdown->setSecs(TIME);
    //呈现倒计时
    countdown->setVisible(true);
    //倒计时开始
    countdown->start();
    //在接收到服务端发送的MOVE信号时结束倒计时（设置为10000秒）
}

void Board::client_send_MOVE()
{
    qDebug() << "client_send_MOVE";

    QString data1;
    QString data2;

    data1 = me->zoom;            //设置data1

    data2 = me->track2str();   //设置data2

    qDebug() << me->userName << "send his move track is" << data2;

    this->socket->send(NetworkData(OPCODE::MOVE_OP, data1, data2));   //发送信息
}

void Board::client_receive_MOVE(NetworkData data)
{
    qDebug() << "client_receive_MOVE";

    countdown->setVisible(false);   // 设置计时器不可见（关闭计时器）
    countdown->setSecs(10000);      // 强行设置计时器停止（方法很土）

    qDebug() << "the moveTrack of " << curUser->userName << " is" << data.data2;

    // 超时判负
    if (data.data2 == "-1")  //如过data2的内容是-1，说明当前用户超时了，直接将他的所有棋子清空
    {
        qDebug() << "clear the piece of " << curUser->userName << " because of timeOut";

        for (int i = 0; i <= 24; i++)
        {
            for (int j = 0; j <= 16; j++)
            {
                if (int(coords[i][j].getColor()) == int(curUser->getUserColor()))  //找出所有颜色和当前用户颜色相同的棋子
                {
                    coords[i][j].setColor(null);    //将颜色和当前用户颜色相同的棋子清空
                }
            }
        }

        //将当前用户的是否负状态设置为是，并且在paintEvent中加入相关的显示流程
        curUser->isFail = true;

        //如果当前玩家是最后一个玩家，退出超时判负的功能，不再进行next
        int overCnt = 0;
        for (int i = 1; i <= userCnt; i++)
        {
            if (users[i].isWin == true || users[i].isFail == true)
            {
                overCnt++;
            }
        }
        if (overCnt == userCnt)
        {
            return;    //如果是最后一个人，直接退出,不再
        }

        next();
        return;
    }

    // 其它玩家的图像更新
    if (me == curUser) //如果当前用户就是自己，则不必再复现移动步骤(因为我们的客户端已经自己移动了)
    {
        next();
        return;
    }
    else
    {
        //在其它玩家的回合，我们不需要关心移动前后的标志设置和撤销的相关设置，只需要move即可

        qDebug() << "move for " << curUser->userName;

        Coord* tempTrack = str2track(data.data2);

        //统计跳跃次数
        int tempJumpTime = 0;     //当前玩家的跳跃次数
        int spaceCnt = 0;         //字符串中的空格个数
        for (int i = 0; i <= data.data2.size() - 1; i++)
        {
            if (data.data2.mid(i, 1) == " ")
            {
                spaceCnt++;
            }
        }

        tempJumpTime = (spaceCnt - 1) / 2;

        qDebug() << "tempJumpTime of " << curUser->userName << tempJumpTime;

        qDebug() << "move for " << curUser->userName;

        for (int i = 1; i <= tempJumpTime; i++)
        {
            move(tempTrack[i], tempTrack[i + 1]);//1->2,2->3,3->4......
            /*TODO unsolved
             * 这里应该添加一个延迟函数
             */
            update();         //move一次就update一次，理想状态下这里应该设置延迟0.5ms
        }
        //下一个人
        next();    // 调用next函数，因为服务端同意了用户发送的请求，在客户端发生了curUser的变化，jumpTime的清零，nextImag图像的变化
    }
}

void Board::client_receive_END_TURN(NetworkData data)
{
    //me->isWin = true;
    curUser->isWin = true;

    /*TODO unsolved
     * 应该在paintEvent里面添加一个展示玩家胜利信息的绘图流程（可以在玩家列表里面添加）
     */
}

void Board::client_receive_END_GAME(NetworkData data)
{
    /*TODO solved
     *这里应该写一个界面来展示排名
     */
    ask2->showWINNER_LIST(data.data1);   // 显示玩家获胜列表

    ask2->show();                        // 展示ask2窗口

    this->close();                       // 这个是一个直接退出程序的简单粗暴的处理方式，要改进！！！
}

void Board::client_send_LEAVE_ROOM()
{
    this->socket->send(NetworkData(OPCODE::LEAVE_ROOM_OP, me->userName, roomNumber));
}

void Board::client_receive_LEAVE_ROOM(NetworkData data)    //client_receive_LEAVE_ROOM采用类链表操作
{
    for (int i = 1; i <= 6; i++)  //重新安排房间中剩下的所有用户的座位向前对齐
    {
        if (playerList[i].userName == data.data1)   //找出离开了房间的人在playerList中的什么位置
        {
            playerList[i].userName = "";    //将这名用户的姓名置空
            playerList[i].isReady = false; //将准备状态设置为未准备

            for (int j = i; j <= userCntInRoom - 1; j++)  //后一名用户直接顶上去
            {
                playerList[j].userName = playerList[j + 1].userName;
                playerList[j].isReady = playerList[j + 1].isReady;
            }

            playerList[userCntInRoom].userName = "";
            playerList[userCntInRoom].isReady = false;
        }
    }

    userCntInRoom--;   //这行代码一定要写在上面那些代码的后面，因为那些代码中内部的实现是基于原来的userCntInRoom
}

//服务端函数---------------------------------------------------------------------------------
void Board::receiveData(QTcpSocket * socket, NetworkData data)
{
    if (data.op == OPCODE::JOIN_ROOM_OP)
    {
        server_receive_JOIN_ROOM(socket, data);
    }

    if (data.op == OPCODE::PLAYER_READY_OP)
    {
        server_receive_PLATER_READY(socket, data);
    }

    if (data.op == OPCODE::LEAVE_ROOM_OP)
    {
        server_receive_LEAVE_ROOM(socket, data);
    }

    if (data.op == OPCODE::MOVE_OP)
    {
        server_receive_MOVE(socket, data);
    }

    /*TODO：写一个处理异常请求的分支*/
}

void Board::server_receive_JOIN_ROOM(QTcpSocket * socket, NetworkData data)
{
    //注：data1是房间号，data2是用户名

    //检查房间是否为满或者用户和房间里的玩家重名
    if (data.data1 == "")    //房间名为空
    {
        server_send_ERROR(socket, "Soory, please enter your room number");
        return;
    }
    if (userCntInRoom == 6)  //房间已满，如果正在游戏中也显示房间已满（data1表示房间号，写多线程的时候要用到） //这里本来应该写userCnt
    {
        server_send_ERROR(socket, "Sorry, the room is full");
        return;       //退出函数
    }

    if (data.data2 == "")    //人名为空
    {
        server_send_ERROR(socket, "Sorry, You are supposes to enter your name");
        return;   //退出函数
    }
    for (int i = 1; i <= 6; i++)  //人名重复检查
    {
        int repeatTime = -1;
        if (playerList[i].userName == data.data2)  //重名
        {
            repeatTime++;
        }
        if (repeatTime >= 0)
        {
            server_send_ERROR(socket, "Sorry, You have the same name as the user in the room");  //报错
            return;   //退出函数
        }
    }

    // 如果检查无误，向所有已经进入房间的玩家发送用户名
    for (int i = 1; i <= userCntInRoom; i++)
    {
        if (playerList[i].userName != "") //只要用户名非空就代表有人(这个判断其实不必要)
        {
            server_send_JOIN_ROOM(playerList[i].socket, data.data2); //data2表示用户名
        }
    }

    // 如果检查无误，向新加入用户发送玩家列表
    QString tempNameList;
    QString tempReadyList;

    for (int i = 1; i <= userCntInRoom ; i++)             //由于playerList采用类链表处理,所用用户位于前userCntInRoom个单位
    {
        tempNameList.append(playerList[i].userName);  //将玩家的名字写入玩家名字列表
        tempNameList.append(" ");                     //在玩家名字的中间加入一个空格
        tempReadyList.append(QString::number(int(playerList[i].isReady)));  //bool->int->QString,将玩家的准备状态写入准备列表
    }
    tempNameList.chop(1);   //最后一个玩家的后面不需要加入空格,故删掉
    server_send_JOIN_ROOM_REPLY(socket, tempNameList, tempReadyList);   //向这名用户发送用户列表

    // 执行在服务端系统中将玩家加入房间
    userCntInRoom++;
    playerList[userCntInRoom].userName = data.data2;   //存入名字，由于playerList采用类链表处理，故最后一个位置就是可以加入的位置
    playerList[userCntInRoom].socket = socket;
}

void Board::server_send_JOIN_ROOM_REPLY(QTcpSocket *socket, QString nameList, QString readyList)
{
    this->server->send(socket, NetworkData(OPCODE::JOIN_ROOM_REPLY_OP, nameList, readyList));
}

void Board::server_send_JOIN_ROOM(QTcpSocket *socket, QString userName)
{
    this->server->send(socket, NetworkData(OPCODE::JOIN_ROOM_OP, userName, ""));
}

void Board::server_receive_PLATER_READY(QTcpSocket * socket, NetworkData data)
{
    qDebug() << "receive ready from " << data.data1;

    // 设置并向所有用户转发这名用户的准备信息
    for (int i = 1; i <= userCntInRoom; i++)
    {
        if (playerList[i].userName == data.data1)   //找出是哪名玩家发出的准备信息
        {
            playerList[i].isReady = true;     //设置这名玩家已准备

            for (int j = 1; j <= userCntInRoom; j++)
            {
                server_send_PLAYER_READY(playerList[j].socket, playerList[i].userName);  //将玩家的信息发送给所有用户
            }

            break;                            //只有一名玩家，找到之后直接break
        }
    }

    // 如果发现所有用户均已准备，直接发送开始游戏请求，每次收到这一请求就检查一次
    int readycnt = 0;
    for (int i = 1; i <= userCntInRoom; i++ )   //进行检查
    {
        if (playerList[i].isReady)
        {
            readycnt++;
        }
    }
    if (readycnt == userCntInRoom)     //如果所有人都已经点击准备
    {
        userCnt = userCntInRoom;       //确定游戏人数

        // 服务端进行座位分配，将玩家对号入座（服务端的座位分配方式是固定的）
        if (userCnt == 2)
        {
            users[1].userName = playerList[1].userName;
            users[1].socket = playerList[1].socket;
            users[4].userName = playerList[2].userName;
            users[4].socket = playerList[2].socket;
        }
        if (userCnt == 3)
        {
            for (int i = 1; i <= 3; i++)
            {
                users[2 * i - 1].userName = playerList[i].userName;
                users[2 * i - 1].socket = playerList[i].socket;
            }
        }
        if (userCnt == 6)
        {
            for (int i = 1; i <= 6; i++)
            {
                users[i].userName = playerList[i].userName;
                users[i].socket = playerList[i].socket;
            }
        }

        //制作玩家名称列表和区域列表，发送给所有玩家
        QString tempNameList;
        QString tempZoomList;

        //制作玩家的名称列表
        for (int i = 1; i <= userCntInRoom ; i++)             //由于playerList采用类链表处理,所用用户位于前userCntInRoom个单位
        {
            tempNameList.append(playerList[i].userName);  //将玩家的名字写入玩家名字列表,玩家列表的制作方法就是按顺序填入
            tempNameList.append(" ");                     //在玩家名字的中间加入一个空格
        }

        tempNameList.chop(1);   //最后一个玩家的后面不需要加入空格,故删掉

        //制作玩家的区域列表
        switch (userCnt)
        {
            case 2:
                tempZoomList = "AD";
                break;

            case 3:
                tempZoomList = "ACE";
                break;

            case 6:
                tempZoomList = "ABCDEF";
                break;

            default:
                break;
        }

        //将这两个列表发送给所有玩家并通知游戏开始
        for (int i = 1; i <= userCnt; i++)
        {
            server_send_START_GAME(playerList[i].socket, tempNameList, tempZoomList);
        }

        //以下是为了让服务端方便呈现画面信息的语块，它们初始化了整个行棋界面
        initUsers();                   //确定了玩家人数就可以开始分配颜色
        INIT_GAME = true;              //确定了玩家人数之后就可以开始绘制棋盘
        USER_HAS_BEEN_INIT = true;     //nextImag图像也可以开始绘制
        update();

        //告知所有用户第一个玩家应该开始下棋
        for (int i = 1; i <= userCnt; i++)
        {
            server_send_START_TURN(playerList[i].socket);
        }

        //在服务端进行倒计时
        countdown->setSecs(TIME);//将倒计时设置为三十秒
        countdown->setVisible(true);//呈现倒计时
        countdown->start();//倒计时开始
    }
}

void Board::server_send_PLAYER_READY(QTcpSocket * socket, QString userName)
{
    this->server->send(socket, NetworkData(OPCODE::PLAYER_READY_OP, userName, ""));
}

void Board::server_receive_LEAVE_ROOM(QTcpSocket * socket, NetworkData data)
{
    //先在服务端系统将这名用户移除玩家列表（采用类链表模式操作）
    for (int i = 1; i <= 6; i++)  //重新安排房间中剩下的所有用户的座位向前对齐
    {
        if (playerList[i].userName == data.data1)   //找出离开了房间的人在playerList中的什么位置
        {
            playerList[i].userName = "";    //将这名用户的姓名置空
            playerList[i].socket = NULL;    //将这名用户的指针置空
            playerList[i].isReady = false; //将准备状态设置为未准备

            for (int j = i; j <= userCntInRoom - 1; j++)  //后一名用户直接顶上去
            {
                playerList[j].userName = playerList[j + 1].userName;
                playerList[j].socket = playerList[j + 1].socket;
                playerList[j].isReady = playerList[j + 1].isReady;
            }

            //列表末端空出来一个位置，直接将它置空
            playerList[userCntInRoom].userName = "";
            playerList[userCntInRoom].socket = NULL;
            playerList[userCntInRoom].isReady = false;
        }
    }

    userCntInRoom--;   //这行代码一定要写在上面那些代码的后面，因为那些代码中内部的实现是基于原来的userCntInRoom

    //通知所有的客户端(除了已经离开的客户端以外)
    for (int i = 1; i <= userCntInRoom; i++)
    {
        server_send_LEAVE_ROOM(playerList[i].socket, data.data1);
    }
}

void Board::server_send_LEAVE_ROOM(QTcpSocket *socket, QString leavingUserName)
{
    this->server->send(socket, NetworkData(OPCODE::LEAVE_ROOM_OP, leavingUserName, ""));
}

void Board::server_send_START_GAME(QTcpSocket *socket, QString nameList, QString zoomList)
{
    this->server->send(socket, NetworkData(OPCODE::START_GAME_OP, nameList, zoomList));
}

void Board::server_send_START_TURN(QTcpSocket *socket)
{
    this->server->send(socket, NetworkData(OPCODE::START_TURN_OP, "", ""));
}

void Board::server_receive_MOVE(QTcpSocket * socket, NetworkData data)
{
    // update();

    //这个里面要写大量的检查机制，哎
    Coord* tempTrack = str2track(data.data2);    //将data2的移动字符串转成坐标数组

    //统计跳跃次数
    int tempJumpTime = 0;     //当前玩家的跳跃次数
    int spaceCnt = 0;         //字符串中的空格个数
    for (int i = 0; i <= data.data2.size() - 1; i++)
    {
        if (data.data2.mid(i, 1) == " ")
        {
            spaceCnt++;
        }
    }
    tempJumpTime = (spaceCnt - 1) / 2;

    qDebug() << "the jumpTime of curUser is " << tempJumpTime;

    qDebug() << "the moveTrack of curUser is ";

    for (int i = 1; i <= tempJumpTime + 1; i++)
    {
        qDebug() << "(" << tempTrack[i].coord_x << "," << tempTrack[i].coord_y << ") ";
    }

    //查看当前跳跃是否存在重复
    for (int i = 1; i <= tempJumpTime + 1 ; i++)
    {
        int repeatTime = -1;//不计入自身

        for (int j = 1; j <= tempJumpTime + 1; j++)
        {
            if (tempTrack[i] == tempTrack[j])
            {
                repeatTime++;
            }
        }
        if (repeatTime != 0)
        {
            server_send_ERROR(socket, "Your jump is invalid");
            qDebug() << curUser->userName << " jumping is invalid because of jump to same place";
            return;   //如果异常，在发送完信号之后直接return退出
        }
    }

    // 模拟每一步的跳跃,分析这个坐标数组的移动是否异常
    for (int i = 1; i <= tempJumpTime; i++)
    {
        choose(tempTrack[i]);   //每次选中的是第i颗棋子

        //检查一下黄色棋子的状态
        for (int i = 0; i <= 24; i++)
        {
            for (int j = 0; j <= 16; j++)
            {
                if (coords[i][j].getColor() == yellow)
                {
                    qDebug() << "yellow: (" << coords[i][j].coord_x << "," << coords[i][j].coord_y << ")";
                }
            }
        }

        search();               //进行搜索

//        //检测一下检测的结果（debug）
//        for (int i = 0; i <= 24; i++)
//        {
//            for (int j = 0 ; j <= 16; j++)
//            {
//                if (coords[i][j].getFeasibility() == true)
//                {
//                    qDebug() << "the feasibility coord is (" << coords[i][j].coord_x << "," << coords[i][j].coord_y << ")";
//                }
//            }
//        }

        if (coords[tempTrack[i + 1].coord_x + 12][tempTrack[ i + 1 ].coord_y + 8].getFeasibility() == false) //如果中间有某点不能到达,注意这里的判断条件：应该是转换成原数组进行判断
        {
            server_send_ERROR(socket, "Sorry, your jump is invalid");
            qDebug() << curUser->userName << " jumping is invalid because one point in your track is invalid";
            qDebug() << "And the point is" << "(" << tempTrack[i + 1].coord_x << "," << tempTrack[i + 1].coord_y << ")";

            cancelChoose();
            cancelFeasibility();    //取消选棋和可行性标志，等下一颗棋子重选

            return;   //如果异常，在发送完信号之后直接return退出
        }

        cancelChoose();
        cancelFeasibility();    //取消选棋和可行性标志，选择轨迹列表的第二个位置继续判断

        curUser->jumpTime++;
    }

    //如果上面的检测没有问题，就通知计时器暂停
    countdown->setVisible(false);   // 设置计时器不可见（关闭计时器）
    countdown->setSecs(10000);      // 强行设置计时器停止（方法很土）

    //如果上面的检测没有问题，就在服务端更新图形界面
    for (int i = 1; i <= tempJumpTime; i++)
    {
        move(tempTrack[i], tempTrack[i + 1]);//1->2,2->3,3->4......
        /*TODO unsolved
         * 这里应该添加一个延迟函数
         */
        update();         //move一次就update一次，理想状态下这里应该设置延迟0.5ms
    }

    //如果上面的检测没有问题，就向所有玩家转发这一信息（包括发送者）
    for (int i = 1; i <= userCnt; i++)
    {
        server_send_MOVE(playerList[i].socket, data.data1, data.data2);
    }

    //当这名玩家的MOVE请求后发现这名玩家的所有棋子已经抵达了对面，判胜
    _userColor tempTargetColor;    //当前用户的目标色（当前玩家要抵达的棋槽的颜色）
    int userFinalPiece = 0;        //当前用户的抵达终点的棋子数
    switch (curUser->getUserColor())
    {
        case uyellow:
            tempTargetColor = ublue;
            break;
        case ublue:
            tempTargetColor = uyellow;
            break;
        case ured:
            tempTargetColor = ucyan;
            break;
        case ucyan:
            tempTargetColor = ured;
            break;
        case upurple:
            tempTargetColor = ugreen;
            break;
        case ugreen:
            tempTargetColor = upurple;
            break;
        default:
            break;
    }
    for (int i = 0; i <= 24; i++)
    {
        for (int j = 0; j <= 16; j++) // 遍历所有的棋槽，如果发现和当前目标颜色同色的棋槽
        {
            if (int(tempTargetColor) == int(coords[i][j].targetColor))   // 如果这个棋槽的颜色和玩家的目标色相吻合
            {
                if (int(coords[i][j].getColor()) == int(curUser->getUserColor()))   //如果这个棋槽上面的棋子的颜色和当前玩家的颜色相吻合
                {
                    userFinalPiece++;  // 成功到达终点的棋子数+1
                }
            }
        }
    }
    if (userFinalPiece == 10)  //如果这名用户到达终点的棋子有10颗
    {
        curUser->isWin = true;

        qDebug() << curUser->userName << " has won";

        //将玩家加入胜利者列表中的空位
        for (int i = 1; i <= 6; i++)
        {
            if (winnerList[i].userName == "") // 如果玩家中某一项的名字为空
            {
                winnerList[i].userName = curUser->userName;
            }
        }

        //告知所有玩家这一信息
        for (int i = 1; i <= userCnt; i++)
        {
            server_send_END_TURN(playerList[i].socket);
        }
        update();              //TODO unsovled：应该在界面上显示玩家已经获胜的信息
    }

    //结束终局判断之后，检查所有玩家的胜利或失败状态
    int overCnt = 0;
    for (int i = 1; i <= 6; i++)
    {
        if (users[i].isWin == true || users[i].isFail == true)
        {
            overCnt++;
        }
    }
    if (overCnt == userCnt)   //如果所有玩家都结束了
    {
        //制作胜利者列表
        QString tempWinnerList = "";
        for (int i = 1; i <= 6; i++)
        {
            if (playerList[i].userName != "") // 如果当前玩家的名字不为空
            {
                tempWinnerList.append(playerList[i].userName);  //将这名用户的名字加到玩家列表字符串中
                tempWinnerList.append(" ");//用空格隔开
            }
            tempWinnerList.chop(1); //删去末尾的空格
        }
        for (int i = 1; i <= userCnt; i++)
        {
            server_send_END_GAME(playerList[i].socket, tempWinnerList); //向所有客户端发送这则消息
        }

        qDebug() << "Game over";
        return;  //游戏结束，直接退出
    }

    //结束终局判断之后，调用next，表示下一位玩家开始下棋
    next();

    //结束终局判断之后, 通知所有玩家，应该开始下一轮
    for (int i = 1; i <= userCnt; i++)
    {
        server_send_START_TURN(playerList[i].socket);
    }

    //在服务端系统为下一轮的玩家调用倒计时
    countdown->setSecs(TIME);//将倒计时设置为三十秒
    countdown->setVisible(true);//呈现倒计时
    countdown->start();//倒计时开始
}

void Board::server_send_MOVE(QTcpSocket * socket, QString zoom, QString track)
{
    this->server->send(socket, NetworkData(OPCODE::MOVE_OP, zoom, track));
}

void Board::server_send_END_TURN(QTcpSocket *socket)
{
    this->server->send(socket, NetworkData(OPCODE::END_TURN_OP, "", ""));
}

void Board::server_send_END_GAME(QTcpSocket *socket, QString winnerList)
{
    this->server->send(socket, NetworkData(OPCODE::END_GAME_OP, winnerList, ""));
}

void Board::server_send_ERROR(QTcpSocket * socket, QString detail)
{
    if (detail == "Soory, please enter your room number")
    {
        this->server->send(socket, NetworkData(OPCODE::ERROR_OP, "INVALID_JOIN", detail));
    }

    if (detail == "Sorry, You have the same name as the user in the room")
    {
        this->server->send(socket, NetworkData(OPCODE::ERROR_OP, "INVALID_JOIN", detail));
    }

    if (detail == "Sorry, You are supposes to enter your name")
    {
        this->server->send(socket, NetworkData(OPCODE::ERROR_OP, "INVALID_JOIN", detail));
    }

    if (detail == "Sorry, the room is full")
    {
        this->server->send(socket,  NetworkData(OPCODE::ERROR_OP, "INVALID_JOIN", detail));
    }

    if (detail == "Sorry, your jump is invalid")
    {
        this->server->send(socket, NetworkData(OPCODE::ERROR_OP, "INVALID_MOVE", detail));
    }

}

//--------------------------------------------特殊错误处理函数--------------------------------------------------

//移动错误信息处理
void Board::movePieceBack()
{
    qDebug() << "enter movePiece";

    // 将终点的颜色复制到起点
    coords[curUser->moveTrack[1][0].coord_x + 12][curUser->moveTrack[1][0].coord_y + 8].setColor(coords[curUser->moveTrack[curUser->jumpTime][1].coord_x + 12][curUser->moveTrack[curUser->jumpTime][1].coord_y + 8].getColor());

    // 将终点的颜色设置为空
    coords[curUser->moveTrack[curUser->jumpTime][1].coord_x + 12][curUser->moveTrack[curUser->jumpTime][1].coord_y + 8].setColor(null);

    // 将起点选作被选中的棋子
    choose(coords[curUser->moveTrack[1][0].coord_x + 12][curUser->moveTrack[1][0].coord_y + 8]);

    //搜索可行性
    search();
}

void Board::server_find_TIME_OUT()
{
    // 将这个用户加入胜利者列表的末端
    for (int i = 6; i >= 1; i--)
    {
        if (playerList[i].userName == "")
        {
            playerList[i].userName = curUser->userName;
            break;             //只添加一次
        }
    }

    // 通知所有棋子这一信息
    for (int i = 1; i <= userCnt; i++)
    {
        server_send_MOVE(playerList[i].socket, curUser->zoom, "-1");
    }

    // 设置服务端图形界面信息
    for (int i = 0; i <= 24; i++)
    {
        for (int j = 0; j <= 16; j++)
        {
            if (int(coords[i][j].getColor()) == int(curUser->getUserColor()))  // 找出所有颜色和当前用户颜色相同的棋子
            {
                coords[i][j].setColor(null);    // 将颜色和当前用户颜色相同的棋子清空
            }
        }
    }

    curUser->isFail = true;  // 将当前用户的是否负状态设置为是，并且在paintEvent中加入相关的显示流程
    update();                // 重绘

    //如果最后一个玩家刚好是以这种方式结局
    int overCnt = 0;
    for (int i = 1; i <= 6; i++)
    {
        if (users[i].isFail == true || users[i].isWin == true)
        {
            overCnt++;
        }
    }
    if (overCnt == userCnt)
    {
        //制作胜利者列表

        qDebug() << "The peoson is the last one";

        QString tempWinnerList = "";
        for (int i = 1; i <= 6; i++)
        {
            if (playerList[i].userName != "") // 如果当前玩家的名字不为空
            {
                tempWinnerList.append(playerList[i].userName);  //将这名用户的名字加到玩家列表字符串中
                tempWinnerList.append(" ");//用空格隔开
            }
            tempWinnerList.chop(1); //删去末尾的空格
        }
        for (int i = 1; i <= userCnt; i++)
        {
            server_send_END_GAME(playerList[i].socket, tempWinnerList); //向所有客户端发送这则消息
        }

        qDebug() << "Game over";
        return;  //游戏结束,直接退出,不再调用next
    }

    next();                  // 如果场上还有用户,强制next,在服务端发生了curUser的变化，jumpTime的清零，nextImag图像的变化

    //通知所有玩家，应该照常开始下一轮
    for (int i = 1; i <= userCnt; i++)
    {
        server_send_START_TURN(playerList[i].socket);
    }

    //在服务端系统照常开始倒计时
    countdown->setSecs(TIME);//将倒计时设置为三十秒
    countdown->setVisible(true);//呈现倒计时
    countdown->start();//倒计时开始
}

