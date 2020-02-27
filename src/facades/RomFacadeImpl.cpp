#include "RomFacadeImpl.hpp"

Logger RomFacadeImpl::LOG(STRINGIFY(RomFacadeImpl));

RomFacadeImpl::RomFacadeImpl(const std::shared_ptr<Cpu>& cpu, const std::shared_ptr<Memory>& memory)
    : cpu(cpu)
    , memory(memory)
{
}

bool RomFacadeImpl::loadRomIntoMemory(const std::shared_ptr<RomInputStream>& romInputStream)
{
    LOG.info("Loading ROM into memory");
    std::istream& inputRom = romInputStream->getStream();
    if (inputRom.fail())
    {
        LOG.error("Could not open ROM");
        return false;
    }

    bool hasHeader = hasChip16Header(inputRom);
    if (hasHeader)
    {
        LOG.info("ROM contains header. CRC32 checksum will be validated.");
        RomHeader header = extractHeaderFromFile(inputRom);
        logRomHeader(header);
        if (!validateRom(inputRom, header))
        {
            LOG.error("CRC32 checksum validation failed");
            return false;
        }
        LOG.info("CRC32 checksum passed succesfully.");
        cpu->getRegisters().pc = header.startAddr;
    }
    else 
    {
        LOG.info("ROM does not contain header. CRC32 checksum validation skipped.");
        cpu->getRegisters().pc = 0;
    }

    inputRom.seekg(hasHeader ? 16 : 0, inputRom.beg);
    memory->loadRomFromStream(inputRom);
    return true;
}

bool RomFacadeImpl::hasChip16Header(std::istream& istream)
{
    const auto MAGIC_NUMBER_SIZE = 4;
    const std::string MAGIC_NUMBER("CH16");
    istream.seekg(0, istream.beg);
    std::string data(MAGIC_NUMBER_SIZE, '\0');
    
    if (!istream.read(&data[0], MAGIC_NUMBER_SIZE))
        return false;

    return data == MAGIC_NUMBER;
}

bool RomFacadeImpl::validateRom(std::istream& istream, const RomHeader& header)
{
    const auto CRC_CHECKSUM = header.crc32Checksum;
    istream.seekg(16, istream.beg);

    std::vector<u8> romData;
    for (auto i = header.romSize; i > 0 && istream.good(); i--) {
        u8 data = 0;
        data = istream.get();
        romData.push_back(data);
    }

    const auto CALCULATED_CHECKSUM = Crc32::checksum(romData.begin(), romData.end());

    if (CALCULATED_CHECKSUM != CRC_CHECKSUM)
    {
        LOG.error("CRC32 checksum validation failed. Expected: ", logHex(CRC_CHECKSUM), " Actual: ", logHex(CALCULATED_CHECKSUM));
        return false;
    }
    return true;
}

RomHeader RomFacadeImpl::extractHeaderFromFile(std::istream& istream)
{
    const auto HEADER_SIZE = 16;
    RomHeader header;
    
    auto readIntFromStream = [](auto& is, auto& var) {
        is.read(reinterpret_cast<char*>(&var), sizeof(var));
    };

    istream.seekg(0, istream.beg);
    readIntFromStream(istream, header.magicNumber);
    readIntFromStream(istream, header.reserved);
    readIntFromStream(istream, header.specVersion);
    readIntFromStream(istream, header.romSize);
    readIntFromStream(istream, header.startAddr);
    readIntFromStream(istream, header.crc32Checksum);

    return header;
}

void RomFacadeImpl::logRomHeader(const RomHeader& header)
{
    const std::string SPECIFICATION_VERSION = std::to_string(header.specVersion >> 4) + '.' + std::to_string(header.specVersion & 0xF);

    LOG.info("ROM header info:\n\t",
        "Specyfication version: ", SPECIFICATION_VERSION       , "\n\t",
        "ROM size in bytes: "    , logNumber(header.romSize)   , "\n\t",
        "Start address: "        , logHex(header.startAddr)    , "\n\t",
        "CRC32 checksum: "       , logHex(header.crc32Checksum));
}
