#include "MonitorLayout.h"

#include <QKeyEvent>
#include <QIcon>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QStyle>
#include <QStyleOptionFocusRect>

MonitorLayout::MonitorLayout(QWidget *parent) : QWidget(parent) {
    setFocusPolicy(Qt::StrongFocus);
    setCursor(Qt::PointingHandCursor);
    setAccessibleName(tr("Monitor setup"));
    setToolTip(tr("Select a monitor to change its vibrance"));
}

void MonitorLayout::setMonitors(const QVector<MonitorTile> &monitors) {
    const int previousPort = currentPort();
    monitors_ = monitors;
    currentIndex_ = -1;
    for (int i = 0; i < monitors_.size(); ++i)
        if (monitors_[i].port == previousPort) currentIndex_ = i;
    if (currentIndex_ < 0 && !monitors_.isEmpty()) currentIndex_ = 0;
    setMinimumWidth(qMax(300, int(monitors_.size()) * cardWidth() + qMax(0, int(monitors_.size()) - 1) * 10));
    setMinimumHeight(contentHeight());
    updateGeometry();
    update();
}

void MonitorLayout::setCurrentPort(int port) {
    for (int i = 0; i < monitors_.size(); ++i)
        if (monitors_[i].port == port) { selectIndex(i); return; }
}

void MonitorLayout::setVibrance(int port, int value) {
    for (auto &monitor : monitors_)
        if (monitor.port == port) { monitor.vibrance = value; update(); return; }
}

int MonitorLayout::currentPort() const {
    return currentIndex_ >= 0 && currentIndex_ < monitors_.size() ? monitors_[currentIndex_].port : -1;
}

bool MonitorLayout::hasPort(int port) const {
    for (const auto &monitor : monitors_) if (monitor.port == port) return true;
    return false;
}

int MonitorLayout::cardWidth() const {
    int width = 130;
    const QFontMetrics metrics(font());
    QFont boldFont = font(); boldFont.setBold(true);
    const QFontMetrics boldMetrics(boldFont);
    for (const auto &monitor : monitors_) {
        width = qMax(width, boldMetrics.horizontalAdvance(tr("%1 · Port %2").arg(monitor.input).arg(monitor.port)) + 28);
        width = qMax(width, metrics.horizontalAdvance(tr("Vibrance: %1").arg(monitor.vibrance)) + 28);
    }
    return width;
}

int MonitorLayout::contentHeight() const { return 14 + 66 + 16 + (fontMetrics().height() + 4) * 2 + 16; }
QSize MonitorLayout::sizeHint() const { return {qMax(300, int(monitors_.size()) * cardWidth() + qMax(0, int(monitors_.size()) - 1) * 10), contentHeight()}; }
QSize MonitorLayout::minimumSizeHint() const { return {qMin(300, sizeHint().width()), contentHeight()}; }

QRectF MonitorLayout::tileRect(int index) const {
    const int count = qMax(1, monitors_.size());
    const qreal gap = 10;
    const qreal width = qMax<qreal>(cardWidth(), (this->width() - gap * (count - 1)) / count);
    const qreal total = width * count + gap * (count - 1);
    return {(this->width() - total) / 2 + index * (width + gap), 8, width, qreal(height() - 16)};
}

void MonitorLayout::paintEvent(QPaintEvent *) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    const QColor accent = palette().color(QPalette::Highlight);
    const QColor text = palette().color(QPalette::Text);
    const QColor selectedText = palette().color(QPalette::HighlightedText);
    const QColor base = palette().color(QPalette::Base);

    for (int i = 0; i < monitors_.size(); ++i) {
        const QRectF tile = tileRect(i);
        const bool selected = i == currentIndex_;
        p.setPen(QPen(selected ? accent : palette().color(QPalette::Mid), selected ? 2.5 : 1.2));
        p.setBrush(selected ? accent : base);
        p.drawRoundedRect(tile, 9, 9);

        // Render the monitor SVG at the device's native DPI, then overlay its number.
        QRectF screen(tile.left() + 15, tile.top() + 14, tile.width() - 30, 66);
        QIcon(QStringLiteral(":/icons/monitor.svg")).paint(&p, screen.toRect());

        QFont numberFont = font(); numberFont.setBold(true); numberFont.setPointSize(numberFont.pointSize() + 7);
        p.setFont(numberFont); p.setPen(selected ? selectedText : text);
        p.drawText(screen, Qt::AlignCenter, QString::number(i + 1));

        const int labelHeight = fontMetrics().height() + 4;
        QFont labelFont = font(); labelFont.setBold(true); p.setFont(labelFont); p.setPen(selected ? selectedText : text);
        p.drawText(QRectF(tile.left() + 6, screen.bottom() + 16, tile.width() - 12, labelHeight), Qt::AlignCenter,
                   tr("%1 · Port %2").arg(monitors_[i].input).arg(monitors_[i].port));
        labelFont.setBold(false); p.setFont(labelFont); p.setPen(selected ? selectedText : text);
        p.drawText(QRectF(tile.left() + 6, screen.bottom() + 16 + labelHeight, tile.width() - 12, labelHeight), Qt::AlignCenter,
                   tr("Vibrance: %1").arg(monitors_[i].vibrance));
    }

    if (hasFocus() && currentIndex_ >= 0) {
        QStyleOptionFocusRect option;
        option.initFrom(this);
        option.rect = tileRect(currentIndex_).adjusted(3, 3, -3, -3).toRect();
        style()->drawPrimitive(QStyle::PE_FrameFocusRect, &option, &p, this);
    }
}

void MonitorLayout::selectIndex(int index) {
    if (index < 0 || index >= monitors_.size() || index == currentIndex_) return;
    currentIndex_ = index;
    update();
    emit currentPortChanged(monitors_[index].port);
}

void MonitorLayout::mousePressEvent(QMouseEvent *event) {
    for (int i = 0; i < monitors_.size(); ++i)
        if (tileRect(i).contains(event->position())) { setFocus(); selectIndex(i); return; }
    QWidget::mousePressEvent(event);
}

void MonitorLayout::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Left) selectIndex(qMax(0, currentIndex_ - 1));
    else if (event->key() == Qt::Key_Right) selectIndex(qMin(monitors_.size() - 1, currentIndex_ + 1));
    else QWidget::keyPressEvent(event);
}
