#pragma once
#include <QMainWindow>
#include <QHash>
class NvibrantController;class ConfigManager;class PresetManager;class DeviceSelector;class MonitorLayout;class VibranceSlider;class PresetList;class QLabel;class QPushButton;class TrayIcon;
class MainWindow final : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(NvibrantController *, ConfigManager *, PresetManager *, QWidget *parent = nullptr);
    void applyPreset(const QString &);
protected:
    void closeEvent(QCloseEvent *) override;
private:
    void populateDevices();
    void populateMonitors();
    void refreshPresets();
    void savePreset();
    void removePreset();
    void applyCurrent(int value);
    void applyPending();
    void loadPreset(const QString &name);
    QString pendingKey(int gpu, int port) const;
    NvibrantController *controller_;
    ConfigManager *config_;
    PresetManager *presets_;
    DeviceSelector *gpu_;
    MonitorLayout *monitor_;
    VibranceSlider *slider_;
    PresetList *presetList_;
    QLabel *driver_;
    QPushButton *apply_;
    QPushButton *reset_;
    TrayIcon *tray_ = nullptr;
    bool quitting_ = false;
    int profileGpu_ = 0;
    int profileMonitor_ = 0;
    int profileValue_ = 0;
    QHash<QString, int> pendingValues_;
};
