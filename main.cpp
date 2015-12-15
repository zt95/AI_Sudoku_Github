#include "mainwindow.h"
#include <QApplication>
#include <iostream>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow w;
    w.show();
    int result = app.exec();
    std::cout << "kkkkkkkkkkkkkkkk" << std::endl;
    app.quit();
    return result;
}
