#include <QGuiApplication>
#include <QDebug>

#include "LineSeries.h"

LineSeries::LineSeries() : QLineSeries(),
                           m_xMin(std::numeric_limits<qreal>::max()),
                           m_xMax(std::numeric_limits<qreal>::min()),
                           m_yMin(std::numeric_limits<qreal>::max()),
                           m_yMax(std::numeric_limits<qreal>::min())
{
    connect(this, &LineSeries::pressed, this, [this]() {
        if (QGuiApplication::mouseButtons() & Qt::LeftButton)
        {
            emit leftClicked(this);
        }
        else if (QGuiApplication::mouseButtons() & Qt::RightButton)
        {
            emit rightClicked(this);
        }
    });
}

void LineSeries::append(qreal x, qreal y)
{
    static QRectF m_bounds = bounds();

    if (x > m_xMax)
    {
        m_xMax = x;
    }

    if (x < m_xMin)
    {
        m_xMin = x;
    }

    if (y > m_yMax)
    {
        m_yMax = y;
    }

    if (y < m_yMin)
    {
        m_yMin = y;
    }

    if (m_bounds != bounds())
    {
        m_bounds = bounds();
        emit boundsChanged(m_bounds);
    }

    QLineSeries::append(x, y);
}

QRectF LineSeries::bounds() const
{
    int xMin, yMin, xMax, yMax;

    if (m_xMin == std::numeric_limits<qreal>::max())
    {
        xMin = 0;
    }
    else
    {
        xMin = m_xMin;
    }

    if (m_yMin == std::numeric_limits<qreal>::max())
    {
        yMin = 0;
    }
    else
    {
        yMin = m_yMin;
    }

    if (m_xMax == std::numeric_limits<qreal>::min())
    {
        xMax = 1;
    }
    else
    {
        xMax = m_xMax;
    }

    if (m_yMax == std::numeric_limits<qreal>::min())
    {
        yMax = 1;
    }
    else
    {
        yMax = m_yMax;
    }

    return QRectF(xMin, yMin, xMax - xMin, yMax - yMin);
}
