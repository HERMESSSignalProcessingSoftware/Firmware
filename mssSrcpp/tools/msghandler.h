#ifndef COMPONENTS_MSGHANDLER_H_
#define COMPONENTS_MSGHANDLER_H_

#include <string>


class MsgHandler {
public:
    /** Singleton access method
     *
     * !!! remove class and integrate into dapi or make TM use
     *
     * Use this method to get the MsgHandler instance
     * @return the instance
     */
    static MsgHandler &getInstance();

    /** Send info message to DAPI port
     *
     * @param msg
     */
    void info (std::string msg);

    void warning (std::string msg);

    void error (std::string msg);


private:
    MsgHandler ();
};


#endif
