#include "TrayIcon.h"
#include "backend/PresetManager.h"
#include <QApplication>
#include <QMenu>

namespace {
QString nativeMenuWidth(const QString &label, int extraEmSpaces) {
    // StatusNotifierItem hosts (notably Plasma) export QAction text over D-Bus
    // and render their own menu, ignoring all QWidget width/style hints. Em
    // spaces are part of the exported label, so the host must reserve room
    // before drawing its submenu indicator.
    return label + QString(extraEmSpaces, QChar(0x2003));
}

class WideMenu final : public QMenu {
public:
    explicit WideMenu(int minimumWidth, QWidget *parent = nullptr) : QMenu(parent), minimumWidth_(minimumWidth) {
        setMinimumWidth(minimumWidth_);
        setStyleSheet(QStringLiteral("QMenu::item { padding: 8px 96px 8px 28px; }"));
    }
    QSize sizeHint() const override {
        QSize size = QMenu::sizeHint();
        size.setWidth(qMax(size.width(), minimumWidth_));
        return size;
    }
    void setEnforcedWidth(int width) { minimumWidth_ = width; setMinimumWidth(width); updateGeometry(); }
private:
    int minimumWidth_;
};
}

TrayIcon::TrayIcon(PresetManager*p,QObject*parent)
    : QSystemTrayIcon(QIcon(":/icons/nvibrant-gui.svg"),parent), presets_(p),
      menu_(new WideMenu(600)), presetMenu_(new WideMenu(520, menu_)) {
    presetMenu_->setTitle(nativeMenuWidth(tr("Presets"), 18));
    presetMenu_->menuAction()->setProperty("accessibleName", tr("Presets"));
    menu_->addMenu(presetMenu_);
    auto*show=menu_->addAction(tr("Show"));menu_->insertAction(presetMenu_->menuAction(),show);menu_->addSeparator();auto*quit=menu_->addAction(tr("Quit"));setContextMenu(menu_);connect(show,&QAction::triggered,this,&TrayIcon::showRequested);connect(quit,&QAction::triggered,this,&TrayIcon::quitRequested);connect(this,&QSystemTrayIcon::activated,this,[this](ActivationReason r){if(r==DoubleClick||r==Trigger)emit showRequested();});refreshPresets();
}
void TrayIcon::refreshPresets(){presetMenu_->clear();int width=520;for(const auto&p:presets_->presets()){auto*a=presetMenu_->addAction(nativeMenuWidth(p.name,8));a->setProperty("accessibleName",p.name);width=qMax(width,presetMenu_->fontMetrics().horizontalAdvance(p.name)+190);connect(a,&QAction::triggered,this,[this,n=p.name]{emit presetRequested(n);});}static_cast<WideMenu*>(presetMenu_)->setEnforcedWidth(width);presetMenu_->setEnabled(!presetMenu_->isEmpty());}
