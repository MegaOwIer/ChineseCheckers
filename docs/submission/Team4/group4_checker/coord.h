#ifndef COORD_H
#define COORD_H

#include <QWidget>
#include <QPoint>

// 横坐标的单位
#define XUNIT 23

// 纵坐标的单位
#define YUNIT 40

// 声明一个int变量的新名称
typedef int COORD;

typedef struct
{
    int _x;
    int _y;
} standardCoord;


// 这个变量用来描述棋子的颜色
// null代表这个点上没有棋子

// 定义棋盘的更新模式
enum _updateMode { updatePieceColor, updateFeasibility, updateChosenState };

enum _pieceColor { red, yellow, blue, green, cyan, purple, null };

class Coord
{
    // 这里继承QObject类就是为了用一下信号和槽hhh

public:

    // 构造函数
    Coord();
    Coord(COORD coord_x,
          COORD coord_y);
    Coord(Coord& coord);

    _pieceColor targetColor = null;    //终局判断的目标颜色

    // ------------------------坐标转换---------------------------

    // 将QPoint系统坐标转化为coord坐标
    QPoint coord2QPoint()const;

    // 将coord坐标转化为用于传递的标准坐标
    standardCoord coord2standardCoord();

    // QPoint系统的x坐标
    COORD coord_x;

    // QPoint系统的y坐标
    COORD coord_y;

    // ------------------------颜色处理------------------------------

    // 用来获取颜色信息
    _pieceColor getColor();

    // -------------------------可行性处理--------------------------
    bool        getFeasibility();

    // 设置当前位置是否有棋槽
    void        setHaveSlot(bool agu_haveSlot);

    // 获取当前位置是否有棋槽的信息
    bool        getHaveSlot();

    // 选中状态处理
    bool        getChosenState();

    // 当前位置能否选中处理
    // void        setEnable(bool enable);
    // bool        getEnable();

    // -------------------------棋盘信息设置函数---------------------

    void setColor(_pieceColor color);

    void setFeasibility(bool feasibility);

    void setChosenState(bool chosenState);

    // -------------------------其它函数-------------------------

    /**
     * @brief operator == 比较两个坐标是否相等
     * @param coord 第二个坐标
     * @return 相等返回true，否则false
     */
    bool operator==(Coord coord);



public slots:

    // 更新棋盘状态的函数(信息设置)
    // 这里删除的原因是槽函数不能是重载函数
    // void updateBoardInfo();

    // void updateBoardInfo(_updateMode);

private:

    // 用来表示当前位置是否可选
    // bool enable;

    // 用来表示位置是否有棋槽
    bool haveSlot = false;

    // 用来表示当前位置的选择状态
    bool chosenState = false;

    // 用来表示当前位置是否具有行棋可行性
    bool feasibility = false;

    _pieceColor pieceColor = null;

};

//-------------------------类外坐标转换函数---------------------------------------------

Coord qPoint2Coord(const QPoint& point);

// 将用于传递的标准坐标转化为coord坐标
Coord standardCoord2coord(standardCoord standardCoord);

#endif // ifndef COORD_H
