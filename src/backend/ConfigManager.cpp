#include "ConfigManager.h"
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QFileInfo>
#include <QSaveFile>
#include <QStandardPaths>

ConfigManager::ConfigManager(QObject *parent) : QObject(parent) {}
QString ConfigManager::path() const { return QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/config.json"; }
bool ConfigManager::load(QString *error) {
    QFile file(path());
    if (!file.exists()) return true;
    if (!file.open(QIODevice::ReadOnly)) { if (error) *error = file.errorString(); return false; }
    QJsonParseError parse;
    const auto doc = QJsonDocument::fromJson(file.readAll(), &parse);
    if (parse.error != QJsonParseError::NoError || !doc.isObject()) { if (error) *error = parse.errorString(); return false; }
    data_ = doc.object(); return true;
}
bool ConfigManager::save(QString *error) const {
    QDir().mkpath(QFileInfo(path()).absolutePath());
    QSaveFile file(path());
    if (!file.open(QIODevice::WriteOnly) || file.write(QJsonDocument(data_).toJson()) < 0 || !file.commit()) {
        if (error) *error = file.errorString();
        return false;
    } return true;
}
QJsonValue ConfigManager::value(const QString &key, const QJsonValue &fallback) const { return data_.value(key).isUndefined() ? fallback : data_.value(key); }
void ConfigManager::setValue(const QString &key, const QJsonValue &value) { data_.insert(key, value); }
bool ConfigManager::setAutostart(bool enabled, QString *error) const {
    const QString dir = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/autostart";
    const QString target = dir + "/nvibrant-gui.desktop";
    if (!enabled) { if (QFile::exists(target) && !QFile::remove(target)) { if (error) *error = tr("Could not remove %1").arg(target); return false; } return true; }
    QDir().mkpath(dir); QSaveFile file(target);
    const QString entry = QString("[Desktop Entry]\nType=Application\nName=nVibrant GUI\nExec=%1 --minimized\nIcon=nvibrant-gui\nTerminal=false\nX-GNOME-Autostart-enabled=true\n")
        .arg(QCoreApplication::applicationFilePath());
    if (!file.open(QIODevice::WriteOnly) || file.write(entry.toUtf8()) < 0 || !file.commit()) { if (error) *error = file.errorString(); return false; }
    return true;
}
