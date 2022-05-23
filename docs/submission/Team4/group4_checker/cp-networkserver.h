#ifndef CPNETWORKSERVER_H
#define CPNETWORKSERVER_H
#ifndef NETWORKSERVER_H
#define NETWORKSERVER_H

#include "cp-networkdata.h"
#include <QtNetwork>

class NetworkServer : public QTcpServer
{
    Q_OBJECT
public:
    /**
     * @brief NetworkServer 构造一个 NetworkServer 对象
     * @param parent 将被提供网络通信功能的对象(一个服务端主窗口)
     */
    explicit NetworkServer(QObject* parent = nullptr);

    /**
     * @brief send 用于向指定机器发送信息
     * @param client 表示目标机器(客户端机器)
     * @param data 需要发送的数据
     */
    void send(QTcpSocket* client, NetworkData data);

signals:
    /**
     * @brief receive 当服务端收到客户端发来的消息并解析成功时发送 receive 信号
     * @param client 表示发送消息的客户端
     * @param data 表示接收到的数据
     */
    void receive(QTcpSocket* client, NetworkData data);

    /**
     * @brief parseError 当服务端收到客户端发来的消息但解析失败时发送 parseError 信号
     * @param msg 注意这个函数传递的也是NetworkData类型的信号
     */
    void parseError(const InvalidMessage& msg);
    /**
     * @brief leave 当服务端旋开与某一客户端的连接时发送 leave 信号
     * @param client 表示被断开连接的客户端
     */
    void leave(QTcpSocket* client);

private slots:
    void receiveData(QObject*);
    void disconnect(QObject*);
    void newconnection();

private:
    QList<QTcpSocket*> clients;           //如何从clients中辨别出玩家的身份呢
    QSignalMapper* disconnMapper;
    QSignalMapper* recvMapper;
};

#endif // NETWORKSERVER_H

#endif // CPNETWORKSERVER_H
