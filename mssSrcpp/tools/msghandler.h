#ifndef COMPONENTS_MSGHANDLER_H_
#define COMPONENTS_MSGHANDLER_H_

#include <string>



/** Message Handler class
 *
 * Use this class whenever sending a text message to the ground station
 * software via DAPI or TM. All functions send via both interfaces.
 */
class MsgHandler {
public:
    /** Singleton access method
     *
     * Use this method to get the MsgHandler instance
     * @return the instance
     */
    static MsgHandler &getInstance();

    /** Send info message to DAPI and TM port
     *
     * Info messages are sent, whenever the SPU reached and successfully ran
     * a critical execution point
     * @param msg the text message to be sent
     */
    void info (std::string msg);

    /** Send warning message to DAPI and TM port
     *
     * Warnings are triggered by events, in which the SPU detected a non-normal
     * state of one of its subsystem not immediately leading to a
     * significant degradation of system performance.
     * @param msg the text message to be sent
     */
    void warning (std::string msg);

    /** Send error message to DAPI and TM port
     *
     * Errors are events, in which the SPU detected a serious defect
     * in its state resulting in a significant loss of data.
     * @param msg the text message to be sent
     */
    void error (std::string msg);

private:
    MsgHandler ();
};


#endif
