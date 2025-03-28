#include <library/cpp/testing/gtest/gtest.h>
#include "sparse_bitmap.h"

namespace NKikimr::NSparseBitmap::NDetail {

    std::vector<TBitMask> BitVector(std::initializer_list<const TBitMask> m) {
        return { m.begin(), m.end() };
    }

    std::vector<TBitMask> BitVector(std::span<const TBitMask> m) {
        return { m.begin(), m.end() };
    }

    TEST(SparseBitmapDetails, SmallBitmap) {
        TLeafBlock<4> block;
        EXPECT_EQ(block.GetControl().Value, 0ull);
        EXPECT_THAT(BitVector(block.GetData()), testing::ContainerEq(BitVector({})));
        ASSERT_TRUE(block.Set(1));
        EXPECT_EQ(block.GetControl().Value, 1ull);
        EXPECT_THAT(BitVector(block.GetData()), testing::ContainerEq(BitVector({2})));
        ASSERT_TRUE(block.Set(2047));
        EXPECT_EQ(block.GetControl().Value, 0x80000001ull);
        EXPECT_THAT(BitVector(block.GetData()), testing::ContainerEq(BitVector({2, 0x8000000000000000ull})));
        ASSERT_TRUE(block.Set(1022));
        EXPECT_EQ(block.GetControl().Value, 0x80008001ull);
        EXPECT_THAT(BitVector(block.GetData()), testing::ContainerEq(BitVector({2, 0x4000000000000000ull, 0x8000000000000000ull})));
        ASSERT_FALSE(block.Set(64));
    }

} // namespace NKikimr::NSparseBitmap::NDetail
