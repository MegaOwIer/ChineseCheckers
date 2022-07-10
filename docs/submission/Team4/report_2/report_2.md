# 第四小组第一、二阶段实验报告

## 效果展示

  <center><video src="/submission/Team4/report_2/picture/demo.mp4" width="600px" controls="controls"></video></center>

  > 附：[代码仓库传送门](https://github.com/CentrifugalAI/assignment-of-team-4)

## 实现内容

### 第一阶段实现内容

#### 主要实现内容

- 所有跳棋规则内支持的相关用户操作实现
- 能够<font color = #0099ff>**阻止用户作出不合理的跳棋行为**</font>，如跳回已经跳过的位置、企图在间隔跳跃后接一步单步跳跃等

#### 次要实现内容
- 设计富有<font color = #0099ff>**古风**</font>气息的初始化欢迎界面、精致美观的桌子和棋盘、优雅合理的界面布局
	- <img src=/submission/Team4/report_2/picture/hello.png width = 22% /> <img src=/submission/Team4/report_2/picture/board.png width = 30% />
- 设计告知用户当前轮到哪个颜色的棋手下棋的功能，并通过富有创意的方式进行展示，增加画面的色彩感
	- <img src=/submission/Team4/report_2/picture/yellow.png width = 19% /> <img src=/submission/Team4/report_2/picture/red.png width = 19% /> <img src=/submission/Team4/report_2/picture/purple.png width = 19% /> 
	- <img src=/submission/Team4/report_2/picture/blue.png width = 19% /> <img src=/submission/Team4/report_2/picture/cyan.png width = 19% /> <img src=/submission/Team4/report_2/picture/green.png width = 19% />
- 实现终局判断功能
***

### 第二阶段实现内容

#### 主要实现内容

- 设计从自定义坐标和统一标准坐标相互转换的函数`coord2standardCoord`和`standardCoord2coord`
- 重用并封装第一阶段的绘图功能代码`paintEvent`，实现客户端的ui界面绘图功能和服务端的ui界面绘图功能，这样，使得客户端和服务端都能够看到当前房间中的对局状态
- 利用客户端和服务端的通信函数实现对各类信号的接收和发送功能。接收信息主要通过对`receive`和`receiveData`两个函数的完善实现，发送信息的功能主要通过`send`函数的调用实现。此外，服务端能够<font color = #0099ff>**处理异常请求**</font>，并告知客户端错误信息
	- <img src=/submission/Team4/report_2/picture/amaze.png width = 30% /> <img src=/submission/Team4/report_2/picture/again.png width = 30% /> <img src=/submission/Team4/report_2/picture/fail.png width = 30% />
  
#### 次要实现内容

- 加入计时功能，添加<font color = #0099ff>**超时判负**</font>的操作
	- <img src=/submission/Team4/report_2/picture/time1.png width = 40% />  <img src=/submission/Team4/report_2/picture/time2.png width = 40% />
- 设计了几个额外的**ui**窗口
	- 询问当前程序是否作为客户端启动的窗口
		- <img src=/submission/Team4/report_2/picture/ask_server_or_client.png width = 29% /></center>
	- 初始进入房间的等待窗口
		- <img src=/submission/Team4/report_2/picture/ask_join_room.png width = 29% />
	- 显示并告知信息的消息窗口
		- <img src=/submission/Team4/report_2/picture/showInfo.png width = 29% />
	- 以上窗口均使用`.ui`文件设计,通过<font color = #0099ff>**css**</font>代码更改样式表
- 继承古风基调，强调程序界面风格排布

***

## 实现思路

### 第一阶段实现思路

#### 界面绘制方法

- 调用`update`函数示例：以下为客户端完成间隔跳棋后调用了`update`函数的代码示例
    ```C++
    if (abs(curChosenCoord.coord_x - curCoord.coord_x)>=4 || abs(curChosenCoord.coord_y - curCoord.coord_y) >= 2)
    {
        curUser->jumpTime++;  //跳跃次数+1
        //记录轨迹
        curUser->moveTrack[curUser->jumpTime][0] = curChosenCoord;
        curUser->moveTrack[curUser->jumpTime][1] = curCoord;
        this->cancelChoose();  //撤销已经选择的棋子
        this->cancelFeasibility();  //撤销所有的可行性标注
        choose(curCoord);      //选择跳到的位置作为当前选择的位置
        search();              //搜索可行性
        update();
        return;
    }
    ```

- 重写`paintEvent`的代码示例
    ```C++
    void Board::paintEvent(QPaintEvent *event)
    {
        QPainter painter(this);
        setDeskBackground(painter); // 调用函数设置背景
        drawBoard(painter); // 绘制棋盘布局
        if (INIT_GAME)  // 初始化一局游戏
        {
            this->initGame(painter);
        }
        if (USER_HAS_BEEN_INIT)
        {
            nextImag(painter);
        }
        this->updateChosenStateImag(painter); //更新选择状态
        this->updatePieceColorImag(painter); //更新当前界面的所有棋子
        this->updateFeasibilityImag(painter); //更新当前界面的可行性标志
    }
    ```

- 值得一提的是，事实上`update`重绘函数并不是Qt中非常高效的操作，但是其简便之处在于能够忠实地根据当前的状态重绘棋盘上的所有信息，每次都进入`paintEvent`确实是<font color = #0099ff>**损耗程序效率的行为**</font>，坦白而言这种方式显得<font color = #0099ff>**简洁却粗犷**</font>，但是它却为程序员提供了高效的开发途径，开发效率和程序运行效率的取舍体现了一种<font color = #0099ff>**设计哲学**</font>

#### 坐标数组设计

- 易于识别：将棋盘中心点定义为(0,0)，将一条水平直线上相邻棋槽之间距离的$\frac{1}{2}$定义为水平横坐标的长度，将一条竖直直线上相邻的棋槽距离的$\frac{1}{2}$定义为竖直纵坐标的长度
- 提高存取速度：使用二维数组来存储棋盘上的点坐标，并且二位数组的下标和棋盘上的点坐标<font color = #0099ff>**一一对应**</font>，这样只要知道坐标就可以直接获取对应的坐标对象
	- 实现数组下标和点坐标对应的初始化函数`initCoords`
    
        ```C++
        // 数组坐标和系统坐标的转换函数
        void initCoords()
        {
            for (int i = 0; i <= 24; i++)
            {
                for (int j = 0; j <= 16; j++)
                {
                    coords[i][j].coord_x = i - 12;
                    coords[i][j].coord_y = j - 8;
                    coords[i][j].setHaveSlot(false);
                }
            }
        }
        ```
    
	- 直接存取坐标进行操作的<font color = #0099ff>**优越性**</font>(下列代码中curChosenCoord指的是当前棋盘上被用户选中的棋子，这个if语句用来分析当前位置适不适合作为行棋的终点，其中大量地利用了点坐标和数组下标直接对应的优势)
    
        ```C++
        //查找行棋位置的可行性
        if (curChosenCoord.coord_x >= -11 && curChosenCoord.coord_y <= 7 &&(coords[curChosenCoord.coord_x + 12 - 1][curChosenCoord.coord_y + 8 + 1].getColor() == null)&& (coords[curChosenCoord.coord_x + 12 - 1][curChosenCoord.coord_y + 8 + 1].getHaveSlot() == true))
        {
            coords[curChosenCoord.coord_x + 12 - 1][curChosenCoord.coord_y + 8 + 1].setFeasibility(true);
        }
        ```

***

### 第二阶段实现思路

#### 服务端功能实现思路

- 完成接收到各种信息时的处理函数，利用`receiveData`函数进行处理
- 在各种信息的处理函数中相应地调用发送信息的函数，通知相应的客户端执行相应操作
- 注意应该考虑到除了规定的一些操作符，客户端还有可能发来<font color = #0099ff>**规定以外的其它操作符**</font>，因此应该增加默认情况的处理机制
- 服务端有义务对客户端发送的各种信息进行检查（不论客户端是否有完善的检查机制）

#### 客户端功能实现思路

- 完成接收到各种信息时地处理函数，利用`receive`函数进行处理
- 客户端涉及到了很多窗口的开闭，这里使用<font color = #0099ff>**Qt的内置功能**</font>来实现能够更加高效
	- 信号和槽机制（connect）的使用
        
        ```C++
        //点击按钮设置模式
        connect(ui->Client, &QPushButton::clicked, this, &ask_server_or_client::setClientMode);
        connect(ui->Server, &QPushButton::clicked, this, &ask_server_or_client::setServerMode);
    
        //设置按钮的点击导致窗口的关闭
        connect(ui->Client, &QPushButton::clicked, this, &ask_server_or_client::close);
        connect(ui->Server, &QPushButton::clicked, this, &ask_server_or_client::close);
        ```
        
    - Qt的`setVisible`函数能够设置窗口的可视状态，利用这个函数能够非常轻松地实现窗口和部件的出现和消失

***

## 其它

### 代码反思录

我设计了复杂的机制，毫无拘束地使用各种<font color = #0099ff>**危险**</font>的操作完成我希望实现的功能

我知晓C++中那些<font color = #0099ff>无微不至的可以轻松地使用搜索引擎查找到的没必要记的细节</font>，并为知道这些感到由衷的自豪

我把所有函数都写成`public`，丝毫没有注意到一些函数只是成员函数中的子过程，应该提高它们的权限避免误用

我自以为全局变量简直是绝妙的设计，我在各种场合，各种函数中无所顾忌地使用全局变量，享受C++的<font color = #0099ff>宽容和便利</font>

我喜欢<font color = #0099ff>把各种功能写进一个函数里面</font>，我写的函数总是名不符实，比如，一个叫做“起床”的函数可能默默包含了“刷牙洗脸”甚至“吃早餐”的功能

我不喜欢继承......所以我选择<font color = #0099ff>堆叠代码</font>，面对新的需求，我只是将原本的类改写成一个功能更加强大的类

我碰到大量的bug，开始接受C++的报复

我凝视着那个将近三千行的类，它在整个只有五千行的程序里显得如此扎眼，我陷入了沉思......

它有好看的ui，有完整的功能，但没有OOP的灵魂

它<font color = #0099ff>金玉其外，败絮其内</font>

我为这个脆弱的工程感到遗憾

### 如果你也碰到一样的问题

C++是一门面向对象的语言

面向对象是基于实践的伟大设计

那些几十行上百行的代码<font color = #0099ff>只够让你知道最基本的知识点</font>

要真正理解面向对象，理解那些繁琐复杂，那些多么不讨人喜欢的继承，多态，模板，名称空间......

需要去在几千行的代码里面晕头转向，去踩坑，去发现不足，去体会天才们为什么要发明OOP......

这里简单地罗列一些工程设计的面向对象思维，感兴趣地同学可以google

- 单一职责原则
- 开闭原则
- 里氏替换原则
- 依赖倒置原则
- 迪米特法则
- 接口隔离原则

写在最后：
> **实践是检验真理的唯一标准。**

***
