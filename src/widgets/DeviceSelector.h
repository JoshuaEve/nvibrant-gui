#pragma once
#include <QComboBox>
class DeviceSelector final : public QComboBox {
    Q_OBJECT
public:
    explicit DeviceSelector(QWidget *parent = nullptr) : QComboBox(parent) {
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        setMinimumContentsLength(1);
    }
};
