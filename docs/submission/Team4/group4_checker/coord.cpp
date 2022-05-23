#include "coord.h"

// #include "iostream"

// using namespace std;

// center是棋盘的中心
QPoint center(366, 375);

//构造函数定义
Coord::Coord(COORD coord_x, COORD coord_y) : coord_x(coord_x), coord_y(coord_y),
    chosenState(false), feasibility(false), pieceColor(null), haveSlot(false)
{}

Coord::Coord() :   chosenState(false), feasibility(false), pieceColor(null),
    haveSlot(false) {}

Coord::Coord(Coord& coord) : coord_x(coord.coord_x), coord_y(coord.coord_y),
    feasibility(coord.feasibility), chosenState(coord.chosenState),
    pieceColor(coord.pieceColor), haveSlot(coord.haveSlot)
{}

// -----------------------------------------------坐标转换类--------------------------------------
// 将coord坐标转化为QPoint系统坐标
QPoint Coord::coord2QPoint() const
{
    QPoint point(center.x() + this->coord_x * XUNIT,
                 center.y() - this->coord_y * YUNIT);

    //    if ((this->coord_x == 0) &&
    //        (this->coord_y ==
    //         6)) qDebug() << "enter and change Coord to point " << point.x()
    // << " " <<
    //            point.y() << "\n";

    return point;
}

// 将QPoint系统坐标转化为coord坐标(不需要使用对象来调用)
Coord qPoint2Coord(const QPoint& point)
{
    Coord coord((point.x() - center.x()) / XUNIT,
                (point.y() - center.y()) / YUNIT);

    return coord;
}

//将coord坐标转化为用于传递的标准坐标()
standardCoord Coord::coord2standardCoord()
{
    standardCoord temp;
    temp._x = (this->coord_x - this->coord_y) / 2; //横坐标发生变化，需要处理
    temp._y = this->coord_y;  //纵坐标不发生变化，无需处理
    return temp;
}

//将用于传递的标准坐标转化为coord坐标
Coord standardCoord2coord(standardCoord standarCoord)
{
    Coord coord;
    coord.coord_y = standarCoord._y;
    coord.coord_x = 2 * standarCoord._x + coord.coord_y;
    return coord;
}

// ------------------------------------------------颜色相关设置--------------------------------------
// 用来获取颜色信息
_pieceColor Coord::getColor()
{
    return this->pieceColor;
}

// ------------------------------------------------棋槽相关设置--------------------------------------
void Coord::setHaveSlot(bool agu_haveSlot)
{
    this->haveSlot = agu_haveSlot;
}

bool Coord::getHaveSlot()
{
    return this->haveSlot;
}

// -------------------------------------------------可行性相关处理-----------------------------------------

bool Coord::getFeasibility()
{
    return this->feasibility;
}

// 选中状态处理
bool Coord::getChosenState()
{
    return this->chosenState;
}

// 当前位置能否选中处理
// void Coord::setEnable(bool enable)
// {
//    this->enable = enable;
// }

// bool Coord::getEnable()
// {
//    return this->enable;
// }

// ------------------------------------------------信息函数设置----------------------------------------
void Coord::setColor(_pieceColor color)
{
    this->pieceColor = color;
}

void Coord::setFeasibility(bool feasibility)
{
    this->feasibility = feasibility;
}

void Coord::setChosenState(bool chosenState)
{
    this->chosenState = chosenState;
}

// 只找到空区域则返回nullCoord
Coord nullCoord(-99, -99);

// --------------------------------------------------重载坐标比对运算符函数"=="----------------------------------
bool Coord::operator==(Coord coord)
{
    if (this->coord_x == coord.coord_x && this->coord_y == coord.coord_y)
    {
        return true;
    }
    else
    {
        return false;
    }
}
