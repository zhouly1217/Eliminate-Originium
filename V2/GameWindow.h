#ifndef GAME_WINDOW_H
#define GAME_WINDOW_H

#include "Ball.h"
#include "Brick.h"
#include "Paddle.h"

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
    class GameWindow;
}
QT_END_NAMESPACE

enum class Difficulty {
    Easy,
    Medium,
    Hard
};


enum class GameState {
    Menu,    
    Playing  
};

class GameWindow : public QMainWindow
{
    Q_OBJECT

public:
    GameWindow(QWidget* parent = nullptr);
    ~GameWindow();

    void stateInit();

    void fillBricksInArea(double aleft, double atop, double awidth, double aheight,
        uint horizontalDivideCount, int verticalRowCount, QVector<Brick>& bricks);
    void collisionDetect();

private:
    Ui::GameWindow* ui;
    QPixmap m_backgroundTexture;

    Difficulty m_currentDifficulty = Difficulty::Easy;
    GameState m_gameState = GameState::Menu; 

    RectangleGeometry m_rightFrame;
    RectangleGeometry m_topFrame;
    RectangleGeometry m_leftFrame;
    RectangleGeometry m_bottomFrame;
    Paddle m_paddle;
    Ball m_ball;
    QVector<Brick> m_bricks;

    int m_stateTimerId;
    int m_paintTimerId;

    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void timerEvent(QTimerEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

private slots:
    void onHit(int);
};

#endif 