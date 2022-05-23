#ifndef CPNETWORKDATA_H
#define CPNETWORKDATA_H
#ifndef NETWORKDATA_H
#define NETWORKDATA_H

#include <QtCore>

//OPCODE用来
//这是一个枚举类
//OPCODE定义了所有的操作信息
enum class OPCODE : int
{
    JOIN_ROOM_OP = 200000,
    JOIN_ROOM_REPLY_OP,
    LEAVE_ROOM_OP,
    CLOSE_ROOM_OP,
    PLAYER_READY_OP,
    START_GAME_OP,
    START_TURN_OP,
    MOVE_OP,
    END_TURN_OP,
    END_GAME_OP,
    ERROR_OP,
};

//ERRCODE定义了所有的错误信息
enum class ERRCODE : int
{
    NOT_IN_ROOM = 400000,
    ROOM_IS_RUNNING,
    ROOM_NOT_RUNNING,
    INVALID_JOIN,
    OUTTURN_MOVE,
    INVALID_MOVE,
    INVALID_REQ,
    OTHER_ERROR,
};

class InvalidMessage : public QException
{
public:
    InvalidMessage() = delete;
    InvalidMessage(QByteArray message);

    /**
     * @brief Get the message which can't be parsed
     * @return Message as QByteArray
     */
    //将接收到的信息message属性直接返回
    QByteArray messageReceived() const;

private:
    QByteArray message;
};

/**
 * @brief Data structure for standard messages defined by protocol
 */
//主要功能：将需要发送和已经接收到的信息按照通信协议表示出来
class NetworkData
{
public:
    NetworkData() = delete;

    /**
     * @brief NetworkData constructor from operation id and data strings
     * @param op Operation ID
     * @param data1 First parameter string of the operation
     * @param data2 Second parameter string of the operation
     */
    //构造一个NetworkData对象
    NetworkData(OPCODE op, QString data1, QString data2);

    /**
     * @brief NetworkData constructor from message received
     * @param message The message received byte block for parsing
     * @exception InvalidMessage Thrown if failed to parse the byte block
     */
    //这个构造函数用来将QByteArray类的message转换为NetworkData对象的形式
    //当传入参数无法解析时会抛出一个 InvalidMessage 异常
    //抓取到异常后可调用 InvalidMessage::messageReceived() 查看引发异常的消息。该方法的返回类型为 QByteArray
    NetworkData(QByteArray message);

    OPCODE op;
    QString data1;
    QString data2;

    /**
     * @brief Encode the data to QByteArray for sending
     */
    // 将NetworkData信息转换成QByteArray类型的信息用来传递
    QByteArray encode();
};

#endif // NETWORKDATA_H

#endif // CPNETWORKDATA_H
