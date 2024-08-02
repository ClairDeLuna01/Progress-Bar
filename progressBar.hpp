#include <cmath>
#include <cstring>
#include <fcntl.h>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <string>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/ioctl.h>
#include <unistd.h>
#endif

enum class ProgressBarFill
{
    // ASCII
    BLOCK, // [#####    ]
    DASH,  // [---->    ]
    DOT,   // [.....    ]
    EQUAL, // [====>    ]

    // Unicode
    SHADE_BLOCK, // [░▒▓█    ]
    FILL_BLOCK,  // [▏▎▍▌▋▊▉█]
    COLOR,       //  ━━━━━━━━━━━━━━
};

namespace progressBarUtils
{
int getTerminalWidth()
{
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    int columns;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    return columns;
#else
    struct winsize size;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
    return size.ws_col;
#endif
}

} // namespace progressBarUtils

#define PROGRESS_BAR_WIDTH_AUTO -1

template <typename T, std::enable_if_t<std::is_arithmetic_v<T>, int> = 0> class ProgressBar
{
  protected:
    T minVal;
    T maxVal;
    T currentValue;
    int barWidth;
    ProgressBarFill fill;
    int terminalWidth;
    int totalWidth;

    void *iterable = nullptr;

    bool showPercentage;
    bool showValue;
    int showValueWidth;
    int showValueFieldWidth;
    int showPercentageWidth = 6;

    int getShowValueWidth()
    {
        if (showValue)
        {
            return std::to_string(maxVal).length();
        }
        return 0;
    }

  public:
    ProgressBar(T minVal, T maxVal, int _barWidth = -1, ProgressBarFill fill = ProgressBarFill::EQUAL,
                bool showPercentage = true, bool showValue = true, bool _isDerived = false)
        : minVal(minVal), maxVal(maxVal), barWidth(_barWidth), fill(fill), showPercentage(showPercentage),
          showValue(showValue)
    {
        terminalWidth = progressBarUtils::getTerminalWidth();
        showValueWidth = getShowValueWidth();
        showValueFieldWidth = showValueWidth * 2 + 1;

        currentValue = minVal;

        if (barWidth == -1)
        {
            barWidth = std::min(terminalWidth - (showPercentage ? showPercentageWidth : 0) -
                                    (showValue ? showValueFieldWidth : 0) - 3,
                                maxVal);
        }

        totalWidth = barWidth + (showPercentage ? showPercentageWidth : 0) + (showValue ? showValueFieldWidth : 0) + 3;

        if (!_isDerived && (fill == ProgressBarFill::SHADE_BLOCK || fill == ProgressBarFill::FILL_BLOCK ||
                            fill == ProgressBarFill::COLOR))
        {
            std::cerr << "Warning: this class is not suitable for unicode characters. Use ProgressBarUnicode instead."
                      << std::endl;
        }
    }

    std::string printProgress(T progressRaw)
    {
        float progress = (float)(progressRaw - minVal) / (float)(maxVal - minVal);

        float posFloat = progress * barWidth;
        int pos = static_cast<int>(posFloat);
        std::string bar(totalWidth + 1, ' ');
        bar[0] = '\r';

        switch (fill)
        {
        case ProgressBarFill::BLOCK:
            bar[1] = '[';
            memset(&bar[2], '#', pos);
            bar[barWidth + 1] = ']';
            break;
        case ProgressBarFill::DASH:
            bar[1] = '[';
            memset(&bar[2], '-', pos);
            bar[pos + 1] = '>';
            bar[barWidth + 1] = ']';
            break;
        case ProgressBarFill::DOT:
            bar[1] = '[';
            memset(&bar[2], '.', pos);
            bar[barWidth + 1] = ']';
            break;
        case ProgressBarFill::EQUAL:
            bar[1] = '[';
            memset(&bar[2], '=', pos);
            bar[pos + 1] = '>';
            bar[barWidth + 1] = ']';
            break;
        default:
            break;
        }

        if (showPercentage)
        {
            sprintf(&bar[barWidth + 2], " %4d%%", (int)(progress * 100));
        }

        if (showValue)
        {
            sprintf(&bar[barWidth + (showPercentage ? 6 : 0) + 2], " %*d/%d", (int)std::to_string(maxVal).length(),
                    progressRaw, maxVal);
        }

        return bar;
    }

    virtual void operator()(T progressRaw)
    {
        std::cout << printProgress(progressRaw) << std::flush;
    }

    // prefix
    void operator++()
    {
        (*this)(currentValue++);
    }

    // postfix
    void operator++(int) // dummy int argument to differentiate from prefix
    {
        (*this)(currentValue++);
    }
};

template <typename T, std::enable_if_t<std::is_arithmetic_v<T>, int> = 0>
class ProgressBarUnicode : public ProgressBar<T>
{
  private:
    static constexpr wchar_t shadeBlock[] = L" ░▒▓█";
    static constexpr int shadeBlockLength = 4;
    static constexpr wchar_t fillBlock[] = L" ▏▎▍▌▋▊▉█";
    static constexpr int fillBlockLength = 8;

  public:
    ProgressBarUnicode(T minVal, T maxVal, int _barWidth = -1, ProgressBarFill fill = ProgressBarFill::SHADE_BLOCK,
                       bool showPercentage = true, bool showValue = true)
        : ProgressBar<T>(minVal, maxVal, _barWidth, fill, showPercentage, showValue, true)
    {
        if (fill != ProgressBarFill::SHADE_BLOCK && fill != ProgressBarFill::FILL_BLOCK &&
            fill != ProgressBarFill::COLOR)
        {
            std::cerr << "Warning: this class is suitable for unicode characters only. Use ProgressBar instead."
                      << std::endl;
        }

        setlocale(LC_ALL, "");
#ifdef _WIN32
        _setmode(_fileno(stdout), _O_U16TEXT);
#endif
    }

    std::wstring printProgress(T progressRaw)
    {
        float progress = (float)(progressRaw - this->minVal) / (float)(this->maxVal - this->minVal);

        float posFloat = progress * this->barWidth;
        int pos = static_cast<int>(posFloat);
        std::wstringstream ss;

        ss << L"\r";
        switch (this->fill)
        {
        case ProgressBarFill::SHADE_BLOCK: {
            ss << L"|";
            for (int i = 0; i < pos; i++)
            {
                ss << shadeBlock[shadeBlockLength];
            }

            if (pos != this->barWidth)
            {
                float segmentProgress = (posFloat - pos);
                wchar_t symbol = shadeBlock[(int)(segmentProgress * shadeBlockLength)];
                ss << symbol;
            }

            for (int i = pos + 1; i < this->barWidth; i++)
            {
                ss << shadeBlock[0];
            }

            ss << L"|";
            break;
        }
        case ProgressBarFill::FILL_BLOCK: {
            ss << L"|";
            for (int i = 0; i < pos; i++)
            {
                ss << fillBlock[fillBlockLength];
            }

            // horrible bad fix
            if (pos != this->barWidth)
            {
                float segmentProgress = (posFloat - pos);
                wchar_t symbol = fillBlock[(int)(segmentProgress * fillBlockLength)];
                ss << symbol;
            }

            for (int i = pos + 1; i < this->barWidth; i++)
            {
                ss << fillBlock[0];
            }

            ss << L"|";
            break;
        }
        case ProgressBarFill::COLOR:
            if (pos != this->barWidth)
            {
                ss << L" \x1b[38;2;255;89;895m" << std::wstring(pos, L'━') << L"\33[38;2;11;11;11m" << L'━'
                   << L"\x1b[38;2;50;50;50m" << std::wstring(this->barWidth - pos - 1, L'━') << L" \33[0m ";
            }
            else
            {
                ss << L" \x1b[38;2;114;156;31m" << std::wstring(this->barWidth, L'━') << L" \33[0m ";
            }
        default:
            break;
        }

        if (this->showPercentage)
        {
            ss << L" " << std::setw(3) << std::setfill(L' ') << (int)(progress * 100) << L"%";
        }

        if (this->showValue)
        {
            ss << L" " << std::setw(this->showValueWidth) << std::setfill(L' ') << progressRaw << L"/" << this->maxVal;
        }

        return ss.str();
    }

    void operator()(T progressRaw)
    {
        std::wcout << printProgress(progressRaw) << std::flush;
    }
};
