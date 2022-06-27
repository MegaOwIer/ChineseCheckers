# 第一小组第三阶段实验报告
第三阶段，本组完成了AI自动下棋的程序，能够支持2人对局和3人对局的托管功能，并允许玩家随时在人工下棋与自动下棋之间切换。另外，我们还实现了客户端的全部可选功能，包括视角变换，连跳动画，以及最后一步行棋路径的查看功能。

小组分工：  

詹倚飞：  

1.程序全部子界面的ui设计  

2.程序全部图形素材的设计制作  

3.第二阶段服务端的搭建和与客户端的对接  

4.第三阶段游戏行棋动画、行棋音效与背景音乐的添加以及查看上一步功能  


欧竞阳：  

1.第一阶段跳棋整体框架的搭建  

2.第二、三阶段客户端的接口实现  

3.附加的视角切换功能  


赵宸：  

1.第三阶段托管功能  


项目地址：https://github.com/VirusCollector/Group1_ChineseCheckers

## 托管程序
### 基本思想
&ensp;&ensp;&ensp;&ensp;在最早筹划托管部分的写作时，我们就有两个方向，一个是单纯采用短时记忆预演算法，即采用非长时记忆化、单纯依靠价值函数和蒙特卡洛束搜索，通过有效搜索剪枝（减掉明显不符合目标实现的策略步）和alpha-beta剪枝（在认为敌我均为理性人的前提下，选择当前搜索深度下该决策者的最优决策步）；另一个则采用机器学习中的强化学习算法，采用n步自举法：通过ε-贪心控制随机游走，采用TD(λ)，根据第一种方法的策略π，给出每一个状态下的V(π|a)，并由此解决短时记忆搜索死板、速度慢、短视的问题。  

&ensp;&ensp;&ensp;&ensp;当然，最后还是用了第一种<s>（机器学习迭代至今不收敛）</s>，但我机器学习版的代码部分依然实现了，也会在后文给出粗略的解释。
### 预演搜索版
&ensp;&ensp;&ensp;&ensp;这是我们程序最终采用的版本。基本的实现逻辑：根据游戏人数制定价值函数，通过use函数调用后，遍历当前所有的走法，将走出一步后的棋盘重建player类，将建好的棋盘传给alpha-beta搜索函数，最终评估所有走法中value最大的走法，将走法以vector<'QPoint>的形式传出，交给网络端传出指令，同时移动本地棋盘。

#### 价值函数
&ensp;&ensp;&ensp;&ensp;函数的主要意义是对当前的局面做一个评估，与机器学习不同，预演算法的value函数是对整个局面的评估，是运行n步之后强行截胡，然后打分。因此在这样价值观（？）的驱使下，程序很难实现搭桥的操作，也很难对“上一步”做出评估。 

&ensp;&ensp;&ensp;&ensp;对于两人的情况，由于是绝对的零和主义，因此我们的程序要紧盯对方的走向。首先对对方的形势做出评估。考虑到整体程序坐标的给出方式，我们不难发现，采用“x+y”可以准确的评估对面的棋子到我方大营的距离。  

<font size = 2>

(PS：player是传进来的当前的各个玩家类，类内有10个marbles存有其相对坐标和绝对坐标，对于每个玩家来说自己所在的大营坐标是正的，所以算分要减。)  

</font>

分数估计我们依据以下三条原则： 

1.以棋子进入对面大营为第一目标，以棋子尽可能往前靠为第二目标，以阻止对方获胜为第三目标。  

2.尽可能不在本家留下孤寡老人，最大程度上避免被闷宫的情况。  

3.尽可能不要偏离中线，避免程序一昧前进而偏到偏营。

&emsp;&emsp;而执行value估计我们依据对家的棋子坐标代数和，人为的将估计分为三个阶段：前期、中期和后期。前期以尽可能往前走为目标，走的越靠前，就越能干扰对方的部署，防止对方背谱；同时对离开自家大营的棋子给予奖励；中期以整体战线往前推进为目的，惩罚尚未离开大营的棋子，少量奖励进入对方大营的棋子，同时控制不要过度偏离中线；后期以不留子为目的，尽可能控制顽固分子拉帮结派前往大营，增大对偏离中线的惩罚。  

&ensp;&ensp;&ensp;&ensp;三人的得分评估也是同理，在这里不详细给出了。主要变化在将中场时机的判断从原本的对方，变成了“两个对方中走的快的那一个”，增大了偏离中线的惩罚，同时放宽了偏离中线的判准。

#### 搜索函数
&ensp;&ensp;&ensp;&ensp;严格意义上的搜索函数是由两部分构成的，但另一部分涉及接口，后续在做介绍。搜索过程采用alpha-beta剪枝，同时在搜索中删去了明显不符合要求的走法，进一步缩小了搜索范围。搜索过程包括对当前某个棋子的所有可能路线的搜索，并将可能的路线采用marbles类中的hint_points（vector类型）储存，在这个搜索中删去不合理走法。  

正式的搜索函数：
<font size = 2>

```c++
int AI::alpha_beta(std::vector<Player*> now,int alpha,int beta,int depth,int num)
{
    int value;
    for(int i = 0; i < num; i++)
    {
    //如果发现当前状态下要下棋的玩家已经获胜，直接return +∞ 表示该玩家一定会沿这条路走，同时终止后续的深入搜索。
        if(now[i]->win())
        {
            if(i == 0) return 10000;
            else return -10000;
        }
    }
    if(!depth)//每深入一次depth-=1，到depth == 0的时候return一个value。
    {
        if(num == 2)
        {
            return get_value2(now);
        }
        if(num == 3)
        {
            return get_value3(now);
        }
        if(num == 6)
        {
            return get_value6(now);
        }
    }
    for(int i = 0; i < 10; i++) 
    //it's depth%num's turn, who has ten marbles.
    {
//调用一个自定义的搜索函数，实现部分搜索（now是当前所有玩家的player类，depth%num是当前搜索中要下棋的玩家，i为要移动的棋子）。
        now[depth%num]->marbles[i]->hint_points.clear();
        search_hint_point(now, depth%num,i);
        for(unsigned int j = 1; j < now[depth%num]->marbles[i]->hint_points.size(); j++)//the ith has j ways to move.
        {
            std::vector<Player*> next;
            for(int k = 0; k < num; k++)//set a chess for this way.
            {
                next.push_back(new Player(now[k],i,now[depth%num]->marbles[i]->hint_points[j],!(depth%num)));
        //将原有的player类、要移动的玩家、棋子和移动信息传入自定义的player构造函数，并将新构造的player类传入下一次递归。
            }
        //如果发生敌我交换，则对方的alpha就是我的-beta，表示当前走法对他来说能取到的最坏结果。
            if(depth%num == 0||depth%num == 1)
            {
                value = -alpha_beta(next, -beta, -alpha, depth-1, num);
                alpha = (alpha>value)? value: alpha;
                if(alpha>= beta) return alpha;//剪枝，减掉“不理性”的走法。
            }
            //如果没有发生敌我交换，则在我的视角他们两个的目的是一致的，因此他们有共同的alpha，beta
            else
            {
                value = alpha_beta(next, alpha, beta, depth-1, num);
                alpha = (alpha>value)? value : alpha;
                if(alpha>= beta) return alpha;//剪掉敌方不利于组织最优的解法
            }
            for(unsigned int is = 0;is < next.size();is++)
            {
                delete next[is];
            }
            next.clear();
        }
    }
    return alpha;//当前的走法比之前的beta都要优，是目前为止的最优走法。
}
```
</font>

#### 接口&第一步搜索
&ensp;&ensp;&ensp;&ensp;因为第一步搜索之后的局势是要一次要出现的，因此单独写便于做接口，具体的过程很简单：
```c++
std::vector<Player*> now;
for(int k = 0 ;k < num_of_players;k++)
{
    now.push_back(new Player(players[k],i,players[0]->marbles[i]->hint_points[j],!k));
}//初始化第一步，完成第一步的全搜索，并储存当前棋盘；

temp = alpha_beta(now,20000,-20000,_n,num_of_players);//调用alpha-beta函数，给出当前走法的局势分（保证传入的alpha足够大，beta足够小）
if(temp>max)//如果大于最大值，予以保留
{
    max = temp;
    number = i;
    best = players[0]->marbles[i]->hint_points[j];
}
```

### 机器学习版
&ensp;&ensp;&ensp;&ensp;这里采用强化学习中最基础的n步自举法完成托管设计。即以ε的概率随机游走，以（1-ε）的概率按照上文给出的预演算法的策略π行走，按这样的策略π'利用蒙特卡洛树搜索，即一次搜n步，再在不同的叶子节点展开，在最后给出底部搜索结果的全展开，根据棋子的移动赋分（不同于之前的局势赋分），最后在按γ-折迭代当前得分。迭代方法采用时序差分的迭代法，即V(S) = V(S) + α[R + γ*V(S')-V(S)]的方法(V(S),V(S')表示需要迭代得而当前和下一步的局势分；R表示单步收益函数；γ为折扣系数，个人认为是与上一种方法相比最大的优越性之所在),实际的实现主要由训练搜索函数，训练函数，使用函数和接口构成，接口除了文件的读入读出外与上文基本相同，不展开介绍。  

&ensp;&ensp;&ensp;&ensp;以下为后续使用的参数介绍：
<font size = 2>
```c++
#define e 10//即为ε
#define t 3//TD(λ)'s λ.
#define gama 0.8//折扣函数折扣比
#define alpha1 0.2//步长参数
#define playernum 3//训练的游戏人数
#define n 50//既定单次训练的训练步数
```
</font>
#### 训练搜索函数
主要就是用来贯彻落实<font size = 1>（？）</font>ε-贪心算法和蒙特卡洛树搜索的，通过这个函数，将未来几步的走法存入到place中，便于后续TD-training使用。
<font size = 2>

```c++

void AI::TD_searching(std::vector<int> &num,std::vector<QPoint> &place)
//num依次压栈表示每次移动的棋子标号；place依次压栈表示每次移动的棋子的移动目标
{
    int ans;
    for(int i=0; i < t; i++)
    {
        if(rand() % 10 >= e)//以1-ε概率按策略π行走
        {
            place.push_back(use(ans));//use为预演算法的应用函数，这里即为应用策略π决定路径
            num.push_back(ans);//按策略π确定棋子编号
            temp_players[i%num_of_players]->marbles[ans]->boardposition = place.back();
            temp_players[i%num_of_players]->marbles[ans]->update_playerposition();
        }
        else//以ε概率随机游走
        {
            ans = rand()%10;//随机确定棋子编号
            if(players[i%num_of_players]->marbles[ans]->hint_points.empty() == true)
            {
                players[i%num_of_players]->marbles[ans]->get_hint_points();
            }
            int way = rand()%(players[i%num_of_players]->marbles[ans]->hint_points.size());
            //随机确定棋子的移动方向
            place.push_back(players[i%num_of_players]->marbles[ans]->hint_points[way]);
            num.push_back(ans);
            temp_players[i%num_of_players]->marbles[ans]->boardposition = place.back();
            temp_players[i%num_of_players]->marbles[ans]->update_playerposition();//更新棋盘
        }
    }
}
```

</font>

#### 训练函数
&ensp;&ensp;&ensp;&ensp;为了弥补在轨学习所产生的数据堆积，引起“有些点早已收敛，而有些局面还没开始”的情况，我们决定采取随机训练，选定中局作为训练起点，保证训练样本的均匀覆盖。具体的实现方式为在棋盘中开一个ui按钮，在中局启动训练，将当前棋盘状态传入代码；同时，在训练函数中给出收益函数R，采用和预演中一样的按坐标“x+y”的思路给分。通知采用γ折扣保证程序能足够“短视”，从而起到贪心的效果；另一方面，α不太大也能保证收敛能够持续进行。  


<font size = 2>

```c++
void AI::TD_training()
{
    readin();//将之前的训练样本读入到save中，从而接着之前的训练继续
    std::vector<int> num;
    std::vector<QPoint> place;
    int i=0,x,x_new,y,y_new,value = 0,flag_0 = 0;
    /*x，y表示当前的棋子坐标，x，y-new表示要前往的位置，
    value代表收益函数，flag = 0表示搜索后发现之前没有迭代过这个局势，
    flag = 1表示之前迭代过，直接继续迭代即可；*/
    TD *td_now, *td_past;//past表示当前位置，now表示下一步走后局势
    while(i < n)//如果训练次数小于既定次数
    {
        i++;
        num.clear();
        place.clear();
        temp_players.clear();
        for(Player* p:players)
        {
            temp_players.push_back(p);
        }
        TD_searching(num,place);
        temp_players.clear();
        for(Player* p:players)
        {
            temp_players.push_back(p);
        }
        flag_0 = 0;
        for(TD* p:save)
        {
            if(equal(p->players,temp_players))
            //自定义相等，即当前的十个棋子的乱序位置与读取的save一致
            {
                td_past = p;
                flag_0 = 1;
                break;
            }
        }
        if(flag_0 == 0)//此前没有这个局势，新建一个
        {
            td_now = new TD(temp_players, 0);
        }
        for(int i = 0; i < t; i++)
        {
            x = temp_players[i]->marbles[num[i]]->playerposition.x();
            y = temp_players[i]->marbles[num[i]]->playerposition.y();
            temp_players[i]->marbles[num[i]]->boardposition = place[i];
            x_new = place[i].x();
            y_new = place[i].y();
            for(TD* p:save)
            {
                if(equal(p->players, temp_players))
                //搜索相等的存储，进行迭代
                {
                    td_now->values += p->values * gama;
                    break;
                }
            }
            value += pow(gama, i) * (x_new+y_new-x-y);
        }
        for(int i = 0; i < 10; i++)
        {
            if(temp_players[i%num_of_players]->marbles[i]->hint_points.empty() == true)
            {
                temp_players[i%num_of_players]->marbles[i]->get_hint_points();
            }//如果这个点的走法还没搜过，先进行搜索
            for(QPoint j:temp_players[i%num_of_players]->marbles[i]->hint_points)
            {
                value += pow(gama,t) * (j.x()+j.y()-(temp_players[i%num_of_players]->marbles[i]->x()
                +temp_players[i%num_of_players]->marbles[i]->y()));//采用γ-折进行分数累计
            }
        }
        td_now->values = value;
        if(flag_0 == 0)//这个点尚未存储过，在save中新建档
        {
            save.push_back(td_now);
            delete td_now;
        }
        else//否则直接搜索然后更新原有档案
        {
            td_past->values = td_past->values + alpha1 * (td_now->values - td_past->values);
        }
        int random = rand()%10;
        //一次搜索完成，将当前局势随机走一步转到一个新的局势进行迭代。
        if(players[i%num_of_players]->marbles[random]->hint_points.empty() == true)
        {
            players[i%num_of_players]->marbles[random]->get_hint_points();
        }
        players[i%num_of_players]->marbles[random]->boardposition = 
        players[i%num_of_players]->marbles[random]->
        hint_points[rand()%(players[i%num_of_players]->marbles[random]->hint_points.size())];
        players[i%num_of_players]->marbles[random]->update_playerposition();
    }
    sendout();//单次学习完成，将存储的内容吐到存储的文档里
}
```
</font>

## 视角变换

在第二阶段，每个玩家在本地客户端显示的初始区域均为服务端发送的区域。视角变换后，每位玩家的初始区域在自己的客户端上均被显示在正下方（即区域 D），这时每个棋子的新坐标相当于对旧坐标施加一个线性变换后得到的结果。我们可以通过每名玩家原来的区域和视角变换变换后的区域（即区域 D），计算出线性变换的矩阵$A$，然后对于从服务端接受的MOVE_OP操作数中的每个坐标，通过左乘矩阵$A$，算出新坐标，再调用移动棋子的函数；同理，向服务端发送的坐标，也需要左乘矩阵$A^{-1}$,算出旧坐标后再发送。

## 连跳动画
连跳过程其实就是每一步跳跃过程的合并，不过每一次跳跃有相应的间隔时间。由于在点击位移终点时已经完成了跳跃全路径的搜索，因而本程序的思路是从起始点到终点每两个相邻点都要展示一次平移动画，间隔0.5秒。这里运用QPropertyAnimation类的平移动画库实现平移效果最简洁。

## 最后一步行棋路径的查看功能
主要想法依然是玩家能够点击任意在场游戏玩家来查看他们的上一步行棋路径，因此为了不在主界面加6个按钮破坏界面美感，我在右上角单独设置了“查看上一步”按钮，点击某个玩家即可调用该玩家类的方法展示他的上一步行棋路径。

## 问题&&解决
1.在实现连跳动画后，我们发现AI的速度大幅降低,采用原来的搜索深度完全无法在15秒内给出结果。经过一步步版本回溯后我们发现，问题出在``marble``类添加的一个``music``类成员。这个成员继承了``QWidget``类，初始化会消耗较长的时间。而AI在搜索过程中会创建大量的``marble``类对象，每次初始化时都会有大量的时间浪费。  

解决方案:将音乐音效类转移至widget主窗口，需要音乐音效时调用主窗口的函数。  


2.关于连续跳跃的动画，不能简单的运用循环来完成，因为动画类指针每次指向的只能是1个动画，在程序运行速度<<0.5s时，后面动画的代码已经执行，但由于前面的动画还未结束故不能展示出后面动画的效果。相应的解决方案是在每一个动画发送finished()信号后再执行下一个动画，即可使动画具有连贯性。

## 项目附加功能
1.背景音乐和行棋音效。  

2.展示行棋步数、当前行棋方、剩余时间。  

3.实现6人托管。  

4.可以查看当前房间的所有玩家。  


## 结语
&emsp;&emsp;执笔到此，回顾做大作业的过程，内心还是有非常多的感悟的：找图抠图时的无比抓狂，设计ui界面时的仔细修参，做服务器时的步步为营，调bug时的无能为力......思来总有些“衣带渐宽终不悔”的感觉。这个工程项目于我们而言，并不是仅仅了解一种新的GUI设计工具和工程设计的艰辛，也让我们体会到了团队合作的重要性和必要性，在彼此磨合中增加了各方面的能力——这种程序员素养的修炼，我认为同他的技术能力一样重要。  

&emsp;&emsp;最后,在此感谢我们团队每一位认真刻苦的队员，同时也感谢在我们困惑之际给于我们帮助的两位助教和部分同学，项目的顺利完成离不开你们的帮助。


