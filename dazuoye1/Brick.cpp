#include "Brick.h"

#include <QRandomGenerator>
#include <QPainterPath>
#include <QVector>

Brick::Brick()
{
    m_durability = 1 + QRandomGenerator::global()->bounded(9);
}

Brick::~Brick()
{

}

void Brick::setTexture(const QString& imagePath)
{
    m_brickTexture.load(imagePath);
}

void Brick::drawSelfEx(QPainter* p, int canvasHeight)
{
    updateVertexes();
    auto vertexes = this->vertexes();

    QVector<QPointF> points;
    for (int i = 0; i < 6; i++) {
        qreal x = positionX() + vertexes[i](0);
        qreal y = canvasHeight - (positionY() + vertexes[i](1));
        points.append(QPointF(x, y));
    }

    QPainterPath path;
    path.addPolygon(points);
    path.closeSubpath();

    if (!m_brickTexture.isNull()) {
        p->save();
        p->setClipPath(path);
        p->drawPixmap(path.boundingRect(), m_brickTexture, m_brickTexture.rect());
        p->restore();
    }
    else {
        drawSelf(p, canvasHeight);
    }

    p->setPen(Qt::black);
    qreal offsetX = -40;
    qreal offsetY = 30;
    QPointF textPos(positionX() + offsetX, canvasHeight - positionY() + offsetY);
    p->drawText(QPointF(positionX(), canvasHeight - positionY()), QString::number(m_durability));
}