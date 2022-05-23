#ifndef HELLO_H
#define HELLO_H

#include <QWidget>
#include <QString>

namespace Ui
{
    class Hello;
}

class Hello : public QWidget
{
    Q_OBJECT

public:

    explicit Hello(QWidget *parent = nullptr);
    ~Hello();


    // 设置信号函数
    void connectBox();
    Ui::Hello *ui;

public slots:

    void set_a_single_mode();
    void set_AI_mode();
    void set_network_mode();

    // 获得当前玩家的人数
    void get_userCnt();
};

#endif // HELLO_H
