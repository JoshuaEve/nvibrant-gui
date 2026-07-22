#pragma once
#include <QDialog>
class QCheckBox; class QPlainTextEdit; class ConfigManager;
class SettingsDialog final : public QDialog {
    Q_OBJECT
public: explicit SettingsDialog(ConfigManager *config, QWidget *parent=nullptr); void accept() override;
private: ConfigManager *config_; QCheckBox *startMinimized_, *startLogin_, *closeTray_, *rememberMonitor_, *rememberGpu_, *restoreExit_, *debugLog_; QPlainTextEdit *profiles_;
};
