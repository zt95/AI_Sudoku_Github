#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFrame>
#include <QPushButton>
#include <QPainter>
#include <QLayout>
#include <QGridLayout>
#include <cstring>
#include <QAction>
#include <QMenu>
#include "grid.h"
#include "paintline.h"
#include <vector>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void changeBoardSize();
    void autoInit();
    void clear();
    void run();
    void about();

private:
    void createMenuAction();
    void displayBoard(int newBoardSize);
    void displayGrids();
    void displayLines();
    void init();
    bool backtrack();
    bool localSearch();
    bool judgeInput();

private:
    Ui::MainWindow *ui;
    QString version;
    QFrame *frame;

    QMenu *operationMenu;
    QAction *changeBoardAction;
    QAction *initAction;
    QAction *clearAction;
    QAction *runAction;
    QAction *quitAction;
    QMenu *helperMenu;
    QAction *aboutAction;

    std::vector<std::vector<Grid*> > board;
};

#endif // MAINWINDOW_H