#ifndef CPNETWORKSOCKET_H
#define CPNETWORKSOCKET_H
#ifndef NETWORKSOCKET_H
#define NETWORKSOCKET_H

#include "cp-networkdata.h"
#include <QtNetwork/QTcpSocket>

//用于表示本机与远程一台机器建立的连接，封装了跳棋游戏客户端所需要的网络功能
class NetworkSocket : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief NetworkSocket 构造一个 NetworkSocket 对象
     * @param socket 表示当前窗口监听的套接字文件，正常情况下应当传入一个新的套接字
     * @param parent 应传入一个指向与当前 NetworkSocket 对象绑定的窗口的指针
     */
    explicit NetworkSocket(QTcpSocket* socket, QObject* parent = nullptr);
    /* TODO unsolved(ywt:2022/4/28)：
     * 每一个 NetworkSocket 实例应与一个客户端主窗口绑定，这个窗口的类型应该是一个你自定义的继承自 QMainWindow 的类
     * 在这个主窗口类中新增一个 NetworkSocket * 类型的成员变量 socket
     * 在窗口类的构造函数中加入下列代码来完成初始化和信号与槽的连接
     * //详见文档guidance of network
     *
     */

    /**
     * @brief send 用于给当前对象表示的远程机器发送消息
     * @param data 需要发送的消息
     */
    void send(NetworkData data);

    /**
     * @brief 返回原生 TCP 套接字，即成员变量 socket
     */
    QTcpSocket* base() const;

private:
    //成员变量 socket ，表示本机与远程一台机器建立的连接
    QTcpSocket* socket;

signals:
    /**
     * @brief 当前客户端收到来自服务端的消息时发送 reveive 信号
     * @param data 表示接收到的数据
     */
    void receive(NetworkData data);

    /**
     * @brief 当服务端收到客户端发来的消息但解析失败时发送 parseError 信号
     */
    void parseError(const InvalidMessage&);

public slots:
    /**
     * @brief 用于与指定远程机器的指定端口建立网络连接。若当前对象已经与远程机器建立连接，则先断开它。
     * @param host 远程机器的 IPv4 地址，应该让所有客户端与服务端接入同一个局域网（如校园网），并在此传入内网地址
     * @param port 远程机器的端口号
     */
    void hello(const QString& host, quint16 port);
    /* TODO solved(ywt:2022/4/28) :
     * 应该查一下RUC-web的局域网地址
     * 应该查一下自己这台电脑的端口号
     */

    /**
     * @brief 关闭当前客户端与服务端的连接
     */
    void bye();

private slots:
    /**
     * @brief 连接到QTcpSocket，解析传输的数据
     */
    void receiveData();
};



#endif // NETWORKSOCKET_H

#endif // CPNETWORKSOCKET_H
