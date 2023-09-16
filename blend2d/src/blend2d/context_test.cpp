// This file is part of Blend2D project <https://blend2d.com>
//
// See blend2d.h or LICENSE.md for license and copyright information
// SPDX-License-Identifier: Zlib

#include "api-build_test_p.h"
#if defined(BL_TEST)

#include "context_p.h"
#include "gradient_p.h"
#include "image_p.h"
#include "pattern_p.h"

// BLContext - Tests
// =================

namespace BLContextTests {

static void test_context_state(BLContext& ctx) {
  // Prepare non-solid styles.
  BLGradient gradient;
  EXPECT_SUCCESS(gradient.create(BLLinearGradientValues(0.0, 0.0, 0.0, 100.0)));
  EXPECT_SUCCESS(gradient.addStop(0.0, BLRgba32(0x00000000u)));
  EXPECT_SUCCESS(gradient.addStop(1.0, BLRgba32(0xFF0000FFu)));

  BLImage image(16, 16, BL_FORMAT_PRGB32);
  BLPattern pattern(image);

  INFO("Testing state management of global composition options");
  {
    EXPECT_SUCCESS(ctx.setCompOp(BL_COMP_OP_SRC_ATOP));
    EXPECT_SUCCESS(ctx.setGlobalAlpha(0.5));

    EXPECT_EQ(ctx.compOp(), BL_COMP_OP_SRC_ATOP);
    EXPECT_EQ(ctx.globalAlpha(), 0.5);

    EXPECT_SUCCESS(ctx.save());
    EXPECT_SUCCESS(ctx.setCompOp(BL_COMP_OP_MULTIPLY));
    EXPECT_SUCCESS(ctx.setGlobalAlpha(0.9));
    EXPECT_EQ(ctx.compOp(), BL_COMP_OP_MULTIPLY);
    EXPECT_EQ(ctx.globalAlpha(), 0.9);
    EXPECT_SUCCESS(ctx.restore());

    EXPECT_EQ(ctx.compOp(), BL_COMP_OP_SRC_ATOP);
    EXPECT_EQ(ctx.globalAlpha(), 0.5);

    EXPECT_SUCCESS(ctx.setCompOp(BL_COMP_OP_SRC_OVER));
    EXPECT_SUCCESS(ctx.setGlobalAlpha(1.0));
  }

  INFO("Testing state management of global, fill, and stroke alpha values");
  {
    EXPECT_SUCCESS(ctx.setGlobalAlpha(0.5));
    EXPECT_SUCCESS(ctx.setFillAlpha(0.3));
    EXPECT_SUCCESS(ctx.setStrokeAlpha(0.4));

    EXPECT_EQ(ctx.globalAlpha(), 0.5);
    EXPECT_EQ(ctx.fillAlpha(), 0.3);
    EXPECT_EQ(ctx.strokeAlpha(), 0.4);

    EXPECT_SUCCESS(ctx.save());

    EXPECT_SUCCESS(ctx.setGlobalAlpha(1.0));
    EXPECT_SUCCESS(ctx.setFillAlpha(0.7));
    EXPECT_SUCCESS(ctx.setStrokeAlpha(0.8));

    EXPECT_EQ(ctx.globalAlpha(), 1.0);
    EXPECT_EQ(ctx.fillAlpha(), 0.7);
    EXPECT_EQ(ctx.strokeAlpha(), 0.8);

    EXPECT_SUCCESS(ctx.restore());

    EXPECT_EQ(ctx.globalAlpha(), 0.5);
    EXPECT_EQ(ctx.fillAlpha(), 0.3);
    EXPECT_EQ(ctx.strokeAlpha(), 0.4);

    EXPECT_SUCCESS(ctx.setGlobalAlpha(0.1));
    EXPECT_SUCCESS(ctx.setFillAlpha(1.0));
    EXPECT_SUCCESS(ctx.setStrokeAlpha(0.9));

    EXPECT_EQ(ctx.globalAlpha(), 0.1);
    EXPECT_EQ(ctx.fillAlpha(), 1.0);
    EXPECT_EQ(ctx.strokeAlpha(), 0.9);
  }

  INFO("Testing state management of styles (Rgba, Rgba32, Rgba64)");
  {
    BLRgba32 rgba32;
    BLRgba64 rgba64;

    // Initial style.
    BLRgba fillColor(0.1f, 0.2f, 0.3f, 0.4f);
    BLRgba strokeColor(0.5f, 0.6f, 0.7f, 0.8f);

    EXPECT_SUCCESS(ctx.setFillStyle(fillColor));
    EXPECT_SUCCESS(ctx.setStrokeStyle(strokeColor));

    BLVar fillStyleVar;
    BLVar strokeStyleVar;

    EXPECT_SUCCESS(ctx.getFillStyle(fillStyleVar));
    EXPECT_SUCCESS(ctx.getStrokeStyle(strokeStyleVar));

    EXPECT_TRUE(fillStyleVar.isRgba());
    EXPECT_TRUE(strokeStyleVar.isRgba());

    EXPECT_EQ(fillStyleVar.as<BLRgba>(), fillColor);
    EXPECT_EQ(strokeStyleVar.as<BLRgba>(), strokeColor);

    // Save and change the style.
    {
      EXPECT_SUCCESS(ctx.save());
      BLRgba newFillColor(0.9f, 0.8f, 0.7f, 0.6f);
      BLRgba newStrokeColor(0.7f, 0.6f, 0.5f, 0.4f);

      EXPECT_SUCCESS(ctx.setFillStyle(newFillColor));
      EXPECT_SUCCESS(ctx.setStrokeStyle(newStrokeColor));

      EXPECT_SUCCESS(ctx.getFillStyle(fillStyleVar));
      EXPECT_SUCCESS(ctx.getStrokeStyle(strokeStyleVar));

      EXPECT_TRUE(fillStyleVar.isRgba());
      EXPECT_TRUE(strokeStyleVar.isRgba());

      EXPECT_EQ(fillStyleVar.as<BLRgba>(), newFillColor);
      EXPECT_EQ(strokeStyleVar.as<BLRgba>(), newStrokeColor);

      EXPECT_SUCCESS(ctx.setFillStyle(BLRgba32(0xFFEEDDCCu)));
      EXPECT_SUCCESS(ctx.setStrokeStyle(BLRgba64(0x9999AAAABBBBCCCCu)));

      EXPECT_SUCCESS(ctx.getFillStyle(fillStyleVar));
      EXPECT_SUCCESS(ctx.getStrokeStyle(strokeStyleVar));

      EXPECT_TRUE(fillStyleVar.isRgba32());
      EXPECT_TRUE(strokeStyleVar.isRgba64());

      EXPECT_SUCCESS(fillStyleVar.toRgba32(&rgba32));
      EXPECT_SUCCESS(strokeStyleVar.toRgba64(&rgba64));

      EXPECT_EQ(rgba32, BLRgba32(0xFFEEDDCCu));
      EXPECT_EQ(rgba64, BLRgba64(0x9999AAAABBBBCCCCu));
      EXPECT_SUCCESS(ctx.restore());
    }

    // Now we should observe the initial style that was active before save().
    EXPECT_SUCCESS(ctx.getFillStyle(fillStyleVar));
    EXPECT_SUCCESS(ctx.getStrokeStyle(strokeStyleVar));

    EXPECT_TRUE(fillStyleVar.isRgba());
    EXPECT_TRUE(strokeStyleVar.isRgba());

    EXPECT_EQ(fillStyleVar.as<BLRgba>(), fillColor);
    EXPECT_EQ(strokeStyleVar.as<BLRgba>(), strokeColor);
  }

  INFO("Testing state management of styles (Rgba32, Gradient)");
  {
    BLRgba32 rgba32;

    BLVar fillStyleVar;
    BLVar strokeStyleVar;

    // Initial style.
    EXPECT_SUCCESS(ctx.setFillStyle(gradient));
    EXPECT_SUCCESS(ctx.setStrokeStyle(BLRgba32(0x44332211u)));

    EXPECT_SUCCESS(ctx.getFillStyle(fillStyleVar));
    EXPECT_SUCCESS(ctx.getStrokeStyle(strokeStyleVar));

    EXPECT_TRUE(fillStyleVar.isGradient());
    EXPECT_TRUE(strokeStyleVar.isRgba32());

    EXPECT_EQ(fillStyleVar.as<BLGradient>(), gradient);
    EXPECT_SUCCESS(strokeStyleVar.toRgba32(&rgba32));
    EXPECT_EQ(rgba32, BLRgba32(0x44332211u));

    // Save and change the style.
    {
      EXPECT_SUCCESS(ctx.save());
      EXPECT_SUCCESS(ctx.setFillStyle(BLRgba32(0xFFFFFFFFu)));
      EXPECT_SUCCESS(ctx.setStrokeStyle(BLRgba32(0x00000000u)));

      EXPECT_SUCCESS(ctx.getFillStyle(fillStyleVar));
      EXPECT_SUCCESS(ctx.getStrokeStyle(strokeStyleVar));

      EXPECT_TRUE(fillStyleVar.isRgba32());
      EXPECT_TRUE(strokeStyleVar.isRgba32());

      EXPECT_SUCCESS(fillStyleVar.toRgba32(&rgba32));
      EXPECT_EQ(rgba32, BLRgba32(0xFFFFFFFFu));

      EXPECT_SUCCESS(strokeStyleVar.toRgba32(&rgba32));
      EXPECT_EQ(rgba32, BLRgba32(0x00000000u));
      EXPECT_SUCCESS(ctx.restore());
    }

    // Now we should observe the initial style that was active before save().
    EXPECT_SUCCESS(ctx.getFillStyle(fillStyleVar));
    EXPECT_SUCCESS(ctx.getStrokeStyle(strokeStyleVar));

    EXPECT_TRUE(fillStyleVar.isGradient());
    EXPECT_TRUE(strokeStyleVar.isRgba32());

    EXPECT_EQ(fillStyleVar.as<BLGradient>(), gradient);
    EXPECT_SUCCESS(strokeStyleVar.toRgba32(&rgba32));
    EXPECT_EQ(rgba32, BLRgba32(0x44332211u));
  }

  INFO("Testing fill and stroke style swapping (Rgba32)");
  {
    BLRgba32 a(0xFF000000u);
    BLRgba32 b(0xFFFFFFFFu);

    double alphaA = 0.5;
    double alphaB = 0.7;

    BLVar va;
    BLVar vb;

    BLRgba32 vaRgba32;
    BLRgba32 vbRgba32;

    EXPECT_SUCCESS(ctx.setFillStyle(a));
    EXPECT_SUCCESS(ctx.setStrokeStyle(b));

    ctx.setFillAlpha(alphaA);
    ctx.setStrokeAlpha(alphaB);

    // Swap styles twice - first time to swap, second time to revert the swap.
    for (uint32_t i = 0; i < 2; i++) {
      for (uint32_t j = 0; j < 2; j++) {
        BLContextStyleSwapMode mode = BLContextStyleSwapMode(j);

        EXPECT_SUCCESS(ctx.swapStyles(mode));
        EXPECT_SUCCESS(ctx.getFillStyle(va));
        EXPECT_SUCCESS(ctx.getStrokeStyle(vb));

        EXPECT_TRUE(va.isRgba32());
        EXPECT_TRUE(vb.isRgba32());
        EXPECT_SUCCESS(va.toRgba32(&vaRgba32));
        EXPECT_SUCCESS(vb.toRgba32(&vbRgba32));

        std::swap(a, b);
        EXPECT_EQ(a, vaRgba32);
        EXPECT_EQ(b, vbRgba32);

        if (mode == BL_CONTEXT_STYLE_SWAP_MODE_STYLES_WITH_ALPHA) {
          std::swap(alphaA, alphaB);
          EXPECT_EQ(ctx.fillAlpha(), alphaA);
          EXPECT_EQ(ctx.strokeAlpha(), alphaB);
        }
      }
    }
  }

  INFO("Testing fill and stroke style swapping (Gradient, Pattern)");
  {
    BLVar va;
    BLVar vb;

    EXPECT_SUCCESS(ctx.setFillStyle(gradient));
    EXPECT_SUCCESS(ctx.setStrokeStyle(pattern));

    // First swap.
    EXPECT_SUCCESS(ctx.swapStyles(BL_CONTEXT_STYLE_SWAP_MODE_STYLES));
    EXPECT_SUCCESS(ctx.getFillStyle(va));
    EXPECT_SUCCESS(ctx.getStrokeStyle(vb));

    EXPECT_TRUE(va.isPattern());
    EXPECT_TRUE(vb.isGradient());

    EXPECT_EQ(va.as<BLPattern>(), pattern);
    EXPECT_EQ(vb.as<BLGradient>(), gradient);

    // Second swap.
    EXPECT_SUCCESS(ctx.swapStyles(BL_CONTEXT_STYLE_SWAP_MODE_STYLES));
    EXPECT_SUCCESS(ctx.getFillStyle(va));
    EXPECT_SUCCESS(ctx.getStrokeStyle(vb));

    EXPECT_TRUE(va.isGradient());
    EXPECT_TRUE(vb.isPattern());

    EXPECT_EQ(va.as<BLGradient>(), gradient);
    EXPECT_EQ(vb.as<BLPattern>(), pattern);
  }

  INFO("Testing state management of fill options");
  {
    BLFillRule initialFillRule = ctx.fillRule();

    EXPECT_SUCCESS(ctx.save());

    EXPECT_SUCCESS(ctx.setFillRule(BL_FILL_RULE_EVEN_ODD));
    EXPECT_EQ(ctx.fillRule(), BL_FILL_RULE_EVEN_ODD);

    EXPECT_SUCCESS(ctx.setFillRule(BL_FILL_RULE_NON_ZERO));
    EXPECT_EQ(ctx.fillRule(), BL_FILL_RULE_NON_ZERO);

    EXPECT_SUCCESS(ctx.setFillRule(
      initialFillRule == BL_FILL_RULE_NON_ZERO
        ? BL_FILL_RULE_EVEN_ODD
        : BL_FILL_RULE_NON_ZERO));
    EXPECT_SUCCESS(ctx.restore());

    EXPECT_EQ(ctx.fillRule(), initialFillRule);
  }

  INFO("Testing state management of stroke options");
  {
    BLArray<double> dashes;
    dashes.append(1.0, 2.0, 3.0, 4.0);

    EXPECT_SUCCESS(ctx.save());

    EXPECT_SUCCESS(ctx.setStrokeWidth(2.0));
    EXPECT_EQ(ctx.strokeWidth(), 2.0);

    EXPECT_SUCCESS(ctx.setStrokeMiterLimit(10.0));
    EXPECT_EQ(ctx.strokeMiterLimit(), 10.0);

    EXPECT_SUCCESS(ctx.setStrokeJoin(BL_STROKE_JOIN_ROUND));
    EXPECT_EQ(ctx.strokeJoin(), BL_STROKE_JOIN_ROUND);

    EXPECT_SUCCESS(ctx.setStrokeStartCap(BL_STROKE_CAP_ROUND_REV));
    EXPECT_EQ(ctx.strokeStartCap(), BL_STROKE_CAP_ROUND_REV);

    EXPECT_SUCCESS(ctx.setStrokeEndCap(BL_STROKE_CAP_TRIANGLE_REV));
    EXPECT_EQ(ctx.strokeEndCap(), BL_STROKE_CAP_TRIANGLE_REV);

    EXPECT_SUCCESS(ctx.setStrokeDashArray(dashes));
    EXPECT_EQ(ctx.strokeDashArray(), dashes);

    EXPECT_SUCCESS(ctx.setStrokeDashOffset(5.0));
    EXPECT_EQ(ctx.strokeDashOffset(), 5.0);

    BLStrokeOptions opt = ctx.strokeOptions();
    EXPECT_EQ(opt, ctx.strokeOptions());
    EXPECT_EQ(opt.width, 2.0);
    EXPECT_EQ(opt.miterLimit, 10.0);
    EXPECT_EQ(BLStrokeJoin(opt.join), BL_STROKE_JOIN_ROUND);
    EXPECT_EQ(BLStrokeCap(opt.caps[BL_STROKE_CAP_POSITION_START]), BL_STROKE_CAP_ROUND_REV);
    EXPECT_EQ(BLStrokeCap(opt.caps[BL_STROKE_CAP_POSITION_END]), BL_STROKE_CAP_TRIANGLE_REV);
    EXPECT_EQ(opt.dashArray, dashes);
    EXPECT_EQ(opt.dashOffset, 5.0);

    EXPECT_SUCCESS(ctx.restore());

    EXPECT_SUCCESS(ctx.save());
    EXPECT_SUCCESS(ctx.setStrokeOptions(opt));
    EXPECT_EQ(ctx.strokeWidth(), 2.0);
    EXPECT_EQ(ctx.strokeMiterLimit(), 10.0);
    EXPECT_EQ(ctx.strokeJoin(), BL_STROKE_JOIN_ROUND);
    EXPECT_EQ(ctx.strokeStartCap(), BL_STROKE_CAP_ROUND_REV);
    EXPECT_EQ(ctx.strokeEndCap(), BL_STROKE_CAP_TRIANGLE_REV);
    EXPECT_EQ(ctx.strokeDashArray(), dashes);
    EXPECT_EQ(ctx.strokeDashOffset(), 5.0);

    BLStrokeOptions opt2 = ctx.strokeOptions();
    EXPECT_EQ(opt, opt2);

    EXPECT_SUCCESS(ctx.restore());
  }

  INFO("Testing state management of transformations");
  {
    BLMatrix2D transform = BLMatrix2D::makeScaling(2.0);
    EXPECT_SUCCESS(ctx.applyTransform(transform));
    EXPECT_EQ(ctx.userTransform(), transform);
    EXPECT_EQ(ctx.metaTransform(), BLMatrix2D::makeIdentity());

    EXPECT_SUCCESS(ctx.save());
    EXPECT_SUCCESS(ctx.userToMeta());
    EXPECT_EQ(ctx.metaTransform(), transform);
    EXPECT_EQ(ctx.userTransform(), BLMatrix2D::makeIdentity());
    EXPECT_SUCCESS(ctx.restore());

    EXPECT_EQ(ctx.userTransform(), transform);
    EXPECT_EQ(ctx.metaTransform(), BLMatrix2D::makeIdentity());

    EXPECT_SUCCESS(ctx.resetTransform());
    EXPECT_EQ(ctx.userTransform(), BLMatrix2D::makeIdentity());
  }
}

UNIT(context, BL_TEST_GROUP_RENDERING_CONTEXT) {
  BLImage img(256, 256, BL_FORMAT_PRGB32);
  BLContext ctx(img);

  test_context_state(ctx);
}

} // {BLContextTests}

#endif // BL_TEST
