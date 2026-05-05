#include "GameWindow.h"
#include "ui_GameWindow.h"

#include <QPaintEvent>
#include <QDebug>

#define DIVIDE_NUM      8
#define PADDLE_MOVE_SPEED   1.5
#define PADDLE_ROTATE_SPEED 0.15
#define BALL_MOVE_SPEED  0.5

GameWindow::GameWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::GameWindow)
{
    ui->setupUi(this);

    stateInit();

    m_stateTimerId = startTimer(1);
    m_paintTimerId = startTimer(10);
}

GameWindow::~GameWindow()
{
    delete ui;
}

void GameWindow::stateInit()
{
    m_topFrame.setWidth(width());
    m_topFrame.setHeight(height() / 50.0);
    m_topFrame.setPosition(width() / 2.0, height() - m_topFrame.height() / 2);

    m_backgroundTexture.load(":/images/background.png");
    m_rightFrame.setWidth(m_topFrame.height());
    m_rightFrame.setHeight(height() - m_topFrame.height());

    m_rightFrame.setPosition(width() - m_rightFrame.width() / 2, m_rightFrame.height() / 2);
    m_leftFrame = m_rightFrame;
    m_leftFrame.setPosition(m_leftFrame.width() / 2, m_leftFrame.positionY());

    m_bricks.clear();

    double brickLeft = m_leftFrame.positionX() + m_leftFrame.width() / 2 + 30;
    double brickTop = m_topFrame.positionY() - m_topFrame.height() / 2 - 30;
    double brickWidth = width() - m_rightFrame.width() * 2 - 60;
    double brickHeight = height() / 2.0 - 30;
    uint horizontalDivideCount = DIVIDE_NUM;

    int verticalRowCount = 0;
    double ballSpeed = 0.0;

    switch (m_currentDifficulty) {
    case Difficulty::Easy:
        verticalRowCount = 3;
        ballSpeed = 1.5;
        break;
    case Difficulty::Medium:
        verticalRowCount = 4;
        ballSpeed = 1.7;
        break;
    case Difficulty::Hard:
        double hexSideLength = brickWidth / horizontalDivideCount / sqrt(3);
        verticalRowCount = brickHeight / hexSideLength / 1.5;
        ballSpeed = 2.0;
        break;
    }

    fillBricksInArea(brickLeft, brickTop, brickWidth, brickHeight,
        horizontalDivideCount, verticalRowCount, m_bricks);

    m_paddle.setHeight(height() / 60.0);
    m_paddle.setWidth(width() / 3.0);
    m_paddle.setPosition(width() / 2.0, m_paddle.height() * 3);
    m_paddle.setTexture(":/images/Hitbar.png");

    m_ball.setRadius(m_paddle.width() / 15);
    m_ball.setPosition(m_paddle.positionX(),
        m_paddle.positionY() + m_paddle.height() / 2.0 + m_ball.radius());

    m_ball.setSpeed(ballSpeed);
    m_ball.registerOnHitCallback([this](int modelId) {
        onHit(modelId);
        });
}

void GameWindow::fillBricksInArea(double aleft, double atop, double awidth, double aheight,
    uint horizontalDivideCount, int verticalRowCount, QVector<Brick>& bricks)
{
    double hexSideLength = awidth / horizontalDivideCount / sqrt(3);
    double hexWidth = awidth / horizontalDivideCount / 2;

    int verticalDivideCount = verticalRowCount;

    Brick first;
    first.setPosition(aleft + hexWidth, atop - hexSideLength);

    int n = 0;
    for (int j = 0; j < verticalDivideCount; j++) {
        for (uint i = 0; i < horizontalDivideCount; i++) {
            Brick brick;
            if (j % 2 == 0) {
                brick.setPosition(first.positionX() + hexWidth * 2 * i,
                    first.positionY() - hexSideLength * 1.5 * j);
            }
            else {
                brick.setPosition(first.positionX() + hexWidth * (2 * i + 1),
                    first.positionY() - hexSideLength * 1.5 * j);
            }
            brick.setRadius(hexSideLength);
            brick.setTexture(":/images/Brick.png");
            if (brick.positionX() + hexWidth <= aleft + awidth) {
                n++;
                brick.setModelId(10000 + n);
                bricks << brick;
            }
        }
    }
}

void GameWindow::collisionDetect()
{
    double delta;

    delta = abs(m_leftFrame.positionX() - m_ball.positionX());
    if (delta <= m_leftFrame.width() / 2 + m_ball.radius()) {
        m_ball.updateCollisionUnits(m_leftFrame.getCollisionUnits());
    }
    delta = abs(m_rightFrame.positionX() - m_ball.positionX());
    if (delta <= m_rightFrame.width() / 2 + m_ball.radius()) {
        m_ball.updateCollisionUnits(m_rightFrame.getCollisionUnits());
    }
    delta = abs(m_topFrame.positionY() - m_ball.positionY());
    if (delta <= m_topFrame.height() / 2 + m_ball.radius()) {
        m_ball.updateCollisionUnits(m_topFrame.getCollisionUnits());
    }

    for (int i = 0; i < m_bricks.size(); i++) {
        delta = sqrt(pow(m_bricks[i].positionX() - m_ball.positionX(), 2) +
            pow(m_bricks[i].positionY() - m_ball.positionY(), 2));
        if (delta <= m_bricks[i].radius() + m_ball.radius()) {
            m_ball.updateCollisionUnits(m_bricks[i].getCollisionUnits());
        }
    }

    delta = sqrt(pow(m_paddle.positionX() - m_ball.positionX(), 2) +
        pow(m_paddle.positionY() - m_ball.positionY(), 2));
    double rw = sqrt(pow(m_paddle.width() / 2, 2) + pow(m_paddle.height() / 2, 2));
    if (delta <= rw + m_ball.radius()) {
        m_ball.updateCollisionUnits(m_paddle.getCollisionUnits());
    }

    delta = abs(m_paddle.positionX() - m_leftFrame.positionX());
    if (delta <= rw + m_leftFrame.width() / 2) {
        m_paddle.updateCollisionUnits(m_leftFrame.getCollisionUnits());
    }
    delta = abs(m_paddle.positionX() - m_rightFrame.positionX());
    if (delta <= rw + m_rightFrame.width() / 2) {
        m_paddle.updateCollisionUnits(m_rightFrame.getCollisionUnits());
    }

    delta = m_ball.positionY() - m_ball.radius();
    if (delta < 0) {
        stateInit();
    }
}

void GameWindow::keyPressEvent(QKeyEvent* event)
{
    int key = event->key();

    if (key == Qt::Key_1) {
        m_currentDifficulty = Difficulty::Easy;
        stateInit();
        qDebug() << "切换到简单难度：3行砖块，小球速度1.5";
    }
    if (key == Qt::Key_2) {
        m_currentDifficulty = Difficulty::Medium;
        stateInit();
        qDebug() << "切换到中等难度：4行砖块，小球速度1.7";
    }
    if (key == Qt::Key_3) {
        m_currentDifficulty = Difficulty::Hard;
        stateInit();
        qDebug() << "切换到困难难度：原有行数，小球速度2.0";
    }

    if (key == Qt::Key_A)
        m_paddle.setMovementSpeed(-1 * PADDLE_MOVE_SPEED);
    if (key == Qt::Key_D)
        m_paddle.setMovementSpeed(PADDLE_MOVE_SPEED);
}

void GameWindow::keyReleaseEvent(QKeyEvent* event)
{
    int key = event->key();

    if (key == Qt::Key_A || key == Qt::Key_D)
        m_paddle.setMovementSpeed(0);
}

void GameWindow::timerEvent(QTimerEvent* event)
{
    if (event->timerId() == m_stateTimerId) {
        m_paddle.updateState();
        m_ball.updateState();
        collisionDetect();
    }

    if (event->timerId() == m_paintTimerId)
        update();
}

void GameWindow::resizeEvent(QResizeEvent* event)
{
}

void GameWindow::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    if (!m_backgroundTexture.isNull()) {
        QPixmap scaledBg = m_backgroundTexture.scaled(
            this->size(),
            Qt::KeepAspectRatioByExpanding,
            Qt::SmoothTransformation
        );
        QRect bgRect = scaledBg.rect();
        bgRect.moveCenter(this->rect().center());
        painter.drawPixmap(bgRect, scaledBg);
    }
    QPen pen;
    pen.setColor(QColor(123, 123, 12));
    painter.setPen(pen);

    for (int i = 0; i < m_bricks.size(); i++) {
        m_bricks[i].drawSelfEx(&painter, height());
    }

    m_topFrame.drawSelf(&painter, height());
    m_rightFrame.drawSelf(&painter, height());
    m_leftFrame.drawSelf(&painter, height());

    m_paddle.drawSelf(&painter, height());

    m_ball.drawSelf(&painter, height());
}

void GameWindow::onHit(int modelId)
{
    int i = 0;
    bool flag = false;
    for (; i < m_bricks.size(); i++) {
        if (m_bricks[i].getModelId() == modelId) {
            m_bricks[i].setDurability(m_bricks[i].durability() - 1);
            if (m_bricks[i].durability() < 1) {
                flag = true;

                double currentSpeed = m_ball.speed();
                m_ball.setSpeed(currentSpeed + 0.05);

                if (m_ball.speed() > 2.0) m_ball.setSpeed(2.0);
            }
            break;
        }
    }

    if (flag) {
        m_bricks.remove(i);
    }
}