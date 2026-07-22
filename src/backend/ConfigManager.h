#pragma once
#include <QJsonObject>
#include <QObject>

class ConfigManager final : public QObject {
    Q_OBJECT
public:
    explicit ConfigManager(QObject *parent = nullptr);
    bool load(QString *error = nullptr);
    bool save(QString *error = nullptr) const;
    QJsonValue value(const QString &key, const QJsonValue &fallback = {}) const;
    void setValue(const QString &key, const QJsonValue &value);
    QString path() const;
    bool setAutostart(bool enabled, QString *error = nullptr) const;
private:
    QJsonObject data_;
};
