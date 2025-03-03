#include <ion/events.h>
#include <layout_events.h>
#include <userland/drivers/svcall.h>

namespace Ion {
namespace Events {

size_t SVC_ATTRIBUTES copyText(uint8_t eventId, char * buffer, size_t bufferSize) {
  SVC_RETURNING_R0(SVC_EVENTS_COPY_TEXT, size_t)
}

bool SVC_ATTRIBUTES isDefined(uint8_t eventId) {
  SVC_RETURNING_R0(SVC_EVENTS_IS_DEFINED, bool)
}

void SVC_ATTRIBUTES setShiftAlphaStatus(ShiftAlphaStatus s) {
  SVC_RETURNING_VOID(SVC_EVENTS_SET_SHIFT_ALPHA_STATUS)
}

ShiftAlphaStatus SVC_ATTRIBUTES shiftAlphaStatus() {
  SVC_RETURNING_R0(SVC_EVENTS_SHIFT_ALPHA_STATUS, ShiftAlphaStatus)
}

int SVC_ATTRIBUTES repetitionFactor() {
  SVC_RETURNING_R0(SVC_EVENTS_REPETITION_FACTOR, int)
}

Event SVC_ATTRIBUTES getEvent(int * timeout) {
  SVC_RETURNING_R0(SVC_EVENTS_GET_EVENT, Event)
}

void SVC_ATTRIBUTES setSpinner(bool spiner) {
  SVC_RETURNING_VOID(SVC_EVENTS_SET_SPINNER)
}

}
}
