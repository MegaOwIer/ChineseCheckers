# 第四小组学期作业最终汇报

附：本小组代码仓库[小组名：大作业是啥](https://github.com/CentrifugalAI/assignment-of-team-4) 

## 一、任务分配

> 本小组当前全部工程代码行数<font color = #0099ff>8117</font>行（不包括ui界面的Qt自动生成代码），其中<font color = #0099ff>第三阶段</font>是当前所作工作最多的阶段

### 1. 李俊霖
- 第一阶段参与研讨，坐标系统设计，探索git的使用方法
- 第二阶段参与研讨
- 第二阶段结束后进行部分代码重构
- 第三阶段参与研讨，部分功能设计，部分代码实现，部分代码调试，ui界面设计
- 在第三阶段结束后协调完成联调bug

### 2. 李天成
- 第一阶段参与研讨
- 第二阶段参与研讨
- 第三阶段参与研讨，部分代码实现，部分代码调试，ui界面设计
- 在第三阶段结束后协调完成联调bug

### 3. 叶伟堂 
- 第一阶段参与研讨，功能设计，代码实现，代码调试。
- 第二阶段参与研讨，功能设计，代码实现，部分代码调试。在第二阶段为部分小组提供了服务端设计上的帮助。
- 第三阶段参与研讨，部分功能设计，部分代码实现，部分代码调试
- 小组实验报告撰写提交，小组展示
- 在第三阶段结束后组织各小组完成联调bug

---

## 二、第三阶段任务完成情况

### 第三阶段效果展示与核心功能的运行视频

- 加入房间（多房间）

  <video src="/submission/Team4/report_3/picture/login.mp4" width="600px" controls="controls"></video>

- 行棋过程、历史复现、复盘模拟（多房间要求的附加功能）

  <video src="/submission/Team4/report_3/picture/playing.mp4" width="600px" controls="controls"></video>

### 第三阶段补充实现内容

- 画风一致的 ui 界面设计

  <img src=/submission/Team4/report_3/picture/board.png width = 40% /> <img src=/submission/Team4/report_3/picture/tv.png width = 40% />

- 补充设计了显示错误信息的弹窗
  
  <img src=/submission/Team4/report_3/picture/connected.png width = 40% /> <img src=/submission/Team4/report_3/picture/invalidJoin.png width = 40% /> 

- 补充设计了选择房间的弹窗

  <img src=/submission/Team4/report_3/picture/chooseRoom.png width = 40% />

- 补充设计了选择历史轮次的弹窗

  <img src=/submission/Team4/report_3/picture/chooseRound.png width = 40% />

- 将鼠标形状修改为“纸飞机”样式
  ```C++
    // 设置鼠标图标
    QCursor cursor;
    QPixmap pixmap(MOUSE);  // MOUSE是纸飞机的矢量图路径（宏）
    cursor = QCursor(pixmap, 0, 0);
    setCursor(cursor);
  ```

- 补充设计了<font color = #0099ff>**客户端强制退出房间而没有收到离开请求的异常处理机制**</font>

- 输出和客户端的通讯信息的调试信息框：<font color = #0099ff>**选择性**</font>地输出调试信息，方便不同小组同时联机时能够观测调试结果
  
  <img src=/submission/Team4/report_3/picture/teminal.png width = 70% />
  <img src=/submission/Team4/report_3/picture/teminal_guide.png width = 70% />

--- 

## 三、项目整体框架

### 1. 类的继承关系
- Board相关类的继承关系
    
    <img src=/submission/Team4/report_3/picture/board_class.png width = 70% />

- Form相关类的继承关系
    
    <img src=/submission/Team4/report_3/picture/form_class.png width = 70% />

### 2. 项目架构&项目历史演变
  
- 第一阶段
    - 完成了`Board`类，`Board`类的代码量尚属客观
    - 采用了一些全局变量来方便开发
    - 整体的对象关系非常简单
        - `Board`中定义了类<font color = #0099ff>`Coord`</font>的数组`coords`<font color = #0099ff>用来储存坐标的状态</font>
        - 坐标的状态包括：这个坐标上是否有棋槽、当前坐标上的棋子是什么颜色的（如果没有棋子就是空色）、当前坐标是否被玩家选中
- 第二阶段：将服务端和客户端功能都追加到Board类中
    - 项目结构相对第一阶段变化不大
    - 直接在`Board`中添加了服务端和客户端的所有所需变量
    - 服务端和客户端共用一套变量（因为程序不会同时作为服务端和客户端启动）
    - `Board`<font color = #0099ff>类成为了一个非常臃肿的类：达到 **3000+** 行</font>
- 第三阶段
    - 第二阶段的代码结构很明显不能满足第三阶段的开发需求，主要体现在：
    - 第二阶段整个程序共用一套变量，很明显同时接入多个房间时，<font color = #0099ff>**哪个房间来使用这套变量是非常棘手的问题**</font>
    - 第二阶段使用了大量的全局变量，全局变量的危险性不言而喻，在多个房间同时运作时很容易<font color = #0099ff>**出现无意的修改全局变量的值导致bug**</font>
    - 第二阶段的`NetworkServer server`和`NetworkSocket socket`是直接绑定在棋盘`Board`上的，如果只是将Board复制若干遍，实际上只是相当于打开了多个程序，达不到多房间的作用，而且没有一个函数将连入的客户端进行引流，<font color = #0099ff>**多个server只会导致盲目连接的不可控局面**</font>
    - 所以我们决定开始重构代码
        - `System`类概述
            - 取消`server`和`socket`与`Board`棋盘类的绑定
            - 新增一个`System`类，将`server`和`socket`绑定到`System`类上
            - 将信号分流函数`receive`(客户端接收信息)，`receiveData`(服务端接收信息)整合到`System`类中，`System`类<font color = #0099ff>**专门用来收发信息**</font>
        - `Room`类概述
            - 添加一个`Room`类，<font color = #0099ff>**在这个类中将棋盘和棋手的概念分开**</font>
            - `User`类的对象数组`users`，对象指针`curUser`，`Board`类的对象指针`board`都被整合到了`Room`类中作为`Room`类的属性
            - 将一些原本处于`board`类的操作用户信息的方法整合到`Room`中，使得整个工程的逻辑更加合理
            - 将相关全局变量整合到`Room`类中并进行适当的保护
        - `System`和`Room`中的相关接口和项目架构关系(以接收信号的过程为例)
        
        <img src=/submission/Team4/report_3/picture/io.png width = 90% />

---

## 四、关键实现方法

### 1. 数组下标和坐标的直接对应：提高存取效率

- 一般方法：给出棋子坐标(x,y)，然后通过(x,y)为参数遍历整个棋盘的所有已经定义的坐标，获取对应的坐标点

> 将坐标对象集定义为Coord coords[121]，在每次得到一组坐标时遍历对应的坐标

  ```C++
    for(int i = 0; i < 120 ; ++i)
    {
        if(coords[i].getX() == x && coords[i].getY() == y)
            coords[i].setColor(PieceColor::yellow);
    }
  ```

- 本组采用的方法：直接将数组下标和坐标对应
> 将坐标对象集定义为二维数组，每个坐标对象和每组坐标成简单双射关系

  ```C++
    /**
    * @brief Room::initCoords 初始化坐标数组的下标和坐标的对应关系
    */
    void Room::initCoords()
    {
        for (int i = 0; i < 25; ++i)
        {
            for (int j = 0; j < 17; ++j)
            {
                coords[i][j].setX(i - 12);
                coords[i][j].setY(j - 8);
                coords[i][j].setHaveSlot(false);
                coords[i][j].setColor(PieceColor::null);
            }
        }
    }
  ```
  ```C++
    // 以下语句能够在程序中直接获取对应棋子
    coords[x + 12][y + 8].setColor(PieceColor::yellow);
  ```

### 2. 历史复现与复盘模拟的实现方法：状态记录和模式转换

- 每个`Room`类中都定义有`Room *historyRoom`指针，在房间被构造时随即初始化：`this->historyRoom = this->buildHistoryRoom();`
    
    ```C++
    Room* Room::buildHistoryRoom()
    {
        Room *buildingRoom = new Room();
      
        // 设置buildingRoom的属性
        buildingRoom->mode = 3;        // 在没有点击you can you up之前
        buildingRoom->board->isServer = false;
        buildingRoom->roomNumber = "history"; // 设置房间号
        buildingRoom->userCnt = this->userCnt;       // 设置房间信息
        buildingRoom->historyRoom = nullptr;         // 历史房间设置为空（历史房间没有历史房间）
        buildingRoom->originRoom = this;             // 设置原始房间
        buildingRoom->IS_HISTORY_ROOM = true;        // 设置当前房间是历史房间
        this->historyRoom = buildingRoom;            // 设置当前房间的历史房间为这个
        buildingRoom->setVisible(false);             // 历史房间默认不显示
      
        // 完成historyRoom的一些初始化工作
        buildingRoom->initUsers();  //初始化用户
        buildingRoom->initCoords();

        return buildingRoom;
    }
    ```

- 在某个房间的玩家行棋后，该房间的历史房间将把当前棋盘的状态记录到历史状态中：`this->historyRoom->recordState();`

    ```C++
    this->historyRoom->round[this->historyRoom->curRoundCnt].state[this->historyRoom->round[this->historyRoom->curRoundCnt].curStateCnt].beforeWho = this->curUser->getUserColor();
    for (int i = 0; i <= 24; ++i)
    {
        for (int j = 0; j <= 16; ++j)
        // 下面这行代码用来将当前状态记录到历史状态的对象数组中
        {
            this->historyRoom->round[this->historyRoom->curRoundCnt].state[this->historyRoom->round[this->historyRoom->curRoundCnt].curStateCnt].coordsState[i][j] = this->coords[i][j];
        }
    }
    this->historyRoom->round[this->historyRoom->curRoundCnt].curStateCnt++;   // 那一轮中的状态数增加
    ```

- 通过函数`Room::changeRoundState`来切换历史状态的轮次

- 复盘模拟的开启：将`mode`切换为1，启用鼠标事件，通过一系列设置将棋盘状态“激活”,使得能够使用鼠标事件，进入单机模式

> 复盘模拟的单机模式不会影响到服务端的正常运行，可以理解为这是两个同时运作的窗口

### 3. 房间转换的实现方法：更改部件可视化

- 每间房间都在`introduceRoom`函数调用后正式“启用”，在启用时默认设置`setVisible(false)`
- 在某间房间被调用以后，先调整原本的房间不可视，再调用目标房间的可视性调整函数
  
  ```C++
  void Room::chooseThisRoom()
  {
      mysystem->curRoom->setVisible(false);
      mysystem->curRoom = this;
      mysystem->curRoom->setVisible(true);
      mysystem->curRoom->ui->changeRoom->setVisible(true);
      mysystem->curRoom->ui->history->setVisible(true);
  }
  ```

---

## 五、鸣谢

感谢老师和助教师兄在一学期在程序设计Ⅱ课程大作业中精心的前期开发与准备、耐心的过程跟进和答疑，我们组在这次编程训练中受益匪浅。

感谢2021届图灵班同学们在完成作业过程中的互相帮助，积极讨论，营造了很好的编程氛围

