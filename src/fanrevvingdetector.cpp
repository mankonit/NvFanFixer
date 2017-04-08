#include "fanrevvingdetector.h"
#include <QDebug>

#define C_UPDATE_TIMER_INTERVAL 500 //ms
#define C_SAMPLE_SIZE 10
#define C_CHECK_TIMER_INTERVAL C_UPDATE_TIMER_INTERVAL*C_SAMPLE_SIZE //ms
#define C_ALLOWED_SPEED_VARIANCE 600
#define C_FIXING_SPEED_PERCENT 30
#define C_FIXING_TIME 10000 //ms
#define C_RECOVERY_TIME 10000 //ms


FanRevvingDetector::FanRevvingDetector(QObject *parent) : QObject(parent)
{
    m_timerUpdate = new QTimer();
    m_timerCheckRevv = new QTimer();
    m_timerCheckRevv->setInterval(C_CHECK_TIMER_INTERVAL);
    m_timerUpdate->setInterval(C_UPDATE_TIMER_INTERVAL);
    m_nvInterface = NvInterface::instance();
    connect(m_timerUpdate, SIGNAL(timeout()), this, SLOT(updateGpuData()));
    connect(m_timerCheckRevv, SIGNAL(timeout()), this, SLOT(checkIfRevving()));
    m_timerUpdate->start();
    m_timerCheckRevv->start();
}

FanRevvingDetector::~FanRevvingDetector()
{
    qDebug() << Q_FUNC_INFO;
}

void FanRevvingDetector::updateGpuData()
{
    int l_fanSpeed = m_nvInterface->getFanSpeed();
    float l_gpuTemp = m_nvInterface->getTemperature();
    emit gpuData(l_fanSpeed, l_gpuTemp);
    if (m_fanSpeedValues.size() > C_SAMPLE_SIZE)
        m_fanSpeedValues.dequeue();
    m_fanSpeedValues.enqueue(l_fanSpeed);
}

void FanRevvingDetector::checkIfRevving()
{
    qDebug() << Q_FUNC_INFO << m_fanSpeedValues;
    int l_min = *std::min_element(m_fanSpeedValues.begin(), m_fanSpeedValues.end());
    int l_max = *std::max_element(m_fanSpeedValues.begin(), m_fanSpeedValues.end());
    if (m_fanSpeedValues.contains(0) && (l_max-l_min > C_ALLOWED_SPEED_VARIANCE))
    {
        emit fanIsRevving();
        this->startFix();
    }
    else
        emit fanIsOk();
}

void FanRevvingDetector::startFix()
{
    qDebug() << Q_FUNC_INFO;
    m_timerCheckRevv->stop();
    m_nvInterface->setFanSpeedPercent(C_FIXING_SPEED_PERCENT);
    QTimer::singleShot(C_FIXING_TIME, this, SLOT(stopFix()));
}

void FanRevvingDetector::stopFix()
{
    qDebug() << Q_FUNC_INFO;
    m_nvInterface->resetFanRegulation();
    QTimer::singleShot(C_RECOVERY_TIME, this, SLOT(startChecking()));
}

void FanRevvingDetector::startChecking()
{
    qDebug() << Q_FUNC_INFO;
    m_fanSpeedValues.clear();
    m_timerCheckRevv->start();
}
