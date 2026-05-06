#include "Ball.h"

#include <QRandomGenerator>
#include <QDebug>

Ball::Ball()
{
    do {
        m_movementAngle = 45 + QRandomGenerator::global()->bounded(90);
    } while (m_movementAngle > 80 && m_movementAngle < 100);

    qDebug() << "move angle:" << m_movementAngle;
    m_ballTexture.load(":/images/nankai.png");
}

Ball::~Ball()
{

}

void Ball::updateState()
{
    move();
    handleCollision();
}

void Ball::move()
{
    setPosition(positionX() + m_velocity(0), positionY() + m_velocity(1));
}

void Ball::handleCollision()
{
    for (int i = 0; i < m_collisionUnits.size(); i++) {
        Matrix2d matrix1 = m_collisionUnits[i].entity;
        double y = matrix1(1, 1) - matrix1(1, 0);
        double x = matrix1(0, 1) - matrix1(0, 0);
        double rad = atan2(y, x);

        Vector2d vec1;
        vec1 << positionX(), positionY();

        Matrix2d rotationMatrix1, rotationMatrix2;
        rotationMatrix1 << cos(-1 * rad), -1 * sin(-1 * rad),
            sin(-1 * rad), cos(-1 * rad);
        rotationMatrix2 << cos(rad), -1 * sin(rad),
            sin(rad), cos(rad);

        matrix1 = rotationMatrix1 * matrix1;
        vec1 = rotationMatrix1 * vec1;

        double delta = abs(vec1(1) - matrix1(1, 1)) - radius();
        if (delta <= 0
            && vec1(0) >= matrix1(0, 0)
            && vec1(0) <= matrix1(0, 1)) {
            m_velocity = rotationMatrix1 * m_velocity;
            Matrix2d flipMatrix;
            flipMatrix << 1, 0,
                0, -1;
            m_velocity = flipMatrix * m_velocity;
            m_velocity = rotationMatrix2 * m_velocity;

            vec1(1) = vec1(1) + 2 * delta;
            vec1 = rotationMatrix2 * vec1;
            setPosition(vec1(0), vec1(1));

            if (m_onHitCallback)
                m_onHitCallback(m_collisionUnits[i].modelId);

            break;
        }
    }
    m_collisionUnits.clear();
}

void Ball::drawSelf(QPainter* painter, int canvasHeight)
{
    painter->setRenderHint(QPainter::Antialiasing);
    double r = radius();
    QRectF rect(positionX() - r, canvasHeight - positionY() - r, 2 * r, 2 * r);

    if (!m_ballTexture.isNull()) {
        painter->save();
        QPainterPath path;
        path.addEllipse(rect);
        painter->setClipPath(path);
        painter->drawPixmap(rect, m_ballTexture, m_ballTexture.rect());
        painter->restore();
    }
    else {
        painter->setBrush(Qt::red);
        painter->drawEllipse(rect);
    }
}