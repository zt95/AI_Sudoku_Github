#ifndef GRID_H
#define GRID_H
#include <QLineEdit>
#include <QValidator>
extern int DisplayInterval;
extern int boardSize;
extern int gridSize;
void changeSize(int newBoardSize);
int getGridSize(int bdSize);
enum Alg {DFS, LOCALSEARCH};
extern Alg algType;
extern bool doInteraction;

class Grid : public QWidget
{
    Q_OBJECT

public:
    Grid(QWidget *parent=0);
    void setter(int val);
    int getter();
    void setGridEnable(bool isEnabled);
    //void changeTextColor(const QColor& color);

private:
    int value; // 0 stands for null value
    QLineEdit *line_edit;

signals:
    void textChanged(QString);
public slots:
    void changeTextColor(const QColor& color);

private slots:
    void changeTextData(const QString& textData);
};


#endif // GRID_H
