# C++ header-only progress bar library

![progress bar with the COLOR fill mode](https://i.imgur.com/tSRVJiX.gif)

This is a simple progress bar library for C++.

This library is header-only, so you don't need to build it.
It is also OS agnostic, so it should (should being the operating word here) work on any platform.

This library is designed to be simple and easy to use. It is also designed to be easily customizable.

## Usage
There are multiple ways to use this library. Here are a few examples.

Incremented in a for loop
```cpp
#include "progressBar.hpp"

int main()
{
#define N 1000
    ProgressBar<int> bar(0, N);
    for (int i = 0; i <= N; i++, bar++)
    {
        // Do something N times
    }
    return 0;
}
```

Printed manually
```cpp
#include "progressBar.hpp"

int main()
{
#define N 1000
    ProgressBar<int> bar(0, N);
    for (int i = 0; i <= N; i++)
    {
        std::cout << bar(i) << std::endl;

        // Do something N times
    }
    return 0;
}
```

This way you can also choose to not update the bar each iteration which can save on performances.
```cpp
#include "progressBar.hpp"

int main()
{
#define N 1000
    ProgressBar<int> bar(0, N);
    for (int i = 0; i <= N; i++)
    {
        if (i % 10 == 0)
            std::cout << bar(i) << std::endl;

        // Do something N times
    }
    return 0;
}
```

## Customization
You can customize the progress bar by changing the `ProgressBar` arguments
    
```cpp
ProgressBar<int> bar(0, N, 50, ProgressBarFill::EQUAL, true, true);
```

The templated argument is the type of value you want to use for the progress bar. Typically this would be your iteration counter. This value needs to be arithmetic (it needs to support `+`, `-`, `*`, `/` and `%`) and be able to be cast to a `float`.

The templated argument will be referred to as `T` in the following list of arguments.

List of arguments:
- `T` `min`: The minimum value of the progress bar. This is the value that will be considered as 0%. 
- `T` `max`: The maximum value of the progress bar. This is the value that will be considered as 100%. 
- `int` `width`: The width of the progress bar. This is the number of characters that will be printed. This value is -1 by default, this means that the progress bar will be as wide as the terminal.
- `ProgressBarFill` `fill`: The fill of the progress bar. The list of fills will be explained right after this list. This value is `ProgressBarFill::EQUAL` by default.
- `bool` `showPercentage`: Whether or not to show the percentage of the progress bar. This value is true by default.
- `bool` `showValue`: Whether or not to show the value of the progress bar. This value is true by default.

It is also important to note that there are two types of loading bar classes, `ProgressBar` and `ProgressBarUnicode` which are the same but the latter uses unicode characters to draw the progress bar, this does make it somewhat slower and it changes your terminal encoding to UTF-8.

List of fills for the ASCII progress bar:
- `ProgressBarFill::EQUAL`: This fill will fill the progress bar with the equal character. This is the default fill. 
  
  It looks like this: `[===>    ]`. 
- `ProgressBarFill::BLOCK`: This fill will fill the progress bar with # signs. 
  
  It looks like this: `[####    ]`.
- `ProgressBarFill::DASH`: This fill will fill the progress bar with dashes. 
  
  It looks like this: `[--->    ]`.
  - `ProgressBarFill::DOT`: This fill will fill the progress bar with dots.

  It looks like this: `[....    ]`.

List of fills for the Unicode progress bar:
- `ProgressBarFill::SHADE_BLOCK`: This fill will fill the progress bar with shade blocks.
  
  It looks like this: `|███▒   |`. (this may not look right if your markdown viewer's monospace font doesn't support these characters)

- `ProgressBarFill::FILL_BLOCK`: This fill will fill the progress bar with blocks and the last one with a partially filled block. this gives a smoother look to the progress bar.
  
  It looks like this: `|███▌   |`. (this may not look right if your markdown viewer's monospace font doesn't support these characters)

- `ProgressBarFill::COLOR`: This fill will fill the progress bar with a colored line. This is the only fill that is colored and it requires the terminal to support ANSI escape codes. 

  It looks like this: ` ━━━━━━━━ `. (but colored)

## Installation
This library is header-only, so you don't need to build it. You can simply copy the `progressBar.hpp` file into your project and include it.

## License
This library is licensed under the GNU GPLv3 license. You can find the full license in the `LICENSE` file.

## Demo
progress bar with the `ProgressBarFill::SHADE_BLOCK` fill mode
![progress bar with the SHADE_BLOCK fill mode](https://i.imgur.com/j9dJe1i.gif)

progress bar with the `ProgressBarFill::FILL_BLOCK` fill mode
![progress bar with the FILL_BLOCK fill mode](https://i.imgur.com/iA5mUjM.gif)

progress bar with the `ProgressBarFill::COLOR` fill mode
![progress bar with the COLOR fill mode](https://i.imgur.com/tSRVJiX.gif)

progress bar with the `ProgressBarFill::EQUAL` fill mode
![progress bar with the EQUAL fill mode](https://i.imgur.com/qTRt0Ai.gif)