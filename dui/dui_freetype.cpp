// dear imgui: FreeType font builder (used as a replacement for the stb_truetype builder)
// (code)

// Get the latest version at https://github.com/ocornut/imgui/tree/master/misc/freetype
// Original code by @vuhdo (Aleksei Skriabin). Improvements by @mikesart. Maintained since 2019 by @ocornut.

// CHANGELOG
// (minor and older changes stripped away, please see git history for details)
//  2023/08/01: added support for SVG fonts, enable by using '#define IMGUI_ENABLE_FREETYPE_LUNASVG' (#6591)
//  2023/01/04: fixed a packing issue which in some occurrences would prevent large amount of glyphs from being packed correctly.
//  2021/08/23: fixed crash when FT_Render_Glyph() fails to render a glyph and returns NULL.
//  2021/03/05: added ImGuiFreeTypeBuilderFlags_Bitmap to load bitmap glyphs.
//  2021/03/02: set 'atlas->TexPixelsUseColors = true' to help some backends with deciding of a prefered texture format.
//  2021/01/28: added support for color-layered glyphs via ImGuiFreeTypeBuilderFlags_LoadColor (require Freetype 2.10+).
//  2021/01/26: simplified integration by using '#define IMGUI_ENABLE_FREETYPE'. renamed ImGuiFreeType::XXX flags to ImGuiFreeTypeBuilderFlags_XXX for consistency with other API. removed ImGuiFreeType::BuildFontAtlas().
//  2020/06/04: fix for rare case where FT_Get_Char_Index() succeed but FT_Load_Glyph() fails.
//  2019/02/09: added RasterizerFlags::Monochrome flag to disable font anti-aliasing (combine with ::MonoHinting for best results!)
//  2019/01/15: added support for imgui allocators + added FreeType only override function SetAllocatorFunctions().
//  2019/01/10: re-factored to match big update in STB builder. fixed texture height waste. fixed redundant glyphs when merging. support for glyph padding.
//  2018/06/08: added support for ImFontConfig::GlyphMinAdvanceX, GlyphMaxAdvanceX.
//  2018/02/04: moved to main imgui repository (away from http://www.github.com/ocornut/imgui_club)
//  2018/01/22: fix for addition of ImFontAtlas::TexUvscale member.
//  2017/10/22: minor inconsequential change to match change in master (removed an unnecessary statement).
//  2017/09/26: fixes for imgui internal changes.
//  2017/08/26: cleanup, optimizations, support for ImFontConfig::RasterizerFlags, ImFontConfig::RasterizerMultiply.
//  2017/08/16: imported from https://github.com/Vuhdo/imgui_freetype into http://www.github.com/ocornut/imgui_club, updated for latest changes in ImFontAtlas, minor tweaks.

// About Gamma Correct Blending:
// - FreeType assumes blending in linear space rather than gamma space.
// - See https://www.freetype.org/freetype2/docs/reference/ft2-base_interface.html#FT_Render_Glyph
// - For correct results you need to be using sRGB and convert to linear space in the pixel shader output.
// - The default dear imgui styles will be impacted by this change (alpha values will need tweaking).

// FIXME: cfg.OversampleH, OversampleV are not supported (but perhaps not so necessary with this rasterizer).

#include <stdint.h>

#include "freetype/ft2build.h"
#include FT_FREETYPE_H          // <freetype/freetype.h>
#include FT_MODULE_H            // <freetype/ftmodapi.h>
#include FT_GLYPH_H             // <freetype/ftglyph.h>
#include FT_SYNTHESIS_H         // <freetype/ftsynth.h>

#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable: 4505)     // unreferenced local function has been removed (stb stuff)
#pragma warning (disable: 26812)    // [Static Analyzer] The enum type 'xxx' is unscoped. Prefer 'enum class' over 'enum' (Enum.3).
#endif

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpragmas"                  // warning: unknown option after '#pragma GCC diagnostic' kind
#pragma GCC diagnostic ignored "-Wunused-function"          // warning: 'xxxx' defined but not used
#pragma GCC diagnostic ignored "-Wsubobject-linkage"        // warning: 'xxxx' has a field 'xxxx' whose type uses the anonymous namespace
#endif


    // Glyph metrics:
    // --------------
    //
    //                       xmin                     xmax
    //                        |                         |
    //                        |<-------- width -------->|
    //                        |                         |
    //              |         +-------------------------+----------------- ymax
    //              |         |    ggggggggg   ggggg    |     ^        ^
    //              |         |   g:::::::::ggg::::g    |     |        |
    //              |         |  g:::::::::::::::::g    |     |        |
    //              |         | g::::::ggggg::::::gg    |     |        |
    //              |         | g:::::g     g:::::g     |     |        |
    //    offsetX  -|-------->| g:::::g     g:::::g     |  offsetY     |
    //              |         | g:::::g     g:::::g     |     |        |
    //              |         | g::::::g    g:::::g     |     |        |
    //              |         | g:::::::ggggg:::::g     |     |        |
    //              |         |  g::::::::::::::::g     |     |      height
    //              |         |   gg::::::::::::::g     |     |        |
    //  baseline ---*---------|---- gggggggg::::::g-----*--------      |
    //            / |         |             g:::::g     |              |
    //     origin   |         | gggggg      g:::::g     |              |
    //              |         | g:::::gg   gg:::::g     |              |
    //              |         |  g::::::ggg:::::::g     |              |
    //              |         |   gg:::::::::::::g      |              |
    //              |         |     ggg::::::ggg        |              |
    //              |         |         gggggg          |              v
    //              |         +-------------------------+----------------- ymin
    //              |                                   |
    //              |------------- advanceX ----------->|

    // A structure that describe a glyph.
    struct GlyphInfo
    {
        int         Width;              // Glyph's width in pixels.
        int         Height;             // Glyph's height in pixels.
        FT_Int      OffsetX;            // The distance from the origin ("pen position") to the left of the glyph.
        FT_Int      OffsetY;            // The distance from the origin to the top of the glyph. This is usually a value < 0.
        float       AdvanceX;           // The distance from the origin to the origin of the next glyph. This is usually a value > 0.
        bool        IsColored;          // The glyph is colored
    };

    // Font parameters and metrics.
    struct FontInfo
    {
        uint32_t    PixelHeight;        // Size this font was generated with.
        float       Ascender;           // The pixel extents above the baseline in pixels (typically positive).
        float       Descender;          // The extents below the baseline in pixels (typically negative).
        float       LineSpacing;        // The baseline-to-baseline distance. Note that it usually is larger than the sum of the ascender and descender taken as absolute values. There is also no guarantee that no glyphs extend above or below subsequent baselines when using this distance. Think of it as a value the designer of the font finds appropriate.
        float       LineGap;            // The spacing in pixels between one row's descent and the next row's ascent.
        float       MaxAdvanceWidth;    // This field gives the maximum horizontal cursor advance for all glyphs in the font.
    };

    // From SDL_ttf: Handy routines for converting from fixed point
    #define FT_CEIL(X)  (((X + 63) & -64) / 64)
