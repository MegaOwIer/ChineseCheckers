网络接口被封装为三个库，下面依次说明其中提供的全部公有接口的作用及其调用方式。对私有方法的实现与作用感兴趣的同学可自行查阅源代码探索。

**注意：** 网络接口在 Qt6 下实现，不保证在更早版本的 Qt 上能正常使用。

### `NetworkServer` 类

`NetworkServer` 类以 `public` 方式继承自 `QTcpServer` 类，在 `networkserver.{h, c}` 中声明与实现。

`NetworkServer` 类封装了跳棋游戏服务端所需要的网络功能，并提供以下公有方法：

```c++
explicit NetworkServer::NetworkServer(QObject* parent = nullptr);
```
构造函数。用于构造一个 `NetworkServer` 对象。

参数列表：
+ `parent`: 应传入一个指向与当前 `NetworkServer` 对象绑定的窗口的指针。默认值为 `nullprt`，即不与任何窗口绑定。
	- 如果你遵守开发规范，这个窗口的类型应该是一个你自定义的继承自 `QMainWindow` 的类。

```c++
void NetworkServer::send(QTcpSocket* client, NetworkData data);
```
公有方法。用于向指定机器发送信息。

参数列表：
+ `client`: 表示目标机器。
+ `data`: 表示需要发送的数据。

### `NetworkData` 类

`NetworkData` 类在 `networkdata.{h, c}` 中声明与实现，其主要功能是将需要发送和已经接收到的信息按照[通信协议](/instructions/inst-protocol)表示出来。

`NetworkData` 类提供以下公有方法：

```c++
NetworkData::NetworkData(OPCODE op, QString data1, QString data2);
```
构造函数。用于构造需要从本机发送出去的消息。

参数列表：
+ `op`: 当前操作的操作符。
	- 其类型 `OPCODE` 是一个枚举类，同样定义在 `networkdata.h` 中。
+ `data1`: 当前操作的第一个数据段。
+ `data2`: 当前操作的第二个数据段。

```c++
NetworkData::NetworkData(QByteArray message);
```
构造函数。用于构造从接受到的从远程传来的消息。

参数列表：
+ `message`: 当前接收到的消息。

异常：
+ 当传入参数无法解析时会抛出一个 `InvalidMessage` 异常。
	- 抓取到异常后可调用 `InvalidMessage::messageReceived()` 查看引发异常的消息。该方法的返回类型为 `QByteArray`。

```c++
QByteArray NetworkData::encode();
```
公有方法。用于将当前对象所表示的信息编码为 `QByteArray` 方便后续进行其他操作（如发送等）。

### `NetworkSocket` 类

`NetworkSocket` 类以 `public` 方式继承自 `QObject` 类，在 `networksocket.{h, c}` 中声明与实现。用于表示本机与远程一台机器建立的连接。

`NetworkSocket` 类封装了跳棋游戏客户端所需要的网络功能，并提供以下公有方法：

```c++
explicit NetworkSocket::NetworkSocket(QTcpSocket* socket, QObject* parent = nullptr);
```
构造函数。用于构造一个 `NetworkSocket` 对象。

参数列表：
+ `socket`: 表示当前窗口监听的套接字文件，正常情况下应当传入一个新的套接字。
 	- 如果你并不了解上面那句话的含义，在构造函数中简单地为这个参数传入 `new QTcpSocket()` 即可。
+ `parent`: 应传入一个指向与当前 `NetworkSocket` 对象绑定的窗口的指针。默认值为 `nullprt`，即不与任何窗口绑定。
	- 如果你遵守开发规范，这个窗口的类型应该是一个你自定义的继承自 `QMainWindow` 的类。

```c++
void NetworkSocket::send(NetworkData data);
```
公有方法。用于给当前对象表示的远程机器发送消息。

参数列表：
+ `data`: 需要发送的消息。

```c++
void NetworkSocket::hello(const QString& host, quint16 port);
```
公有方法。用于与指定远程机器的指定端口建立网络连接。若当前对象已经与远程建立连接，则先断开它。

参数列表：
+ `host`: 远程机器的 IPv4 地址。
	- 除非你的电脑有专属的 IPv4 地址（通常情况下这不可能），否则你应该让所有客户端与服务端接入同一个局域网（如校园网），并在此传入内网地址。
	- 如果你的设备接入的是 `RUC-Web`，你的内网地址大概率形如 `10.46.X.X`；如果你的设备接入的是 `RUC-Mobile`，你的内网地址大概率形如 `10.47.X.X`。
+ `port`: 远程机器的端口号。

```c++
QTcpSocket* NetworkSocket::base() const;
```
公有方法。返回原生 TCP 套接字。

```c++
void NetworkSocket::bye();
```
公有槽函数。用于关闭当前连接。
