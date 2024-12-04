#pragma once

#include "Arduino.h"

#include "commands/commandService.h"

#include "flkwsMessage.h"

class FlkwsCommandService: public CommandService {
public:
    FlkwsCommandService();
};
