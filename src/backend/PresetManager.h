#pragma once
#include <QObject>
#include <QVector>
#include <QJsonArray>

struct Preset { QString name; int vibrance = 0; int monitor = 0; int gpu = 0; };

class PresetManager final : public QObject {
    Q_OBJECT
public:
    explicit PresetManager(QObject *parent = nullptr);
    const QVector<Preset> &presets() const;
    bool load(QString *error = nullptr);
    bool save(QString *error = nullptr) const;
    bool upsert(Preset preset);
    bool remove(const QString &name);
    bool importFile(const QString &path, QString *error = nullptr);
    bool exportFile(const QString &path, QString *error = nullptr) const;
    void configureProcessProfiles(const QJsonArray &profiles);
signals:
    void processPresetActivated(const QString &preset);
    void processPresetEnded();
private:
    void removeLegacyDefaultPresets();
    QString path() const;
    QVector<Preset> presets_;
    QJsonArray processProfiles_;
    QString activeProcess_;
};
