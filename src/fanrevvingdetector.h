#ifndef FANREVVINGDETECTOR_H
#define FANREVVINGDETECTOR_H

#include <QObject>
#include <QTimer>
#include <QQueue>
#include "nvinterface.h"

class FanRevvingDetector : public QObject
{
    Q_OBJECT
public:
    explicit FanRevvingDetector(QObject *parent = 0);
    ~FanRevvingDetector();

signals:
    void fanIsRevving();
    void fanIsOk();
    void gpuData(int speed, float temp);

public slots:
    void updateGpuData();
    void checkIfRevving();
    void startFix();
    void stopFix();
    void startChecking();

private:
    QTimer *m_timerUpdate;
    QTimer *m_timerCheckRevv;
    QQueue<int> m_fanSpeedValues;
    NvInterface *m_nvInterface;

};

#endif // FANREVVINGDETECTOR_H
