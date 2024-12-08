#include "monitoringCommandService.h"
#include "monitoring.h" // FF commented?

MonitoringCommandService::MonitoringCommandService() {
    addCommand(Command("/monitoringIdle", "Set the monitoring On specifying the source in hex (like the display)", MonitoringState::mIdle, 1,
        [this](String args) {
        return String(Monitoring::getInstance().monitoringIdle(strtol(args.c_str(), NULL, 16)));
    }));

    addCommand(Command("/monitoringActive", "Set the monitoring Off specifying the source in hex (like the display)", MonitoringState::mActive, 1,
        [this](String args) {
        return String(Monitoring::getInstance().monitoringActive(strtol(args.c_str(), NULL, 16)));
    }));
}