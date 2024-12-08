#include "queryCommandService.h"
#include "query.h"

QueryCommandService::QueryCommandService() {
    addCommand(Command("/queryOn", "Set the Query On specifying the source in hex (like the display)", QueryCommand::services, 1,
        [this](String args) {
        return String(Query::getInstance().queryOn(strtol(args.c_str(), NULL, 16)));
    }));

    addCommand(Command("/queryOff", "Set the Query Off specifying the source in hex (like the display)", QueryCommand::routes, 1,
        [this](String args) {
        return String(Query::getInstance().queryOff(strtol(args.c_str(), NULL, 16)));
    }));
}
