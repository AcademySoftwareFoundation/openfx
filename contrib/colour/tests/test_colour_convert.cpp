// Copyright OpenFX and contributors to the OpenFX project.
// SPDX-License-Identifier: BSD-3-Clause
//
// Unit tests for contrib/colour/ofxColourConvert.h, validated against OpenColorIO.
//
// For every colourspace the header supports, we build an OCIO processor to and
// from ACES2065-1 using OCIO's built-in studio config, run a set of sample
// pixels through both OCIO and the header, and require them to agree.
//
// OCIO is the reference: the header's matrices and transfer-function constants
// were taken from the same OCIO ACES transforms, so agreement here is the
// strongest correctness check we can make short of shipping OCIO itself.

#include "ofxColourConvert.h"

#include <OpenColorIO/OpenColorIO.h>
#include <gtest/gtest.h>

#include <array>
#include <cmath>
#include <string>
#include <vector>

namespace OCIO = OCIO_NAMESPACE;
using ofx::colour::Colourspace;
using ofx::colour::RGB;

namespace {

// The OCIO built-in config the OFX native colourspaces are based on. Falls
// back to the "latest" studio alias if this exact name isn't registered in
// the linked OCIO version.
OCIO::ConstConfigRcPtr studioConfig()
{
    static OCIO::ConstConfigRcPtr cfg = [] {
        const char *names[] = {
            "studio-config-v2.1.0_aces-v1.3_ocio-v2.3",
            "studio-config-latest",
        };
        for (const char *n : names) {
            try {
                return OCIO::Config::CreateFromBuiltinConfig(n);
            } catch (const std::exception &) {
                // try the next candidate
            }
        }
        return OCIO::ConstConfigRcPtr();
    }();
    return cfg;
}

// Apply an OCIO colourspace conversion to a single RGB triple.
RGB ocioConvert(const char *src, const char *dst, RGB in)
{
    auto cfg = studioConfig();
    auto proc = cfg->getProcessor(src, dst);
    auto cpu = proc->getDefaultCPUProcessor();
    float pix[3] = { float(in.r), float(in.g), float(in.b) };
    cpu->applyRGB(pix);
    return { pix[0], pix[1], pix[2] };
}

struct Case {
    const char *name;     // OCIO / OFX colourspace identifier
    Colourspace cs;       // matching header enum
    double tolAbs;        // absolute tolerance (dominates near zero)
    double tolRel;        // relative tolerance (dominates for large values;
                          //   log decodes can reach 10s-100s in scene-linear)
    bool clampLow;        // OCIO clamps this space's ->AP0 output to [0, inf)
};

// Spaces that reduce to a matrix and an analytic curve match OCIO's analytic
// built-in transforms to float32 round-off. A few are evaluated through
// interpolated LUTs / baked CLF matrices in this OCIO config (ACEScc and the
// Canon logs are LUTs; Blackmagic/DaVinci come from CLF files rather than
// analytic built-ins), so they need more slack. The header's analytic version
// is in fact the more accurate of the two; the point is that it tracks OCIO.
constexpr double kAbsAnalytic = 1e-4, kRelAnalytic = 5e-4;
constexpr double kAbsLut      = 5e-3, kRelLut      = 5e-3;

Case analytic(const char *n, Colourspace cs)
{
    return { n, cs, kAbsAnalytic, kRelAnalytic, false };
}
Case lut(const char *n, Colourspace cs, bool clampLow = false)
{
    return { n, cs, kAbsLut, kRelLut, clampLow };
}

const std::vector<Case> &cases()
{
    static const std::vector<Case> c = {
        analytic("ACEScg", Colourspace::ACEScg),
        lut("ACEScc", Colourspace::ACEScc, /*clampLow=*/true),
        analytic("ACEScct", Colourspace::ACEScct),
        analytic("lin_p3d65", Colourspace::lin_p3d65),
        analytic("lin_rec2020", Colourspace::lin_rec2020),
        analytic("lin_rec709_srgb", Colourspace::lin_rec709_srgb),
        analytic("g18_rec709_tx", Colourspace::g18_rec709_tx),
        analytic("g22_rec709_tx", Colourspace::g22_rec709_tx),
        analytic("g24_rec709_tx", Colourspace::g24_rec709_tx),
        analytic("g22_ap1_tx", Colourspace::g22_ap1_tx),
        analytic("srgb_tx", Colourspace::srgb_tx),
        analytic("srgb_encoded_ap1_tx", Colourspace::srgb_encoded_ap1_tx),
        analytic("srgb_encoded_p3d65_tx", Colourspace::srgb_encoded_p3d65_tx),
        analytic("camera_rec709", Colourspace::camera_rec709),
        analytic("lin_arri_wide_gamut_3", Colourspace::lin_arri_wide_gamut_3),
        analytic("arri_logc3_ei800", Colourspace::arri_logc3_ei800),
        analytic("lin_arri_wide_gamut_4", Colourspace::lin_arri_wide_gamut_4),
        analytic("arri_logc4", Colourspace::arri_logc4),
        // Blackmagic / DaVinci come from CLF files in this config, not analytic
        // built-ins, so both the matrix and the curve carry LUT-level error.
        lut("lin_bmd_widegamut_gen5", Colourspace::lin_bmd_widegamut_gen5),
        lut("bmdfilm_widegamut_gen5", Colourspace::bmdfilm_widegamut_gen5),
        lut("davinci_intermediate_widegamut", Colourspace::davinci_intermediate_widegamut),
        lut("lin_davinci_widegamut", Colourspace::lin_davinci_widegamut),
        analytic("lin_cinemagamut_d55", Colourspace::lin_cinemagamut_d55),
        lut("canonlog2_cinemagamut_d55", Colourspace::canonlog2_cinemagamut_d55),
        lut("canonlog3_cinemagamut_d55", Colourspace::canonlog3_cinemagamut_d55),
        analytic("lin_vgamut", Colourspace::lin_vgamut),
        analytic("vlog_vgamut", Colourspace::vlog_vgamut),
        analytic("lin_redwidegamutrgb", Colourspace::lin_redwidegamutrgb),
        analytic("log3g10_redwidegamutrgb", Colourspace::log3g10_redwidegamutrgb),
        analytic("lin_sgamut3", Colourspace::lin_sgamut3),
        analytic("slog3_sgamut3", Colourspace::slog3_sgamut3),
        analytic("lin_sgamut3cine", Colourspace::lin_sgamut3cine),
        analytic("slog3_sgamut3cine", Colourspace::slog3_sgamut3cine),
        analytic("lin_venice_sgamut3", Colourspace::lin_venice_sgamut3),
        analytic("slog3_venice_sgamut3", Colourspace::slog3_venice_sgamut3),
        analytic("lin_venice_sgamut3cine", Colourspace::lin_venice_sgamut3cine),
        analytic("slog3_venice_sgamut3cine", Colourspace::slog3_venice_sgamut3cine),
    };
    return c;
}

// Compare with a combined absolute+relative tolerance. EXPECT_NEAR with a flat
// absolute tolerance is meaningless when scene-linear values reach 10s-100s.
::testing::AssertionResult close(double mine, double ref, const Case &c,
                                 const char *chan, double in)
{
    const double tol = c.tolAbs + c.tolRel * std::fabs(ref);
    if (std::fabs(mine - ref) <= tol)
        return ::testing::AssertionSuccess();
    return ::testing::AssertionFailure()
           << c.name << " " << chan << " in=" << in << ": mine=" << mine
           << " ref=" << ref << " |d|=" << std::fabs(mine - ref) << " tol=" << tol;
}

// Representative pixels in [0,1]. This range is meaningful both as code values
// for the encoded spaces and as scene-linear values for the linear spaces, and
// stays inside every gamut/curve's well-defined region (no negatives, which
// would invite gamut-clip differences with OCIO).
// Source-space values for the space -> AP0 direction. Spans the endpoints
// (black and white), the near-endpoint toe/shoulder, transfer-function break
// regions (sRGB ~0.04 / 0.0031, log toes), mid-grey, and saturated colours.
// All channels are in [0,1], which is a valid code value for the encoded
// spaces and a valid scene-linear value for the linear spaces.
const std::array<RGB, 14> kSamples = { {
    { 0.00, 0.00, 0.00 },   // black endpoint
    { 1.00, 1.00, 1.00 },   // white endpoint
    { 0.18, 0.18, 0.18 },   // 18% mid grey
    { 0.50, 0.50, 0.50 },
    { 0.01, 0.01, 0.01 },   // near-black / log toe
    { 0.99, 0.99, 0.99 },   // near-white / shoulder
    { 0.0031, 0.0045, 0.0065 }, // around the sRGB / linear break
    { 0.04, 0.02, 0.08 },   // small, mixed (sRGB knee region)
    { 0.45, 0.55, 0.65 },
    { 0.75, 0.25, 0.10 },   // saturated
    { 0.10, 0.60, 0.90 },   // saturated
    { 0.92, 0.85, 0.40 },
    { 0.33, 0.66, 0.99 },
    { 0.05, 0.05, 0.05 },
} };

// For the AP0 -> space direction we use near-neutral AP0 values that stay
// in-gamut (non-negative linear) for the narrowest target gamut (Rec.709),
// spanning deep shadow, mid-grey, diffuse white, and HDR highlights well
// above 1.0. A saturated AP0 colour can map to negative linear RGB in a
// smaller gamut, and there the display/texture *encode* of a negative value
// is undefined and handled differently by every implementation (OCIO clamps,
// mirrors, or linear-extends). That regime isn't meaningful to compare; the
// round-trip test below still exercises saturated values.
const std::array<RGB, 12> kAcesSamples = { {
    { 0.002, 0.002, 0.002 }, // deep shadow / toe
    { 0.05, 0.05, 0.05 },
    { 0.18, 0.18, 0.18 },    // 18% mid grey
    { 0.50, 0.50, 0.50 },
    { 1.00, 1.00, 1.00 },    // diffuse white
    { 2.00, 2.00, 2.00 },    // HDR highlight (> 1)
    { 4.00, 3.80, 3.60 },    // brighter HDR, gentle warm tint
    { 0.98, 0.96, 0.94 },    // near-white gentle tint
    { 0.40, 0.45, 0.50 },    // gentle cool tint
    { 0.55, 0.50, 0.45 },    // gentle warm tint
    { 0.30, 0.32, 0.35 },
    { 0.80, 0.78, 0.74 },
} };

class ColourspaceTest : public ::testing::TestWithParam<Case> {};

TEST_P(ColourspaceTest, ToACES2065_1MatchesOCIO)
{
    const Case &c = GetParam();
    ASSERT_TRUE(studioConfig()) << "OCIO built-in studio config not available";
    if (!studioConfig()->getColorSpace(c.name))
        GTEST_SKIP() << c.name << " not present in this OCIO config";

    for (const RGB &s : kSamples) {
        const RGB ref = ocioConvert(c.name, "ACES2065-1", s);
        RGB mine = ofx::colour::toACES2065_1(s, c.cs);
        if (c.clampLow) {  // match OCIO's [0, inf) output clamp (e.g. ACEScc)
            mine.r = std::max(mine.r, 0.0);
            mine.g = std::max(mine.g, 0.0);
            mine.b = std::max(mine.b, 0.0);
        }
        EXPECT_TRUE(close(mine.r, ref.r, c, "R", s.r));
        EXPECT_TRUE(close(mine.g, ref.g, c, "G", s.g));
        EXPECT_TRUE(close(mine.b, ref.b, c, "B", s.b));
    }
}

TEST_P(ColourspaceTest, FromACES2065_1MatchesOCIO)
{
    const Case &c = GetParam();
    ASSERT_TRUE(studioConfig()) << "OCIO built-in studio config not available";
    if (!studioConfig()->getColorSpace(c.name))
        GTEST_SKIP() << c.name << " not present in this OCIO config";

    for (const RGB &s : kAcesSamples) {
        const RGB ref  = ocioConvert("ACES2065-1", c.name, s);
        const RGB mine = ofx::colour::fromACES2065_1(s, c.cs);
        // Safety net: skip any channel that still landed non-positive, where
        // the transfer-function encode is implementation-defined.
        auto chk = [&](double m, double r, const char *ch, double in) {
            if (m <= 0.0 || r <= 0.0) return;
            EXPECT_TRUE(close(m, r, c, ch, in));
        };
        chk(mine.r, ref.r, "R", s.r);
        chk(mine.g, ref.g, "G", s.g);
        chk(mine.b, ref.b, "B", s.b);
    }
}

INSTANTIATE_TEST_SUITE_P(
    AllColourspaces, ColourspaceTest, ::testing::ValuesIn(cases()),
    [](const ::testing::TestParamInfo<Case> &info) {
        std::string n = info.param.name;
        for (char &ch : n)
            if (!std::isalnum(static_cast<unsigned char>(ch))) ch = '_';
        return n;
    });

// A couple of header-only invariants that don't need OCIO.

TEST(SelfConsistency, RoundTripsThroughACES)
{
    for (std::size_t i = 0; i < std::size_t(Colourspace::Count); ++i) {
        const auto cs = Colourspace(i);
        for (const RGB &s : kSamples) {
            const RGB aces = ofx::colour::toACES2065_1(s, cs);
            const RGB back = ofx::colour::fromACES2065_1(aces, cs);
            EXPECT_NEAR(back.r, s.r, 1e-9);
            EXPECT_NEAR(back.g, s.g, 1e-9);
            EXPECT_NEAR(back.b, s.b, 1e-9);
        }
    }
}

TEST(SelfConsistency, ACES2065_1IsIdentity)
{
    for (const RGB &s : kSamples) {
        const RGB a = ofx::colour::toACES2065_1(s, Colourspace::ACES2065_1);
        EXPECT_DOUBLE_EQ(a.r, s.r);
        EXPECT_DOUBLE_EQ(a.g, s.g);
        EXPECT_DOUBLE_EQ(a.b, s.b);
    }
}

TEST(NameLookup, MapsOfxIdentifiers)
{
    Colourspace cs;
    EXPECT_TRUE(ofx::colour::colourspaceFromName("slog3_sgamut3", cs));
    EXPECT_EQ(cs, Colourspace::slog3_sgamut3);
    EXPECT_FALSE(ofx::colour::colourspaceFromName("does_not_exist", cs));
}

} // namespace

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
