#ifndef USER_H
#define USER_H

#include <QString>
#include <coord.h>
#include <cp-networkdata.h>
#include <QtNetwork/QTcpSocket>

//如果是null则代表这名用户不在游戏中(没有这名玩家或者这名玩家已经获得了胜利)
enum _userColor { ured, uyellow, ublue, ugreen, ucyan, upurple, unull };

class User
{
public:

    explicit User(_userColor userColor = unull);

    _userColor getUserColor();

    void       setUserColor(_userColor userColor);

    int id;           //id是作为的标号

    QString zoom = "O";     //zoom的可取值为"A","B","C","D","E","F","O"(表示没有任何区域)

    QString userName;

    int jumpTime = 0;

    bool isWin = false;  //是否已经获胜

    bool isFail = false; //是否因为一些特殊原因被中途判负

    QTcpSocket* socket;  //在服务端接收到客户端的名字信息后确认客户端所属用户的名字

    //---------------------------------------------------行棋轨迹相关函数----------------------------------------------------------

    //写一个数组来记录行棋轨迹
    //moveTrack[i][0]就表示第i次跳跃时的起点,moveTrack[i][1]就表示第i次跳跃时的终点
    //显然i应该从1开始计数
    Coord moveTrack[21][2];

    //重新设置行棋轨迹
    void initMoveTrack();

    QString track2str();

private:
    _userColor userColor = unull;

};

//player数组只在进入房间之前使用，进入房间后将它们对应给相应的user
class Player
{
public:
    bool isReady = false;
    QString userName;
    QTcpSocket *socket = NULL;//表示还没有玩家进入到这个位置
};

Coord* str2track(QString str);

#endif // USER_H
