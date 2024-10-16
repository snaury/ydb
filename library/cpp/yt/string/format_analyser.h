#pragma once

#include "format_arg.h"

#include <util/generic/strbuf.h>

#include <array>
#include <string_view>

namespace NYT::NDetail {

////////////////////////////////////////////////////////////////////////////////

struct TFormatAnalyser
{
public:
    // TODO(arkady-e1ppa): Until clang-19 consteval functions
    // defined out of line produce symbols in rare cases
    // causing linker to crash.
    template <class... TArgs>
    static consteval void ValidateFormat(std::string_view fmt)
    {
        DoValidateFormat<TArgs...>(fmt);
    }

private:
    static consteval bool Contains(std::string_view sv, char symbol)
    {
        return sv.find(symbol) != std::string_view::npos;
    }

    struct TSpecifiers
    {
        std::string_view Conversion;
        std::string_view Flags;
    };

    template <class TArg>
    static consteval auto GetSpecifiers()
    {
        return TSpecifiers{
            .Conversion = std::string_view{
                std::data(TFormatArg<TArg>::ConversionSpecifiers),
                std::size(TFormatArg<TArg>::ConversionSpecifiers)},
            .Flags = std::string_view{
                std::data(TFormatArg<TArg>::FlagSpecifiers),
                std::size(TFormatArg<TArg>::FlagSpecifiers)},
        };
    }

    static constexpr char IntroductorySymbol = '%';

    template <class... TArgs>
    static consteval void DoValidateFormat(std::string_view format)
    {
        std::array<std::string_view, sizeof...(TArgs)> markers = {};
        std::array<TSpecifiers, sizeof...(TArgs)> specifiers{GetSpecifiers<TArgs>()...};

        int markerCount = 0;
        int currentMarkerStart = -1;

        for (int index = 0; index < std::ssize(format); ++index) {
            auto symbol = format[index];

            // Parse verbatim text.
            if (currentMarkerStart == -1) {
                if (symbol == IntroductorySymbol) {
                    // Marker maybe begins.
                    currentMarkerStart = index;
                }
                continue;
            }

            // NB: We check for %% first since
            // in order to verify if symbol is a specifier
            // we need markerCount to be within range of our
            // specifier array.
            if (symbol == IntroductorySymbol) {
                if (currentMarkerStart + 1 != index) {
                    // '%a% detected'
                    throw "You may not terminate flag sequence other than %% with \'%\' symbol";
                }
                // '%%' detected --- skip
                currentMarkerStart = -1;
                continue;
            }

            // We are inside of marker.
            if (markerCount == std::ssize(markers)) {
                // Too many markers
                throw "Number of arguments supplied to format is smaller than the number of flag sequences";
            }

            if (Contains(specifiers[markerCount].Conversion, symbol)) {
                // Marker has finished.

                markers[markerCount]
                    = std::string_view(format.begin() + currentMarkerStart, index - currentMarkerStart + 1);
                currentMarkerStart = -1;
                ++markerCount;

                continue;
            }

            if (!Contains(specifiers[markerCount].Flags, symbol)) {
                throw "Symbol is not a valid flag specifier; See FlagSpecifiers";
            }
        }

        if (currentMarkerStart != -1) {
            // Runaway marker.
            throw "Unterminated flag sequence detected; Use \'%%\' to type plain %";
            return;
        }

        if (markerCount < std::ssize(markers)) {
            // Missing markers.
            throw "Number of arguments supplied to format is greater than the number of flag sequences";
        }

        // TODO(arkady-e1ppa): Consider per-type verification
        // of markers.
    }
};

////////////////////////////////////////////////////////////////////////////////

} // namespace NYT::NDetail
