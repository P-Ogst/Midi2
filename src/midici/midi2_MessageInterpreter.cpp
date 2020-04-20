#include <midici\midi2_MessageInterpreter.h>

namespace midi2 { namespace midici {

UniversalSysExMessageBase MessageInterpreter::ReadMessage(char* buffer, size_t bufferSize)
{
    return InvalidMessage();
}

}
}