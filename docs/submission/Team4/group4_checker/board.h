#ifndef BOARD_H
#define BOARD_H
#include "coord.h"
#include "user.h"
#include <QPainter>
#include <QMouseEvent>
#include <QPixmap>
#include <QPushButton>
#include <QMessageBox>
#include "cp-networkdata.h"
#include "cp-networkserver.h"
#include "cp-networksocket.h"
#include <QMainWindow>

// 初始化函数
// 初始化users
void initUsers();

void initMode();
/*TODO(ywt:2022\4\29)
 * 实现这个初始化模式的函数
 */

// 初始化棋盘上的坐标设置
void initCoords();

class Board : public QMainWindow
{
    Q_OBJECT

public:

    /*由于测试的需要，我们写的代码既要能作为服务端也要能作为客户端
     * 所以我们在Board函数中既要定义server也要定义socket
     */

    //在这个服务端主窗口类中新增一个 NetworkServer * 类型的成员变量 server
    NetworkServer* server;

    //NetworkSocket用于表示本机与远程一台机器建立的连接, 封装了跳棋游戏客户端所需要的网络功能
    NetworkSocket* socket;

    //定义一个决定这个board对象是作为客户端还是作为服务端
    //0代表客户端，1代表服务端
    bool isServer;

    // 改变next的颜色（改成cur_user的颜色）
    void nextImag(QPainter& painter);

    // 改变curUser（）
    void next();

    // 棋盘离顶部的距离
    const int board2top = 35;

    // 棋盘离左部的距离
    const int board2left = 60;

    // 构造函数：指定父窗口
    Board(QWidget *parent = nullptr);

    void move(Coord& beginCoord,
              Coord& endCoord);

    void search();

    void choose(Coord& chosenCoord);

    void cancelChoose();

    void cancelFeasibility();

    // 更新棋盘绘图的函数----------------------------------------------------------------------

    // 重写paintEvent虚函数，实现绝大部分绘图功能------------------------------------------------------
    virtual void paintEvent(QPaintEvent *event) override;

    // 设置背景的函数
    void         setDeskBackground(QPainter& painter);

    void         drawBoard(QPainter& painter);

    // 初始化游戏的函数
    void initGame(QPainter& painter);

    // 更新棋盘绘图的子函数(更新棋子的颜色)
    void         updatePieceColorImag(QPainter& painter);

    // 更新棋盘绘图的子函数（更新棋盘的可执行性）
    void         updateFeasibilityImag(QPainter& painter);

    void         updateBoardImag(QPainter& painter);

    // 更新棋盘绘图的子函数（更新棋盘的选中对象）
    void         updateChosenStateImag(QPainter& painter);

    // 更新玩家列表绘图的子函数
    void         updataPlayerImag();

    // 重写鼠标释放函数（因为对棋子的操作仅在鼠标释放式生效）------------------------------------------------
    virtual void mouseReleaseEvent(QMouseEvent *event) override;

    // 根据鼠标点击的位置得到当前点击的棋子对象
    Coord&       getCoord(QPoint& curPoint);


private:

    //这是一些网络实现函数的子过程函数，我们一般不需要在成员函数外使用它们

    /**
     * @brief setExistedUserName  设置其它在自己之前就已经进入房间的用户的名字信息
     * @param data1
     */
    void setExistedUserName(QString data1);

    /**
     * @brief setExistedUserReady 设置其它在自己之前就已经进入房间的用户的准备状态
     */
    void setExistedUserReady(QString data2);

    //-----------------------------------关于网络传递的函数----------------------------------------

    //所有关于网络操作相关的函数全部被定义在board类中
    //所有关于网络的操作都被命名为槽函数方便调用

public slots:

    //客户端函数----------------------------------------------------------------------------------

    /**
     * @brief receive 用来实现对NetworkData的接收和解析
     * @param data NetworkData对象
     */
    void receive(NetworkData data);

    /**
     * @brief connected 输出连接成功信息
     */
    void connected();
    /*
     * TODO unsolved(ywt:2022\4\30)
     * 完成临时显示成功信息的窗口的设计
     */

    /**
     * @brief displayError 输出连接失败信息，并设计一个临时窗口展示给用户
     */
    void displayError();
    /*
     * TODO unsolved(ywt:2022\4\30)
     * 完成临时显示错误信息的窗口的设计
     */

    /**
     * @brief client_send_JOIN_ROOM 发送用户名和房间号
     */
    void client_send_JOIN_ROOM();

    /**
     * @brief client_receive_JOIN_ROOM_REPLY 获取并设置在自己进入房间之前其它已经进入房间的用户的名称和准备状态
     * 会先将自己计入总人数：即总人数+1
     * 封装了两个函数setExistedUserName和setExistedUserReady
     * 先调用setExistedUserReady获取当前房间的总人数
     * 通知重绘ask2并允许ask2中Ready键的点击
     */
    void client_receive_JOIN_ROOM_REPLY(NetworkData data);

    /**
     * @brief client_send_PLAYER_READY 当收到来自ask_join_room的getReady信号时，触发client_send_PLAYER_READY请求,发送用户名
     */
    void client_send_PLAYER_READY();

    /**
     * @brief client_receive_JOIN_ROOM 设置users数组中新玩家的信息，并将总人数+1
     * 通知重绘ask2
     */
    void client_receive_JOIN_ROOM(NetworkData data);

    void client_receive_PLAYER_READY(NetworkData data);

    /**
     * @brief reConnect 在显示连接出错的界面中，用户点击重连随即调用hello进行重连
     */
    void reConnect();

    void client_receive_START_GAME(NetworkData data);

    void client_receive_START_TURN(NetworkData data);

    void client_send_MOVE();

    void client_receive_MOVE(NetworkData data);

    void client_receive_END_TURN(NetworkData data);

    void client_receive_END_GAME(NetworkData data);

    void client_send_LEAVE_ROOM();

    void client_receive_LEAVE_ROOM(NetworkData data);

    //服务端函数---------------------------------------------------------------------------------

    void receiveData(QTcpSocket *socket, NetworkData data);

    void server_receive_JOIN_ROOM(QTcpSocket * socket, NetworkData data);

    void server_send_JOIN_ROOM_REPLY(QTcpSocket * socket, QString nameList, QString readyList);

    void server_send_JOIN_ROOM(QTcpSocket * socket, QString userName);

    void server_receive_LEAVE_ROOM(QTcpSocket * socket, NetworkData data);

    void server_send_LEAVE_ROOM(QTcpSocket *socket, QString leavingUserName);

    void server_receive_PLATER_READY(QTcpSocket * socket, NetworkData data);

    void server_send_PLAYER_READY(QTcpSocket * socket, QString userName);

    void server_send_START_GAME(QTcpSocket * socket, QString nameList, QString zoomList);

    void server_send_START_TURN(QTcpSocket *socket);

    void server_receive_MOVE(QTcpSocket * socket, NetworkData data);

    void server_send_MOVE(QTcpSocket * socket, QString zoom, QString track);

    void server_send_END_TURN(QTcpSocket *socket);

    void server_send_END_GAME(QTcpSocket *socket, QString winnerList);

    void server_find_TIME_OUT();

    void server_send_ERROR(QTcpSocket * socket, QString detail);

    //特殊错误处理函数-------------------------------------------------------------------------------

    /**
     * @brief movePieceBack 当客户端发现移动错误时，应该将棋子移动回原位
     */
    void movePieceBack();

signals:

    void forbidButton();

    void Ready_is_enabled(bool isEnabled);

    /**
     * @brief sendWrong 该信号函数用于发送错误信息
     */
    void client_receive_ERROR(QString ERRCODE, QString wrongInfo);

};

#endif // BOARD_H
