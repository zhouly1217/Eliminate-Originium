#include "Paddle.h"
#include <QPainterPath>
#include <QtDebug>

Paddle::Paddle()
{

}

void Paddle::drawSelf(QPainter* p, int canvasHeight)
{
    updateVertexes();
    auto vertexes = this->vertexes();

    QVector<QPointF> points;
    for (int i = 0; i < 4; i++) {
        qreal x = positionX() + vertexes[i](0);
        qreal y = canvasHeight - (positionY() + vertexes[i](1));
        points.append(QPointF(x, y));
    }

    QPainterPath path;
    path.addPolygon(points);
    path.closeSubpath();

    if (!m_texture.isNull()) {
        p->save();
        p->setClipPath(path);
        p->drawPixmap(path.boundingRect(), m_texture, m_texture.rect());
        p->restore();
    }
    else {
        p->drawPolygon(points);
    }
}

void Paddle::move()
{
    setPosition(positionX() + m_movementSpeed, positionY());
}

void Paddle::updateState()
{
    move();
    updateVertexes();
    handleCollision();
}

void Paddle::updateCollisionUnits(QList<CollisionUnit> units)
{
    m_collisionUnits = units;
}

void Paddle::handleCollision()
{
    for (int i = 0; i < m_collisionUnits.size(); i++) {
        QList<CollisionUnit> selfUnits = getCollisionUnits();
        for (int j = 0; j < selfUnits.size(); j++) {
            Matrix2d matrix = m_collisionUnits[i].entity;
            Vector2d positionVec(positionX(), positionY());

            double y = selfUnits[j].entity(1, 1) - selfUnits[j].entity(1, 0);
            double x = selfUnits[j].entity(0, 1) - selfUnits[j].entity(0, 0);
            double rad1 = atan2(y, x);
            Matrix2d rotationMatrix1;
            rotationMatrix1 << cos(-1 * rad1), -1 * sin(-1 * rad1),
                sin(-1 * rad1), cos(-1 * rad1);

            matrix = rotationMatrix1 * matrix;
            selfUnits[j].entity = rotationMatrix1 * selfUnits[j].entity;
            positionVec = rotationMatrix1 * positionVec;

            double y1 = matrix(1, 0);
            double y2 = matrix(1, 1);
            if (y1 < y2) {
                double temp = y1;
                y1 = y2;
                y2 = temp;
            }
            if (selfUnits[j].entity(1, 1) >= y2 && selfUnits[j].entity(1, 1) <= y1) {
                y = matrix(1, 1) - matrix(1, 0);
                x = matrix(0, 1) - matrix(0, 0);
                double rad2 = atan2(y, x);
                Matrix2d rotationMatrix2;
                rotationMatrix2 << cos(-1 * rad2), -1 * sin(-1 * rad2),
                    sin(-1 * rad2), cos(-1 * rad2);

                matrix = rotationMatrix2 * matrix;
                selfUnits[j].entity = rotationMatrix2 * selfUnits[j].entity;
                positionVec = rotationMatrix2 * positionVec;

                y1 = selfUnits[j].entity(1, 0);
                y2 = selfUnits[j].entity(1, 1);
                if (y1 < y2) {
                    double temp = y1;
                    y1 = y2;
                    y2 = temp;
                }

                if (matrix(1, 1) >= y2 && matrix(1, 1) <= y1) {
                    double delta = y1 - matrix(1, 1);
                    qDebug() << "delta=" << delta;
                    positionVec(1) -= (delta + 0.01);

                    Matrix2d rotationMatrix3;
                    rotationMatrix3 << cos(rad1 + rad2), -1 * sin(rad1 + rad2),
                        sin(rad1 + rad2), cos(rad1 + rad2);

                    positionVec = rotationMatrix3 * positionVec;
                    setPosition(positionVec(0), positionVec(1));

                    m_collisionUnits.clear();
                    return;
                }
                else {
                    continue;
                }
            }
            else {
                continue;
            }
        }
    }
    m_collisionUnits.clear();
}