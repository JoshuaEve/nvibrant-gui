#include "VibranceSlider.h"
#include <QHBoxLayout>
#include <QSlider>
#include <QSpinBox>
#include <QTimer>
VibranceSlider::VibranceSlider(QWidget *parent) : QWidget(parent), slider_(new QSlider(Qt::Horizontal,this)), valueInput_(new QSpinBox(this)), debounce_(new QTimer(this)) {
    slider_->setRange(0,1023); slider_->setPageStep(64); slider_->setAccessibleName(tr("Digital Vibrance"));
    valueInput_->setRange(0,1023); valueInput_->setKeyboardTracking(false); valueInput_->setAccessibleName(tr("Digital Vibrance value")); valueInput_->setMinimumWidth(80);
    auto *l=new QHBoxLayout(this); l->setContentsMargins(0,0,0,0); l->addWidget(slider_,1); l->addWidget(valueInput_);
    debounce_->setSingleShot(true); debounce_->setInterval(200);
    connect(slider_,&QSlider::valueChanged,valueInput_,&QSpinBox::setValue);
    connect(valueInput_,qOverload<int>(&QSpinBox::valueChanged),slider_,&QSlider::setValue);
    connect(slider_,&QSlider::valueChanged,this,[this](int v){emit valuePreviewed(v);debounce_->start();});
    connect(debounce_,&QTimer::timeout,this,[this]{emit valueSettled(slider_->value());});
}
int VibranceSlider::value() const{return slider_->value();} void VibranceSlider::setValue(int v){slider_->setValue(v);}
