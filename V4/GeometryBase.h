#ifndef GEOMETRY_BASE_H
#define GEOMETRY_BASE_H

#include <Eigen/Core>
#include <QPainter>

using namespace Eigen;

struct CollisionUnit
{
    Matrix2d entity;
    int modelId;
};

class GeometryBase
{
public:
    GeometryBase() {}
    ~GeometryBase() {}

    virtual void updateVertexes() = 0;
    virtual void drawSelf(QPainter* p, int canvasHeight) = 0;
    virtual QList<CollisionUnit> getCollisionUnits() = 0;

    inline void setPosition(double posX, double posY)
    {
        m_positionX = posX;
        m_positionY = posY;
    }
    inline double positionX() const
    {
        return m_positionX;
    }
    inline double positionY() const
    {
        return m_positionY;
    }
    inline void setRotationAngle(double angle)
    {
        m_rotationAngle = angle;
    }
    inline double rotationAngle() const
    {
        return m_rotationAngle;
    }
    inline QVector<Vector2d> vertexes() const
    {
        return m_vertexes;
    }
    inline void setVertexes(const QVector<Vector2d>& vertexes)
    {
        m_vertexes = vertexes;
    }
    inline int getModelId() const
    {
        return m_modelId;
    }
    inline void setModelId(int value)
    {
        m_modelId = value;
    }

private:
    int m_modelId = 0;
    double m_positionX = 0;
    double m_positionY = 0;
    double m_rotationAngle = 0;

    QVector<Vector2d> m_vertexes;
};

class RectangleGeometry : public GeometryBase
{
public:
    RectangleGeometry() {}
    ~RectangleGeometry() {}

    void updateVertexes() override;
    void drawSelf(QPainter* p, int canvasHeight) override;
    QList<CollisionUnit> getCollisionUnits() override;

    inline void setWidth(double width)
    {
        m_width = width;
    }
    inline void setHeight(double height)
    {
        m_height = height;
    }
    inline double width() const
    {
        return m_width;
    }
    inline double height() const
    {
        return m_height;
    }

private:
    double m_height = 0;
    double m_width = 0;
};

class CircleGeometry : public GeometryBase
{
public:
    CircleGeometry() {}
    ~CircleGeometry() {}

    void updateVertexes() override;
    void drawSelf(QPainter* p, int canvasHeight) override;
    QList<CollisionUnit> getCollisionUnits() override;

    inline void setRadius(double radius)
    {
        m_radius = radius;
    }
    inline double radius() const
    {
        return m_radius;
    }

private:
    double m_radius = 0;
};

class HexagonGeometry : public GeometryBase
{
public:
    HexagonGeometry() {}
    ~HexagonGeometry() {}

    inline void setRadius(double radius)
    {
        m_radius = radius;
    }
    inline double radius() const
    {
        return m_radius;
    }

    void updateVertexes() override;
    void drawSelf(QPainter* p, int canvasHeight) override;
    QList<CollisionUnit> getCollisionUnits() override;

private:
    double m_radius = 0;
};

#endif 
