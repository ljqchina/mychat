#include "widget.h"
#include "ui_widget.h"
#include <QTcpSocket>
#include <QHostAddress>
#include <iostream>
#include <sys/time.h>
#include <unistd.h>

using namespace std;

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    connect(&sock,SIGNAL(connected()),this,SLOT(onConnected()));
    connect(&sock,SIGNAL(disconnected()),this,SLOT(onDisconnected()));
    connect(&sock,SIGNAL(readyRead()),this,SLOT(slotDataRecived()));

    QHostAddress serverIp;
    short port = 19000;
    serverIp.setAddress("192.168.10.141");

    cout << "connect to host\n" << endl;
    sock.connectToHost(serverIp,port);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::onConnected()
{
    cout << "onConnected\n" << endl;
    //while(1)
    {
        int ret = sock.write("abc", 3);
        cout << "send:" << "abc" << "," << ret << endl;
        //sleep(2);
    }
}

void Widget::onDisconnected()
{
    cout << "onDisConnected\n" << endl;
}

void Widget::slotDataRecived()
{
    cout << "slotDataRecived\n" << endl;
    char data[200] = {0};
    sock.read(data, 4);
    cout << "head:" << data << endl;

    int len = atoi(data);
    qint64 l = sock.read(data, len - 4);
    cout << "l:" << l << ", body:" << data << endl;

    int ret = 0;
    ret = sock.write("abc", 3);
    ret = sock.write("abc", 3);
    cout << "send:" << "abc" << "," << ret << endl;

    //sock.close();

}


