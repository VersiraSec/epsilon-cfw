#include "../regs/regs.h"
#include "../usb/calculator.h"

void ion_main(int argc, char * argv[]) {
  while (true) {
    while (!OTG.GINTSTS()->getENUMDNE()) {
    }
    Ion::USB::Device::Calculator::PollAndReset();
  }
}
