#include "user.h"

extern Coord nullCoord;
User* curUser;
//因为再初始化me的时候我们还不知道一局游戏中有多少玩家
//这个时候正好可以利用一下users的保留位users[0],先赋值me的信息然后等收到服务端发送的玩家信息再进行信息传递
User users[7];  //这些users在客户端和服务端会分别获得不同程度的应用
User *me;

User::User(_userColor userColor) : userColor(userColor) {}

_userColor User::getUserColor()
{
    return this->userColor;
}

void User::setUserColor(_userColor userColor)
{
    this->userColor = userColor;
}

//----------------------------------------------轨迹记录函数----------------------------------------------------------

//初始化轨迹记录数组
void User::initMoveTrack()
{
    for (int i = 0; i <= 20; i++)
    {
        this->moveTrack[i][0] = nullCoord;
        this->moveTrack[i][1] = nullCoord;
    }
}

QString User::track2str()
{
    QString temp;

    //将第一次跳跃的起点的坐标加入str
    temp.append(QString::number(this->moveTrack[1][0].coord2standardCoord()._x));  //先向字符串加x
    temp.append(" ");
    temp.append(QString::number(this->moveTrack[1][0].coord2standardCoord()._y));  //后向字符串加y
    temp.append(" ");

    qDebug() << "track2str: jumpTime = " << this->jumpTime;

    for (int i = 1; i <= this->jumpTime; i++)
    {
        //将每次跳跃的终止点坐标加入str（包括第一次的终点）
        temp.append(QString::number(this->moveTrack[i][1].coord2standardCoord()._x)); //先向字符串加x
        temp.append(" ");
        temp.append(QString::number(this->moveTrack[i][1].coord2standardCoord()._y)); //后向字符串加y
        temp.append(" ");
    }
    temp.chop(1);  // 最终会多加一个空格，删除它

    return temp;
}

Coord* str2track(QString str)
{
    // str.size()/2就是轨迹中的点数
    int pointCnt = str.size() / 2;

    Coord* tempTrack = new Coord[21];

    // 先将整个数组设置为nullCoord
    for (int i = 0; i <= 20; i++)
    {
        tempTrack[i] = nullCoord;
    }

    for (int i = 1; i <= pointCnt; i++)
    {
        standardCoord tempStandard;

        // i表示是轨迹中的第i个点
        tempStandard._x = str.section(" ", 2 * i - 2, 2 * i - 2).toInt();
        tempStandard._y = str.section(" ", 2 * i - 1, 2 * i - 1).toInt();

        tempTrack[i] = standardCoord2coord(tempStandard);
    }

    return tempTrack;
}


