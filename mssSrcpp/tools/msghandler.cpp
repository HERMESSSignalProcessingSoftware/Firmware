#include "msghandler.h"
#include "../components/dapi.h"
#include "../components/tm.h"


MsgHandler &MsgHandler::getInstance () {
    static MsgHandler instance;
    return instance;
}


void MsgHandler::info (std::string msg) {
    Dapi::getInstance() << (std::string("\0", 1) + msg + "0\x0F\x17\xF0");
    Tm::getInstance() << (msg + "0");
}


void MsgHandler::warning (std::string msg) {
    Dapi::getInstance() << (std::string("\0", 1) + msg + "1\x0F\x17\xF0");
    Tm::getInstance() << (msg + "1");
}


void MsgHandler::error (std::string msg) {
    Dapi::getInstance() << (std::string("\0", 1) + msg + "2\x0F\x17\xF0");
    Tm::getInstance() << (msg + "2");
}


MsgHandler::MsgHandler () {}
