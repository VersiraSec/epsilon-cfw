#ifndef ION_DEVICE_SHARED_DRIVERS_BOARD_H
#define ION_DEVICE_SHARED_DRIVERS_BOARD_H

#include <stdint.h>

namespace Ion {
namespace Device {
namespace Board {

void init();

void initFPU();
void initClocks();
void shutdownClocks(bool keepLEDAwake = false);

void initPeripherals(bool initBacklight);
void shutdownPeripherals(bool keepLEDAwake = false);

enum class Frequency {
  Low = 0,
  High = 1
};

Frequency standardFrequency();
void setStandardFrequency(Frequency f);
void setClockFrequency(Frequency f);

typedef uint32_t PCBVersion;

PCBVersion readPCBVersion();
PCBVersion readPCBVersionInMemory();
void writePCBVersion(PCBVersion version);
void lockVersionOTP();

}
}
}

#endif
