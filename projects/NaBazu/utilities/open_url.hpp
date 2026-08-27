#ifndef NABAZU_OPEN_URL_HPP
#define NABAZU_OPEN_URL_HPP

#include <string>

#if defined(_WIN32)
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
    #include <shellapi.h>
#else
    #include <cstdlib>
#endif

namespace NaBazu
{
    // Opens a URL in the user's default browser. The engine has no hyperlink concept
    // (it renders text into a GL context), so "clickable link" means handing the URL
    // to the OS shell and letting it launch the browser over the game window.
    inline void OpenUrlInBrowser(const std::string &url)
    {
        if (url.empty())
        {
            return;
        }

#if defined(_WIN32)
        // ShellExecute rather than system("start ..."): no console window flashes up
        // and the URL is not re-parsed by a command interpreter.
        ShellExecuteA(nullptr, "open", url.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
#elif defined(__APPLE__)
        std::system(("open \"" + url + "\"").c_str());
#else
        std::system(("xdg-open \"" + url + "\" &").c_str());
#endif
    }
}

#endif
