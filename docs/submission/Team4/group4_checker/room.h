#ifndef ROOM_H
#define ROOM_H

#include<QString>
#include<user.h>



class Room
{
public:
    Room();

private:
    int userCnt;  // 最大能容纳人数
    int userCntInRoom; //
    QString roomNumber;
    Player playList[7];


};

namespace Server
{
    Room *room;
}

#endif // ROOM_H
