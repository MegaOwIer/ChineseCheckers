#ifndef QWHTIMIMG_H
#define QWHTIMIMG_H

/*
 * 可设置倒计时秒数
 * 可设置文本颜色
 * 可设置背景填充
 */

#include <QWidget>
#include <QPropertyAnimation>
#include <QTimer>
#include <QPaintEvent>
#include <QPainter>
#include <QPalette>

class QWHTimimg : public QWidget
{
    Q_OBJECT
public:
    explicit QWHTimimg(QWidget *parent = nullptr);
    ~QWHTimimg();

    //设置倒计时秒数
    void setSecs(int secs);

    //设置文本颜色
    void setTextColor(const QColor &color);

    //设置背景填充
    void setBgBrush(const QBrush &brush);

    //开启倒计时
    void start(int secs = -1);

    //获取倒计时秒数
    int getSecs()   const;

    //获取文本颜色
    QColor getTextColor()   const;

    //获取背景填充
    QBrush getBgBrush() const;

protected:
    void paintEvent(QPaintEvent *event);

signals:
    void timingFinished();              //倒计时结束的信号函数

private slots:
    void onTimeout();

private:
    int m_secs;                         //倒计时总时间（秒）
    int m_curSecs;                      //当前倒计时秒数
    bool m_zoomIn;                      //放大或缩小
    QColor m_textColor;                 //文本颜色
    QBrush m_bgBrush;                   //背景填充
    QTimer *m_timer;                    //定时器控制放大缩小
    QPropertyAnimation *m_animation;    //动画实现放大缩小
};

#endif // QWHTIMIMG_H
