#include "grid.h"
#include <ctime>
#include <time.h>
#include <QApplication>

/*
 * Global variables
 * Initialized in the constructor of class MainWindow
*/
int boardSize;
int gridSize;
int DisplayInterval;
Alg algType;
bool doInteraction;

int getGridSize(int bdSize)
{
    return (750/(bdSize*bdSize))+1;
}

void changeSize(int newBoardSize)
{
    boardSize = newBoardSize;
    gridSize = getGridSize(boardSize);
}

Grid::Grid(QWidget *parent) : QWidget(parent)
{
    line_edit = new QLineEdit(this);
    line_edit->setAlignment(Qt::AlignCenter);

    QFont font;
    font.setPixelSize(0.7*gridSize);
    line_edit->setFont(font);
    line_edit->setMinimumSize(gridSize, gridSize);
    line_edit->setMaximumSize(gridSize, gridSize);

    QValidator *validator = new QIntValidator(1, boardSize*boardSize, this);
    line_edit->setValidator(validator);

    connect(line_edit, SIGNAL(textChanged(QString)), this, SLOT(changeTextData(QString)));

    line_edit->setText("");
    this->value = 0;
}

void Grid::setter(int val)
{
    if (val > 0) {
        this->value = val;
        line_edit->setText(QString::number(this->value));
        if (doInteraction) {
            QApplication::processEvents();
            time_t dbegin;
            dbegin = clock();
            while(true)
            {
                if(clock()-dbegin > DisplayInterval)break;
            }
        }
    }
    else {
        this->value = 0;
        line_edit->setText("");
        if (doInteraction) {
            QApplication::processEvents();
            time_t dbegin;
            dbegin = clock();
            while(true)
            {
                if(clock()-dbegin > DisplayInterval)break;
            }
        }
    }
}

int Grid::getter()
{
    return this->value;
}

void Grid::setGridEnable(bool isEnabled)
{
    line_edit->setEnabled(isEnabled);
}

void Grid::changeTextData(const QString& textData)
{
    if (textData.isEmpty()) {
        this->value = 0;
    }
    else {
        this->value = textData.toInt();
    }
}

void Grid::changeTextColor(const QColor& color)
{
    QPalette plt;
    plt.setColor(QPalette::Text, color);
    line_edit->setPalette(plt);
}
