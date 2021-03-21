#include "chart.h"
#include "lineSeries.h"

Chart::Chart()
    : QChart(),
      m_bounds(0, 0, 0, 0)
{
}

QRectF Chart::bounds() const
{
    return m_bounds;
}

void Chart::createDefaultAxes()
{
    QChart::createDefaultAxes();
    enableBoundChanges(true);

    xAxisGet()->setMinorTickCount(5);
    yAxisGet()->setMinorTickCount(5);
    xAxisGet()->setLabelFormat("%.2f");
    yAxisGet()->setLabelFormat("%.2f");

    connect(xAxisGet(), &QValueAxis::rangeChanged, this, &Chart::changeZoomLevel, Qt::UniqueConnection);
    connect(yAxisGet(), &QValueAxis::rangeChanged, this, &Chart::changeZoomLevel, Qt::UniqueConnection);
}

void Chart::addSeries(LineSeries *series)
{
    QChart::addSeries(series);
    lines << series;

    emit seriesAdded(series);

    if (m_bounds == QRectF(0, 0, 0, 0))
        m_bounds = series->bounds();
    else
        m_bounds = m_bounds.united(series->bounds());

    connect(series, &LineSeries::boundsChanged, this, lineBoundsChanges);

    emit boundsChanged(m_bounds);
}

int Chart::seriesCount()
{
    return series().count();
}

void Chart::seriesAppend(int line, QPointF data)
{
    bool reset = isZoomReseteed();

    if (line >= lines.count())
    {
        return;
    }

    lines[line]->append(data.x(), data.y());

    if (reset)
    {
        updateRange();
    }
}

void Chart::lineBoundsChanges(QRectF range)
{
    bool reset = isZoomReseteed();

    m_bounds = m_bounds.united(range);

    if (reset)
    {
        updateRange();
    }
}

void Chart::zoom(const QRectF &rect)
{
    enableBoundChanges(false);

    xAxisGet()->setRange(rect.left(), rect.width() + rect.left());
    yAxisGet()->setRange(rect.top(), rect.height() + rect.top());

    QRectF realRect = changeZoomLevel(false);
    if (m_bounds == realRect)
    {
        emit zoomed(realRect);
    }
    else if (realRect.left() < m_bounds.left() || realRect.right() > m_bounds.right())
    {
        this->zoomReset();
    }
    else
    {
        emit zoomed(realRect);
    }
    enableBoundChanges(true);

    return;
}

void Chart::zoomReset()
{
    enableBoundChanges(false);
    xAxisGet()->setRange(m_bounds.left(), m_bounds.width() + m_bounds.left());
    yAxisGet()->setRange(m_bounds.top(), m_bounds.height() + m_bounds.top());
    enableBoundChanges(true);
    updateRange();
    changeZoomLevel();
    return;
}

void Chart::setXAxiTickCount(int count)
{
    enableBoundChanges(false);
    xAxisGet()->setTickCount(count);
    enableBoundChanges(true);
}

void Chart::setYAxiTickCount(int count)
{
    enableBoundChanges(false);
    yAxisGet()->setTickCount(count);
    enableBoundChanges(true);
}

void Chart::setPointsVisible(bool visible)
{
    for (auto &&i : lines)
    {
        i->setPointsVisible(visible);
    }
}

void Chart::setPointLabelsVisible(bool visible)
{
    for (auto &&i : lines)
    {
        i->setPointLabelsVisible(visible);
    }
}

QRectF Chart::changeZoomLevel(bool emitSignal /*= true*/)
{
    auto rect = QRectF(xAxisGet()->min(), yAxisGet()->min(), xAxisGet()->max() - xAxisGet()->min(), yAxisGet()->max() - yAxisGet()->min());
    if (emitSignal)
    {
        emit zoomed(rect);
    }
    return rect;
}

void Chart::enableBoundChanges(bool val)
{
    if (val)
    {
        if (!m_xBoundsConnection)
        {
            m_xBoundsConnection = connect(
                xAxisGet(), &QValueAxis::rangeChanged, this, [this](qreal min, qreal max) {
                    if (min < m_bounds.left())
                    {
                        m_bounds.setLeft(min);
                    }
                    if (max > m_bounds.right())
                    {
                        m_bounds.setRight(max);
                    }
                    emit boundsChanged(m_bounds);
                },
                Qt::UniqueConnection);
        }

        if (!m_yBoundsConnection)
        {
            m_yBoundsConnection = connect(
                yAxisGet(), &QValueAxis::rangeChanged, this, [this](qreal min, qreal max) {
                    // remember, for QRect the 'top' is the lower value.
                    if (min < m_bounds.top())
                    {
                        m_bounds.setTop(min);
                    }
                    if (max > m_bounds.bottom())
                    {
                        m_bounds.setBottom(max);
                    }
                    emit boundsChanged(m_bounds);
                },
                Qt::UniqueConnection);
        }
    }
    else
    {
        disconnect(m_xBoundsConnection);
        disconnect(m_yBoundsConnection);
    }
}

bool Chart::isZoomReseteed()
{
    return xAxisGet()->min() == m_bounds.left() &&
           xAxisGet()->max() == m_bounds.width() + m_bounds.left() &&
           yAxisGet()->min() == m_bounds.top() &&
           yAxisGet()->max() == m_bounds.height() + m_bounds.top();
}

void Chart::updateRange()
{
    enableBoundChanges(false);
    xAxisGet()->setRange(m_bounds.left(), m_bounds.width() + m_bounds.left());
    yAxisGet()->setRange(m_bounds.top(), m_bounds.height() + m_bounds.top());
    enableBoundChanges(true);
}

QValueAxis *Chart::xAxisGet()
{
    return dynamic_cast<QtCharts::QValueAxis *>(this->axes(Qt::Horizontal)[0]);
}

QValueAxis *Chart::yAxisGet()
{
    return dynamic_cast<QtCharts::QValueAxis *>(this->axes(Qt::Vertical)[0]);
}
