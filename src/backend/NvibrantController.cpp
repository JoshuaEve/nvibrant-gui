#include "NvibrantController.h"
#include "ProcessRunner.h"
#include <QDir>
#include <QFile>
#include <QProcessEnvironment>
#include <QRegularExpression>
#include <QStandardPaths>
#include <QTimer>

NvibrantController::NvibrantController(QObject *parent) : QObject(parent), runner_(new ProcessRunner(this)) {
    connect(runner_, &ProcessRunner::startFailed, this, [this](const QString &e) { emit statusChanged(e, true); });
    connect(runner_, &ProcessRunner::finished, this, [this](int code, const QString &out, const QString &err) {
        if (probingGpu_ >= 0) {
            if (code == 0) parseProbe(probingGpu_, out + '\n' + err);
            ++probingGpu_;
            if (probingGpu_ < gpus_.size()) runProbeForGpu(probingGpu_); else { probingGpu_ = -1; emit probeCompleted(); }
            return;
        }
        if (code == 0) {
            if (pendingResetAll_) {
                for (const auto &monitor : monitors_.value(pendingGpu_)) {
                    if (!monitor.connected) continue;
                    values_[QString::number(pendingGpu_) + ':' + QString::number(monitor.port)] = 0;
                    emit applyCompleted(pendingGpu_, monitor.port, 0);
                }
            } else {
                for (auto it = pendingValues_.cbegin(); it != pendingValues_.cend(); ++it) {
                    values_[QString::number(pendingGpu_) + ':' + QString::number(it.key())] = it.value();
                    emit applyCompleted(pendingGpu_, it.key(), it.value());
                }
            }
            pendingValues_.clear();
            pendingResetAll_ = false;
            emit statusChanged(tr("Vibrance applied successfully."), false);
        }
        else emit statusChanged(err.isEmpty() ? tr("nvibrant exited with code %1.").arg(code) : err, true);
        if (probeDeferred_) { probeDeferred_ = false; probe(); }
    });
    auto *hotplugTimer = new QTimer(this);
    hotplugTimer->setInterval(2000);
    connect(hotplugTimer, &QTimer::timeout, this, [this] {
        const QString signature = hardwareSignature();
        if (!hardwareSignature_.isEmpty() && signature != hardwareSignature_) probe();
    });
    hotplugTimer->start();
}
const QVector<GpuInfo> &NvibrantController::gpus() const { return gpus_; }
const QHash<int, QVector<MonitorInfo>> &NvibrantController::monitors() const { return monitors_; }
QString NvibrantController::nvibrantVersion() const { return version_; }
int NvibrantController::rememberedValue(int gpu, int port) const { return values_.value(QString::number(gpu) + ':' + QString::number(port), 0); }
void NvibrantController::probe() {
    if (runner_->busy() || probingGpu_ >= 0) { probeDeferred_ = true; return; }
    hardwareSignature_ = hardwareSignature();
    gpus_.clear();
    QDir proc("/proc/driver/nvidia/gpus"); const auto dirs = proc.entryList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);
    int index = 0; for (const auto &d : dirs) { QFile f(proc.filePath(d + "/information")); QString name = tr("NVIDIA GPU %1").arg(index), driver;
        if (f.open(QIODevice::ReadOnly)) {
            const QString s = QString::fromUtf8(f.readAll());
            const auto modelMatch = QRegularExpression("Model:\\s*(.+)").match(s);
            if (modelMatch.hasMatch()) name = modelMatch.captured(1).trimmed();
            const auto firmwareMatch = QRegularExpression("GPU Firmware:\\s*([0-9.]+)").match(s);
            if (firmwareMatch.hasMatch()) driver = firmwareMatch.captured(1);
        }
        QFile vf("/proc/driver/nvidia/version");
        if (driver.isEmpty() && vf.open(QIODevice::ReadOnly)) {
            const auto m = QRegularExpression("NVRM version:.*?\\s([0-9]+(?:\\.[0-9]+)+)\\s+Release Build").match(QString::fromUtf8(vf.readAll()));
            if (m.hasMatch()) driver = m.captured(1);
        }
        gpus_.append({index++, name, driver}); }
    for (auto it = monitors_.begin(); it != monitors_.end();)
        if (it.key() >= gpus_.size()) it = monitors_.erase(it); else ++it;
    if (gpus_.isEmpty()) { emit statusChanged(tr("No NVIDIA GPU was found."), true); emit probeCompleted(); return; }
    if (QStandardPaths::findExecutable("nvibrant").isEmpty()) { emit statusChanged(tr("nvibrant is not installed or is not in PATH."), true); emit probeCompleted(); return; }
    probingGpu_ = 0; runProbeForGpu(0);
}
void NvibrantController::runProbeForGpu(int gpu) {
    auto env = QProcessEnvironment::systemEnvironment(); env.insert("NVIDIA_GPU", QString::number(gpu));
    // Reapply the complete remembered vector. This makes nvibrant report current
    // connector status without changing any value already known by this session.
    int maxPort = 0;
    for (const auto &monitor : monitors_.value(gpu)) maxPort = qMax(maxPort, monitor.port);
    QStringList args; for (int port = 0; port <= maxPort; ++port) args << QString::number(rememberedValue(gpu, port));
    runner_->run("nvibrant", args, env);
}
void NvibrantController::parseProbe(int gpu, const QString &text) {
    const auto vm = QRegularExpression("(?i)(?:nvibrant|driver version)[^0-9]*([0-9]+(?:\\.[0-9]+)+)").match(text); if (vm.hasMatch()) version_ = vm.captured(1);
    QVector<MonitorInfo> found;
    // nvibrant formats short connector names to a fixed width, for example
    // "(5, DP  )". Accept that padding before the closing parenthesis.
    QRegularExpression re("\\((\\d+),\\s*([A-Za-z0-9-]+)\\s*\\).*?(Success|None)"); auto it = re.globalMatch(text);
    while (it.hasNext()) { const auto m = it.next(); found.append({m.captured(1).toInt(), m.captured(2), m.captured(3) != "None"}); }
    if (found.isEmpty()) found.append({0, tr("Display 0"), true});
    monitors_[gpu] = found;
}
void NvibrantController::apply(int gpu, int port, int value) {
    applyValues(gpu, {{port, value}});
}
void NvibrantController::applyValues(int gpu, const QHash<int, int> &values) {
    if (runner_->busy()) { emit statusChanged(tr("nvibrant is still running."), true); return; }
    if (values.isEmpty()) return;
    pendingGpu_ = gpu; pendingPort_ = -1; pendingValue_ = 0; pendingResetAll_ = false; pendingValues_.clear();
    int maxPort = 0;
    for (auto it = values.cbegin(); it != values.cend(); ++it) {
        pendingValues_.insert(it.key(), qBound(0, it.value(), 1023));
        maxPort = qMax(maxPort, it.key());
    }
    for (const auto &m : monitors_.value(gpu)) maxPort = qMax(maxPort, m.port);
    QStringList args; for (int p = 0; p <= maxPort; ++p) args << QString::number(pendingValues_.value(p, rememberedValue(gpu, p)));
    auto env = QProcessEnvironment::systemEnvironment(); env.insert("NVIDIA_GPU", QString::number(gpu));
    emit statusChanged(tr("Applying…"), false); runner_->run("nvibrant", args, env);
}
void NvibrantController::resetAll(int gpu) {
    if (runner_->busy()) { emit statusChanged(tr("nvibrant is still running."), true); return; }
    pendingGpu_ = gpu; pendingPort_ = -1; pendingValue_ = 0; pendingResetAll_ = true; pendingValues_.clear();
    // Positional values must extend through the highest connected port; positions
    // before it are zero-filled so every detected monitor is reset.
    int portCount = 1;
    for (const auto &monitor : monitors_.value(gpu))
        if (monitor.connected) portCount = qMax(portCount, monitor.port + 1);
    QStringList args(portCount, QStringLiteral("0"));
    auto env = QProcessEnvironment::systemEnvironment(); env.insert("NVIDIA_GPU", QString::number(gpu));
    emit statusChanged(tr("Resetting vibrance on all monitors…"), false); runner_->run("nvibrant", args, env);
}
QString NvibrantController::hardwareSignature() const {
    QStringList parts;
    QDir gpuDir(QStringLiteral("/proc/driver/nvidia/gpus"));
    for (const auto &gpu : gpuDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name)) parts << QStringLiteral("gpu:") + gpu;
    QDir drmDir(QStringLiteral("/sys/class/drm"));
    for (const auto &connector : drmDir.entryList({QStringLiteral("card*-*")}, QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name)) {
        QFile status(drmDir.filePath(connector + QStringLiteral("/status")));
        if (status.open(QIODevice::ReadOnly)) parts << connector + QLatin1Char(':') + QString::fromUtf8(status.readAll()).trimmed();
    }
    return parts.join(QLatin1Char('|'));
}
