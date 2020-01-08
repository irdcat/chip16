#pragma once

#include <fstream>
#include <sstream>

#include "Cpu.hpp"
#include "Memory.hpp"
#include "RomHeader.hpp"
#include "RomFacade.hpp"
#include "utils/Crc32.hpp"
#include "log/Logger.hpp"
#include "log/HexModificator.hpp"
#include "log/NumberModificator.hpp"

class RomFacadeImpl : public RomFacade
{
public:
    RomFacadeImpl() = default;

    RomFacadeImpl(const std::shared_ptr<Cpu>& cpu, const std::shared_ptr<Memory>& memory);

    ~RomFacadeImpl() = default;

    bool loadRomIntoMemory(const std::string& filename) override;

private:
    bool hasChip16Header(std::ifstream& ifstream);

    bool validateRom(std::ifstream& ifstream, const RomHeader& header);

    RomHeader extractHeaderFromFile(std::ifstream& ifstream);

    void logRomHeader(const RomHeader& header);

    std::shared_ptr<Cpu> cpu;
    std::shared_ptr<Memory> memory;
    static Logger LOG;
};