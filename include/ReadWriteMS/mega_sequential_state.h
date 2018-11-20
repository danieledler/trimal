#ifndef MEGASSTATE_H
#define MEGASSTATE_H

#include "ReadWriteBaseState.h"

class mega_sequential_state : public ReadWriteBaseState {
public:

    explicit mega_sequential_state(ReadWriteMS *MachineState) {
        Machine = MachineState;
        name = "mega_sequential";
        extension = "mega";
        canLoad = true;
        canSave = true;
    };

    int CheckAlignment(std::istream *origin) override;

    newAlignment *LoadAlignment(std::string filename) override;

    bool SaveAlignment(newAlignment *alignment, std::ostream *output, std::string *FileName) override;

    bool RecognizeOutputFormat(std::string FormatName) override;

};

#endif // MEGASSTATE_H
