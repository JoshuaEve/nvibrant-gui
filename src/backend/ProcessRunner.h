#pragma once
#include <QObject>
#include <QProcessEnvironment>

class QProcess;
class QTimer;

class ProcessRunner final : public QObject {
    Q_OBJECT
public:
    explicit ProcessRunner(QObject *parent = nullptr);
    bool busy() const;
    void run(const QString &program, const QStringList &arguments,
             const QProcessEnvironment &environment = QProcessEnvironment::systemEnvironment(),
             int timeoutMs = 5000);
signals:
    void finished(int exitCode, const QString &output, const QString &error);
    void startFailed(const QString &message);
private:
    QProcess *process_;
    QTimer *timeout_;
};
