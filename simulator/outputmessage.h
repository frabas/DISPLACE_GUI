#ifndef OUTPUTMESSAGE_H
#define OUTPUTMESSAGE_H

/** @brief an abstract class to serialize outputs through the OutputQueueManager
 *
 * */

class OutputMessage
{
public:
    OutputMessage();
    virtual ~OutputMessage() {
    }

    /** @brief the operation to perform.
     * @return false if the outer queue loop should be shut down. Most of the classes will return true
     * */
    virtual bool send() = 0;
};

#endif // OUTPUTMESSAGE_H
