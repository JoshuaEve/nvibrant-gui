#pragma once
#include <QWidget>
class QSlider; class QSpinBox; class QTimer;
class VibranceSlider final : public QWidget {
    Q_OBJECT
public: explicit VibranceSlider(QWidget *parent = nullptr); int value() const; void setValue(int value);
signals: void valuePreviewed(int value); void valueSettled(int value);
private: QSlider *slider_; QSpinBox *valueInput_; QTimer *debounce_;
};
