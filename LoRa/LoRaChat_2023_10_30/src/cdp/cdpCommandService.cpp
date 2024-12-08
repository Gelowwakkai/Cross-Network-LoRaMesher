#include "cdpCommandService.h"
#include "cdp.h" // FF commented?

CdpCommandService::CdpCommandService() {
    addCommand(Command("/cdpIdle", "Set the cdp On specifying the source in hex (like the display)", CdpState::Idle, 1,
        [this](String args) {
        return String(Cdp::getInstance().cdpIdle(strtol(args.c_str(), NULL, 16)));
    }));

    addCommand(Command("/cdpActive", "Set the cdp Off specifying the source in hex (like the display)", CdpState::Active, 1,
        [this](String args) {
        return String(Cdp::getInstance().cdpActive(strtol(args.c_str(), NULL, 16)));
    }));
}