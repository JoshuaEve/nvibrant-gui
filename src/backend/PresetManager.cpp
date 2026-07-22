#include "PresetManager.h"
#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFileInfo>
#include <QSaveFile>
#include <QStandardPaths>
#include <QTimer>

PresetManager::PresetManager(QObject *parent) : QObject(parent) { auto *timer=new QTimer(this);timer->setInterval(2000);connect(timer,&QTimer::timeout,this,[this]{
    QSet<QString> running; QDir proc("/proc"); for(const auto &pid:proc.entryList(QDir::Dirs|QDir::NoDotAndDotDot)){bool ok=false;pid.toInt(&ok);if(!ok)continue;QFile f(proc.filePath(pid+"/comm"));if(f.open(QIODevice::ReadOnly))running.insert(QString::fromUtf8(f.readAll()).trimmed());}
    QString match; for(const auto &v:processProfiles_){const auto o=v.toObject();if(running.contains(o.value("process").toString())){match=o.value("preset").toString();break;}}
    if(match!=activeProcess_){const bool ended=!activeProcess_.isEmpty()&&match.isEmpty();activeProcess_=match;if(ended)emit processPresetEnded();if(!match.isEmpty())emit processPresetActivated(match);}
});timer->start(); }
const QVector<Preset> &PresetManager::presets() const { return presets_; }
QString PresetManager::path() const { return QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/presets.json"; }
bool PresetManager::load(QString *error) {
    const bool loaded = importFile(path(), error);
    if (loaded) removeLegacyDefaultPresets();
    return loaded;
}
bool PresetManager::save(QString *error) const { return exportFile(path(), error); }
bool PresetManager::upsert(Preset preset) {
    for (auto &p : presets_) if (p.name.compare(preset.name, Qt::CaseInsensitive) == 0) { p = std::move(preset); return true; }
    presets_.append(std::move(preset));
    return true;
}
bool PresetManager::remove(const QString &name) {
    return presets_.removeIf([&](const Preset &p) { return p.name == name; }) > 0;
}
bool PresetManager::importFile(const QString &source, QString *error) {
    QFile f(source); if (!f.exists()) return true;
    if (!f.open(QIODevice::ReadOnly)) { if (error) *error = f.errorString(); return false; }
    QJsonParseError pe; const auto doc = QJsonDocument::fromJson(f.readAll(), &pe);
    if (pe.error != QJsonParseError::NoError || !doc.isArray()) { if (error) *error = pe.errorString(); return false; }
    QVector<Preset> parsed; for (const auto &v : doc.array()) { const auto o = v.toObject(); const auto n = o.value("name").toString().trimmed(); if (!n.isEmpty()) parsed.append({n, qBound(0, o.value("vibrance").toInt(), 1023), qMax(0, o.value("monitor").toInt()), qMax(0, o.value("gpu").toInt())}); }
    presets_ = std::move(parsed); return true;
}
bool PresetManager::exportFile(const QString &target, QString *error) const {
    QDir().mkpath(QFileInfo(target).absolutePath()); QJsonArray a;
    for (const auto &p : presets_) a.append(QJsonObject{{"name",p.name},{"vibrance",p.vibrance},{"monitor",p.monitor},{"gpu",p.gpu}});
    QSaveFile f(target); if (!f.open(QIODevice::WriteOnly) || f.write(QJsonDocument(a).toJson()) < 0 || !f.commit()) { if (error) *error = f.errorString(); return false; } return true;
}
void PresetManager::configureProcessProfiles(const QJsonArray &profiles) { processProfiles_=profiles; activeProcess_.clear(); }
void PresetManager::removeLegacyDefaultPresets() {
    presets_.removeIf([](const Preset &preset) {
        return preset.name.compare(QStringLiteral("Default"), Qt::CaseInsensitive) == 0
            || preset.name.compare(QStringLiteral("None"), Qt::CaseInsensitive) == 0;
    });
}
