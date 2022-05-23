#ifndef SHOWINFO_H
#define SHOWINFO_H

#include <QWidget>

namespace Ui
{
    class ShowInfo;
}

class ShowInfo : public QWidget
{
    Q_OBJECT

public:
    explicit ShowInfo(QWidget *parent = nullptr);
    ~ShowInfo();

private:
    Ui::ShowInfo *ui;

public slots:

    /**
     * @brief showCONNECTING 显示正在连接的状态
     */
    void showCONNECTING();

    /**
     * @brief receiveInfo 用来信号函数接收由Board信号函数发出的信息，并分流给不同的函数进行处理
     * @param info 错误码或者连接成功信息，实际上是一个QString对象
     * @param detail 详细的报错信息
     */
    void receiveERROR(QString info, QString detail);

    /**
     * @brief showSUCCESSFULLY_CONNECTE 成功连接
     */
    void showSUCCESSFULLY_CONNECTE();

    /**
     * @brief showFAIL_TO_CONNECT
     */
    void showFAIL_TO_CONNECT();

    /**
     * @brief showINVALID_JOIN 加入房间失败。可能由于用户名冲突引起。
     * @param detail
     */
    void showINVALID_JOIN(QString detail);

    /**
     * @brief showINVALID_REQ 无法解析该请求
     * @param detail
     */
    void showINVALID_REQ(QString detail);

    /**
     * @brief showNOT_IN_ROOM 当前用户不在房间内
     * @param detail
     */
    void showNOT_IN_ROOM(QString detail);

    /**
     * @brief showROOM_IS_RUNNING 当前房间内的游戏正在进行
     * @param detail
     */
    void showROOM_IS_RUNNING(QString detail);

    /**
     * @brief showROOM_NOT_RUNNING 当前房间内没有正在进行的游戏
     * @param detail
     */
    void showROOM_NOT_RUNNING(QString detail);

    /**
     * @brief showOTHER_ERROR 前面没有提及的其他错误
     * @param detail
     */
    void showOTHER_ERROR(QString detail);

    void showINVALID_MOVE(QString detail);

};

#endif // SHOWINFO_H
