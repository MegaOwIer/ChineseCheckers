#ifndef ASK_SERVER_OR_CLIENT_H
#define ASK_SERVER_OR_CLIENT_H

#include <QWidget>

// 定义在main.cpp文件中
// 0代表Client , 1代表Server

namespace Ui
{
    class ask_server_or_client;
}

class ask_server_or_client : public QWidget
{
    Q_OBJECT

public:
    explicit ask_server_or_client(QWidget *parent = nullptr);
    ~ask_server_or_client();

public slots:

    //将联网模式修该为客户端模式
    void setClientMode();

    //将联网模式修该为服务端模式
    void setServerMode();

private:
    Ui::ask_server_or_client *ui;
};

#endif // ASK_SERVER_OR_CLIENT_H
