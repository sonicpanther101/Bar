// build (example, adjust paths as needed):
// g++ bar_layershell.cpp -o bar_layershell \
//   `pkg-config --cflags --libs wayland-client egl glesv2`
//
// You must also generate and compile the wlr-layer-shell protocol headers:
// wayland-scanner client-header \
//   wlr-layer-shell-unstable-v1.xml wlr-layer-shell-unstable-v1-client-protocol.h
// wayland-scanner private-code \
//   wlr-layer-shell-unstable-v1.xml wlr-layer-shell-unstable-v1-protocol.c
//
// Then add wlr-layer-shell-unstable-v1-protocol.c to your build.

#include <wayland-client.h>
#include "wlr-layer-shell-unstable-v1-client-protocol.h"
#include <wayland-egl.h>

#include <EGL/egl.h>
#include <GLES2/gl2.h>

#include <cstring>
#include <cstdio>
#include <cstdlib>

static wl_display*             g_display        = nullptr;
static wl_compositor*          g_compositor     = nullptr;
static wl_registry*            g_registry       = nullptr;
static wl_output*              g_output         = nullptr; // primary output (first seen)
static zwlr_layer_shell_v1*    g_layer_shell    = nullptr;
static wl_surface*             g_surface        = nullptr;
static zwlr_layer_surface_v1*  g_layer_surface  = nullptr;

static EGLDisplay              g_egl_display    = EGL_NO_DISPLAY;
static EGLContext              g_egl_context    = EGL_NO_CONTEXT;
static EGLSurface              g_egl_surface    = EGL_NO_SURFACE;

static int                     g_width          = 1920;
static int                     g_height         = 24;
static bool                    g_running        = true;
static bool                    g_configured     = false;

// ---- Registry ----

static void registry_global(
    void* data, wl_registry* registry,
    uint32_t name, const char* interface, uint32_t version)
{
    if (strcmp(interface, wl_compositor_interface.name) == 0) {
        g_compositor = (wl_compositor*)wl_registry_bind(
            registry, name, &wl_compositor_interface, 4);
    } else if (strcmp(interface, wl_output_interface.name) == 0) {
        if (!g_output) {
            g_output = (wl_output*)wl_registry_bind(
                registry, name, &wl_output_interface, 1);
        }
    } else if (strcmp(interface, zwlr_layer_shell_v1_interface.name) == 0) {
        g_layer_shell = (zwlr_layer_shell_v1*)wl_registry_bind(
            registry, name, &zwlr_layer_shell_v1_interface, 4);
    }
}

static void registry_global_remove(void* data, wl_registry* registry, uint32_t name) {
    (void)data;
    (void)registry;
    (void)name;
}

static const wl_registry_listener g_registry_listener = {
    registry_global,
    registry_global_remove
};

// ---- Layer surface callbacks ----

static void layer_surface_configure(
    void* data,
    zwlr_layer_surface_v1* surface,
    uint32_t serial,
    uint32_t width,
    uint32_t height)
{
    (void)data;
    (void)surface;

    // compositor suggests a size; we respect height and let width be full
    if (width > 0)  g_width  = (int)width;
    if (height > 0) g_height = (int)height;

    zwlr_layer_surface_v1_ack_configure(surface, serial);
    g_configured = true;
}

static void layer_surface_closed(void* data, zwlr_layer_surface_v1* surface)
{
    (void)data;
    (void)surface;
    g_running = false;
}

static const zwlr_layer_surface_v1_listener g_layer_surface_listener = {
    layer_surface_configure,
    layer_surface_closed
};

// ---- EGL setup ----

static bool init_egl()
{
    g_egl_display = eglGetDisplay((EGLNativeDisplayType)g_display);
    if (g_egl_display == EGL_NO_DISPLAY) {
        std::fprintf(stderr, "eglGetDisplay failed\n");
        return false;
    }

    if (!eglInitialize(g_egl_display, nullptr, nullptr)) {
        std::fprintf(stderr, "eglInitialize failed\n");
        return false;
    }

    static const EGLint config_attribs[] = {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_RED_SIZE,   8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE,  8,
        EGL_ALPHA_SIZE, 8,
        EGL_NONE
    };

    EGLConfig config;
    EGLint num_configs = 0;
    if (!eglChooseConfig(g_egl_display, config_attribs, &config, 1, &num_configs) || num_configs == 0) {
        std::fprintf(stderr, "eglChooseConfig failed\n");
        return false;
    }

    static const EGLint context_attribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE
    };

    g_egl_context = eglCreateContext(g_egl_display, config, EGL_NO_CONTEXT, context_attribs);
    if (g_egl_context == EGL_NO_CONTEXT) {
        std::fprintf(stderr, "eglCreateContext failed\n");
        return false;
    }

    // EGL window surface from wl_surface via wl_egl_window
    // This requires <wayland-egl.h> and libwayland-egl.
    // If not available, you can switch to SHM instead of EGL.
    struct wl_egl_window* egl_window =
        wl_egl_window_create(g_surface, g_width, g_height);
    if (!egl_window) {
        std::fprintf(stderr, "wl_egl_window_create failed\n");
        return false;
    }

    g_egl_surface = eglCreateWindowSurface(g_egl_display, config,
                                           (EGLNativeWindowType)egl_window, nullptr);
    if (g_egl_surface == EGL_NO_SURFACE) {
        std::fprintf(stderr, "eglCreateWindowSurface failed\n");
        return false;
    }

    if (!eglMakeCurrent(g_egl_display, g_egl_surface, g_egl_surface, g_egl_context)) {
        std::fprintf(stderr, "eglMakeCurrent failed\n");
        return false;
    }

    return true;
}

// ---- Main ----

int main()
{
    g_display = wl_display_connect(nullptr);
    if (!g_display) {
        std::fprintf(stderr, "Failed to connect to Wayland display\n");
        return 1;
    }

    g_registry = wl_display_get_registry(g_display);
    wl_registry_add_listener(g_registry, &g_registry_listener, nullptr);

    // Roundtrips so globals are advertised
    wl_display_roundtrip(g_display);
    wl_display_roundtrip(g_display);

    if (!g_compositor || !g_layer_shell) {
        std::fprintf(stderr, "Missing compositor or layer_shell globals\n");
        return 1;
    }

    g_surface = wl_compositor_create_surface(g_compositor);
    if (!g_surface) {
        std::fprintf(stderr, "Failed to create wl_surface\n");
        return 1;
    }

    g_layer_surface = zwlr_layer_shell_v1_get_layer_surface(
        g_layer_shell,
        g_surface,
        g_output,                               // can be nullptr for “default” output
        ZWLR_LAYER_SHELL_V1_LAYER_TOP,
        "test-bar"
    );
    if (!g_layer_surface) {
        std::fprintf(stderr, "Failed to create layer_surface\n");
        return 1;
    }

    zwlr_layer_surface_v1_add_listener(g_layer_surface, &g_layer_surface_listener, nullptr);

    // anchor to top, stretch horizontally
    zwlr_layer_surface_v1_set_anchor(
        g_layer_surface,
        ZWLR_LAYER_SURFACE_V1_ANCHOR_TOP |
        ZWLR_LAYER_SURFACE_V1_ANCHOR_LEFT |
        ZWLR_LAYER_SURFACE_V1_ANCHOR_RIGHT
    );

    // Request full width, fixed height
    zwlr_layer_surface_v1_set_size(g_layer_surface, 0, g_height);

    // Reserve exclusive zone equal to bar height
    zwlr_layer_surface_v1_set_exclusive_zone(g_layer_surface, g_height);

    wl_surface_commit(g_surface);
    wl_display_roundtrip(g_display);

    // Wait for configure so we know real size
    while (!g_configured && wl_display_dispatch(g_display) != -1) {
        // just wait
    }

    if (!init_egl()) {
        std::fprintf(stderr, "EGL init failed\n");
        return 1;
    }

    // Main loop: simple clear + swap
    while (g_running && wl_display_dispatch(g_display) != -1) {
        eglMakeCurrent(g_egl_display, g_egl_surface, g_egl_surface, g_egl_context);

        glViewport(0, 0, g_width, g_height);
        glClearColor(0.1f, 0.2f, 0.4f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        eglSwapBuffers(g_egl_display, g_egl_surface);
    }

    // TODO: clean up EGL, wl_egl_window, Wayland objects

    return 0;
}