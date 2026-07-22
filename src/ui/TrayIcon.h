#pragma once
#include <QSystemTrayIcon>
class QMenu; class PresetManager;
class TrayIcon final : public QSystemTrayIcon {
    Q_OBJECT
public:
    explicit TrayIcon(PresetManager *presets, QObject *parent = nullptr);
    void refreshPresets();
signals:
    void showRequested();
    void quitRequested();
    void presetRequested(const QString &);
private:
    PresetManager *presets_;
    QMenu *menu_;
    QMenu *presetMenu_;
};
