#pragma once

#include <util/system/types.h>

#include <bit>
#include <array>
#include <cstdint>
#include <cstring>
#include <span>

static_assert(std::endian::native == std::endian::little, "Only little-endian systems are supported");

namespace NKikimr::NSparseBitmap::NDetail {

    // We use 64-bit chunks
    // Note: control word uses the same size
    using TBitMask = ui64;

    static_assert(sizeof(TBitMask) == 8, "Expected ui64 to be 8 bytes");

    template<bool LargePointerType = false>
    struct TSelectInnerDataType {
        using TType = TBitMask;
    };

    template<>
    struct TSelectInnerDataType<true> {
        using TType = uintptr_t;
    };

    // We want inner nodes to store either d
    using TInnerData = TSelectInnerDataType<(sizeof(uintptr_t) > sizeof(TBitMask))>::TType;

    enum : int {
        // Divides/multiplies by 64 (size of the data block)
        BlockIndexShift = 6,
    };

    enum : ui64 {
        BitIndexMask = 63,
        BlockAllOne = ui64(0xFFFFFFFFFFFFFFFFull),
        BlockAllZero = ui64(0x0000000000000000ull),
        ControlWordPresenceMask = ui64(0x00000000FFFFFFFFull),
        ControlWordAllOne = ui64(0xFFFFFFFF00000000ull),
        ControlWordAllZero = ui64(0x0000000000000000ull),
    };

    // Lower bits of the pointer are used as tags
    enum class EBlockTypeTag : ui64 {
        Capacity32 = 0,
        Capacity16 = 1,
        Capacity8 = 2,
        Capacity4 = 3,
        Capacity2 = 4,
    };

    struct TControlWord {
        TBitMask Value;

        bool IsAllOne() const {
            return Value == ControlWordAllOne;
        }

        bool IsAllZero() const {
            return Value == ControlWordAllZero;
        }

        int MixedBlocksCount() const {
            return std::popcount(Value & ControlWordPresenceMask);
        }
    };

    template<size_t Capacity>
    class TLeafBlock {
    public:
        TControlWord GetControl() const {
            return TControlWord{ Data_[0] };
        }

        std::span<const TBitMask> GetData() const {
            return { Data_ + 1, size_t(GetControl().MixedBlocksCount()) };
        }

        TBitMask GetBlock(int blockIndex) const {
            // Lower 32 bits of the control word indicate block presence
            TBitMask controlBit = TBitMask(1) << blockIndex;
            if (Data_[0] & controlBit) {
                // Control word indicates block is present
                // Use std::popcount to find how many other blocks are present
                int offset = 1 + std::popcount(Data_[0] & (controlBit - 1));
                return Data_[offset];
            } else {
                // Control word indicates block is optimized
                return Data_[0] & (controlBit << 32) ? BlockAllOne : BlockAllZero;
            }
        }

        bool Get(size_t index) const {
            // Lower 32 bits of the control word indicate block presence
            TBitMask controlBit = TBitMask(1u) << (index >> BlockIndexShift);
            if (Data_[0] & controlBit) {
                // Control word indicates block is present
                // Use std::popcount to find how many other bits are set below
                int offset = 1 + std::popcount(Data_[0] & (controlBit - 1));
                return Data_[offset] & (TBitMask(1) << (index & BitIndexMask));
            } else {
                // Control word indicates block is optimized
                // All bits have the value same as the high bit
                return Data_[0] & (controlBit << 32);
            }
        }

        void Clear() {
            Data_[0] = 0;
        }

        bool SetBlockBits(int blockIndex, TBitMask bitMask) {
            // Lower 32 bits of the control word indicate block presence
            TBitMask controlBit = TBitMask(1) << blockIndex;
            if (Data_[0] & controlBit) {
                // Control word indicates block is present
                // Use std::popcount to find how many other blocks are present
                int offset = 1 + std::popcount(Data_[0] & (controlBit - 1));
                TBitMask result = Data_[offset] | bitMask;
                if (result == BlockAllOne) {
                    // Optimize this block to all ones
                    Data_[0] = (Data_[0] & ~controlBit) | (controlBit << 32);
                    int end = 1 + GetControl().MixedBlocksCount();
                    if (offset != end) {
                        // Move bits after offset to the left
                        ::memmove(Data_ + offset, Data_ + (offset + 1), (end - offset) * sizeof(TBitMask));
                    }
                } else {
                    Data_[offset] = result;
                }
                return true;
            } else if (Data_[0] & (controlBit << 32)) {
                // This bit is already part of an all-one block
                return true;
            } else {
                // Create a new mixed block
                int end = 1 + GetControl().MixedBlocksCount();
                if (end == Capacity) {
                    // Not enough space to add a new block
                    return false;
                }
                int offset = 1 + std::popcount(Data_[0] & (controlBit - 1));
                if (offset != end) {
                    // Move bits at offset to the right
                    ::memmove(Data_ + (offset + 1), Data_ + offset, (end - offset) * sizeof(TBitMask));
                }
                Data_[offset] = bitMask;
                Data_[0] |= controlBit;
                return true;
            }
        }

        bool Set(size_t bitIndex) {
            // Lower 32 bits of the control word indicate block presence
            return SetBlockBits(bitIndex >> BlockIndexShift, TBitMask(1) << (bitIndex & BitIndexMask));
        }

        bool SetRange(size_t lowIndex, size_t highIndex) {
            int lowBlockIndex = lowIndex >> BlockIndexShift;
            int highBlockIndex = highIndex >> BlockIndexShift;
            lowIndex &= BitIndexMask;
            highIndex &= BitIndexMask;
            if (lowBlockIndex == highBlockIndex) {
                // All bits are in the same block
                TBitMask lowBit = TBitMask(1u) << lowIndex;
                TBitMask highBit = TBitMask(1u) << highIndex;
                return SetBlockBits(lowBlockIndex, highBit | (highBit - lowBit));
            }
            int end = 1 + GetControl().MixedBlocksCount();
            int lowFull = lowBlockIndex + (lowIndex != 0);
            int highFull = highBlockIndex - (highIndex != BitIndexMask);
            if (lowFull <= highFull) {
                // All blocks between low and high will become all ones
                TBitMask lowControlBit = TBitMask(1) << lowFull;
                TBitMask highControlBit = TBitMask(1) << highFull;
                TBitMask controlMask = highControlBit | (highControlBit - lowControlBit);
                if (Data_[0] & controlMask) {
                    int count = std::popcount(Data_[0] & controlMask);
                    int begin = 1 + std::popcount(Data_[0] & (lowControlBit - 1));
                    if (end != begin + count) {
                        ::memmove(Data_ + begin, Data_ + (begin + count), (end - begin - count) * sizeof(TBitMask));
                    }
                    // Set all presence bits to zero
                    Data_[0] &= ~controlMask;
                    end -= count;
                }
                // Set all value bits to 1
                Data_[0] |= controlMask << 32;
            }
            // Note: there is a suboptimal case where high needs to grow while low shrinks, we have enough capacity but still fail
            if (highIndex != BitIndexMask) {
                // Set all bits from 0 to highIndex in highBlockIndex to 1
                TBitMask mask = (TBitMask(1) << (highIndex - 1)) - 1;
                if (!SetBlockBits(highBlockIndex, mask)) {
                    return false;
                }
            }
            if (lowIndex != 0) {
                // Set all bits lowIndex to 63 in lowBlockIndex to 1
                TBitMask mask = BlockAllOne - ((TBitMask(1) << lowIndex) - 1);
                if (!SetBlockBits(lowBlockIndex, mask)) {
                    return false;
                }
            }
            return true;
        }

    private:
        // The first word is the control word
        TBitMask Data_[Capacity] = { 0 };
    };

    template<>
    class TLeafBlock<32> {
    public:
        TControlWord GetControl() const {
            TBitMask value = 0;
            TBitMask controlBit = 1;
            for (int i = 0; i < 32; ++i) {
                if (Data_[i] == BlockAllZero) {
                    // Value bit = 0
                } else if (Data_[i] == BlockAllOne) {
                    // Value bit = 1
                    value |= (controlBit << 32);
                } else {
                    // Presence bit = 1
                    value |= controlBit;
                }
                controlBit <<= 1;
            }
            return TControlWord{ value };
        }

        std::span<const TBitMask> GetData() const {
            return { Data_, 32 };
        }

        TBitMask GetBlock(int blockIndex) const {
            return Data_[blockIndex];
        }

        bool Get(size_t index) const {
            return Data_[index >> BlockIndexShift] & (TBitMask(1) << (index & BitIndexMask));
        }

        void Clear() {
            for (int i = 0; i < 32; ++i) {
                Data_[i] = 0;
            }
        }

        bool SetBlockBits(int blockIndex, TBitMask bitMask) {
            Data_[blockIndex] |= bitMask;
            return true;
        }

        bool Set(size_t index) {
            Data_[index >> BlockIndexShift] |= (TBitMask(1) << (index & BitIndexMask));
            return true;
        }

        bool SetRange(size_t lowIndex, size_t highIndex) {
            int lowBlockIndex = lowIndex >> BlockIndexShift;
            int highBlockIndex = highIndex >> BlockIndexShift;
            lowIndex &= BitIndexMask;
            highIndex &= BitIndexMask;
            if (lowBlockIndex < highBlockIndex && lowIndex != 0) {
                TBitMask mask = BlockAllOne - ((TBitMask(1) << lowIndex) - 1);
                Data_[lowBlockIndex++] |= mask;
                lowIndex = 0;
            }
            while (lowBlockIndex < highBlockIndex) {
                Data_[lowBlockIndex++] |= BlockAllOne;
            }
            // All bits are in the same block
            TBitMask lowBit = TBitMask(1u) << lowIndex;
            TBitMask highBit = TBitMask(1u) << highIndex;
            Data_[lowBlockIndex] |= highBit | (highBit - lowBit);
            return true;
        }

    private:
        TBitMask Data_[32] = { 0 };
    };

    static_assert(sizeof(TLeafBlock<32>) == 256);
    static_assert(sizeof(TLeafBlock<16>) == 128);
    static_assert(sizeof(TLeafBlock<8>) == 64);
    static_assert(sizeof(TLeafBlock<4>) == 32);
    static_assert(sizeof(TLeafBlock<2>) == 16);

} // namespace NKikimr::NSparseBitmap::NDetail
