# 第二阶段报告

## 服务端`ServerWindow`

### 简单的初步操作

1. 新建`NetworkServer`类指针`server`，并`connect`网络接口中的`NetworkServer::receive`与自己的`ServerWindow::receive`，使得来自客户端的数据能够用传进`receive`函数中。
2. 设置一个`LineEdit`用来输入端口号，赋值给port。
3. 开启客户端监听`server->listen(QHostAddress::Any,port)`。
4. 在头文件中设置6个`QTcpSocket`类指针`_socket[6]`，用来存储在房间内的客户端指针。（这里做了简化，只考虑了一个房间。）
5. 继承自第一阶段的棋盘界面`MainWindow`用来现实当前对局状况。

### `ServerWindow::receive`函数

1. 传进来的信息参量是`QTcpSocket`类指针，设为`*s`；`Networkdata`类，设为`w`。s为方便服务端识别是哪个客户端发来的信息。
2. 通过`if、else`语句对`w.op`进行分类讨论，收到客户端的信息后进行相应的操作，必要时给客户发送信息`this->server->send(_socket，data)`;
3. 收到客户端的`MOVE_OP`请求后，调用判棋函数，判棋函数与第一阶段的类似。
4. 每次行棋结束后，检索棋盘，判断是否有玩家胜利，并发送胜利信息。

## 客户端`ClientWindow`

### 简单的初步操作

1. 新建`NetworkSocket`类指针`socket`，并`connect`网络接口中的`NetworkSocket::receive`与自己的`ClientWindow::receive`，使得来自服务端的数据能够用传进`receive`函数中。

2. 分别做如下两个connect，用来显示连接成功与否，并引导用户进行下一步操作。

   ```
   connect(socket->base(), &QAbstractSocket::errorOccurred, this, &ClientWindow::displayError);
   connect(socket->base(), &QAbstractSocket::connected, this, &ClientWindow::connected);
   ```

   `connected：show`一个可视化窗口，显示“连接成功”，并附带按钮“OK”。当用户点击OK后，进入房间界面`RoomWindow`。

   `displayError：show`一个可视化窗口，显示“连接失败”，并附带按钮“OK”。当用户点击OK后，重新进入输入`IP`和端口号界面，即`ClientWindow`。

3. 设置两个`LineEdit`用来输入`IP`和端口号，分别赋值给`host`和`port`。

4. 尝试连接服务端`this->socket->hello(host, port)`，接下来分别对应2中的两种情况。

### `ClientWindow::receive`函数

1. 传进来的信息是个`Networkdata`类，设为`w`。
2. 通过`if、else`语句对`w.op`进行分类讨论，收到服务端的信息后进行相应的操作，必要时给服务端发送信息`this->socket->send(data)`

### `RoomWindow`界面

1. 六行两列的`QLabel`，分别现实六个玩家名和六个准备状态。
2. 三个按钮，加入房间、退出房间、准备。
3. 当收到服务端的进出房间请求后，更新房间显示。
4. 当收到`START_GAME_OP`，关闭`RoomWindow`，打开`ClientWindow`。

### 其他操作

1. 与标准坐标的坐标转化。
2. 对第一阶段成果进行修改，如点击函数、轮转时机等。
