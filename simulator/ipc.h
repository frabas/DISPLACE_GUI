#ifndef IPC_H
#define IPC_H

#include <string>
#include <memoryinfo.h>

void initIpcQueue();
void guiSendCurrentStep (unsigned int tstep);
void guiSendUpdateCommand (const std::string &filename, int tstep);
void guiSendMemoryInfo(const MemoryInfo &info);
void guiSendCapture(bool on);
void guiSendTerminalMessage(const std::string &ss);
void guiSendOutputInfo(std::string sqlPath);
void finalizeIpcQueue();

#endif // IPC_H
