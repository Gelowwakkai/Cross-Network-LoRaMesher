#pragma once

#include "Arduino.h"

#include "commands/commandService.h"

#include "cdpMessage.h"

class CdpCommandService: public CommandService {
public:
    CdpCommandService();
};