#include "qwhtimimg.h"
#include <QtMath>
#include <QDebug>
#include <board.h>
#include <coord.h>
#include "predefine.h"
extern Board *board;

QWHTimimg::QWHTimimg(QWidget *parent) : QWidget(parent)
{
    m_secs = TIME;             //倒计时总时间
    m_curSecs = m_secs;      //当前倒计时秒数
    m_zoomIn = false;        //放大或缩小
    m_textColor = Qt::white; //文本颜色

    //下面的函数用于设置背景透明
    QPalette pal = palette();
    pal.setColor(QPalette::Window, QColor(0x00, 0xff, 0x00, 0x00));
    setPalette(pal);

    //下面设置位置和大小
    //位置和大小刚好能适应next按键的位置
    QRect rect(Coord(16, 6).coord2QPoint().x(), Coord(16, 6).coord2QPoint().y(), Coord(28, 4).coord2QPoint().x() - Coord(16, 6).coord2QPoint().x(), Coord(28, 4).coord2QPoint().y() - Coord(28, 6).coord2QPoint().y());
    this->setGeometry(rect);

    //下面这些是倒计时的内部实现，不用管，copy就行
    m_animation = new QPropertyAnimation(this, "", this);
    m_animation->setStartValue(100);
    m_animation->setEndValue(300);
    m_animation->setDuration(400);

    connect(m_animation, &QPropertyAnimation::valueChanged, this, [&](const QVariant & value)
    {
        update();
    });

    m_timer = new QTimer(this);
    m_timer->setInterval(500);

    connect(this->m_timer, &QTimer::timeout, this, &QWHTimimg::onTimeout);

    //倒计时结束触发服务端强制清除措施
    connect(this, &QWHTimimg::timingFinished, board, &Board::server_find_TIME_OUT);
}

QWHTimimg::~QWHTimimg()
{
    if (m_timer != nullptr)
    {
        delete m_timer;
        m_timer = nullptr;
    }

    if (m_animation != nullptr)
    {
        if (m_animation->state() != QAbstractAnimation::Stopped)
        {
            m_animation->stop();
        }

        delete m_animation;
        m_animation = nullptr;
    }
}

void QWHTimimg::setSecs(int secs)
{
    m_secs = secs;
}

void QWHTimimg::setTextColor(const QColor &color)
{
    m_textColor = color;
}

void QWHTimimg::setBgBrush(const QBrush &brush)
{
    m_bgBrush = brush;
}

void QWHTimimg::start(int secs)
{
    if (secs > 0)
    {
        m_secs = secs;
    }

    m_zoomIn = false;
    m_curSecs = m_secs;
    m_animation->setStartValue(100);
    m_animation->setEndValue(300);
    m_animation->start();
    m_timer->start();
}

int QWHTimimg::getSecs() const
{
    return m_secs;
}

QColor QWHTimimg::getTextColor() const
{
    return m_textColor;
}

QBrush QWHTimimg::getBgBrush() const
{
    return m_bgBrush;
}

void QWHTimimg::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

    int width = this->width();
    int height = this->height();

    //绘制背景
    painter.setPen(Qt::NoPen);
    painter.setBrush(m_bgBrush);
    painter.drawRect(rect());

    //坐标轴平移
    painter.translate(width / 2, height / 2);

    //绘制文本
    QFont font = painter.font();
    font.setPixelSize(m_animation->currentValue().toFloat() / 100 * (height / 3));
    painter.setFont(font);
    int opacity = 300 - m_animation->currentValue().toInt() + 55;
    m_textColor.setAlpha(opacity);
    painter.setPen(QPen(m_textColor, 2));
    painter.drawText(-width / 2, -height / 2, width, height, Qt::AlignCenter, QString::number(m_curSecs));
}

void QWHTimimg::onTimeout()
{
    m_animation->stop();


    if (m_zoomIn)
    {
        if (m_curSecs == 1)
        {
            m_timer->stop();
            m_curSecs = 0;
            if (board->isServer == true)  //服务端才有权利发送清空信息
            {
                emit timingFinished();
            }
            update();
            return;
        }

        m_animation->setStartValue(100);
        m_animation->setEndValue(300);
        m_curSecs--;
    }
    else
    {
        m_animation->setStartValue(300);
        m_animation->setEndValue(100);
    }

    m_animation->start();
    m_zoomIn = !m_zoomIn;
}
