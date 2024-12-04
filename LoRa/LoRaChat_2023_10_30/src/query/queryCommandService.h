#pragma once

#include "Arduino.h"

#include "commands/commandService.h"

#include "queryMessage.h"

class QueryCommandService: public CommandService {
public:
    QueryCommandService();
};
