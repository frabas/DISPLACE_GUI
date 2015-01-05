#ifndef OUTPUTMESSAGE_H
#define OUTPUTMESSAGE_H

#include <ostream>

/** @brief an abstract class to serialize outputs through the OutputQueueManager
 *
 * */

class OutputMessage
{
public:
    OutputMessage();
    virtual ~OutputMessage() {
    }

    virtual int getType() const = 0;

    /** @brief the operation to perform.
     * @return false if the outer queue loop should be shut down. Most of the classes will return true
     * */
    virtual bool send(std::ostream &) = 0;

    /** @brief formats a binary block of data to be sent across the IPC stream
     * @arg buffer address of the data buffer
     * @arg maxlen the maximum size of the block
     * @return lenght of the block, or -1 if the process should exit (due to error, or other)
    */
    virtual int sendBinary(void *buffer, int maxlen) = 0;
};

#endif // OUTPUTMESSAGE_H
