//
// Created by Federico Fuga on 21/03/16.
//

#ifndef DISPLACE_STEPMESSAGE_H
#define DISPLACE_STEPMESSAGE_H

#include <ipc/messages/genericconsolestringoutputmessage.h>
#include "genericconsolestringoutputmessage.h"

#include <boost/format.hpp>

namespace displace {
    namespace ipc {

        class StepMessage : public GenericConsoleStringOutputMessage {
        public:
            StepMessage(int tstep)
            : GenericConsoleStringOutputMessage(boost::str(boost::format
                                                                   {"=S%d"} % tstep))
            {
            }
        };
    }
}

#endif //DISPLACE_STEPMESSAGE_H
