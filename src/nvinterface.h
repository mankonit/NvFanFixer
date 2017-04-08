#ifndef NVINTERFACE_H
#define NVINTERFACE_H

#include <QApplication>
#include <QLibrary>

#include "../lib/nvapi/nvapi.h"

#define NVAPI_MAX_COOLER_PER_GPU    20
#define GPU_COOLER_LEVELS_VER 0x10000

typedef struct struct_NvLevel
{
  NvS32 Level;
  NvS32 Policy;
} NvLevel;

typedef struct struct_NV_GPU_COOLER_LEVELS
{
  NvU32    Version;
  NvLevel  Levels[NVAPI_MAX_COOLER_PER_GPU];

} NV_GPU_COOLER_LEVELS;

typedef void *(*NvAPI_QueryInterface_t)(unsigned int offset);
typedef NvAPI_Status (*NvAPI_GPU_SetCoolerLevels_t)(NvPhysicalGpuHandle hPhysicalGpu, NvU32 coolerIndex, NV_GPU_COOLER_LEVELS *pCoolerLevels);
typedef NvAPI_Status (*NvAPI_GPU_RestoreCoolerSettings_t) (NvPhysicalGpuHandle hPhysicalGpu, NvU32 coolerIndex);

class NvInterface
{
public:
    static NvInterface *instance();
    ~NvInterface();
    void unload();
    int getFanSpeed();
    void setFanSpeedPercent(int i_percent);
    void resetFanRegulation();
    QString getGpuInfo();
    QString getDriverInfo();
    bool getInitStatus();
    QString getInitError();
    float getTemperature();

private:
    explicit NvInterface();
    static NvInterface *m_instance;
    bool m_initStatus;
    QString m_initError;
    NvPhysicalGpuHandle m_physicalGpuHandle[NVAPI_MAX_PHYSICAL_GPUS];
    QLibrary *m_nvapiLib;
    NvAPI_QueryInterface_t f_NvQueryInterface;
    NvAPI_GPU_SetCoolerLevels_t f_NvSetCoolerLevels;
    NvAPI_GPU_RestoreCoolerSettings_t f_NvRestoreCoolerSettings;
};

#endif // NVINTERFACE_H
