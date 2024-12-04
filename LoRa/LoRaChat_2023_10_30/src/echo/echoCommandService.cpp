#include "echoCommandService.h"
#include "echo.h"

EchoCommandService::EchoCommandService() {
    addCommand(Command("/echoOn", "Set the Echo On specifying the source in hex (like the display)", EchoCommand::POn, 1,
        [this](String args) {
        return String(Echo::getInstance().echoOn(strtol(args.c_str(), NULL, 16)));
    }));

    addCommand(Command("/echoOff", "Set the Echo Off specifying the source in hex (like the display)", EchoCommand::POff, 1,
        [this](String args) {
        return String(Echo::getInstance().echoOff(strtol(args.c_str(), NULL, 16)));
    }));
}
