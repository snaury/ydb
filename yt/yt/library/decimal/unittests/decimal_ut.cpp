#include <yt/yt/core/test_framework/framework.h>

#include <yt/yt/library/decimal/decimal.h>

#include <util/string/hex.h>

namespace NYT::NDecimal {
namespace {

////////////////////////////////////////////////////////////////////////////////

TString RoundConvertText(TStringBuf textValue, int precision, int scale)
{
    auto binary = TDecimal::TextToBinary(textValue, precision, scale);
    return TDecimal::BinaryToText(binary, precision, scale);
}

#define CHECK_ROUND_CONVERSION(text, precision, scale) \
    do { \
        EXPECT_EQ(text, RoundConvertText(text, precision, scale)); \
    } while (0)

////////////////////////////////////////////////////////////////////////////////

TEST(TDecimal, TestTextBinaryConversion)
{
#define TEST_TEXT_BINARY_CONVERSION(precision, scale, text, binary) \
    do {                                                                 \
        EXPECT_EQ(binary, HexEncode(TDecimal::TextToBinary(text, precision, scale))); \
        EXPECT_EQ(text, TDecimal::BinaryToText(HexDecode(binary), precision, scale)); \
    } while (0)

    TEST_TEXT_BINARY_CONVERSION(3, 2, "3.14", "8000013A");
    TEST_TEXT_BINARY_CONVERSION(10, 2, "3.14", "80000000" "0000013A");
    TEST_TEXT_BINARY_CONVERSION(35, 2, "3.14", "80000000" "00000000" "00000000" "0000013A");

    TEST_TEXT_BINARY_CONVERSION(10, 9, "-2.718281828",  "7FFFFFFF" "5DFA4F9C");
    TEST_TEXT_BINARY_CONVERSION(35, 9, "-2.718281828", "7FFFFFFF" "FFFFFFFF" "FFFFFFFF" "5DFA4F9C");

    TEST_TEXT_BINARY_CONVERSION(3, 2, "nan", "FFFFFFFF");
    TEST_TEXT_BINARY_CONVERSION(3, 2, "inf", "FFFFFFFE");
    TEST_TEXT_BINARY_CONVERSION(3, 2, "-inf", "00000002");
    EXPECT_EQ("FFFFFFFE", HexEncode(TDecimal::TextToBinary("+inf", 3, 2)));

    TEST_TEXT_BINARY_CONVERSION(10, 2,"nan", "FFFFFFFF" "FFFFFFFF");
    TEST_TEXT_BINARY_CONVERSION(10, 2,"inf", "FFFFFFFF" "FFFFFFFE");
    TEST_TEXT_BINARY_CONVERSION(10, 2,"-inf", "00000000" "00000002");
    EXPECT_EQ("FFFFFFFF" "FFFFFFFE", HexEncode(TDecimal::TextToBinary("+inf", 10, 2)));

    TEST_TEXT_BINARY_CONVERSION(35, 2,"nan", "FFFFFFFF" "FFFFFFFF" "FFFFFFFF" "FFFFFFFF");
    TEST_TEXT_BINARY_CONVERSION(35, 2,"inf", "FFFFFFFF" "FFFFFFFF" "FFFFFFFF" "FFFFFFFE");
    TEST_TEXT_BINARY_CONVERSION(35, 2,"-inf", "00000000" "00000000" "00000000" "00000002");
    EXPECT_EQ("FFFFFFFF" "FFFFFFFF" "FFFFFFFF" "FFFFFFFE", HexEncode(TDecimal::TextToBinary("+inf", 35, 2)));

    EXPECT_THROW_WITH_SUBSTRING(TDecimal::TextToBinary("-nan", 3, 2), "is not valid Decimal");
    EXPECT_THROW_WITH_SUBSTRING(TDecimal::TextToBinary("infinity", 3, 2), "is not valid Decimal");
    EXPECT_THROW_WITH_SUBSTRING(TDecimal::TextToBinary("-infinity", 3, 2), "is not valid Decimal");

    // Few test cases with big numbers generated by python snippet:
    //    import random
    //    def print_test_case(plus):
    //        textval = "".join(random.choice("0123456789") for _ in range(35))
    //        if not plus:
    //            textval = "-" + textval
    //        binval = hex(2 ** 127 + int(textval))
    //        binval = binval[2:].strip('L')  # strip 0x and final 'L'
    //        binval = binval.upper()
    //        print(
    //            "TEST_TEXT_BINARY_CONVERSION(\n"
    //            "   35, 0,\n"
    //            "   \"{text}\",\n"
    //            "   \"{binary}\");\n"
    //            .format(text=textval, binary=binval)
    //        )
    //    random.seed(42)
    //    print_test_case(False)
    //    print_test_case(False)
    //    print_test_case(True)
    //    print_test_case(True)

    TEST_TEXT_BINARY_CONVERSION(
        35, 0,
        "-60227680402501652580863193008687593",
        "7FF4668BCCE002BD685B3A3811CE3617");

    TEST_TEXT_BINARY_CONVERSION(
        35, 0,
        "-58685702202126332296617139656872032",
        "7FF4B2924D28572FF19525515205AFA0");

    TEST_TEXT_BINARY_CONVERSION(
        35, 0,
        "29836394225258329500167403959807652",
        "8005BF0C3D439F6FFD649D99A704EEA4");

    TEST_TEXT_BINARY_CONVERSION(
        35, 0,
        "61449825198266175750309883089040771",
        "800BD5B5D5F0C73E0C9CD4943298B583");

    // A few more test cases with big numbers for various precisions generated by python snippet:
    //     import random
    //     def print_test_case(plus, binary_size, precision):
    //         textval = "".join(random.choice("0123456789") for _ in range(precision))
    //         if not plus:
    //             textval = "-" + textval
    //         binval = hex(2 ** (binary_size * 8 - 1) + int(textval))
    //         binval = binval[2:].strip('L')  # strip 0x and final 'L'
    //         binval = binval.upper()
    //         print(
    //             "TEST_TEXT_BINARY_CONVERSION(\n"
    //             "   {precision}, 0,\n"
    //             "   \"{text}\",\n"
    //             "   \"{binary}\");\n"
    //             .format(precision=precision, text=textval.lstrip("0"), binary=binval)
    //         )
    //     random.seed(42)
    //
    //     for binary_size, precision in (4, 9), (8, 18), (16, 35), (16, 38), (32, 39), (32, 76):
    //         print_test_case(False, binary_size, precision)
    //         print_test_case(False, binary_size, precision)
    //         print_test_case(True, binary_size, precision)
    //         print_test_case(True, binary_size, precision)

    TEST_TEXT_BINARY_CONVERSION(
        9, 0,
        "-104332181",
        "79C8046B");

    TEST_TEXT_BINARY_CONVERSION(
        9, 0,
        "-960013389",
        "46C75BB3");

    TEST_TEXT_BINARY_CONVERSION(
        9, 0,
        "83863794",
        "84FFA8F2");

    TEST_TEXT_BINARY_CONVERSION(
        9, 0,
        "26542351",
        "8195010F");

    TEST_TEXT_BINARY_CONVERSION(
        18, 0,
        "-161559407816184959",
        "7DC2069316FE6B81");

    TEST_TEXT_BINARY_CONVERSION(
        18, 0,
        "-310341316475255341",
        "7BB17237885D85D3");

    TEST_TEXT_BINARY_CONVERSION(
        18, 0,
        "928327648350305641",
        "8CE21513E317FD69");

    TEST_TEXT_BINARY_CONVERSION(
        18, 0,
        "395376724238849696",
        "857CA90930B6D6A0");

    TEST_TEXT_BINARY_CONVERSION(
        35, 0,
        "-53287101226916697848018451462704828",
        "7FF5BCBE39B5F1A05ED5F0135FD2B144");

    TEST_TEXT_BINARY_CONVERSION(
        35, 0,
        "-14893252880957015430391171822782489",
        "7FFD21B4B88705D84E5A0ECF560DF7E7");

    TEST_TEXT_BINARY_CONVERSION(
        35, 0,
        "63834657871331509839301031051834738",
        "800C4B4AA7E81EF6726D934AA12AB572");

    TEST_TEXT_BINARY_CONVERSION(
        35, 0,
        "29973763116566701065133387262473178",
        "8005C5D2141747EF1198888FB96033DA");

    TEST_TEXT_BINARY_CONVERSION(
        38, 0,
        "-10801326773602606474687234309805009788",
        "77DFBD795731FA57AD40A913BEB49484");

    TEST_TEXT_BINARY_CONVERSION(
        38, 0,
        "-20812191361939909169985435346247510799",
        "7057B7BE1F13B6F1D6DB2F569BA994F1");

    TEST_TEXT_BINARY_CONVERSION(
        38, 0,
        "11838425135427849808412411824493534874",
        "88E7FF6C4CB25E5053056568535B9E9A");

    TEST_TEXT_BINARY_CONVERSION(
        38, 0,
        "1640052427868011280598262045053315869",
        "813BDCD3E2BEF11A4624071A88406B1D");

    TEST_TEXT_BINARY_CONVERSION(
        39, 0,
        "-232260256342160733754330365414586850142",
        "7FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF51444D3EF60040D9A177B4EBDCD1E4A2");

    TEST_TEXT_BINARY_CONVERSION(
        39, 0,
        "-940196556981693406088356159514846564823",
        "7FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFD3CACBACF9473427D637FA1697111EE29");

    TEST_TEXT_BINARY_CONVERSION(
        39, 0,
        "662994680443699577738721489513433200379",
        "80000000000000000000000000000001F2C8217C5832413CEDC2D8713542F6FB");

    TEST_TEXT_BINARY_CONVERSION(
        39, 0,
        "176936763201632870831727889579868727743",
        "80000000000000000000000000000000851CC7F2FA54AF2AC8A1868AB524A9BF");

    TEST_TEXT_BINARY_CONVERSION(
        76, 0,
        "-4873471434558122362316658760366909670546688937346706562729806990162720465375",
        "7539B681CC20CEEDAB96C358A49B105EB5C51C958B66D69F946E52B2C4CBDE21");

    TEST_TEXT_BINARY_CONVERSION(
        76, 0,
        "-5646417080531003309232719374529912419049663193149190586518506716572628498776",
        "73843DBE5B00AA3065C94D18C3DE5B54ABBCB7AD4AB9775BD7308BA12A14C6A8");

    TEST_TEXT_BINARY_CONVERSION(
        76, 0,
        "9453147379965075273545494808313678377701436349578856855744431351823374989413",
        "94E64AB40D1C30A9A2BFEA9C96B7584F559D5572641F0C90E86764F3187BB865");

    TEST_TEXT_BINARY_CONVERSION(
        76, 0,
        "4352408240084271094777520471167190229413186999386774964990913341232812067974",
        "899F603224EC624E2351628FF4637023C84FDBB0E2BBCBBB9395D8DD19B30C86");

#undef TEST_TEXT_BINARY_CONVERSION

}

TEST(TDecimal, TestPrecisionScaleLimits)
{
    EXPECT_THROW_WITH_SUBSTRING(TDecimal::TextToBinary("0", -1, 0), "Invalid decimal precision");
    EXPECT_THROW_WITH_SUBSTRING(TDecimal::TextToBinary("0", 0, 0), "Invalid decimal precision");
    EXPECT_THROW_WITH_SUBSTRING(TDecimal::TextToBinary("0", TDecimal::MaxPrecision + 1, 0), "Invalid decimal precision");

    EXPECT_THROW_WITH_SUBSTRING(TDecimal::BinaryToText("0", -1, 0), "Invalid decimal precision");
    EXPECT_THROW_WITH_SUBSTRING(TDecimal::BinaryToText("0", 0, 0), "Invalid decimal precision");
    EXPECT_THROW_WITH_SUBSTRING(TDecimal::BinaryToText("0", TDecimal::MaxPrecision + 1, 0), "Invalid decimal precision");

    CHECK_ROUND_CONVERSION("0", 1, 0);
    CHECK_ROUND_CONVERSION("0", TDecimal::MaxPrecision, 0);


    CHECK_ROUND_CONVERSION("-3.14", 3, 2);
    EXPECT_THROW_WITH_SUBSTRING(TDecimal::BinaryToText("0000", 3, 4), "Invalid decimal scale");

    EXPECT_THROW_WITH_SUBSTRING(TDecimal::TextToBinary("3.1415", 10, 3), "too many digits after decimal point");
    EXPECT_THROW_WITH_SUBSTRING(TDecimal::TextToBinary("-3.1415", 10, 3), "too many digits after decimal point");
    EXPECT_EQ("3.140", RoundConvertText("3.14", 10, 3));
    EXPECT_EQ("-3.140", RoundConvertText("-3.14", 10, 3));

    EXPECT_THROW_WITH_SUBSTRING(TDecimal::TextToBinary("314.15", 5, 3), "too many digits before decimal point");
    EXPECT_THROW_WITH_SUBSTRING(TDecimal::TextToBinary("-314.15", 5, 3), "too many digits before decimal point");

    // This group of tests checks that text values which cause signed overflow throw valid exceptions.
    // The values for each precision are equal to 2^(binary_size - 1) + 0.11.
    EXPECT_THROW_WITH_SUBSTRING(TDecimal::TextToBinary("2147483647.11", 9, 2), "too many digits before decimal point");
    EXPECT_THROW_WITH_SUBSTRING(TDecimal::TextToBinary("9223372036854775807.11", 18, 2), "too many digits before decimal point");
    EXPECT_THROW_WITH_SUBSTRING(TDecimal::TextToBinary("170141183460469231731687303715884105727.11", 35, 2), "too many digits before decimal point");
    EXPECT_THROW_WITH_SUBSTRING(TDecimal::TextToBinary("57896044618658097711785492504343953926634992332820282019728792003956564819967.11", TDecimal::MaxPrecision, 2), "too many digits before decimal point");

    // Sometimes we want to print values that are not representable with given precision
    // (e.g. in error messages we sometimes want to print text value of invalid decimal to explain that it has
    // more digits than allowed by precision).
    //
    // Here we test that extreme values are printed ok.
    auto maxBinaryDecimal = HexDecode("FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFD");
    auto minBinaryDecimal1 = HexDecode("0000000000000000000000000000000000000000000000000000000000000000");
    auto minBinaryDecimal2 = HexDecode("0000000000000000000000000000000000000000000000000000000000000003");
    EXPECT_EQ(TDecimal::MaxBinarySize, std::ssize(maxBinaryDecimal));   // If max TDecimal::MaxBinarySize ever increases
    EXPECT_EQ(TDecimal::MaxBinarySize, std::ssize(minBinaryDecimal1));  // please update this test
    EXPECT_EQ(TDecimal::MaxBinarySize, std::ssize(minBinaryDecimal2));  // with better values.
    EXPECT_EQ("578960446186580977117854925043439539266349923328202820197287920039565648199.65", TDecimal::BinaryToText(maxBinaryDecimal, TDecimal::MaxPrecision, 2));
    EXPECT_EQ("-578960446186580977117854925043439539266349923328202820197287920039565648199.68", TDecimal::BinaryToText(minBinaryDecimal1, TDecimal::MaxPrecision, 2));
    EXPECT_EQ("-578960446186580977117854925043439539266349923328202820197287920039565648199.65", TDecimal::BinaryToText(minBinaryDecimal2, TDecimal::MaxPrecision, 2));
}

TEST(TDecimal, TestValidation)
{
    EXPECT_NO_THROW(TDecimal::ValidateBinaryValue(HexDecode("8000013A"), 3, 2));
    EXPECT_NO_THROW(TDecimal::ValidateBinaryValue(HexDecode("80000000" "0000013A"), 10, 2));
    EXPECT_NO_THROW(TDecimal::ValidateBinaryValue(HexDecode("80000000" "00000000" "00000000" "0000013A"), 35, 2));
    EXPECT_NO_THROW(TDecimal::ValidateBinaryValue(HexDecode("80000000" "00000000" "00000000" "00000000" "00000000" "00000000" "00000000" "0000013A"), 76, 2));
}

class TDecimalWithPrecisionTest
    : public ::testing::Test
    , public ::testing::WithParamInterface<int>
{
public:
    static std::vector<int> GetAllPossiblePrecisions()
    {
        std::vector<int> result;
        for (int i = 1; i <= TDecimal::MaxPrecision; ++i) {
            result.push_back(i);
        }

        YT_VERIFY(result.back() == TDecimal::MaxPrecision);
        return result;
    }

    static TString GetTextNines(int precision)
    {
        return TString(precision, '9');
    }

    static TString GetTextMinusNines(int precision)
    {
        return "-" + GetTextNines(precision);
    }

    static TString GetTextZillion(int precision)
    {
        return "1" + TString(precision, '0');
    }

    static TString GetTextMinusZillion(int precision)
    {
        return "-" + GetTextZillion(precision);
    }
};

INSTANTIATE_TEST_SUITE_P(
    Precisions,
    TDecimalWithPrecisionTest,
    ::testing::ValuesIn(TDecimalWithPrecisionTest::GetAllPossiblePrecisions())
);

TEST_P(TDecimalWithPrecisionTest, TestTextLimits)
{
    auto precision = GetParam();

    CHECK_ROUND_CONVERSION("0", precision, 0);
    CHECK_ROUND_CONVERSION("1", precision, 0);
    CHECK_ROUND_CONVERSION("-1", precision, 0);
    CHECK_ROUND_CONVERSION("2", precision, 0);
    CHECK_ROUND_CONVERSION("-2", precision, 0);
    CHECK_ROUND_CONVERSION("3", precision, 0);
    CHECK_ROUND_CONVERSION("-3", precision, 0);
    CHECK_ROUND_CONVERSION("4", precision, 0);
    CHECK_ROUND_CONVERSION("-4", precision, 0);
    CHECK_ROUND_CONVERSION("5", precision, 0);
    CHECK_ROUND_CONVERSION("-5", precision, 0);
    CHECK_ROUND_CONVERSION("6", precision, 0);
    CHECK_ROUND_CONVERSION("-6", precision, 0);
    CHECK_ROUND_CONVERSION("7", precision, 0);
    CHECK_ROUND_CONVERSION("-7", precision, 0);
    CHECK_ROUND_CONVERSION("8", precision, 0);
    CHECK_ROUND_CONVERSION("-8", precision, 0);
    CHECK_ROUND_CONVERSION("9", precision, 0);
    CHECK_ROUND_CONVERSION("-9", precision, 0);
    CHECK_ROUND_CONVERSION("inf", precision, 0);
    CHECK_ROUND_CONVERSION("-inf", precision, 0);
    CHECK_ROUND_CONVERSION("nan", precision, 0);

    CHECK_ROUND_CONVERSION(GetTextNines(precision), precision, 0);
    CHECK_ROUND_CONVERSION(GetTextMinusNines(precision), precision, 0);

    EXPECT_THROW_WITH_SUBSTRING(TDecimal::TextToBinary(GetTextZillion(precision), precision, 0),
        "too many digits before decimal point");
    EXPECT_THROW_WITH_SUBSTRING(TDecimal::TextToBinary(GetTextMinusZillion(precision), precision, 0),
        "too many digits before decimal point");
}

TEST_P(TDecimalWithPrecisionTest, TestBinaryValidation)
{
    auto precision = GetParam();

#define TO_BINARY_THEN_VALIDATE(text, precision, scale) \
    do {                                                \
        auto binaryValue = TDecimal::TextToBinary(text, precision, scale); \
        EXPECT_NO_THROW(TDecimal::ValidateBinaryValue(binaryValue, precision, scale)); \
    } while (0)

    TO_BINARY_THEN_VALIDATE("0", precision, 0);
    TO_BINARY_THEN_VALIDATE("1", precision, 0);
    TO_BINARY_THEN_VALIDATE("-1", precision, 0);
    TO_BINARY_THEN_VALIDATE("2", precision, 0);
    TO_BINARY_THEN_VALIDATE("-2", precision, 0);
    TO_BINARY_THEN_VALIDATE("3", precision, 0);
    TO_BINARY_THEN_VALIDATE("-3", precision, 0);
    TO_BINARY_THEN_VALIDATE("4", precision, 0);
    TO_BINARY_THEN_VALIDATE("-4", precision, 0);
    TO_BINARY_THEN_VALIDATE("5", precision, 0);
    TO_BINARY_THEN_VALIDATE("-5", precision, 0);
    TO_BINARY_THEN_VALIDATE("6", precision, 0);
    TO_BINARY_THEN_VALIDATE("-6", precision, 0);
    TO_BINARY_THEN_VALIDATE("7", precision, 0);
    TO_BINARY_THEN_VALIDATE("-7", precision, 0);
    TO_BINARY_THEN_VALIDATE("8", precision, 0);
    TO_BINARY_THEN_VALIDATE("-8", precision, 0);
    TO_BINARY_THEN_VALIDATE("9", precision, 0);
    TO_BINARY_THEN_VALIDATE("-9", precision, 0);
    TO_BINARY_THEN_VALIDATE("inf", precision, 0);
    TO_BINARY_THEN_VALIDATE("-inf", precision, 0);
    TO_BINARY_THEN_VALIDATE("nan", precision, 0);

#undef TO_BINARY_THEN_VALIDATE

    auto binaryValueIncrement = [] (void* buffer, int size) {
        auto cur = static_cast<ui8*>(buffer) + size -1;
        for (; cur >= buffer; --cur) {
            if (++(*cur) != 0) {
                break;
            }
        }
    };

    auto binaryValueDecrement = [] (void* buffer, int size) {
        auto cur = static_cast<ui8*>(buffer) + size -1;
        for (; cur >= buffer; --cur) {
            if ((*cur)-- != 0) {
                break;
            }
        }
    };

    TStringBuf binValue;
    char binBuffer[TDecimal::MaxBinarySize];
    // Check 99..9
    binValue = TDecimal::TextToBinary(GetTextNines(precision), precision, 0, binBuffer, sizeof(binBuffer));
    EXPECT_NO_THROW(TDecimal::ValidateBinaryValue(binValue, precision, 0));
    // Check 100..0
    binaryValueIncrement(binBuffer, binValue.size());
    EXPECT_THROW_WITH_SUBSTRING(TDecimal::ValidateBinaryValue(binValue, precision, 0), "does not have enough precision to represent");

    // Check -99..9
    binValue = TDecimal::TextToBinary(GetTextMinusNines(precision), precision, 0, binBuffer, sizeof(binBuffer));
    EXPECT_NO_THROW(TDecimal::ValidateBinaryValue(binValue, precision, 0));
    // Check -100..0
    binaryValueDecrement(binBuffer, binValue.size());
    EXPECT_THROW_WITH_SUBSTRING(TDecimal::ValidateBinaryValue(binValue, precision, 0), "does not have enough precision to represent");
}

////////////////////////////////////////////////////////////////////////////////

} // namespace
} // namespace NYT::NDecimal
