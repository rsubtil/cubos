#include <cubos/engine/plugins/window.hpp>
#include <cubos/core/settings.hpp>

static void startup(cubos::core::io::Window& window, ShouldQuit& quit, const cubos::core::Settings& settings)
{
    quit.value = false;
    window = cubos::core::io::openWindow(
        settings.getString("window.title", "Cubos"),
        {settings.getInteger("window.width", 800), settings.getInteger("window.height", 600)});
}

static void pollSystem(const cubos::core::io::Window& window, ShouldQuit& quit)
{
    window->pollEvents();
    if (window->shouldClose())
    {
        quit.value = true;
    }
}

static void swapBuffersSystem(const cubos::core::io::Window& window)
{
    window->swapBuffers();
}

void cubos::engine::plugins::windowPlugin(Cubos& cubos)
{
    cubos
        .addResource<cubos::core::io::Window>()

        .addStartupSystem(startup, "OpenWindow")

        .addSystem(pollSystem, "Poll")
        .addSystem(swapBuffersSystem, "SwapBuffers");
}
