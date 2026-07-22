#pragma once
#include <QListWidget>
class PresetList final : public QListWidget {
    Q_OBJECT
public:
    explicit PresetList(QWidget *parent = nullptr) : QListWidget(parent) { setAcceptDrops(false); }
};
