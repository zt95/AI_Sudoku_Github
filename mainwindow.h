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
#include <unordered_set>

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
    void changeAlgtoDFS();
    void changeAlgtoLocalSearch();
    void changeDisplayInterval();
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
    bool backtrack2();
    void prune();
    bool localSearch();
    bool judgeInput();
    void randomSolution();
    void initRandom();
    void initLock();
    int localError();

private:
    Ui::MainWindow *ui;
    QString version;
    QFrame *frame;

    QMenu *operationMenu;
    QMenu *settingMenu;
    QAction *changeBoardAction;
    QAction* changeIntervalAction;
    QMenu* changeAlgMenu;
    QAction *algDFS;
    QAction *algLocalSearch;
    QAction *initAction;
    QAction *clearAction;
    QAction *runAction;
    QAction *quitAction;
    QMenu *helperMenu;
    QAction *aboutAction;

    std::vector<std::vector<std::unordered_set<int> > > possibleNum;
    std::vector<std::vector<bool> > locked;
    std::vector<std::vector<int> > boardVal;
    std::vector<std::vector<std::unordered_set<int> > > col;   //col[1][5]={3,6} means boardVal[3][1] = boardVal[6][1] = 5
    std::vector<std::vector<std::unordered_set<int> > > block; //block[1][5]={3,6} means boardVal[blockX(1)][blockY(3)] = boardVal[blockX(1)][block(6)] = 5

    std::vector<std::vector<Grid*> > board; //Use for display/interaction

    bool isRunning;
};

#endif // MAINWINDOW_H
