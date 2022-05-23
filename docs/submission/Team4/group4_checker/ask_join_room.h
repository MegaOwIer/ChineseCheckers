#ifndef ASK_JOIN_ROOM_H
#define ASK_JOIN_ROOM_H

#include <QWidget>

namespace Ui
{
    class ask_join_room;
}

class ask_join_room : public QWidget
{
    Q_OBJECT

public:
    explicit ask_join_room(QWidget *parent = nullptr);
    ~ask_join_room();

    void drawUserList(QPainter &painter);

public slots:
    //------------------单个用户自己的操作全部被封装在了ask_join_room类中，获取其它用户信息的操作则被封装在了board类中------------------

    /**
     * @brief setUserName 设置自己的名字,直接在users数组中修改
     */
    void setUserName();

    /**
     * @brief clickEnter 设置房间号和名字，直接在playerList数组中修改
     */
    void clickEnter();

    /**
     * @brief setReady 设置自己的准备状态，直接在playerList数组中修改
     */
    void clickReady();

    /**
     * @brief setRoomNumber
     */
    void setRoomNumber();

    /**
     * @brief forbidButton 如果点击了Enter按钮，就直接禁用Enter,OK1,OK2三个按钮
     */
    void forbidButton();

    /**
     * @brief updateTable 显示当前房间内所有用户的名字和准备状态，并且应该能够更新这些状态
     * 重新绘制玩家列表,每次被相关的信号函数触发时就重新进行一次
     * 这个函数的使用类似于Qt的重绘函数
     */
    void updateTable();

    void showWINNER_LIST(QString detail);

    //TODO：leave功能待开发
    void init();

signals:

    /**
     * @brief sendReady 信号函数，触发Board::sendReady函数
     */
    void sendReady();

private:
    Ui::ask_join_room *ui;
};

#endif // ASK_JOIN_ROOM_H
