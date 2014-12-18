#ifndef OUTPUTMESSAGE_H
#define OUTPUTMESSAGE_H

/** @brief an abstract class to serialize outputs through the OutputQueueManager
 *
 * */

class OutputMessage
{
public:
    OutputMessage();
    virtual ~OutputMessage() = 0;

    virtual bool send() = 0;
};

#endif // OUTPUTMESSAGE_H
