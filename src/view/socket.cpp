/* See LICENSE file for copyright and license details. */

#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneDragDropEvent>
#include <QGraphicsScene>
#include <QFont>
#include <QFontMetrics>
#include <QList>
#include <QDrag>
#include <QMimeData>
#include <QDebug>

#include <iostream>
#include <algorithm>

#include "edge.hpp"

#include "socket.hpp"

using namespace std;

// TODO:
// * prepareGeometryChange

#define PEN_COLOR_CIRCLE QColor("#FF000000")
#define PEN_COLOR_TEXT QColor("#FFFFFFFF")
#define BRUSH_COLOR_SINK QColor("#FF0077FF")
#define BRUSH_COLOR_SOURCE QColor("#FFFF7700")
#define TEXT_ALIGNMENT_SINK Qt::AlignLeft
#define TEXT_ALIGNMENT_SOURCE Qt::AlignRight

// const qreal height = 20.0;
// const qreal width = 30.0;

GraphicsNodeSocket::GraphicsNodeSocket(Socket socket, QGraphicsItem *parent)
    : _socket(socket),
      QGraphicsItem(parent),
      _socket_type(socket.port.lock()->direction),
      _pen_circle(PEN_COLOR_CIRCLE),
      _pen_text(PEN_COLOR_TEXT),
      _brush_circle((_socket_type == Port::Direction::IN) ? BRUSH_COLOR_SINK
                                                          : BRUSH_COLOR_SOURCE),
      _text(QString::fromStdString(socket.port.lock()->name)),
      _edge(nullptr) {
    _pen_circle.setWidth(0);
    setAcceptDrops(true);
}

GraphicsNodeSocket::~GraphicsNodeSocket() {
    qWarning() << "Port deleted";
}


Port::Direction GraphicsNodeSocket::socket_type() const { return _socket_type; }

bool GraphicsNodeSocket::is_sink() const {
    return _socket_type == Port::Direction::IN;
}

bool GraphicsNodeSocket::is_source() const {
    return _socket_type == Port::Direction::OUT;
}

QRectF GraphicsNodeSocket::boundingRect() const {
    QSizeF size = getSize();
    const qreal x = -_circle_radius - _pen_width / 2;
    const qreal y = -size.height() / 2.0 - _pen_width / 2;
    if (_socket_type == Port::Direction::IN)
        return QRectF(x, y, size.width(), size.height()).normalized();
    else
        return QRectF(-size.width() - x, y, size.width(), size.height())
            .normalized();
}

QSizeF GraphicsNodeSocket::getMinimalSize() const {
    QSizeF size;
    QFont font;
    QFontMetrics fm(font);
    int text_width = fm.width(_text);
    const qreal text_height = static_cast<qreal>(fm.height());
    size.setWidth(std::max(_min_width, _circle_radius * 2 + _text_offset +
                                           text_width + _pen_width));
    size.setHeight(std::max(_min_height, text_height + _pen_width));
    return size;
}

QSizeF GraphicsNodeSocket::getSize() const { return getMinimalSize(); }

void GraphicsNodeSocket::drawAlignedText(QPainter *painter) {
    int flags = Qt::AlignVCenter;
    const qreal size = 32767.0;
    QPointF corner(0, 0);
    if (_socket_type == Port::Direction::IN) {
        corner.setX(_circle_radius + _text_offset);
        corner.setY(-size);
        corner.ry() += size / 2.0;
        flags |= Qt::AlignLeft;
    } else {
        corner.setX(-_circle_radius - _text_offset);
        corner.setY(-size);
        corner.ry() += size / 2.0;
        corner.rx() -= size;
        flags |= Qt::AlignRight;
    }
    QRectF rect(corner, QSizeF(size, size));
    painter->setPen(_pen_text);

    painter->drawText(rect, flags, _text, 0);
}

QPointF GraphicsNodeSocket::sceneAnchorPos() const { return mapToScene(0, 0); }

void GraphicsNodeSocket::paint(QPainter *painter,
                               const QStyleOptionGraphicsItem * /*option*/,
                               QWidget * /*widget*/) {
    painter->setPen(_pen_circle);
    painter->setBrush(_brush_circle);
    painter->drawEllipse(-_circle_radius, -_circle_radius, _circle_radius * 2,
                         _circle_radius * 2);
    drawAlignedText(painter);

// debug painting the bounding box
#if 0
    QPen debugPen = QPen(QColor(Qt::red));
    debugPen.setWidth(0);
    auto r = boundingRect();
    painter->setPen(debugPen);
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(r);

    painter->drawPoint(0,0);
    painter->drawLine(0,0, r.width(), 0);
#endif
}

void GraphicsNodeSocket::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsItem::mouseMoveEvent(event);
}

void GraphicsNodeSocket::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsItem::mouseReleaseEvent(event);
}

bool GraphicsNodeSocket::isInSocketCircle(const QPointF &p) const {
    return p.x() >= -_circle_radius && p.x() <= _circle_radius &&
           p.y() >= -_circle_radius && p.y() <= _circle_radius;
}

void GraphicsNodeSocket::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsItem::mousePressEvent(event);
}

void GraphicsNodeSocket::set_edge(shared_ptr<GraphicsDirectedEdge> edge) {
    // TODO: handle edge conflict
    _edge = edge;
    notifyPositionChange();
}

shared_ptr<GraphicsDirectedEdge> GraphicsNodeSocket::get_edge() { return _edge; }

void GraphicsNodeSocket::notifyPositionChange() {
    if (!_edge) return;

    switch (_socket_type) {
        case Port::Direction::IN:
            _edge->set_stop(mapToScene(0, 0));
            break;
        case Port::Direction::OUT:
            _edge->set_start(mapToScene(0, 0));
            break;
    }
}


