// main.cpp -- MarkAmp entry point
// Include full headers for types used in MarkAmpApp's unique_ptr members,
// because wxIMPLEMENT_APP_NO_MAIN triggers the implicit default constructor.
#include "app/MarkAmpApp.h"
#include "core/AppState.h"
#include "core/Command.h"
#include "core/Config.h"
#include "core/EventBus.h"
#include "core/ThemeEngine.h"
#include "core/ThemeRegistry.h"
#include "platform/PlatformAbstraction.h"

#include <wx/wx.h>

wxIMPLEMENT_APP_NO_MAIN(markamp::app::MarkAmpApp);

auto main(int argc, char* argv[]) -> int
{
    return wxEntry(argc, argv);
}
