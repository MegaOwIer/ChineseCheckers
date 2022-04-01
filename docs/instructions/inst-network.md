本次大作业所要求实现的游戏框架分为两个部分：服务端（Server）和客户端（Client）。Client 作为参与玩家与 server 连接并发送请求（Request），server 将所有 client 的请求汇总并进行回复。

<center><img src="/instructions/fig/s-c.png" width="400"></center>

在一个合理的网络交互模式中，下列事件会依次发生：

1. client 尝试与 server 建立连接，成功后 server 会给 client 返回成功的信号。
2. 对于 server/client，当它收到 client/server 发送的请求时，接收这一请求并做出相应处理（如向对方发送另一个请求）。
3. 通信结束时，client 或 server 向对方发送断开连接的请求并在本地断开连接，另一方收到这一请求后也断开连接，至此通信结束。

> 请注意：
> 
> 在本次作业中，不允许 client 之间进行直接通信。Client 之间的通信只能通过 server 间接地进行。
> 
