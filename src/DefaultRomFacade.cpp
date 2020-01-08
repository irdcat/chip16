#include "RomFacadeImpl.hpp"

Logger RomFacadeImpl::LOG(STRINGIFY(RomFacadeImpl));

RomFacadeImpl::RomFacadeImpl(const std::shared_ptr<Cpu>& cpu, const std::shared_ptr<Memory>& memory)
    : cpu(cpu)
    , memory(memory)
{
}

bool RomFacadeImpl::loadRomIntoMemory(const std::string& filename)
{
    LOG.info("Loading ROM ", filename, " into memory");
    std::ifstream inputRom(filename, std::ios::binary);
    if (!inputRom.is_open())
    {
        LOG.error("Could not open ROM with name ", filename);
        return false;
    }

    bool hasHeader = hasChip16Header(inputRom);
    if (hasHeader)
    {
        LOG.info("ROM contains header. CRC32 checksum will be validated.");
        RomHeader header = extractHeaderFromFile(inputRom);
        logRomHeader(header);
        if (validateRom(inputRom, header))
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
    return true;
}

bool RomFacadeImpl::hasChip16Header(std::ifstream& ifstream)
{
    const auto MAGIC_NUMBER_SIZE = 4;
    const std::string MAGIC_NUMBER("CH16");
    ifstream.seekg(0, ifstream.beg);
    std::string data(MAGIC_NUMBER_SIZE, '\0');
    
    if (!ifstream.read(&data[0], MAGIC_NUMBER_SIZE))
        return false;

    ifstream.seekg(0, ifstream.beg);
    return data == MAGIC_NUMBER;
}

bool RomFacadeImpl::validateRom(std::ifstream& ifstream, const RomHeader& header)
{
    const auto CRC_CHECKSUM = header.crc32Checksum;
    ifstream.seekg(16, ifstream.beg);

    std::vector<u8> romData;
    do {
        u8 data = 0;
        ifstream >> data;
        romData.push_back(data);
    } while (ifstream.good());

    const auto CALCULATED_CHECKSUM = Crc32::checksum(romData.begin(), romData.end());

    if (CALCULATED_CHECKSUM != CRC_CHECKSUM)
    {
        LOG.error("CRC32 checksum validation failed. Expected: ", logHex(CRC_CHECKSUM), " Actual: ", logHex(CALCULATED_CHECKSUM));
        return false;
    }
    return true;
}

RomHeader RomFacadeImpl::extractHeaderFromFile(std::ifstream& ifstream)
{
    const auto HEADER_SIZE = 16;
    RomHeader header;
    
    ifstream.read(&header.rawData[0], HEADER_SIZE);

    return header;
}

void RomFacadeImpl::logRomHeader(const RomHeader& header)
{
    const std::string SPECYFICATION_VERSION = std::to_string(header.specVersion >> 4) + '.' + std::to_string(header.specVersion & 0xF);

    LOG.info("ROM header info:\n\t",
        "Specyfication version: ", SPECYFICATION_VERSION       , "\n\t",
        "ROM size in bytes: "    , logNumber(header.romSize)   , "\n\t",
        "Start address: "        , logHex(header.startAddr)    , "\n\t",
        "CRC32 checksum: "       , logHex(header.crc32Checksum));
}
