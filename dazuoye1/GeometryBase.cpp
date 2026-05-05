#include "GeometryBase.h"

void RectangleGeometry::updateVertexes()
{
    QVector<Vector2d> vertexes(5);
    vertexes[0](0) = -1 * m_width / 2;
    vertexes[0](1) = -1 * m_height / 2;
    vertexes[1](0) = m_width / 2;
    vertexes[1](1) = -1 * m_height / 2;
    vertexes[2](0) = m_width / 2;
    vertexes[2](1) = m_height / 2;
    vertexes[3](0) = -1 * m_width / 2;
    vertexes[3](1) = m_height / 2;
    vertexes[4] = vertexes[0];

    double rad = rotationAngle() * M_PI / 180;
    Matrix2d rotationMatrix;
    rotationMatrix << cos(rad), -1 * sin(rad),
        sin(rad), cos(rad);
    for (int i = 0; i < 5; i++) {
        vertexes[i] = rotationMatrix * vertexes[i];
    }
    setVertexes(vertexes);
}

void RectangleGeometry::drawSelf(QPainter* p, int canvasHeight)
{
    updateVertexes();
    auto vertexes = this->vertexes();

    QPointF points[5];
    for (int i = 0; i < 5; i++) {
        points[i].setX(positionX() + vertexes[i](0));
        points[i].setY(canvasHeight - (positionY() + vertexes[i](1)));
    }

    for (int i = 0; i < 4; i++) {
        p->drawLine(points[i], points[i + 1]);
    }
}

QList<CollisionUnit> RectangleGeometry::getCollisionUnits()
{
    updateVertexes();

    QList<CollisionUnit> result;
    for (int i = 0; i < 4; i++) {
        CollisionUnit unit;
        unit.modelId = 0;

        auto vertexes = this->vertexes();
        Matrix2d matrix;
        matrix << vertexes[i](0) + positionX(), vertexes[i + 1](0) + positionX(),
            vertexes[i](1) + positionY(), vertexes[i + 1](1) + positionY();
        unit.entity = matrix;
        result << unit;
    }

    return result;
}

void CircleGeometry::updateVertexes()
{

}

void CircleGeometry::drawSelf(QPainter* p, int canvasHeight)
{
    p->drawEllipse(QPointF(positionX(), canvasHeight - positionY()), radius(), radius());
}

QList<CollisionUnit> CircleGeometry::getCollisionUnits()
{
    return QList<CollisionUnit>();
}

void HexagonGeometry::updateVertexes()
{
    QVector<Vector2d> vertexes(7);
    vertexes[0](0) = 0;
    vertexes[0](1) = 0 - radius();
    vertexes[1](0) = vertexes[0](0) + sqrt(3) / 2.0 * radius();
    vertexes[1](1) = vertexes[0](1) + radius() / 2.0;
    vertexes[2](0) = vertexes[1](0);
    vertexes[2](1) = vertexes[1](1) + radius();
    vertexes[3](0) = vertexes[0](0);
    vertexes[3](1) = vertexes[0](1) + radius() * 2.0;
    vertexes[4](0) = vertexes[0](0) - sqrt(3) / 2.0 * radius();
    vertexes[4](1) = vertexes[2](1);
    vertexes[5](0) = vertexes[4](0);
    vertexes[5](1) = vertexes[1](1);
    vertexes[6] = vertexes[0];

    double rad = rotationAngle() * M_PI / 180;
    Matrix2d rotationMatrix;
    rotationMatrix << cos(rad), -1 * sin(rad),
        sin(rad), cos(rad);
    for (int i = 0; i < 7; i++) {
        vertexes[i] = rotationMatrix * vertexes[i];
    }
    setVertexes(vertexes);
}

void HexagonGeometry::drawSelf(QPainter* p, int canvasHeight)
{
    updateVertexes();
    auto vertexes = this->vertexes();

    QPointF points[7];
    for (int i = 0; i < 7; i++) {
        points[i].setX(positionX() + vertexes[i](0));
        points[i].setY(canvasHeight - (positionY() + vertexes[i](1)));
    }

    for (int i = 0; i < 6; i++) {
        p->drawLine(points[i], points[i + 1]);
    }
}

QList<CollisionUnit> HexagonGeometry::getCollisionUnits()
{
    updateVertexes();

    QList<CollisionUnit> result;
    for (int i = 0; i < 6; i++) {
        CollisionUnit unit;
        unit.modelId = getModelId();

        auto vertexes = this->vertexes();
        Matrix2d matrix;
        matrix << vertexes[i](0) + positionX(), vertexes[i + 1](0) + positionX(),
            vertexes[i](1) + positionY(), vertexes[i + 1](1) + positionY();
        unit.entity = matrix;
        result << unit;
    }

    return result;
}