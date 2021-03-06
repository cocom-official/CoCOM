//--------------------------------------------------------------------------------------------------
//
//	Chart: A more useful extension of QtChart
//
//--------------------------------------------------------------------------------------------------
//
// The MIT License (MIT)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software
// and associated documentation files (the "Software"), to deal in the Software without
// restriction, including without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or
// substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
// BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//--------------------------------------------------------------------------------------------------
//
// Copyright (c) 2017 Nic Holthaus
//
//--------------------------------------------------------------------------------------------------

#ifndef chart_h__
#define chart_h__

//------------------------
//	INCLUDES
//------------------------

#include <QtCharts>
#include <QValueAxis>

class LineSeries;

using namespace QtCharts;

class Chart : public QChart
{
    Q_OBJECT

public:
    explicit Chart();

    QRectF bounds() const;
    void createDefaultAxes();
    void addSeries(LineSeries *series);
    int seriesCount();
    void seriesAppend(int line, QPointF data);
    void zoom(const QRectF &rect);
    void zoomReset();
    void setXAxiTickCount(int count);
    void setYAxiTickCount(int count);
    void setPointsVisible(bool visible = true);
    void setPointLabelsVisible(bool visible = true);

signals:
    void seriesAdded(QAbstractSeries *series);
    void zoomed(QRectF bounds);
    void boundsChanged(QRectF bounds);

protected slots:
    QRectF changeZoomLevel(bool emitSignal = true);
    void enableBoundChanges(bool val);
    void lineBoundsChanges(QRectF range);

private:
    QRectF m_bounds;
    QList<LineSeries *> lines;
    QMetaObject::Connection m_xBoundsConnection;
    QMetaObject::Connection m_yBoundsConnection;

    QValueAxis *xAxisGet();
    QValueAxis *yAxisGet();

    bool isZoomReseteed();
    void updateRange();
};

#endif // chart_h__