#include "nvinterface.h"
//#include <windows.h>
#include <QDebug>

NvInterface *NvInterface::m_instance = 0;

NvInterface *NvInterface::instance()
{
    if (m_instance == 0)
        m_instance = new NvInterface();
    return m_instance;
}

NvInterface::NvInterface()
{
    qDebug() << Q_FUNC_INFO;

    NvAPI_Status l_ret = NVAPI_OK;
    m_initStatus = true;
    m_initError = "";
    l_ret = NvAPI_Initialize();
    if(l_ret != NVAPI_OK)
    {
        m_initStatus = false;
        m_initError = "Unable to initialize Nvidia API";
        qDebug() << Q_FUNC_INFO << m_initError << l_ret;
        return;
    }

    NvU32 l_gpuCount = 0;
    NvAPI_EnumPhysicalGPUs(m_physicalGpuHandle, &l_gpuCount);

    if (l_gpuCount == 0)
    {
        m_initStatus = false;
        m_initError = QString("No Nvidia GPU detected");
        qDebug() << Q_FUNC_INFO << m_initError;
        return;
    }
    else if (l_gpuCount > 1)
    {
        m_initStatus = false;
        m_initError = QString("More than one Nvidia GPU detected\nSLI is not supported");
        qDebug() << Q_FUNC_INFO << m_initError;
        qDebug() << Q_FUNC_INFO << "Number of GPU :" << l_gpuCount;
        return;
    }

    m_nvapiLib = new QLibrary("nvapi64_78");
    m_nvapiLib->load();

    if (!m_nvapiLib->isLoaded())
    {
        m_initStatus = false;
        m_initError = QString("Unable to load nvapi library\n") + m_nvapiLib->errorString();
        qDebug() << Q_FUNC_INFO << m_initError;
        qDebug() << Q_FUNC_INFO << m_nvapiLib->errorString();
        return;
    }

    f_NvQueryInterface = (NvAPI_QueryInterface_t)m_nvapiLib->resolve("nvapi_QueryInterface");
    // Par utilisation de la fonction de windows.h
    // f_NvQueryInterface = (NvAPI_QueryInterface_t)GetProcAddress(LoadLibraryA("nvapi64"), "nvapi_QueryInterface");
    if (!f_NvQueryInterface)
    {
        m_initStatus = false;
        m_initError = QString("Unable to resolve nvapi_QueryInterface from library");
        qDebug() << Q_FUNC_INFO << m_initError;
        return;
    }

    f_NvRestoreCoolerSettings = (NvAPI_GPU_RestoreCoolerSettings_t)f_NvQueryInterface(0x8F6ED0FB);
    f_NvSetCoolerLevels = (NvAPI_GPU_SetCoolerLevels_t)f_NvQueryInterface(0x891FA0AE);
}

NvInterface::~NvInterface()
{
    qDebug() << Q_FUNC_INFO;
}

void NvInterface::unload()
{
    qDebug() << Q_FUNC_INFO;
    NvAPI_Unload();
}

int NvInterface::getFanSpeed()
{
    NvU32 l_fanSpeed = 0;
    NvAPI_GPU_GetTachReading(m_physicalGpuHandle[0], &l_fanSpeed);
    //qDebug() << Q_FUNC_INFO << "Fan speed" << l_fanSpeed;
    return l_fanSpeed;
}

void NvInterface::setFanSpeedPercent(int i_percent)
{
    NvAPI_Status l_ret = NVAPI_OK;
    NvU32 l_coolerIndex = 0;
    NV_GPU_COOLER_LEVELS l_coolerLevels;
    NvLevel l_level;
    l_coolerLevels.Version = GPU_COOLER_LEVELS_VER | sizeof (NV_GPU_COOLER_LEVELS);
    l_level.Level = i_percent;
    l_level.Policy = 1;
    l_coolerLevels.Levels[0] = l_level;
    qDebug() << Q_FUNC_INFO << "Set fan speed" << i_percent;
    l_ret = f_NvSetCoolerLevels(m_physicalGpuHandle[0], l_coolerIndex, &l_coolerLevels);
    qDebug() << Q_FUNC_INFO << "Set fan speed done" << l_ret;
    return;
}

void NvInterface::resetFanRegulation()
{
    qDebug() << Q_FUNC_INFO;
    NvU32 l_coolerIndex = 0;
    qDebug() << Q_FUNC_INFO << "Reseting fan speed" <<  f_NvRestoreCoolerSettings(m_physicalGpuHandle[0], l_coolerIndex);
    // attention, si on fait un
    // l_ret = f_NvRestoreCoolerSettings(m_physicalGpuHandle[0], l_coolerIndex);
    // alors on obtient un code retour = -5 (parametre invalide)
    return;
}

QString NvInterface::getGpuInfo()
{
    NV_SYSTEM_TYPE l_sysType;
    QString l_sysTypeString = "";
    NvAPI_GPU_GetSystemType(m_physicalGpuHandle[0], &l_sysType);
    switch(l_sysType)
    {
    case NV_SYSTEM_TYPE_LAPTOP:
        l_sysTypeString = "Laptop";
        break;
    case NV_SYSTEM_TYPE_DESKTOP:
        l_sysTypeString = "Desktop";
        break;
    default:
        l_sysTypeString = "Unknown";
        break;
    }

    NvAPI_ShortString l_gpuName;
    NvAPI_GPU_GetFullName(m_physicalGpuHandle[0], l_gpuName);
    qDebug() << Q_FUNC_INFO << "GPU name :" << l_gpuName;

    return l_sysTypeString + "|" + l_gpuName;
}

QString NvInterface::getDriverInfo()
{
    NvAPI_ShortString l_version;
    NvAPI_GetInterfaceVersionString(l_version);
    qDebug() << Q_FUNC_INFO << "Interface version :" << l_version;

    NvU32 l_driverVersion = 0;
    NvAPI_ShortString l_buildBranch;
    NvAPI_SYS_GetDriverAndBranchVersion(&l_driverVersion, l_buildBranch);
    qDebug() << Q_FUNC_INFO << "Driver version :" << QString::number(l_driverVersion) << l_buildBranch;

    return QString(l_version) + "|" + QString::number(l_driverVersion) + "|" + l_buildBranch ;
}

bool NvInterface::getInitStatus()
{
    return m_initStatus;
}

QString NvInterface::getInitError()
{
    return m_initError;
}

float NvInterface::getTemperature()
{
    //qDebug() << Q_FUNC_INFO;
    NV_GPU_THERMAL_SETTINGS l_thermalSettings;
    l_thermalSettings.version = NV_GPU_THERMAL_SETTINGS_VER;
    NvAPI_GPU_GetThermalSettings(m_physicalGpuHandle[0], 0, &l_thermalSettings);
    //qDebug() << Q_FUNC_INFO << l_thermalSettings.sensor[0].currentTemp;
    return l_thermalSettings.sensor[0].currentTemp;
}
