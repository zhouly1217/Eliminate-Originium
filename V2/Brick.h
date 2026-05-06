#ifndef BRICK_H
#define BRICK_H

#include "GeometryBase.h"
#include <QPixmap>
#include <QPainter>
#include <QPainterPath>

class Brick : public HexagonGeometry
{
public:
    Brick();
    ~Brick();
    void setTexture(const QString& imagePath);
    void drawSelfEx(QPainter* p, int canvasHeight);

    inline void setDurability(int value)
    {
        m_durability = value;
    }
    inline int durability() const
    {
        return m_durability;
    }

private:
    int m_durability = 0;
    QPixmap m_brickTexture;
};

#endif 