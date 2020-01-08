#include <memory>
#include <cstdarg>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "../src/CpuImpl.hpp"
#include "../src/Memory.hpp"
#include "../src/Bus.hpp"

namespace
{
    using ::testing::Return;
    using ::testing::Eq;

    class MemoryMock : public Memory
    {
    public:
        MOCK_CONST_METHOD1(readByte, u8(u16));
        MOCK_METHOD2(writeByte, void(u16, u8));
        MOCK_CONST_METHOD1(readWord, u16(u16));
        MOCK_METHOD2(writeWord, void(u16, u16));
        MOCK_CONST_METHOD1(readControllerState, ControllerState(unsigned));
        MOCK_CONST_METHOD1(readByteReference, std::vector<u8>::const_iterator(u16));
        MOCK_METHOD1(loadRomFromStream, void(std::istream&));
    };

    class BusMock : public Bus
    {
    public:
        MOCK_METHOD1(loadPalette, void(const Palette&));
        MOCK_METHOD0(clearScreen, void());
        MOCK_METHOD1(setBackgroundColorIndex, void(u8));
        MOCK_METHOD2(setSpriteDimensions, void(u8, u8));
        MOCK_METHOD3(drawSprite, bool(u16, u16, std::vector<u8>::const_iterator));
        MOCK_METHOD1(setHFlip, void(bool));
        MOCK_METHOD1(setVFlip, void(bool));
        MOCK_CONST_METHOD0(isVBlank, bool());
    };

    class PaletteInstructionsTests : public ::testing::Test
    {
    protected:
        void SetUp() override
        {
            memory = std::make_shared<MemoryMock>();
            bus = std::make_shared<BusMock>();
            testedCpu = std::make_unique<CpuImpl>(memory, bus);
        }

        void setMemoryMocks(u16 startAddr, std::initializer_list<u8> args)
        {
            for (auto&& arg : args)
                ON_CALL(*memory, readByte(startAddr++)).WillByDefault(Return(arg));
        }

        std::unique_ptr<CpuImpl> testedCpu;
        std::shared_ptr<MemoryMock> memory;
        std::shared_ptr<BusMock> bus;

        const u16 LOAD_PALETTE_ABSOLUTE_INSTRUCTION_OPCODE = 0xD000;
        const u16 LOAD_PALETTE_INDIRECT_INSTRUCTION_OPCODE = 0xD100;
    };
};

TEST_F(PaletteInstructionsTests, testLoadPaletteAbsolute)
{
    // PAL $addr
    setMemoryMocks(0x2000, {
        0x54, 0x32, 0x21, // Color 0
        0x77, 0x55, 0x22, // Color 1
        0x93, 0xF1, 0x32, // Color 2
        0x41, 0x23, 0xFF, // Color 3
        0x53, 0xA1, 0xBC, // Color 4
        0x56, 0x32, 0xF1, // Color 5
        0xC3, 0xDD, 0xE1, // Color 6
        0xFE, 0xE3, 0xA2, // Color 7
        0xBC, 0xB5, 0x7E, // Color 8
        0x9A, 0x4B, 0x98, // Color 9
        0xAA, 0xB4, 0x45, // Color A
        0x99, 0x98, 0x78, // Color B
        0xC9, 0xAB, 0x45, // Color C
        0x96, 0x99, 0x34, // Color D
        0xB4, 0xB5, 0x33, // Color E
        0x65, 0x45, 0xAA  // Color F
        });
    EXPECT_CALL(*memory, readWord(0x102)).Times(1).WillOnce(Return(0x2000));
    auto& regs = testedCpu->getRegisters();
    regs.pc = 0x102;
    
    const Palette EXPECTED_PALETTE = {
        0x543221FF, 0x775522FF, 0x93F132FF, 0x4123FFFF,
        0x53A1BCFF, 0x5632F1FF, 0xC3DDE1FF, 0xFEE3A2FF,
        0xBCB57EFF, 0x9A4B98FF, 0xAAB445FF, 0x999878FF,
        0xC9AB45FF, 0x969934FF, 0xB4B533FF, 0x6545AAFF
    };
    EXPECT_CALL(*bus, loadPalette(Eq(EXPECTED_PALETTE))).Times(1);
    testedCpu->executeInstruction(LOAD_PALETTE_ABSOLUTE_INSTRUCTION_OPCODE);
}

TEST_F(PaletteInstructionsTests, testLoadPaletteIndirect)
{
    // PAL Rx
    setMemoryMocks(0x2000, {
        0x54, 0x32, 0x21, // Color 0
        0x77, 0x55, 0x22, // Color 1
        0x93, 0xF1, 0x32, // Color 2
        0x41, 0x23, 0xFF, // Color 3
        0x53, 0xA1, 0xBC, // Color 4
        0x56, 0x32, 0xF1, // Color 5
        0xC3, 0xDD, 0xE1, // Color 6
        0xFE, 0xE3, 0xA2, // Color 7
        0xBC, 0xB5, 0x7E, // Color 8
        0x9A, 0x4B, 0x98, // Color 9
        0xAA, 0xB4, 0x45, // Color A
        0x99, 0x98, 0x78, // Color B
        0xC9, 0xAB, 0x45, // Color C
        0x96, 0x99, 0x34, // Color D
        0xB4, 0xB5, 0x33, // Color E
        0x65, 0x45, 0xAA  // Color F
        });
    const auto REG_INDEX = 5;
    auto& regs = testedCpu->getRegisters();
    regs.r[REG_INDEX] = 0x2000;

    const Palette EXPECTED_PALETTE = {
        0x543221FF, 0x775522FF, 0x93F132FF, 0x4123FFFF,
        0x53A1BCFF, 0x5632F1FF, 0xC3DDE1FF, 0xFEE3A2FF,
        0xBCB57EFF, 0x9A4B98FF, 0xAAB445FF, 0x999878FF,
        0xC9AB45FF, 0x969934FF, 0xB4B533FF, 0x6545AAFF
    };
    EXPECT_CALL(*bus, loadPalette(Eq(EXPECTED_PALETTE))).Times(1);
    testedCpu->executeInstruction(LOAD_PALETTE_INDIRECT_INSTRUCTION_OPCODE + REG_INDEX);
}