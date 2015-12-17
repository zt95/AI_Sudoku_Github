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
    isRunning = false;
    DisplayInterval = 0;
    algType = DFS;
    doInteraction = false;
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

    algDFS = new QAction(tr("Depth first search"), this);
    algDFS->setCheckable(true);
    algDFS->setChecked(true);
    algDFS->setStatusTip("DFS Algorithm");
    connect(algDFS, SIGNAL(triggered()), this, SLOT(changeAlgtoDFS()));
    changeAlgMenu->addAction(algDFS);

    algOptimizedDFS = new QAction(tr("Optimized DFS"), this);
    algOptimizedDFS->setCheckable(true);
    algOptimizedDFS->setChecked(false);
    algOptimizedDFS->setStatusTip("DFS with some optimizations");
    connect(algOptimizedDFS, SIGNAL(triggered()), this, SLOT(changeAlgtoOptimizedDFS()));
    changeAlgMenu->addAction(algOptimizedDFS);

    algLocalSearch = new QAction(tr("Simulated annealing"), this);
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

    changeInteractionMenu = new QMenu(tr("Show Interaction"), this);
    settingMenu->addMenu(changeInteractionMenu);

    interactionYes = new QAction(tr("Yes"), this);
    interactionYes->setCheckable(true);
    interactionYes->setChecked(false);
    interactionYes->setStatusTip("Show interatcion");
    connect(interactionYes, SIGNAL(triggered()), this, SLOT(showInteractionYes()));
    changeInteractionMenu->addAction(interactionYes);

    interactionNo = new QAction(tr("No"), this);
    interactionNo->setCheckable(true);
    interactionNo->setChecked(true);
    interactionNo->setStatusTip("Don't show interatcion");
    connect(interactionNo, SIGNAL(triggered()), this, SLOT(showInteractionNo()));
    changeInteractionMenu->addAction(interactionNo);

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

int blockX(int x, int y, int m)
{
    return (x/m)*m+y/m;
}

int blockY(int x,int y, int m)
{
    return (x%m)*m+y%m;
}
void MainWindow::randomSolution()
{
    int m = boardSize;
    int x=0;
    vector<int> perm;
    for(int i=0;i<m*m;i++)
    {
        int j = rand()%(i+1);
        perm.push_back(i);
        int tmp = perm[j];
        perm[j] = perm[i];
        perm[i] = tmp;
    }
    for(int i=0;i<m;i++)
    {
        for(int j=0;j<m;j++)
        {
            for(int k=0;k<m*m;k++)
            {
                //cout<<"hi"<<endl;
                board[m*i+j][k]->setter(perm[(x%(m*m))]+1);
                x++;
            }
            x += m;
        }
        x++;
    }
}

void MainWindow::prune()
{
    int n = boardSize*boardSize;
    int m = boardSize;
    possibleNum.clear();
    for(int i=0;i<n;i++)
    {
        vector<unordered_set<int> > posI;
        for(int j=0;j<n;j++)
        {
            unordered_set<int> posIJ;
            if(board[i][j]->getter() == 0)
            {
                for(int k=0;k<n;k++)
                {
                    posIJ.insert(k);
                }
            }
            else posIJ.insert(board[i][j]->getter()-1);
            posI.push_back(posIJ);
        }
        possibleNum.push_back(posI);
    }
    int iter = 0;
    bool update = true;
    while(iter<80 && update)
    {
        iter++;
        update = false;
        for(int i=0;i<n;i++)
        {
            for(int j=0;j<n;j++)
            {
                if(board[i][j]->getter()>0)
                {
                    for(int k = 0; k < n; k++)
                    {
                        //Erase row, column
                        if(j != k)
                            possibleNum[i][k].erase(board[i][j]->getter()-1);
                        if(i != k)
                            possibleNum[k][j].erase(board[i][j]->getter()-1);

                        //Erase block
                        int bx = blockX(i,j,m);
                        int by = blockY(i,j,m);
                        if(by != k)
                        {
                            int i2 = blockX(bx,k,m);
                            int j2 = blockY(bx,k,m);
                            possibleNum[i2][j2].erase(board[i][j]->getter()-1);
                        }
                    }
                }
            }
        }
        for(int i=0;i<n;i++)
        {
            for(int j=0;j<n;j++)
            {
                if(board[i][j]->getter()==0 && possibleNum[i][j].size()==1)
                {
                    update = true;
                    auto it = possibleNum[i][j].begin();
                    board[i][j]->setter(*it + 1);
                }
            }
        }
        for(int i=0;i<n;i++)
        {
            for(int num=0;num<n;num++)
            {
                int rowCnt = 0;
                int rowOnly = -1;
                int colCnt = 0;
                int colOnly = -1;
                int blockCnt = 0;
                int blockOnly = -1;
                for(int j = 0;j<n;j++)
                {
                    if(possibleNum[i][j].find(num) != possibleNum[i][j].end())
                    {
                        rowOnly = j;
                        rowCnt++;
                    }
                    if(possibleNum[j][i].find(num) != possibleNum[j][i].end())
                    {
                        colOnly = j;
                        colCnt++;
                    }
                    int bx = blockX(i,j,m);
                    int by = blockY(i,j,m);
                    if(possibleNum[bx][by].find(num) != possibleNum[bx][by].end())
                    {
                        blockOnly = j;
                        blockCnt++;
                    }
                }
                if(rowCnt == 1)
                {
                    if(board[i][rowOnly]->getter() == 0)
                    {
                        update = true;
                        board[i][rowOnly]->setter(num+1);
                        possibleNum[i][rowOnly].clear();
                        possibleNum[i][rowOnly].insert(num);
                    }
                }
                if(colCnt == 1)
                {
                    if(board[colOnly][i]->getter() == 0)
                    {
                        update = true;
                        board[colOnly][i]->setter(num+1);
                        possibleNum[colOnly][i].clear();
                        possibleNum[colOnly][i].insert(num);
                    }
                }
                if(blockCnt == 1)
                {
                    int bx = blockX(i,blockOnly,m);
                    int by = blockY(i,blockOnly,m);
                    if(board[bx][by]->getter() == 0)
                    {
                        update = true;
                        board[bx][by]->setter(num+1);
                        possibleNum[bx][by].clear();
                        possibleNum[bx][by].insert(num);
                    }
                }
            }
        }
    }
}

bool MainWindow::backtrack2()
{
    int bestExpandX=-1;
    int bestExpandY=-1;
    int n = boardSize*boardSize;
    int m = boardSize;
    int bestBranch = n+1;
    bool done = true;
    for(int i=0;i<n;i++)
    {
        //if(!done)break;
        for(int j=0;j<n;j++)
        {
            if(board[i][j]->getter() == 0)
            {
                done = false;
                if(possibleNum[i][j].size()<bestBranch)
                {
                    bestBranch = possibleNum[i][j].size();
                    bestExpandX = i;
                    bestExpandY = j;
                }
            }
        }
    }
    if(done)return true;
    int x=bestExpandX;
    int y=bestExpandY;
    std::vector<std::vector<std::unordered_set<int> > > history = possibleNum;
    unordered_set<int> posXY = possibleNum[x][y];
    for(auto it = posXY.begin(); it != posXY.end(); ++it)
    {
        int num = *it;
        for(int i=0;i<n;i++)
        {
            //Erase row,col
            if(i != x)possibleNum[i][y].erase(num);
            if(i != y)possibleNum[x][i].erase(num);

            //Erase block
            int bx = blockX(x,y,m);
            int by = blockY(x,y,m);
            if(i != by)
            {
                int i2 = blockX(bx,i,m);
                int j2 = blockY(bx,i,m);
                possibleNum[i2][j2].erase(num);
            }
        }
        board[x][y]->setter(num+1);
        if(this->backtrack2())return true;
        possibleNum = history;
        board[x][y]->setter(0);
    }
    return false;
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

void MainWindow::initLock()
{
    int n = boardSize * boardSize;
    locked.clear();
    for (int i = 0; i < n; ++i) {
        std::vector<bool> lockedI(n);
        for (int j = 0; j < n; ++j) {
            int valAtIJ = board[i][j]->getter();
            if (valAtIJ > 0)
                lockedI[j] = true;
            else
                lockedI[j] = false;
        }
        locked.push_back(lockedI);
    }
}

void MainWindow::initRandom()
{
    int n = boardSize * boardSize;
    int m = boardSize;
    boardVal.clear();

    for (int i = 0; i < n; ++i) {
        std::vector<int> boardValI(n);
        std::vector<bool> numberUsed(n,false);
        int totalUsed = 0;
        for (int j = 0; j < n; ++j) {
            int valAtIJ = board[i][j]->getter();
            if (valAtIJ && locked[i][j]> 0){
                numberUsed[valAtIJ-1] = true;
                boardValI[j] = valAtIJ-1;
                totalUsed++;
            }
            else
            {
                boardValI[j] = -1;
            }
        }
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
}

int MainWindow::localError()
{
    int n = boardSize * boardSize;
    int m = boardSize;

    int TotalErr = 0;
    col.clear();
    block.clear();
    for(int i=0;i<n;i++)
    {
        vector<unordered_set<int> > tmp;
        for(int j=0;j<n;j++)
        {
            unordered_set<int> tmpUS= {};
            tmp.push_back(tmpUS);
        }
        col.push_back(tmp);
        block.push_back(tmp);
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
            TotalErr += col[i][j].size()*(col[i][j].size()-1);
            TotalErr += block[i][j].size()*(block[i][j].size()-1);
        }
    }
    return TotalErr;
}

bool MainWindow::localSearch()
{
    int n = boardSize * boardSize;
    int m = boardSize;

    initLock();
    initRandom();
    int TotalErr = localError();

    int iter = 0;
    vector<double> tValue = {100.0,100.0,100.0,200.0,400.0,800.0,1600.0,3200.0,8000.0};
    vector<double> aValue = {0.78,0.8,0.83,0.85,0.88,0.88,0.9,0.93,0.95};
    vector<double> bValue = {1.1,1.1,1.1,1.1,1.1,1.1,1.1,1.1,1.1};
    double T = tValue[m-1];
    double alpha = aValue[m-1];
    double beta = bValue[m-1];
    cout<<"=====Local Search begin======"<<endl;
    cout<<"T = "<<T<<endl;
    cout<<"alpha = "<<alpha<<endl;
    cout<<"beta = "<<beta<<endl;
    int M = 10;
    int mIter = 0;
    while(TotalErr>0)
    {
        int i = rand()%n;
        int j1 = rand()%n;
        int j2 = rand()%n;
        if(j1==j2)continue;
        if(locked[i][j1] || locked[i][j2])continue;

        int val1 = boardVal[i][j1];
        int val2 = boardVal[i][j2];
        int bx1 = blockX(i,j1,m);
        int by1 = blockY(i,j1,m);
        int bx2 = blockX(i,j2,m);
        int by2 = blockY(i,j2,m);
        if(col[j1][val1].size()==1 && col[j2][val2].size()==1 && block[bx1][val1].size()==1 && block[bx2][val2].size()==1)continue;

        iter++;
        mIter++;
        int tmpErr = 0;

        tmpErr-=2*(col[j1][val1].size()-1);
        tmpErr+=2*(col[j1][val2].size());
        tmpErr-=2*(col[j2][val2].size()-1);
        tmpErr+=2*(col[j2][val1].size());
        if(bx1!=bx2)
        {
            tmpErr-=2*(block[bx1][val1].size()-1);
            tmpErr+=2*(block[bx1][val2].size());
            tmpErr-=2*(block[bx2][val2].size()-1);
            tmpErr+=2*(block[bx2][val1].size());
        }

        if(exp((-tmpErr)/T)*999>rand()%1000)
        {
            TotalErr += tmpErr;
            int tmp = boardVal[i][j1];
            boardVal[i][j1] = boardVal[i][j2];
            boardVal[i][j2] = tmp;

            col[j1][val1].erase(i);
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
        if(mIter == M)
        {
            mIter = 0;
            T *= alpha;
            M *= beta;
        }
        if(iter % (2000*m*m*m) == 0)
        {
            cout<<"--------------------------"<<endl;
            cout<<"Iter:"<<iter<<endl;
            cout<<"TotalErr"<<TotalErr<<endl;
            cout<<"T="<<T<<endl;
            cout<<"M="<<M<<endl;
            cout<<"Random Restart Begin"<<endl;
            M = 10;
            T = tValue[m];
            mIter = 0;
            initRandom();
            TotalErr  = localError();
        }
    }
    cout<<"--------------------------"<<endl;
    cout<<"Total iteration:"<<iter<<endl;
    cout<<"TotalErr"<<TotalErr<<endl;
    cout<<"=========End Local========="<<endl;
    return true;
}

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
    if (isRunning) {
        return;
    }
    isRunning = true;
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
        this->prune();
        if (algType == DFS) {
            this->backtrack();
        }
        else if (algType == OPTIMIZEDDFS) {
            this->backtrack2();
        }
        else if (algType == LOCALSEARCH) {
            this->localSearch();
        }
        QMessageBox::about(this, "Result", tr("Calculation complete!"));
        if(judgeInput())cout<<"Correct!!!"<<endl;
    }
    dstop = clock();
    cout<<dstop-dbegin<<endl;
    isRunning = false;
}

// initialized by computer algorithm
void MainWindow::autoInit()
{
    if (isRunning) {
        return;
    }
    clear();
    bool savDoInteraction = doInteraction;
    doInteraction = false;
    randomSolution();
    int n = boardSize*boardSize;
    int crossNum = n*n + rand()%(n*n);
    cout << crossNum<<endl;
    for(int i=0;i<crossNum;i++)
    {
        int x = rand()%n;
        int y = rand()%n;
        board[x][y]->setter(0);
    }
    doInteraction = savDoInteraction;
}

// clear the board
void MainWindow::clear()
{
    if (isRunning) {
        return;
    }
    bool savDoInteraction = doInteraction;
    doInteraction = false;
    for (int i = 0; i < board.size(); ++i) {
        for (int j = 0; j < board[i].size(); ++j) {
            board[i][j]->setter(0);
            board[i][j]->changeTextColor(Qt::black);
            board[i][j]->setGridEnable(true);
        }
    }
    doInteraction = savDoInteraction;
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
    if (isRunning) {
        return;
    }
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
    algOptimizedDFS->setChecked(false);
    algLocalSearch->setChecked(false);
    algType = DFS;
    cout << "DFS" << endl;
}

void MainWindow::changeAlgtoOptimizedDFS()
{
    algDFS->setChecked(false);
    algOptimizedDFS->setChecked(true);
    algLocalSearch->setChecked(false);
    algType = OPTIMIZEDDFS;
    cout << "Optimized DFS" << endl;
}

void MainWindow::changeAlgtoLocalSearch()
{
    algDFS->setChecked(false);
    algOptimizedDFS->setChecked(false);
    algLocalSearch->setChecked(true);
    algType = LOCALSEARCH;
    cout << "Local Search (SA)" << endl;
}

void MainWindow::changeDisplayInterval()
{
    if (isRunning) {
        return;
    }
    bool isOK;
    int newDisplayInterval = 1000 * QInputDialog::getInt(NULL, "Change Display Interval", "Change the display interval (measured by the millisecond).\nPlease enter an integer between 0 and 3000.",
                                 0, 0, 3000, 100, &isOK);
    if(isOK) {
        DisplayInterval = newDisplayInterval;
    }
}


void MainWindow::showInteractionYes()
{
    interactionYes->setChecked(true);
    interactionNo->setChecked(false);
    doInteraction = true;
    cout << "interaction yes" << endl;
}

void MainWindow::showInteractionNo()
{
    interactionYes->setChecked(false);
    interactionNo->setChecked(true);
    doInteraction = false;
    cout << "interaction no" << endl;
}
