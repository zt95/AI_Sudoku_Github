#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QInputDialog>
#include <iostream>
#include <unordered_set>
#include <cmath>
#include <time.h>
using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    version = "1.0.0 Beta";
    this->setWindowTitle("Sudoku Solver");
    createMenuAction();
    displayBoard(3);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::createMenuAction()
{
    operationMenu = new QMenu(tr("Operation"), this);
    ui->menuBar->addMenu(operationMenu);

    settingMenu = new QMenu(tr("Change Settings"), this);
    operationMenu->addMenu(settingMenu);

    changeBoardAction = new QAction(tr("Board Size"), this);
    changeBoardAction->setShortcut(tr("Ctrl+b"));
    changeBoardAction->setStatusTip("Change the Board Size");
    connect(changeBoardAction, SIGNAL(triggered()), this, SLOT(changeBoardSize()));
    settingMenu->addAction(changeBoardAction);

    changeAlgMenu = new QMenu(tr("Algorithm"), this);
    settingMenu->addMenu(changeAlgMenu);

    algDFS = new QAction(tr("DFS"), this);
    algDFS->setCheckable(true);
    algDFS->setChecked(true);
    algDFS->setStatusTip("DFS Algorithm");
    connect(algDFS, SIGNAL(triggered()), this, SLOT(changeAlgtoDFS()));
    changeAlgMenu->addAction(algDFS);

    algLocalSearch = new QAction(tr("Local Search"), this);
    algLocalSearch->setCheckable(true);
    algLocalSearch->setChecked(false);
    algLocalSearch->setStatusTip("Local Search Algorithm");
    connect(algLocalSearch, SIGNAL(triggered()), this, SLOT(changeAlgtoLocalSearch()));
    changeAlgMenu->addAction(algLocalSearch);

    changeIntervalAction = new QAction(tr("Display Interval"), this);
    changeIntervalAction->setShortcut(tr("Ctrl+t"));
    changeIntervalAction->setStatusTip("Change the Display Interval");
    connect(changeIntervalAction, SIGNAL(triggered()), this, SLOT(changeDisplayInterval()));
    settingMenu->addAction(changeIntervalAction);

    initAction = new QAction(tr("Initialize"), this);
    initAction->setShortcut(tr("Ctrl+d"));
    initAction->setStatusTip("Initialize the sudoku");
    connect(initAction, SIGNAL(triggered()), this, SLOT(autoInit()));
    operationMenu->addAction(initAction);

    clearAction = new QAction(tr("Clear"), this);
    clearAction->setShortcut(tr("Ctrl+w"));
    clearAction->setStatusTip("Clear the sudoku");
    connect(clearAction, SIGNAL(triggered()), this, SLOT(clear()));
    operationMenu->addAction(clearAction);

    runAction = new QAction(tr("Run"), this);
    runAction->setShortcut(tr("Ctrl+r"));
    runAction->setStatusTip("Solve the sudoku");
    connect(runAction,SIGNAL(triggered()), this, SLOT(run()));
    operationMenu->addAction(runAction);

    quitAction = new QAction(tr("Close"), this);
    quitAction->setStatusTip("Quit the program");
    connect(quitAction, SIGNAL(triggered()), this, SLOT(close()));
    operationMenu->addAction(quitAction);

    helperMenu = new QMenu(tr("About"), this);
    ui->menuBar->addMenu(helperMenu);

    aboutAction = new QAction(tr("Info"), this);
    aboutAction->setShortcut(tr("Ctrl+i"));
    aboutAction->setStatusTip("About the Sudoku Solver");
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));
    helperMenu->addAction(aboutAction);
}

void MainWindow::displayBoard(int newBoardSize)
{
    frame = new QFrame(this);
    changeSize(newBoardSize);
    displayGrids();
    displayLines();
    this->setCentralWidget(frame);
    this->setFixedSize(frame->width(), frame->height()+ui->statusBar->height()+ui->menuBar->height());
}

void MainWindow::displayGrids()
{
    board.clear();
    int vecSize = boardSize*boardSize;
    board.resize(vecSize);
    for (int i = 0; i < vecSize; ++i) {
        board[i].resize(vecSize);
        for (int j = 0; j < vecSize; ++j) {
            board[i][j] = new Grid(frame);
            board[i][j]->move(j*gridSize, i*gridSize);
        }
    }
    frame->setMinimumSize(gridSize*vecSize+1, gridSize*vecSize+1);
}

void MainWindow::displayLines()
{
    int delta = gridSize*boardSize;
    int lineWidth = 20/boardSize;
    QColor lineColor(0, 0, 0);
    // draw horizontal lines
    vector<paintLine*> rows(boardSize+1);
    for (int i = 0; i < rows.size(); i++) {
        rows[i] = new paintLine(frame);
        rows[i]->setColor(lineColor);
        rows[i]->setLineSizeDir(lineWidth, frame->width(), 0);
        if (i == 0) {
            rows[i]->move(0, 0);
        }
        else if (i == rows.size()-1) {
            rows[i]->move(0, i*delta-lineWidth+1);
        }
        else {
            rows[i]->move(0, i*delta-lineWidth/2);
        }
    }
    // draw vertical lines
    vector<paintLine*> cols(boardSize+1);
    for (int i = 0; i < rows.size(); i++) {
        cols[i] = new paintLine(frame);
        cols[i]->setColor(lineColor);
        cols[i]->setLineSizeDir(lineWidth, frame->height(), 1);
        if (i == 0) {
            cols[i]->move(0, 0);
        }
        else if (i == cols.size()-1) {
            cols[i]->move(i*delta-lineWidth+1, 0);
        }
        else {
            cols[i]->move(i*delta-lineWidth/2, 0);
        }
    }
}

bool checkBound(int i,int j,int n)
{
    if(i<0 || j<0)return false;
    if(i>=n || j>=n)return false;
    return true;
}

bool MainWindow::backtrack()
{
    int x=0;
    int y=0;
    int n = boardSize*boardSize;
    bool done = true;
    for(int i=0;i<n;i++)
    {
        if(!done)break;
        for(int j=0;j<n;j++)
        {
            if(board[i][j]->getter() == 0)
            {
                done = false;
                x = i;
                y = j;
                break;
            }
        }
    }
    if(done)return true;
    if(!checkBound(x,y,n))return false;
    std::vector<bool> poss;
    for(int i=0;i<n;i++) poss.push_back(true);
    for(int i=0;i<n;i++)
    {
        int val = board[i][y]->getter();
        if(val>0) poss[val-1] = false;
    }
    for(int i=0;i<n;i++)
    {
        int val = board[x][i]->getter();
        if(val>0) poss[val-1] = false;
    }
    int blockx = boardSize*(x/boardSize);
    int blocky = boardSize*(y/boardSize);
    for(int i=blockx;i<blockx+boardSize;i++)
    {
        for(int j=blocky;j<blocky+boardSize;j++)
        {
            int val = board[i][j]->getter();
            if(val>0) poss[val-1] = false;
        }
    }
    for(int i=0;i<n;i++)
    {
        if(poss[i] == true)
        {
            board[x][y]->setter(i+1);
            if(this->backtrack())return true;
            board[x][y]->setter(0);
        }
    }
    return false;
}
/*
std::vector<int> randomPerm(int n)
{
    std::vector<int> perm;
    for(int i=0;i<n;i++)
    {
        perm.push_back(i);
        int xchPos = rand()%(i+1);
        int tmp = perm[i];
        perm[i] = perm[xchPos];
        perm[xchPos] = tmp;
    }
    return perm;
}*/
int blockX(int x, int y, int m)
{
    return (x/m)*m+y/m;
}

int blockY(int x,int y, int m)
{
    return (x%m)*m+y%m;
}

bool MainWindow::localSearch()
{
    int n = boardSize * boardSize;
    int m = boardSize;
    std::vector<std::vector<bool> > locked;
    std::vector<std::vector<int> > boardVal;
    for (int i = 0; i < n; ++i) {
        std::vector<bool> lockedI(n);
        std::vector<int> boardValI(n);
        std::vector<bool> numberUsed(n,false);
        int totalUsed = 0;
        for (int j = 0; j < n; ++j) {
            int valAtIJ = board[i][j]->getter();
            if (valAtIJ > 0){
                lockedI[j] = true;
                numberUsed[valAtIJ-1] = true;
                boardValI[j] = valAtIJ-1;
                totalUsed++;
            }
            else
            {
                lockedI[j] = false;
                boardValI[j] = -1;
            }
        }
        locked.push_back(lockedI);
        for(int j = 0;j < n; ++j)
        {
            if(numberUsed[j]) continue;
            int totalLeft = n - totalUsed;
            int cnt = rand()%totalLeft;
            int pos = 0;
            while(true)
            {
                while(boardValI[pos]>-1)pos++;
                if(cnt == 0)break;
                cnt--;
                pos++;
            }
            boardValI[pos] = j;
            numberUsed[j] = true;
            totalUsed++;
        }
        boardVal.push_back(boardValI);
    }
    for (int i = 0; i < board.size(); ++i) {
        for (int j = 0; j < board.size(); ++j) {
            if (board[i][j]->getter() == 0) {
                board[i][j]->setter(boardVal[i][j]+1);
            }
        }
    }
    vector<vector<unordered_set<int> > > col;   //col[1][5]={3,6} means boardVal[3][1] = boardVal[6][1] = 5
    vector<vector<unordered_set<int> > > block; //block[1][5]={3,6} means boardVal[blockX(1)][blockY(3)] = boardVal[blockX(1)][block(6)] = 5
    //vector<vector<int> > ErrPoint;
    int TotalErr = 0;
    for(int i=0;i<n;i++)
    {
        vector<unordered_set<int> > tmp;
        //vector<int> tErr(n,0);
        for(int j=0;j<n;j++)
        {
            unordered_set<int> tmpUS= {};
            tmp.push_back(tmpUS);
        }
        col.push_back(tmp);
        block.push_back(tmp);
        //ErrPoint.push_back(tErr);
    }
    for(int i=0;i<n;i++)
    {
        for(int j=0;j<n;j++)
        {
            int valIJ = boardVal[i][j];
            int bx = blockX(i,j,m);
            int by = blockY(i,j,m);
            col[j][valIJ].insert(i);
            block[bx][valIJ].insert(by);
        }
    }
    for(int i=0;i<n;i++)
    {
        for(int j=0;j<n;j++)
        {
            if(col[i][j].size()>1)
            {
                /*for(auto it = col[i][j].begin();it!=col[i][j].end();++it)
                {
                    ErrPoint[*it][i]++;
                    if(!locked[*it][i])board[*it][i]->changeTextColor(Qt::blue);
                }*/
                TotalErr += col[i][j].size()-1;
            }
            if(block[i][j].size()>1)
            {
                /*for(auto it = block[i][j].begin();it!=block[i][j].end();++it)
                {
                    int bx = blockX(i,*it,m);
                    int by = blockY(i,*it,m);
                    ErrPoint[bx][by] ++;
                    if(!locked[bx][by])board[bx][by]->changeTextColor(Qt::blue);
                }*/
                TotalErr += block[i][j].size()-1;
            }
        }
    }
    int iter = 0;
    vector<double> tValue = {100.0,100.0,100.0,200.0,400.0,800.0,1600.0,3200.0,8000.0};
    vector<double> aValue = {0.9,0.9,0.9,0.9,0.9,0.9,0.9,0.9,0.9};
    double T = tValue[m];
    double alpha = aValue[m];
    while(TotalErr>0 && iter<10000*m)
    {
        int i = rand()%board.size();
        int j1 = rand()%board.size();
        int j2 = rand()%board.size();
        if(j1==j2)continue;
        if(locked[i][j1] || locked[i][j2])continue;
        //if(ErrPoint[i][j1]==0 && ErrPoint[i][j2]==0)continue;
        int val1 = boardVal[i][j1];
        int val2 = boardVal[i][j2];
        int bx1 = blockX(i,j1,m);
        int by1 = blockY(i,j1,m);
        int bx2 = blockX(i,j2,m);
        int by2 = blockY(i,j2,m);
        if(col[j1][val1].size()==1 && col[j2][val2].size()==1 && block[bx1][val1].size()==1 && block[bx2][val2].size()==1)continue;
        iter++;
        int tmpErr = 0;
        if(col[j1][val1].size()>1)tmpErr--;
        if(col[j1][val2].size()>0)tmpErr++;
        if(col[j2][val2].size()>1)tmpErr--;
        if(col[j2][val1].size()>0)tmpErr++;

        if(bx1!=bx2)
        {
            if(block[bx1][val1].size()>1)tmpErr--;
            if(block[bx1][val2].size()>0)tmpErr++;
            if(block[bx2][val2].size()>1)tmpErr--;
            if(block[bx2][val1].size()>0)tmpErr++;
        }

        if(exp((-tmpErr)/T)*999>rand()%1000)
        {
            TotalErr += tmpErr;
            int tmp = boardVal[i][j1];
            boardVal[i][j1] = boardVal[i][j2];
            boardVal[i][j2] = tmp;

            col[j1][val1].erase(i);
            /*if(col[j1][val1].size() > 0)
            {
                ErrPoint[i][j1]--;
                if(col[j1][val1].size() == 1)
                {
                    auto it = col[j1][val1].begin();
                    ErrPoint[*it][j1] --;
                    if(ErrPoint[*it][j1] == 0)board[*it][j1]->changeTextColor(Qt::red);
                }
            }*/

            col[j2][val2].erase(i);

            col[j1][val2].insert(i);

            col[j2][val1].insert(i);

            block[bx1][val1].erase(by1);

            block[bx2][val2].erase(by2);

            block[bx1][val2].insert(by1);

            block[bx2][val1].insert(by2);

            board[i][j1]->setter(boardVal[i][j1]+1);
            board[i][j2]->setter(boardVal[i][j2]+1);
        }
        T *= alpha;
        //cout<<TotalErr<<" "<<iter<<endl;
    }
    cout<<"Total iteration:"<<iter<<endl;
    return true;
}

/*
int boardError(std::vector<std::vector<int> > B)
{
    int err = 0;
    int n = B.size();
    int m = sqrt(n);
    for(int j=0;j<n;j++)
    {
        for(int i1=0;i1<n-1;i1++)
        {
            bool duplicate = false;
            for(int i2=i1+1;i2<n;i2++)
            {
                if(B[i1][j] == B[i2][j])
                {
                    duplicate = true;
                    break;
                }
            }
            if(duplicate)err++;
        }
    }
    for(int s=0;s<n;s++)
    {
        int sx = s/m;
        int sy = s%m;
        for(int r1=0;r1<n-1;r1++)
        {
            int x1 = sx*m+r1/m;
            int y1 = sy*m+r1%m;
            bool duplicate = false;
            for(int r2=r1+1;r2<n;r2++)
            {
                int x2 = sx*m+r2/m;
                int y2 = sy*m+r2%m;
                if(B[x1][y1]==B[x2][y2])
                {
                    duplicate = true;
                    break;
                }
            }
            if(duplicate)err++;
        }
    }
    return err;
}

bool MainWindow::localSearch()
{
    std::vector<std::vector<bool> > locked;
    std::vector<std::vector<int> > boardVal;
    for (int i = 0; i < board.size(); ++i) {
        vector<bool> lockedI;
        vector<int> permI = randomPerm(board.size());
        for (int j = 0; j < board.size(); ++j) {
            int valAtIJ = board[i][j]->getter();
            if (valAtIJ > 0){
                lockedI.push_back(true);
                int t;
                for(t=0;t<board.size();t++)
                    if(permI[t] == valAtIJ-1)
                        break;
                int tmp = permI[j];
                permI[j] = permI[t];
                permI[t] = tmp;
            }
            else lockedI.push_back(false);
        }
        locked.push_back(lockedI);
        boardVal.push_back(permI);
    }
    
    
    for (int i = 0; i < board.size(); ++i) {
        for (int j = 0; j < board.size(); ++j) {
            if (board[i][j]->getter() == 0) {
                board[i][j]->setter(boardVal[i][j]+1);
            }
        }
    }
    int err = boardError(boardVal);
    int iter = 0;
    double T = 100.0;
    double alpha = 0.99;
    while(err>0 && iter<100000)
    {
        int i = rand()%board.size();
        int j1 = rand()%board.size();
        int j2 = rand()%board.size();
        if(j1==j2)continue;
        if(locked[i][j1] || locked[i][j2])continue;

        iter++;
        int tmp = boardVal[i][j1];
        boardVal[i][j1] = boardVal[i][j2];
        boardVal[i][j2] = tmp;

        int newErr = boardError(boardVal);
        if(newErr<err || exp((err-newErr)/T)*999>rand()%1000)
        {
            err = newErr;
            board[i][j1]->setter(boardVal[i][j1]+1);
            board[i][j2]->setter(boardVal[i][j2]+1);
        }
        else
        {
            tmp = boardVal[i][j1];
            boardVal[i][j1] = boardVal[i][j2];
            boardVal[i][j2] = tmp;
        }
        T *= alpha;
        cout<<err<<endl;
    }
    if(err == 0)return true;
    else return false;
}
*/
bool MainWindow::judgeInput()
{
    int n = board.size();
    int m = sqrt(n);
    for(int i=0;i<n;i++)
    {
        for(int j1=0;j1<n-1;j1++)
        {
            for(int j2=j1+1;j2<n;j2++)
            {
                if(board[i][j1]->getter()>0 && board[i][j1]->getter() == board[i][j2]->getter())return false;
                if(board[j1][i]->getter()>0 && board[j1][i]->getter() == board[j2][i]->getter())return false;
                int sx = i/m;
                int sy = i%m;
                int x1 = sx*m+j1/m;
                int y1 = sy*m+j1%m;
                int x2 = sx*m+j2/m;
                int y2 = sy*m+j2%m;
                if(board[x1][y1]->getter()>0 && board[x1][y1]->getter() == board[x2][y2]->getter())return false;
            }
        }
    }
    return true;
}
// entrance to run different methods
void MainWindow::run()
{
    time_t dbegin,dstop;
    dbegin = clock();
    for (int i = 0; i < board.size(); ++i) {
        for (int j = 0; j < board.size(); ++j) {
            board[i][j]->setGridEnable(false);
            if (board[i][j]->getter() == 0) {
                board[i][j]->changeTextColor(Qt::red);
            }
        }
    }
    if(!judgeInput()) {
        QMessageBox::about(this, "Error", tr("Input invalid!"));
        for (int i = 0; i < board.size(); ++i) {
            for (int j = 0; j < board[i].size(); ++j) {
                board[i][j]->setGridEnable(true);
                board[i][j]->changeTextColor(Qt::black);
            }
        }
    }
    else {
        if (algType == DFS) {
            this->backtrack();
        }
        else if (algType == LOCALSEARCH) {
            this->localSearch();
        }
        QMessageBox::about(this, "Result", tr("Calculation complete!"));
        if(judgeInput())cout<<"Correct!!!"<<endl;
    }
    dstop = clock();
    cout<<dstop-dbegin<<endl;
}

// initialized by computer algorithm
void MainWindow::autoInit()
{
    clear();

}

// clear the board
void MainWindow::clear()
{
    for (int i = 0; i < board.size(); ++i) {
        for (int j = 0; j < board[i].size(); ++j) {
            board[i][j]->setter(0);
            board[i][j]->changeTextColor(Qt::black);
            board[i][j]->setGridEnable(true);
        }
    }
}

//show the information, e.g. version, developers
void MainWindow::about()
{
    QMessageBox::about(this, "About", tr("<h1>Sudoku Solver</h1>"
                                        "<h4>Solve the sudoku problem with differenct methods like local search and backtracking.</h4>"
                                        "<h4>Based on QT 5.5.1</h4>"
                                        "<h4>Version: %1</h4>"
                                        "<h4>By: Xianqiang Gu (xg95), Ziyang Tang (zt95)</h4>\n"
                                         ).arg(version));
}

// change the size of board
void MainWindow::changeBoardSize()
{
    bool isOK;
    int newBoardSize = QInputDialog::getInt(NULL, "Change Board Size", "The board size is represented as n^2*n^2.\nPlease enter n between 2 and 9.",
                                 3, 2, 9, 1, &isOK);
    if(isOK) {
        delete frame;
        displayBoard(newBoardSize);
    }
}

void MainWindow::changeAlgtoDFS()
{
    algDFS->setChecked(true);
    algLocalSearch->setChecked(false);
    algType = DFS;
    cout << "DFS" << endl;
}

void MainWindow::changeAlgtoLocalSearch()
{
    algDFS->setChecked(false);
    algLocalSearch->setChecked(true);
    algType = LOCALSEARCH;
    cout << "Local Search" << endl;
}

void MainWindow::changeDisplayInterval()
{
    bool isOK;
    int newDisplayInterval = 1000 * QInputDialog::getInt(NULL, "Change Display Interval", "Change the display interval (measured by the millisecond).\nPlease enter an integer between 0 and 3000.",
                                 0, 0, 3000, 100, &isOK);
    if(isOK) {
        DisplayInterval = newDisplayInterval;
    }
}

