# 跳棋游戏第一阶段任务实验报告



### 一、第一阶段任务要求

1、 进入游戏、退出游戏。

2、 多名玩家轮流操作以实现本机对局。

​	（1） 你的游戏客户端应有判断玩家操作是否合法的功能。

​	（2） 暂不要求实现包括终局判断、每步限时、超时判负等内容。

3、 图形素材（如棋盘、棋子的贴图等）的设计与制作。



### 二、 小组任务分工

周小琳：程序编译和调试，添加选择几人模式的功能和设置玩家顺序，调整qt上生成的效果，ui文件编辑，github成果上传。

杨爽：完成主要的代码编写（cpp文件和h文件），调试程序，写报告。

赵铎淞：寻找图片素材，调整棋盘，棋子，图片美观，游戏试玩。



### 三、代码框架设计

目前阶段一共有三个层次的窗口

##### 1、第一个层次：主页面

这个就是一开始进入游戏的界面，需要设置开始按钮，点击以后可以进入到第二个曾层次的页面并且关闭当前层次的页面；并且点击关闭按钮后可以退出游戏。这个只要设置信号和简单的槽函数就可以实现了。

其他就是一些这个页面的标题背景等的设计。

##### 2、第二个层次：选择模式的页面

本次游戏设计要求可以实现2人模式，3人模式和6人模式，所以再点击进入开始以后，应该要出现一个页面，上面有三个可供玩家选择的按钮。这个只要在第一层次的页面的开始按钮的槽函数里面new一个choose页面的对象就可以了，然后再对不同模式的按钮分别生成不同类型的第三层次的页面。

##### 3、第三个层次：游戏页面

第二个层次的页面里面传进来一个参数，是2，3或6，然后根据这个参数，调用不同的初始化模式的函数， 不同的设置模式的函数的区别仅仅在于在哪些角上面放上开局的棋子。

第三层次的页面主要用类Widget来实现，类Widget中主要包含了以下内容

（1）初始化棋盘坐标的函数setCoordinate()

（2）用来画棋盘的函数paintEvent()

（3）当前具有控制权的玩家control

（4）当前游戏模式type

（5）根据游戏模式设置不同棋盘状态的函数set2people_mode(), set3people_mode(), set6people_mode() 

（6）玩家点击某个棋子后的响应槽函数chessClick()

（7）玩家选择某个走棋的位置后的函数mousePressEvent()

 （8）当前玩家选中的走棋的棋子select

（9）判断玩家的移动是否合法的函数isLegalMove()

（10）递归用于判断连跳的函数ifreach()

在这个页面上，支持玩家展开游戏轮流下棋并判断玩家的移动和轮流次序是否合法。



##### 另外一个重要的类

还有一个类是棋子类，用来存储每个棋子相关的信息，主要是操控当前棋子的玩家，棋子所在的横纵坐标和棋子所在的存储的数组里面的行列标号。



### 四、重要部分的代码内容展示



#### 1、坐标的设置

```c++
void Widget:: assign_xy(int area, int inv, int topx, int topy)  // 用来分配六个区域的坐标
// area 代表A，B，C，D中的某个区域，inv代表是正三角还是倒三角
{

    int pre_edge = inv * _edge;
    Coordinate[area][0].x = topx; Coordinate[area][0].y = topy;
    Coordinate[area][1].x = topx + half_edge; Coordinate[area][1].y = topy + pre_edge;
    Coordinate[area][2].x = topx - half_edge; Coordinate[area][2].y = topy + pre_edge;
    Coordinate[area][3].x = Coordinate[area][2].x - half_edge;
    Coordinate[area][3].y = Coordinate[area][2].y + pre_edge;
    for (int i = 4; i <= 5; i++)
    {
        Coordinate[area][i].x = Coordinate[area][i-1].x + half_edge * 2;
        Coordinate[area][i].y = Coordinate[area][i-1].y;
    }
    Coordinate[area][6].x = Coordinate[area][5].x + half_edge;
    Coordinate[area][6].y = Coordinate[area][5].y + pre_edge;
    for (int i = 7; i <= 9; i++)
    {
        Coordinate[area][i].x = Coordinate[area][i-1].x - half_edge * 2;
        Coordinate[area][i].y = Coordinate[area][i-1].y;
    }

}

void Widget:: setCoordinate()
{
    // A(1): 上， B(2)： 下， C(3)：右上， D(4)：左下， E(5)：右下， F(6)：左上
    //每个节点的内部按顺时针进行标号

    //首先得到对应的用来传输和转换的int的数据
    for (int j = 0; j < 6; j++ )
      for (int i = 0; i < 10; i++)
          Coordinate[j+1][i].Value = (('A'+ j) << 16) + i;
    //中间空白的部分共有 (5 + 6 + 7 + 8) * 2 + 9 = 61 个位置，用Coodinate[0]表示
    for (int j = 0; j < 61; j++)
        Coordinate[0][j].Value = j;

    //下面我们为每个部分来设置绘图时候需要的坐标
    //首先是棋子摆放的部分
    assign_xy(1, 1, A_top_x, A_top_y);
    assign_xy(2, -1, A_top_x, A_top_y + _edge * 16);
    assign_xy(3, -1, A_top_x + half_edge * 9, A_top_y + _edge * 7);
    assign_xy(4, 1, A_top_x - half_edge * 9, A_top_y + _edge * 9);
    assign_xy(5, 1, A_top_x + half_edge * 9, A_top_y + _edge * 9);
    assign_xy(6, -1, A_top_x - half_edge * 9, A_top_y + _edge * 7);
    //for(int i=0;i<=9;i++)
    //    qDebug()<<"("<<Coordinate[2][i].x<<","<<Coordinate[2][i].y<<")";
    // 然后是中间空白部分的坐标，我们就用行列遍历的顺序来标号
    // 5 + 6 + 7 + 8 + 9 + 8 + 7 + 6 + 5
    Coordinate[0][0].x = Coordinate[1][9].x - half_edge;
    Coordinate[0][0].y = Coordinate[1][9].y +_edge; //改
    // temp数组计算出每行开始的节点
    int temp[10]; temp[0] = 0;
    for (int i = 1; i < 5; i++) temp[i] = temp[i-1] + i + 4;
    for (int i = 5; i < 9; i++) temp[i] = temp[i-1] + 14 - i;
    int p = 1;
    for (int i = 1; i < 61; i++)
        if (i == temp[p])
        {
            int f;
            if (p < 5) f = -half_edge; else f = half_edge;
            Coordinate[0][i].x = Coordinate[0][temp[p-1]].x + f;
            Coordinate[0][i].y = Coordinate[0][temp[p-1]].y + _edge;
            p++;
        }
        else
        {
            Coordinate[0][i].x = Coordinate[0][i-1].x + half_edge * 2;
            Coordinate[0][i].y = Coordinate[0][i-1].y;
        }

    //下面设置空白位置的初始状态
    for (int i = 0; i < 61; i++) Coordinate[0][i].ifOccupied = 0;

    //构建矩阵坐标和传输值的关系
    mapping();

    // 在places里面添加可以用来放置棋子的区域

    for (int i = 1 ; i <= 6 ; i++)
        for (int j = 0 ; j < 10 ; j++)
            places.push_back(&Coordinate[i][j]);
    for (int i = 0 ; i < 61 ; i++) places.push_back(&Coordinate[0][i]);

}
```

代码的细节部分可见上面的具体注释。

首先固定棋盘的主要是上面的宏的4个参数，分别表示最上面的角的顶点的坐标，内部小三角形的一半的变长和高的长度。

主要思路是先设置六个角的部分的坐标，每个角可以看作是一个正三角形或者是一个倒三角形，然后调用assign_xy来分配三角形上的角的各个点的坐标，用inv来表示是正三角还是倒三角；然后空白的部分就是按行按列，从左往右，从上到下的顺序标号存放在数组里面，用temp计算出每行开始的位置，然后如果换行则根据前一行计算坐标，否则根据同一行的前面一个位置来计算当前的坐标。

同时用一个传输值来进行坐标和存储的数组的下标的转换，如('A' << 16 + j)，这样就可以根据这个值来反转得到坐标。



#### 2、棋盘的绘制

```c++
void Widget :: paintEvent(QPaintEvent *)
{
    //整个图形可以看作是两个大的等边三角形的拼接
    //在这里最重要的是先确定好三个顶角的位置

    //Step 1: 首先画出每个角落的不同的玩家的不同的底色
    draw_BottomCol(1, A_top_x ,A_top_y , 1);
    draw_BottomCol(2, A_top_x ,A_top_y + 16 * _edge , -1);
    draw_BottomCol(3, A_top_x + 8 * half_edge ,A_top_y + 8 * _edge , -1);
    draw_BottomCol(4, A_top_x - 8 * half_edge ,A_top_y + 8 * _edge , 1);
    draw_BottomCol(5, A_top_x + 8 * half_edge ,A_top_y + 8 * _edge , 1);
    draw_BottomCol(6, A_top_x - 8 * half_edge ,A_top_y + 8 * _edge , -1);

    //Step 2: 分别画两个三角形里面的线，与此同时可以画出轮廓
    //思路 ：分别从每个顶点出发画出横线
    QPen pen(Qt::SolidLine);
    QPainter p(this);
    p.setPen(pen);

    const int B_top_x = A_top_x; const int B_top_y = A_top_y + 16 * _edge;
    const int C_top_x = A_top_x + 12 * half_edge; const int C_top_y = A_top_y + 4 * _edge;
    const int D_top_x = A_top_x - 12 * half_edge; const int D_top_y = A_top_y + 12 * _edge;
    const int E_top_x = A_top_x + 12 * half_edge; const int E_top_y = A_top_y + 12 * _edge;
    const int F_top_x = A_top_x - 12 * half_edge; const int F_top_y = A_top_y + 4 * _edge;

        //正三角形中的线
    for (int i = 1; i <= 12; i ++)
    {
        p.drawLine(A_top_x - i * half_edge,A_top_y + i * _edge, A_top_x + i *half_edge, A_top_y + i * _edge); //表示从左边的点出发到右边画一条线

        p.drawLine(D_top_x + i * 2 * half_edge, D_top_y, D_top_x + i * half_edge, D_top_y - i * _edge);

        p.drawLine(E_top_x - i * 2 * half_edge, E_top_y, E_top_x - i * half_edge, E_top_y - i * _edge);
    }
        //倒三角形中的线
    for (int i = 1; i <= 12; i ++)
    {
        p.drawLine(B_top_x - i * half_edge,B_top_y - i * _edge, B_top_x + i *half_edge, B_top_y - i * _edge); //表示从左边的点出发到右边画一条线

        p.drawLine(F_top_x + i * 2 * half_edge, F_top_y, F_top_x + i * half_edge, F_top_y + i * _edge);

        p.drawLine(C_top_x - i * 2 * half_edge, C_top_y, C_top_x - i * half_edge, C_top_y + i * _edge);
    }
    //Step 3: 画出棋盘上面的圆圈
    QBrush brush;
    brush.setColor(Qt::white); //棋盘上的圆圈用白色填涂
    p.setBrush(brush);

    //正三角画圆
    for(int i = 0; i <= 12; i++)
        for(int j = 0; j <= i; j++)
            p.drawEllipse(QPoint(A_top_x - i * half_edge + j * 2 * half_edge ,A_top_y + i * _edge),chess_bias,chess_bias); //在矩形里面画内切圆

    //倒三角里面画圆
    for(int i = 0; i <= 12; i++)
        for(int j = 0; j <= i; j++)
            p.drawEllipse(QPoint(B_top_x - i * half_edge + j * 2 * half_edge ,B_top_y - i * _edge),chess_bias,chess_bias);


}

```

具体实现细节见上面的注释，按照先画出各个区域的底色，再画出上面的线，最后再画出放棋子的圆圈，这样后画上去的部分就可以盖住之前画上去的部分。





#### 3、点击某个棋子后的响应的槽函数

```c++
void Widget::chessClick(int player, int tx, int ty)
{
   // qDebug()<<"enter chessclick";
   // qDebug()<<control<<" "<<player;
    if (control == player) // 限制为当前的玩家操作
    {
      //  qDebug()<<control<<" "<<player;
        switch (player) //根据玩家分类设置不同的展示效果图
        {
            case 1:
            {
                if (clickTime == 0) //第一次点击选中棋子
                {
                    clickTime = 1;
                    this->select = this->Coordinate[tx][ty].chess;
                    this->select->setIcon(QIcon(":/image2/1.png"));
                }
                else //第二次点击取消选中
                {

                    clickTime = 0;
                    this->select->setIcon(QIcon(":/image2/1.png"));
                    this->select = nullptr; //取消选中
                }
                break;
            }
            case 2:
            {
             //  qDebug()<<"enter case2";
                if (clickTime == 0) //第一次点击选中棋子
                {
                  //   qDebug()<<"enter case2-1";

                    clickTime = 1;
                    this->select = this->Coordinate[tx][ty].chess;
                    this->select->setIcon(QIcon(":/image2/2.png"));
                }
                else //第二次点击取消选中
                {

                    clickTime = 0;
                    this->select->setIcon(QIcon(":/image2/2.png"));
                    this->select = nullptr; //取消选中
                }
                break;
            }
            case 3:
            {
                if (clickTime == 0) //第一次点击选中棋子
                {
                    clickTime = 1;
                    this->select = this->Coordinate[tx][ty].chess;
                    this->select->setIcon(QIcon(":/image2/3.png"));
                }
                else //第二次点击取消选中
                {

                    clickTime = 0;
                    this->select->setIcon(QIcon(":/image2/3.png"));
                    this->select = nullptr; //取消选中
                }
                break;
            }
            case 4:
            {
                if (clickTime == 0) //第一次点击选中棋子
                {
                    clickTime = 1;
                    this->select = this->Coordinate[tx][ty].chess;
                    this->select->setIcon(QIcon(":/image2/4.png"));
                }
                else //第二次点击取消选中
                {

                    clickTime = 0;
                    this->select->setIcon(QIcon(":/image2/4.png"));
                    this->select = nullptr; //取消选中
                }
                break;
            }
            case 5:
            {
                if (clickTime == 0) //第一次点击选中棋子
                {
                    clickTime = 1;
                    this->select = this->Coordinate[tx][ty].chess;
                    this->select->setIcon(QIcon(":/image2/5.png"));
                }
                else //第二次点击取消选中
                {

                    clickTime = 0;
                    this->select->setIcon(QIcon(":/image2/5.png"));
                    this->select = nullptr; //取消选中
                }
                break;
            }
            case 6:
            {
                if (clickTime == 0) //第一次点击选中棋子
                {
                    clickTime = 1;
                    this->select = this->Coordinate[tx][ty].chess;
                    this->select->setIcon(QIcon(":/image2/6.png"));
                }
                else //第二次点击取消选中
                {

                    clickTime = 0;
                    this->select->setIcon(QIcon(":/image2/6.png"));
                    this->select = nullptr; //取消选中
                }
                break;
            }
            default :
            ;


        }
    }
}


```

具体细节见上面的注释。主要的操作方法是点击有棋子的地方，棋子处就会显示被选中的状态，然后再点一次，就会取消选中。



#### 4、玩家选择某个走棋的位置的响应函数

```c++
void Widget::mousePressEvent(QMouseEvent *event)
{
    //qDebug()<<"a";

    // 如果是左击
    if (select != nullptr && event -> button()  == Qt :: LeftButton)
    {
        // 循环所有棋子的位置
        for (QVector<Coordinate_struct*>::iterator iter = places.begin();iter != places.end();iter++)
        {
            Coordinate_struct* ptr = *iter;
            //如果和当前棋子的距离足够近，那么就判断是操作该棋子
         //   qDebug()<<ptr->x<<" "<<ptr->y;
            if(get_dis(event->x() , event->y() , ptr->x , ptr->y) < Dis_lim  )//get_dis 是什么函数？
            {
                //判断移动是否合法
              //  qDebug()<<"1";
                if(isLegalMove(select , ptr))
                {

                    qDebug()<<"judge as islegalmove";
                //    qDebug()<<"2";
                    //更新数据
                    Update(this->select->x,this->select->y,ptr->x,ptr->y);
                    //提示下一名玩家下棋，并且设置图标            
                    //根据当前是哪一个玩家进行分来
                    switch (this -> control)
                    {
                        case 1:
                        {
                            this->select->setIcon(QIcon(":/image2/1.png"));
                            if(type == 2 || type == 6)
                            {control =2 ; ui->label->setText("紫色下棋");}
                          else
                          {control=4;
                              ui->label->setText("紫色下棋");
                          }

                            break;
                        }
                        case 2:
                        {
                            this->select->setIcon(QIcon(":/image2/2.png"));
                        if(type==6)
                        {control = 3;
                            ui->label->setText("蓝色下棋");}
                        else if(type==2)
                        {
                            control=1;
                           ui->label->setText("蓝色下棋");
                        }
                            break;
                        }
                        case 3:
                        {
                            this->select->setIcon(QIcon(":/image2/3.png"));
                            control = 4;
                            ui->label->setText("橙色下棋");
                            break;
                        }
                        case 4:
                        {
                            this->select->setIcon(QIcon(":/image2/4.png"));
                            control = 5;
                            ui->label->setText("黄色下棋");
                            break;
                        }
                        case 5:
                        {
                            this->select->setIcon(QIcon(":/image2/5.png"));
                        if(type==6)
                           {control = 6;
                            ui->label->setText("红色下棋");
                           }
                        else if(type==3)
                        {
                            control=1;
                            ui->label->setText("红色下棋");
                        }
                            break;
                        }
                        case 6:
                        {
                            this->select->setIcon(QIcon(":/image2/6.png"));
                            control = 1;
                            ui->label->setText("绿色下棋");
                            break;
                        }
                        default:
                    ;

                    }

                }
                this->clickTime=0;
                this->select = nullptr;
                break;
            }
        }
    }

}
```

具体细节见上面的注释。在某个棋子处于选中状态的时候，如果点击另外一个空白的合法移动的地方，棋子的图标就会移动到对应的位置。需要注意的是，由于玩家点击的位置不一定正好处于棋子的正中心的坐标，所以允许玩家点击距离棋子中心一定的误差即可视为选中当前棋子。选中以后判断是不是合法的移动，是的话，就修改相关棋子和棋盘的信息，然后改变select和clicktime的状态和玩家的控制权。



#### 5、判断玩家的移动是否合法的函数

```c++
bool Widget :: isLegalMove(Chess* prechess , Coordinate_struct* des)
{
    //目标移动位置被占有
    if (des -> ifOccupied != 0) return 0;
    //移动的目标位置不在棋盘上
    if (!onboard(des -> x, des -> y)) return 0;
    //横向平移
    if (abs(prechess -> x - des -> x) == half_edge * 2 && abs(prechess -> y - des -> y) == 0 ) return 1;
    //斜向平移
    qDebug()<<prechess->x<<" "<<prechess->y<<" "<<des->x<<" "<<des->y;
    if (abs(prechess -> x - des -> x) == half_edge  && abs(prechess -> y - des -> y) == _edge) return 1;
    //下面考虑是连续跳跃的情况
    count++; // 定义为private，用来计数打标签

    return ifreach(prechess -> x, prechess -> y,  des);
}
```

具体细节见上面的注释。



#### 6、递归用于判断连跳的函数

```c++
bool Widget :: ifreach(int x , int y , Coordinate_struct* des)
{
    //dx,dy的初始化
    /*
       const int dx[6] = {-(half_edge * 2), -half_edge, half_edge, half_edge * 2, half_edge , -half_edge};
       const int dy[6] = {0, -_edge, -_edge, 0, _edge, _edge};
     */

    // 如果已经递归到达目标
    if (x == des -> x && y == des -> y) return 1;
    // 如果已经访问过，那么就不重复递归
    if (iftouch[x][y] == count) return 0;
    //打上已经访问过的标签
    iftouch[x][y] = count;
    //朝6个可能的方向跳跃
    for (int i = 0; i < 6; i++)
    {
        int tx1 =  x + dx[i]; int ty1 =  y + dy[i];
        int tx2 =  x + dx[i] * 2; int ty2 = y + dy[i] * 2; //dx dy 是什么？
        //如果这两个位置有任何一个不在有效的棋盘范围内，显然是不合法的
        if (!onboard(tx1,ty1) || !onboard(tx2,ty2)) continue;
        //下面我们判断作为中间点的位置上是否有棋子，若没有那么也不能跳跃。
        int mx1 = get_Coordinatex(g[tx1][ty1]); int my1 = get_Coordinatey(g[tx1][ty1]);
        if (Coordinate[mx1][my1].ifOccupied == 0) continue;
        //如果跳跃的目标地点上面有棋子，那么也不能跳跃。
        int mx2 = get_Coordinatex(g[tx2][ty2]); int my2 = get_Coordinatey(g[tx2][ty2]);
        if (Coordinate[mx2][my2].ifOccupied > 0) continue;
        //下面我们尝试移动
        if (ifreach(tx2,ty2,des)) return 1;
    }
    //否则没有递归到成功移动的情况
    return 0;
}
```

具体细节见上面的注释。总体来说递归搜索出所有可能连续跳跃到的位置，然后和当前操作到的位置进行比较，判断是否有相等的，iftouch判断当前的位置是否有访问过的，dx，dy是方向数组，就和经典的走迷宫问题是一样的。



### 五、遇到的问题以及解决的方法

我之前并不怎么会用qt，对整个游戏的设计长什么样子并没有什么概念，后来到github上面找了一些类似的代码实现，学习和参考了一下基本的构架，然后这一阶段的任务的难度并不算大，剩下的一些具体的内容和细节的实现就主要是代码风格要好一些，思路逻辑清晰，就基本没什么大问题，剩下的就是体力活。

在代码调试的过程中，我发现有些编写时遗留下的问题是对qt界面人机交互的过程中槽函数的调用的过程理解不是很深入。通过调试和实验，我有了更加深入的理解。

另外，添加和修改代码在和小组成员合作的时候也容易引入一些bug，以后要充分交流和沟通。



### 六、游戏运行结果截图

现在第一阶段所有的要求已经全部完成了，经实验已经可以正常运行，下面是游戏试玩的一些截图。

<img src="/Users/delia/Library/Application Support/typora-user-images/image-20220415214358139.png" alt="image-20220415214358139" style="zoom:60%;" />



<img src="/Users/delia/Library/Application Support/typora-user-images/image-20220415214419321.png" alt="image-20220415214419321" style="zoom:60%;" />

<img src="/Users/delia/Library/Application Support/typora-user-images/image-20220415214435883.png" alt="image-20220415214435883" style="zoom:60%;" />

<img src="/Users/delia/Library/Application Support/typora-user-images/image-20220415214447519.png" alt="image-20220415214447519" style="zoom:60%;" />
