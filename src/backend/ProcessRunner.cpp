#include "ProcessRunner.h"
#include <QProcess>
#include <QTimer>

ProcessRunner::ProcessRunner(QObject *parent) : QObject(parent), process_(new QProcess(this)), timeout_(new QTimer(this)) {
    timeout_->setSingleShot(true);
    connect(timeout_, &QTimer::timeout, this, [this] {
        process_->kill();
        emit startFailed(tr("nvibrant timed out and was terminated."));
    });
    connect(process_, &QProcess::errorOccurred, this, [this](QProcess::ProcessError error) {
        if (error == QProcess::FailedToStart) emit startFailed(tr("Could not start nvibrant: %1").arg(process_->errorString()));
    });
    connect(process_, qOverload<int, QProcess::ExitStatus>(&QProcess::finished), this,
            [this](int code, QProcess::ExitStatus) {
        timeout_->stop();
        emit finished(code, QString::fromUtf8(process_->readAllStandardOutput()).trimmed(),
                      QString::fromUtf8(process_->readAllStandardError()).trimmed());
    });
}
bool ProcessRunner::busy() const { return process_->state() != QProcess::NotRunning; }
void ProcessRunner::run(const QString &program, const QStringList &arguments,
                        const QProcessEnvironment &environment, int timeoutMs) {
    if (busy()) return;
    process_->setProcessEnvironment(environment);
    process_->start(program, arguments, QIODevice::ReadOnly);
    timeout_->start(timeoutMs);
}
