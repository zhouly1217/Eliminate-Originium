#ifndef BALL_H
#define BALL_H

#include "GeometryBase.h"

#include <functional>
#include <QPixmap>
#include <QPainter>
#include <QStyleOptionGraphicsItem>  
#include <QPainterPath>  

class Ball : public CircleGeometry
{
public:
    Ball();
    ~Ball();

    void updateState();

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option = nullptr, QWidget* widget = nullptr);
    inline void setSpeed(double speed)
    {
        m_movementSpeed = speed;
        m_velocity(0) = m_movementSpeed * cos(m_movementAngle * M_PI / 180);
        m_velocity(1) = m_movementSpeed * sin(m_movementAngle * M_PI / 180);
    }

    inline void updateCollisionUnits(QList<CollisionUnit> units)
    {
        m_collisionUnits = units;
    }
    inline void registerOnHitCallback(std::function<void(int)> callback)
    {
        m_onHitCallback = callback;
    }

    void drawSelf(QPainter* painter, int canvasHeight) override;
    inline double speed() const
    {
        return m_movementSpeed;
    }

private:
    double m_movementSpeed = 0;
    double m_movementAngle = 0;
    Vector2d m_velocity;      
    QList<CollisionUnit> m_collisionUnits;

    QPixmap m_ballTexture;
    void move();
    void handleCollision();

    std::function<void(int)> m_onHitCallback = nullptr;
};

#endif 