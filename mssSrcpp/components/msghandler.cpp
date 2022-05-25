#include "msghandler.h"
#include "dapi.h"


MsgHandler &MsgHandler::getInstance () {
    static MsgHandler instance;
    return instance;
}


void MsgHandler::info (std::string msg) {
    Dapi::getInstance() << "INFO:" << msg;
}


void MsgHandler::warning (std::string msg) {
    Dapi::getInstance() << "WARN:" << msg;
}


void MsgHandler::error (std::string msg) {
    Dapi::getInstance() << "ERROR:" << msg;
}


MsgHandler::MsgHandler () {

}
