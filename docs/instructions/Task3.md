# 第三阶段任务：跳棋游戏功能扩展

## DeadLine

请在 2022 年 6 月 17 日 23:59 前完成这一阶段的任务并提交 Pull Request。

!!! note
    由于期末考试提前到六月初，请合理安排期末复习与完成大作业的时间。

    我们会根据学校政策实时调整大作业第三阶段的安排。


## 任务简述

根据前期自愿报名的结果，[第 4 小组](../submission/Team4/index.md)将在本阶段完善服务端的功能，其余小组将在本阶段完善客户端的功能。

请根据自己所属的小组查看你们的任务。

### 服务端功能要求

在本阶段，跳棋游戏服务端需额外下列功能：

+ 房间功能，即允许多盘对局在同一服务端上**同时**进行。
+ 支持查看指定房间的对局情况。

### 客户端功能要求

在本阶段，跳棋游戏客户端需额外支持程序自动下棋的功能，并允许玩家随时在人工下棋与自动下棋之间切换。

为降低算法设计与实现难度，自动下棋程序只需考虑 $2$ 人对局和 $3$ 人对局的情形。

## 行为约定

根据前两阶段的完成情况，这里对跳棋游戏的最终成品的部分行为进行统一约定：

+ 客户端应留有输入服务端 IP 地址和端口的接口，不应将上述两个信息硬编码入代码中。
+ 当对局进行到仅剩 $1$ 名玩家未获得胜利且未被判负时，判定该玩家自动获得胜利。
+ 为保证客户端之间计时的统一，我们更新了 `START_TURN_OP` 请求的格式与行为，请相应地修改你的实现。
+ 在联机对局时，每位玩家每回合的时限是 $15$ 秒。

## 可选功能列表

你可以根据小组实际情况选择是否实现下列列表中的部分或全部功能。

+ （客户端）进行适当的视角变化，使得每位玩家的初始区域在自己的客户端上均被显示在正下方（即区域 D）
+ （客户端）允许玩家查看指定玩家的最后一步行棋路径。
+ （客户端、服务端）向玩家展示连跳操作的具体过程，而非从起点直接跳到终点。
+ （服务端）对局历史：允许用户在任意时刻查看当前对局的任一历史局面。
+ （服务端）复盘模拟：在对局历史功能的基础上允许用户随时做出与对局不同的尝试。

## 大作业总结

请在完成开发任务后在 `docs/submission/` 下你的小组文件夹中上传你第三阶段的阶段性报告，并相应地修改 `mkdocs.yml`。在报告中请简要描述你的小组分工、代码框架设计、遇到的问题及解决方法和其他你认为有必要的内容。

除此之外，你还需要将本小组的大作业代码上传到 Github 上并在实验报告中附上项目仓库的链接。

!!! warning
    如果你在前两阶段中并未按要求上传阶段性报告，在本阶段你应上传一个完整的大作业总结报告（内容参考阶段性报告），**否则你将无法获得大作业实验报告的分数**。
