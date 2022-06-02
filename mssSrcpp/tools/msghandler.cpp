#include "msghandler.h"
#include "../components/dapi.h"


MsgHandler &MsgHandler::getInstance () {
    static MsgHandler instance;
    return instance;
}


void MsgHandler::info (std::string msg) {
    Dapi::getInstance() << "INFO:" << msg << "\n";
}


void MsgHandler::warning (std::string msg) {
    Dapi::getInstance() << "WARN:" << msg << "\n";
}


void MsgHandler::error (std::string msg) {
    Dapi::getInstance() << "ERROR:" << msg << "\n";
}


MsgHandler::MsgHandler () {}
