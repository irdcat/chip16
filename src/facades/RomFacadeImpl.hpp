#pragma once

#include <fstream>
#include <sstream>
#include <utility>

#include "../Cpu.hpp"
#include "../Memory.hpp"
#include "RomHeader.hpp"
#include "RomFacade.hpp"
#include "../utils/Crc32.hpp"
#include "../log/Logger.hpp"
#include "../log/HexModificator.hpp"
#include "../log/NumberModificator.hpp"

class RomFacadeImpl : public RomFacade
{
public:
    RomFacadeImpl() = default;

    RomFacadeImpl(const std::shared_ptr<Cpu>& cpu, const std::shared_ptr<Memory>& memory);

    ~RomFacadeImpl() = default;

    bool loadRomIntoMemory(const std::shared_ptr<RomInputStream>& romInputStream) override;

private:
    bool hasChip16Header(std::istream& ifstream);

    bool validateRom(std::istream& ifstream, const RomHeader& header);

    RomHeader extractHeaderFromFile(std::istream& ifstream);

    void logRomHeader(const RomHeader& header);

    std::shared_ptr<Cpu> cpu;
    std::shared_ptr<Memory> memory;
    static Logger LOG;
};