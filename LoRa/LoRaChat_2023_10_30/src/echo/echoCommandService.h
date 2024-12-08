#pragma once

#include "Arduino.h"

#include "commands/commandService.h"

#include "echoMessage.h"

class EchoCommandService: public CommandService {
public:
    EchoCommandService();
};
