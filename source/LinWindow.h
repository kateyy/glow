#pragma once

#ifdef __linux__

#include <set>
#include <string>
#include <X11/Xlib.h>

#include <glow/glow.h>
#include <glow/KeyEvent.h>

#include "AbstractNativeWindow.h"


namespace glow
{

class WindowEventHandler;
class ContextFormat;
class Context;

class LinWindow : public AbstractNativeWindow
{
    struct Rect
    {
        int left;
        int top;
        unsigned int width;
        unsigned int height;
    };

public:
    LinWindow(Window & window);
    virtual ~LinWindow();

    virtual bool create(
        const ContextFormat & format
    ,   const std::string & title
    ,   const unsigned int width
    ,   const unsigned int height);

    virtual void close();
    virtual void destroy();

    virtual int width() const;
    virtual int height() const;

    virtual int handle() const;

    virtual void repaint();

    virtual void show();
    virtual void hide();

    virtual void fullScreen();
    virtual void windowed();

public:
    static int run();
    static void quit(int code = 0);

protected:

//    // design similar to:
//    // http://www.codeproject.com/Articles/2556/A-Simple-Win32-Window-Wrapper-Class
//    // http://members.gamedev.net/sicrane/articles/WindowClass.html

//    static LRESULT CALLBACK InitialProc(
//        HWND hWnd
//    ,   UINT message
//    ,   WPARAM wParam
//    ,   LPARAM lParam);

//    static LRESULT CALLBACK Proc(
//        HWND hWnd       ///< Unique handle of the window. Check this against own handle.
//    ,   UINT message
//    ,   WPARAM wParam
//    ,   LPARAM lParam);

//    LRESULT CALLBACK dispatch(
//        HWND hWnd
//    ,   UINT message
//    ,   WPARAM wParam
//    ,   LPARAM lParam);

//    static void restoreDisplaySettings();
//    static void printChangeDisplaySettingsErrorResult(const LONG result);

protected:
    static std::set<LinWindow*> s_windows;

protected:
    void onRepaint();
    void onResize(
        const int width
    ,   const int height);

    void onDestroy();

protected:
    Display * m_display;
    ::Window m_hWnd;

    Rect m_rect;
    Rect m_backup;
};

} // namespace glow

#endif
