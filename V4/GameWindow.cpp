#include "GameWindow.h"
#include "ui_GameWindow.h"

#include <QPaintEvent>
#include <QDebug>
#include <QFont> 

#define DIVIDE_NUM      8
#define PADDLE_MOVE_SPEED   3.0
#define PADDLE_ROTATE_SPEED 0.15
#define BALL_MOVE_SPEED  0.5

GameWindow::GameWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::GameWindow)
{
    ui->setupUi(this);

    m_hitSound = new QSoundEffect(this);
    m_hitSound->setSource(QUrl("qrc:/audio/hit.mp3"));
    m_hitSound->setVolume(0.8);


    m_audioOutput = new QAudioOutput(this);
    m_bgPlayer = new QMediaPlayer(this);
    m_bgPlayer->setAudioOutput(m_audioOutput);
    m_bgPlayer->setLoops(QMediaPlayer::Infinite);


    m_bgPlayer->setSource(QUrl("qrc:/audio/menu_bgm.mp3"));
    m_bgPlayer->play();

    m_gameState = GameState::Menu;
    stateInit();

    m_stateTimerId = startTimer(1);
    m_paintTimerId = startTimer(10);
}

GameWindow::~GameWindow()
{
    delete ui;
    delete m_hitSound;
    delete m_bgPlayer;
    delete m_audioOutput;
}

void GameWindow::stateInit()
{
    m_topFrame.setWidth(width());
    m_topFrame.setHeight(height() / 50.0);
    m_topFrame.setPosition(width() / 2.0, height() - m_topFrame.height() / 2);

    m_bottomFrame.setWidth(width());
    m_bottomFrame.setHeight(height() / 50.0);
    m_bottomFrame.setPosition(width() / 2.0, m_bottomFrame.height() / 2);

    m_backgroundTexture.load(":/images/background.png");
    m_rightFrame.setWidth(m_topFrame.height());
    m_rightFrame.setHeight(height());

    m_rightFrame.setPosition(width() - m_rightFrame.width() / 2, height() / 2);
    m_leftFrame = m_rightFrame;
    m_leftFrame.setPosition(m_leftFrame.width() / 2, height() / 2);

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
        ballSpeed = 2.0;
        break;
    case Difficulty::Medium:
        verticalRowCount = 4;
        ballSpeed = 3.0;
        break;
    case Difficulty::Hard:
        double hexSideLength = brickWidth / horizontalDivideCount / sqrt(3);
        verticalRowCount = brickHeight / hexSideLength / 1.5;
        ballSpeed = 4.0;
        break;
    }

    fillBricksInArea(brickLeft, brickTop, brickWidth, brickHeight,
        horizontalDivideCount, verticalRowCount, m_bricks);

    m_paddle.setHeight(height() / 60.0);
    m_paddle.setWidth(width() / 3.0);
    m_paddle.setPosition(width() / 2.0, m_paddle.height() * 3);
    m_paddle.setTexture(":/images/Hitbar.png");


    m_ball.setRadius(m_paddle.width() / 15);
    if (m_gameState == GameState::Menu) {

        m_ball.setPosition(width() / 2.0, height() / 2.0);
        m_ball.setSpeed(1.0);
    }
    else {

        m_ball.setPosition(m_paddle.positionX(),
            m_paddle.positionY() + m_paddle.height() / 2.0 + m_ball.radius());
        m_ball.setSpeed(ballSpeed);
    }

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
    if (m_gameState == GameState::Menu) {

        double delta;
        delta = abs(m_leftFrame.positionX() - m_ball.positionX());
        if (delta <= m_leftFrame.width() / 2 + m_ball.radius() + 1) {
            m_ball.updateCollisionUnits(m_leftFrame.getCollisionUnits());
        }
        delta = abs(m_rightFrame.positionX() - m_ball.positionX());
        if (delta <= m_rightFrame.width() / 2 + m_ball.radius() + 1) {
            m_ball.updateCollisionUnits(m_rightFrame.getCollisionUnits());
        }
        delta = abs(m_topFrame.positionY() - m_ball.positionY());
        if (delta <= m_topFrame.height() / 2 + m_ball.radius()) {
            m_ball.updateCollisionUnits(m_topFrame.getCollisionUnits());
        }
        delta = abs(m_bottomFrame.positionY() - m_ball.positionY());
        if (delta <= m_bottomFrame.height() / 2 + m_ball.radius()) {
            m_ball.updateCollisionUnits(m_bottomFrame.getCollisionUnits());
        }
        return;
    }


    double delta;

    delta = abs(m_leftFrame.positionX() - m_ball.positionX());
    if (delta <= m_leftFrame.width() / 2 + m_ball.radius() + 1) {
        m_ball.updateCollisionUnits(m_leftFrame.getCollisionUnits());
    }
    delta = abs(m_rightFrame.positionX() - m_ball.positionX());
    if (delta <= m_rightFrame.width() / 2 + m_ball.radius() + 1) {
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

    if (m_gameState == GameState::Menu) {

        if (key == Qt::Key_1) {
            m_currentDifficulty = Difficulty::Easy;
            stateInit();
   
        }
        else if (key == Qt::Key_2) {
            m_currentDifficulty = Difficulty::Medium;
            stateInit();

        }
        else if (key == Qt::Key_3) {
            m_currentDifficulty = Difficulty::Hard;
            stateInit();

        }
        else {

            m_gameState = GameState::Playing;
            m_bgPlayer->stop();
            m_bgPlayer->setSource(QUrl("qrc:/audio/game_bgm.mp3"));
            m_bgPlayer->play();

            stateInit();
            qDebug() << "开始游戏！当前难度：" << static_cast<int>(m_currentDifficulty);
        }
        return;
    }

    if (key == Qt::Key_1 || key == Qt::Key_2 || key == Qt::Key_3) {
        m_currentDifficulty = (key == Qt::Key_1) ? Difficulty::Easy : (key == Qt::Key_2) ? Difficulty::Medium : Difficulty::Hard;
        stateInit();
        qDebug() << "切换到" << (m_currentDifficulty == Difficulty::Easy ? "简单" : m_currentDifficulty == Difficulty::Medium ? "中等" : "困难") << "难度";
    }

    if (key == Qt::Key_Escape && m_gameState == GameState::Playing) {
        m_gameState = GameState::Menu;
        m_bgPlayer->stop();
        m_bgPlayer->setSource(QUrl("qrc:/audio/menu_bgm.mp3"));
        m_bgPlayer->play();
        stateInit();
    }

    if (key == Qt::Key_1) {
        m_currentDifficulty = Difficulty::Easy;
        stateInit();

    }
    if (key == Qt::Key_2) {
        m_currentDifficulty = Difficulty::Medium;
        stateInit();
 
    }
    if (key == Qt::Key_3) {
        m_currentDifficulty = Difficulty::Hard;
        stateInit();

    }

    if (key == Qt::Key_A)
        m_paddle.setMovementSpeed(-1 * PADDLE_MOVE_SPEED);
    if (key == Qt::Key_D)
        m_paddle.setMovementSpeed(PADDLE_MOVE_SPEED);
}

void GameWindow::keyReleaseEvent(QKeyEvent* event)
{

    if (m_gameState != GameState::Playing) return;

    int key = event->key();
    if (key == Qt::Key_A || key == Qt::Key_D)
        m_paddle.setMovementSpeed(0);
}

void GameWindow::timerEvent(QTimerEvent* event)
{
    if (event->timerId() == m_stateTimerId) {

        if (m_gameState == GameState::Playing) {
            m_paddle.updateState();
        }

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


    if (m_gameState == GameState::Menu) {

        QFont titleFont("Microsoft YaHei", 30, QFont::Bold);
        QFont tipFont("Microsoft YaHei", 20);
        painter.setPen(Qt::white);

        painter.setFont(titleFont);
        QString title = "苹果乐消除源石";
        QRect titleRect = painter.boundingRect(rect(), Qt::AlignCenter, title);
        titleRect.moveTop(rect().top() + 100);
        painter.drawText(titleRect, Qt::AlignCenter, title);

        painter.setFont(tipFont);
        QString diffText = QString("当前难度：%1").arg(
            m_currentDifficulty == Difficulty::Easy ? "简单" :
            m_currentDifficulty == Difficulty::Medium ? "中等" : "困难"
        );
        QRect diffRect = painter.boundingRect(rect(), Qt::AlignCenter, diffText);
        diffRect.moveTop(titleRect.bottom() + 50);
        painter.drawText(diffRect, Qt::AlignCenter, diffText);

        QString selectTip = "按 1(简单) / 2(中等) / 3(困难) 选择难度";
        QRect selectRect = painter.boundingRect(rect(), Qt::AlignCenter, selectTip);
        selectRect.moveTop(diffRect.bottom() + 20);
        painter.drawText(selectRect, Qt::AlignCenter, selectTip);

        QString startTip = "按任意键开始游戏";
        QRect startRect = painter.boundingRect(rect(), Qt::AlignCenter, startTip);
        startRect.moveTop(selectRect.bottom() + 50);
        painter.drawText(startRect, Qt::AlignCenter, startTip);

        m_topFrame.drawSelf(&painter, height());
        m_bottomFrame.drawSelf(&painter, height());
        m_rightFrame.drawSelf(&painter, height());
        m_leftFrame.drawSelf(&painter, height());
        m_ball.drawSelf(&painter, height());
        return;
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
    m_hitSound->play();

    int i = 0;
    bool flag = false;
    for (; i < m_bricks.size(); i++) {
        if (m_bricks[i].getModelId() == modelId) {
            m_bricks[i].setDurability(m_bricks[i].durability() - 1);
            if (m_bricks[i].durability() < 1) {
                flag = true;

                double currentSpeed = m_ball.speed();
                m_ball.setSpeed(currentSpeed + 0.1);

                double speedLimit = 4.0;

                if (m_ball.speed() > speedLimit) {
                    m_ball.setSpeed(speedLimit);
                }
            }
            break;
        }
    }

    if (flag) {
        m_bricks.remove(i);
    }
}
