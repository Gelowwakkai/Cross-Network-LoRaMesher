#pragma once

#include "Arduino.h"

#include "commands/commandService.h"

#include "monitoringMessage.h"

class MonitoringCommandService: public CommandService {
public:
    MonitoringCommandService();
};