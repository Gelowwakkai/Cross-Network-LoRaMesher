#include "flkwsCommandService.h"
#include "flkws.h"

FlkwsCommandService::FlkwsCommandService() {
    addCommand(Command("/flkwsOn", "Set the Flkws On specifying the source in hex (like the display)", FlkwsCommand::POn, 1,
        [this](String args) {
        return String(Flkws::getInstance().flkwsOn(strtol(args.c_str(), NULL, 16)));
    }));

    addCommand(Command("/flkwsOff", "Set the Flkws Off specifying the source in hex (like the display)", FlkwsCommand::POff, 1,
        [this](String args) {
        return String(Flkws::getInstance().flkwsOff(strtol(args.c_str(), NULL, 16)));
    }));
}
