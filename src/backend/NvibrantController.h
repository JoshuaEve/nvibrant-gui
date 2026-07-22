#pragma once
#include <QObject>
#include <QHash>

class ProcessRunner;

struct GpuInfo { int index; QString name; QString driver; };
struct MonitorInfo { int port; QString type; bool connected; };

class NvibrantController final : public QObject {
    Q_OBJECT
public:
    explicit NvibrantController(QObject *parent = nullptr);
    void probe();
    void apply(int gpu, int port, int value);
    void applyValues(int gpu, const QHash<int, int> &values);
    void resetAll(int gpu);
    int rememberedValue(int gpu, int port) const;
    const QVector<GpuInfo> &gpus() const;
    const QHash<int, QVector<MonitorInfo>> &monitors() const;
    QString nvibrantVersion() const;
signals:
    void probeCompleted();
    void applyCompleted(int gpu, int port, int value);
    void statusChanged(const QString &message, bool error);
private:
    void runProbeForGpu(int gpu);
    void parseProbe(int gpu, const QString &text);
    QString hardwareSignature() const;
    ProcessRunner *runner_;
    QVector<GpuInfo> gpus_;
    QHash<int, QVector<MonitorInfo>> monitors_;
    QHash<QString, int> values_;
    QString version_ = tr("Unknown");
    int probingGpu_ = -1;
    int pendingGpu_ = -1, pendingPort_ = -1, pendingValue_ = 0;
    bool pendingResetAll_ = false;
    QHash<int, int> pendingValues_;
    QString hardwareSignature_;
    bool probeDeferred_ = false;
};
