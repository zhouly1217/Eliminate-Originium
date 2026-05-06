#ifndef PADDLE_H
#define PADDLE_H

#include "GeometryBase.h"
#include <QPixmap>

class Paddle : public RectangleGeometry
{
public:
    Paddle();
    void drawSelf(QPainter* p, int canvasHeight) override;
    inline void setTexture(const QString& path) {
        m_texture.load(path);
    }
    inline void setMovementSpeed(double speed)
    {
        m_movementSpeed = speed;
    }
    inline double movementSpeed() const
    {
        return m_movementSpeed;
    }

    void updateState();
    void updateCollisionUnits(QList<CollisionUnit> units);
    void handleCollision();

private:
    double m_movementSpeed = 0;
    QList<CollisionUnit> m_collisionUnits;
    double m_rotationSpeed = 0;
    QPixmap m_texture;

    void move();
};

#endif 
