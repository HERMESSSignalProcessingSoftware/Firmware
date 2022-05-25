#ifndef COMPONENTS_MSGHANDLER_H_
#define COMPONENTS_MSGHANDLER_H_

#include <string>


class MsgHandler {
public:
    /** Singleton access method
     *
     * Use this method to get the MsgHandler instance
     * @return the instance
     */
    static MsgHandler &getInstance();

    void info (std::string msg);

    void warning (std::string msg);

    void error (std::string msg);


private:
    MsgHandler ();
};


#endif
