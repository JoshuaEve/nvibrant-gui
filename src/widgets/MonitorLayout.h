#pragma once

#include <QWidget>
#include <QVector>

struct MonitorTile {
    int port;
    QString input;
    int vibrance;
};

class MonitorLayout final : public QWidget {
    Q_OBJECT
public:
    explicit MonitorLayout(QWidget *parent = nullptr);
    void setMonitors(const QVector<MonitorTile> &monitors);
    void setCurrentPort(int port);
    void setVibrance(int port, int value);
    int currentPort() const;
    bool hasPort(int port) const;

signals:
    void currentPortChanged(int port);

protected:
    void paintEvent(QPaintEvent *) override;
    void mousePressEvent(QMouseEvent *) override;
    void keyPressEvent(QKeyEvent *) override;
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

private:
    QRectF tileRect(int index) const;
    int cardWidth() const;
    int contentHeight() const;
    void selectIndex(int index);
    QVector<MonitorTile> monitors_;
    int currentIndex_ = -1;
};
