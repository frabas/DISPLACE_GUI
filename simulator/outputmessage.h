#ifndef OUTPUTMESSAGE_H
#define OUTPUTMESSAGE_H

#include <ipcmsgtypes.h>
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

    virtual IpcMessageTypes getType() const = 0;

    /** @brief perform an operation non related to the gui feedback channel
     * @return false if the calling function should exit. True otherwise.
     */
    virtual bool process() = 0;

    /** @brief the operation to perform.
     * @return false if error.
     * */
    virtual bool send(std::ostream &) = 0;

    /** @brief formats a binary block of data to be sent across the IPC stream
     * @arg buffer address of the data buffer
     * @arg maxlen the maximum size of the block
     * @return lenght of the block, or -1 if the process should exit (due to error, or other)
    */
    virtual size_t sendBinary(void *buffer, size_t maxlen) = 0;

    template <typename T>
    size_t put (void *buffer, size_t index, T v) {
        for (size_t i = 0; i < sizeof(T); ++i) {
            *((char *)buffer + index + i) = *((char *)&v + i);
        }
        return index + sizeof(T);
    }
};

#endif // OUTPUTMESSAGE_H
