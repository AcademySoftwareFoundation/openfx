#ifndef _ofxColourConvert_h_
#define _ofxColourConvert_h_

// Copyright OpenFX and contributors to the OpenFX project.
// SPDX-License-Identifier: BSD-3-Clause

/** @file ofxColourConvert.h

A small, dependency-free, header-only C++17 utility for converting RGB
triplets between the OFX "native" colourspaces (see ofxColour.h and the
config header ofx-native-v1.5_aces-v1.3_ocio-v2.3.h) and the ACES2065-1
(AP0) reference colourspace.

This is a convenience for plug-in and host authors who want simple
colour conversions without a full OCIO dependency. It is NOT a
replacement for OCIO: it does not implement display rendering (the ACES
Output Transform / RRT+ODT), so the display-referred colourspaces
(the @c *_display spaces), the ADX film-density encodings, and the
abstract "basic" @c ofx_* spaces are intentionally out of scope. What is
implemented are the scene-referred working spaces and the camera/texture
encodings, each of which reduces exactly to:

    code value --(transfer function)--> linear RGB --(3x3 matrix)--> AP0

and back. Every conversion here is colorimetrically exact and round-trips
to ACES2065-1.

@par Derivation
The gamut matrices are derived at compile time from the published xy
chromaticities of each colourspace using the Normalized Primaries Matrix
plus a von Kries chromatic adaptation to the ACES white point, exactly as
done by OpenColorIO's built-in transforms. The adaptation method (Bradford
or CAT02) and the transfer-function constants match the OCIO ACES config
that the OFX native colourspaces are based on, so results agree with OCIO
to within floating-point precision.

@par Usage
@code
    using namespace ofx::colour;
    RGB c = { 0.2, 0.5, 0.8 };                       // an S-Log3/S-Gamut3 pixel
    RGB aces = toACES2065_1(c, Colourspace::slog3_sgamut3);
    RGB out  = fromACES2065_1(aces, Colourspace::lin_rec709_srgb);
    // or directly:
    RGB out2 = convert(c, Colourspace::slog3_sgamut3, Colourspace::lin_rec709_srgb);
@endcode
*/

#if !defined(__cplusplus) || __cplusplus < 201703L
#  error "ofxColourConvert.h requires C++17 or later"
#endif

#include <array>
#include <cmath>
#include <cstring>

namespace ofx {
namespace colour {

// ---------------------------------------------------------------------------
// Small linear-algebra core (constexpr: matrices are built at compile time).
// Vectors are column vectors; a Matrix33 M acts as v' = M * v.
// ---------------------------------------------------------------------------

/** @brief An RGB (or XYZ) triplet. */
struct RGB { double r, g, b; };

/** @brief A 3x3 matrix in row-major order. */
struct Matrix33 { double m[3][3]; };

/** @brief Matrix * column-vector. */
constexpr RGB operator*(const Matrix33 &a, const RGB &v)
{
    return {
        a.m[0][0] * v.r + a.m[0][1] * v.g + a.m[0][2] * v.b,
        a.m[1][0] * v.r + a.m[1][1] * v.g + a.m[1][2] * v.b,
        a.m[2][0] * v.r + a.m[2][1] * v.g + a.m[2][2] * v.b
    };
}

/** @brief Matrix * matrix. */
constexpr Matrix33 operator*(const Matrix33 &a, const Matrix33 &b)
{
    Matrix33 r{};
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            r.m[i][j] = a.m[i][0] * b.m[0][j]
                      + a.m[i][1] * b.m[1][j]
                      + a.m[i][2] * b.m[2][j];
    return r;
}

/** @brief Inverse of a 3x3 matrix (assumes the matrix is non-singular). */
constexpr Matrix33 inverse(const Matrix33 &a)
{
    const double det =
          a.m[0][0] * (a.m[1][1] * a.m[2][2] - a.m[1][2] * a.m[2][1])
        - a.m[0][1] * (a.m[1][0] * a.m[2][2] - a.m[1][2] * a.m[2][0])
        + a.m[0][2] * (a.m[1][0] * a.m[2][1] - a.m[1][1] * a.m[2][0]);
    const double inv = 1.0 / det;
    Matrix33 r{};
    r.m[0][0] = (a.m[1][1] * a.m[2][2] - a.m[1][2] * a.m[2][1]) * inv;
    r.m[0][1] = (a.m[0][2] * a.m[2][1] - a.m[0][1] * a.m[2][2]) * inv;
    r.m[0][2] = (a.m[0][1] * a.m[1][2] - a.m[0][2] * a.m[1][1]) * inv;
    r.m[1][0] = (a.m[1][2] * a.m[2][0] - a.m[1][0] * a.m[2][2]) * inv;
    r.m[1][1] = (a.m[0][0] * a.m[2][2] - a.m[0][2] * a.m[2][0]) * inv;
    r.m[1][2] = (a.m[0][2] * a.m[1][0] - a.m[0][0] * a.m[1][2]) * inv;
    r.m[2][0] = (a.m[1][0] * a.m[2][1] - a.m[1][1] * a.m[2][0]) * inv;
    r.m[2][1] = (a.m[0][1] * a.m[2][0] - a.m[0][0] * a.m[2][1]) * inv;
    r.m[2][2] = (a.m[0][0] * a.m[1][1] - a.m[0][1] * a.m[1][0]) * inv;
    return r;
}

constexpr Matrix33 kIdentity33 = { { {1,0,0}, {0,1,0}, {0,0,1} } };

// ---------------------------------------------------------------------------
// Chromaticities and matrix derivation (NPM + von Kries adaptation).
// ---------------------------------------------------------------------------

/** @brief CIE xy chromaticities of three primaries and a white point. */
struct Primaries {
    double rx, ry;
    double gx, gy;
    double bx, by;
    double wx, wy;
};

/** @brief Chromatic adaptation transform used when white points differ. */
enum class Adaptation { None, Bradford, CAT02 };

/** @brief XYZ tristimulus of a white point (Y normalised to 1). */
constexpr RGB whiteXYZ(double wx, double wy)
{
    return { wx / wy, 1.0, (1.0 - wx - wy) / wy };
}

/** @brief Normalized Primaries Matrix: linear RGB (these primaries) -> CIE XYZ.

Follows the standard construction (SMPTE RP 177): build the matrix of
primary chromaticities, scale its columns so that RGB=(1,1,1) maps to the
white point's XYZ. */
constexpr Matrix33 npm(const Primaries &p)
{
    // Columns are the primaries' (x, y, z=1-x-y).
    const Matrix33 P = { {
        { p.rx,            p.gx,            p.bx            },
        { p.ry,            p.gy,            p.by            },
        { 1 - p.rx - p.ry, 1 - p.gx - p.gy, 1 - p.bx - p.by }
    } };
    const Matrix33 Pinv = inverse(P);
    const RGB w = whiteXYZ(p.wx, p.wy);
    // gains = Pinv * whiteXYZ
    const RGB gains = Pinv * w;
    Matrix33 r = P;
    for (int row = 0; row < 3; ++row) {
        r.m[row][0] *= gains.r;
        r.m[row][1] *= gains.g;
        r.m[row][2] *= gains.b;
    }
    return r;
}

constexpr Matrix33 coneResponse(Adaptation a)
{
    if (a == Adaptation::CAT02)
        return { { { 0.7328,  0.4296, -0.1624 },
                   { -0.7036, 1.6975,  0.0061 },
                   { 0.0030,  0.0136,  0.9834 } } };
    // Bradford
    return { { { 0.8951,  0.2664, -0.1614 },
               { -0.7502, 1.7135,  0.0367 },
               { 0.0389, -0.0685,  1.0296 } } };
}

/** @brief von Kries chromatic adaptation matrix (XYZ src white -> dst white). */
constexpr Matrix33 vonKries(const RGB &srcW, const RGB &dstW, Adaptation method)
{
    const Matrix33 M = coneResponse(method);
    const Matrix33 Minv = inverse(M);
    const RGB s = M * srcW;
    const RGB d = M * dstW;
    const Matrix33 scale = { { { d.r / s.r, 0, 0 },
                               { 0, d.g / s.g, 0 },
                               { 0, 0, d.b / s.b } } };
    return Minv * (scale * M);
}

/** @brief ACES AP0 (ACES2065-1) primaries, from SMPTE ST 2065-1. */
constexpr Primaries kAP0 = { 0.7347, 0.2653, 0.0000, 1.0000, 0.0001, -0.0770, 0.32168, 0.33767 };
/** @brief ACES AP1 (ACEScg) primaries. */
constexpr Primaries kAP1 = { 0.713, 0.293, 0.165, 0.830, 0.128, 0.044, 0.32168, 0.33767 };

/** @brief Build the linear-RGB -> ACES2065-1(AP0) matrix for a set of primaries. */
constexpr Matrix33 toAP0Matrix(const Primaries &src, Adaptation method)
{
    const Matrix33 srcToXYZ = npm(src);
    const Matrix33 ap0ToXYZ = npm(kAP0);
    const Matrix33 xyzToAP0 = inverse(ap0ToXYZ);

    const bool sameWhite = (src.wx == kAP0.wx && src.wy == kAP0.wy);
    if (method == Adaptation::None || sameWhite)
        return xyzToAP0 * srcToXYZ;

    const RGB srcW = srcToXYZ * RGB{1, 1, 1};
    const RGB ap0W = ap0ToXYZ * RGB{1, 1, 1};
    const Matrix33 vk = vonKries(srcW, ap0W, method);
    return xyzToAP0 * (vk * srcToXYZ);
}

// ---------------------------------------------------------------------------
// Transfer functions.  Each encoding provides a decode (code -> scene-linear)
// and an encode (scene-linear -> code).  All are sign-preserving where the
// formula would otherwise be undefined for negatives.
// ---------------------------------------------------------------------------

namespace detail {

inline double signedPow(double x, double e)
{
    return x < 0.0 ? -std::pow(-x, e) : std::pow(x, e);
}

// --- Generic OCIO-style "camera log" with an optional linear toe segment. ---
struct CameraLog {
    double base;
    double logSlope, logOffset;   // log-side slope / offset
    double linSlope, linOffset;   // lin-side slope / offset
    double linBreak;              // scene-linear value where the segments meet
    double linearSlope;           // slope of the toe; <=0 means "derive it"
};

inline double logBreak(const CameraLog &p)
{
    return p.logSlope * std::log(p.linSlope * p.linBreak + p.linOffset) / std::log(p.base)
         + p.logOffset;
}

inline double linearSlopeOf(const CameraLog &p)
{
    return p.linearSlope > 0.0
        ? p.linearSlope
        : p.logSlope * p.linSlope
            / ((p.linSlope * p.linBreak + p.linOffset) * std::log(p.base));
}

inline double cameraLogToLinear(double v, const CameraLog &p)
{
    const double ls = linearSlopeOf(p);
    const double lb = logBreak(p);
    const double lo = lb - ls * p.linBreak;     // linear-segment offset
    if (v <= lb)
        return (v - lo) / ls;
    return (std::pow(p.base, (v - p.logOffset) / p.logSlope) - p.linOffset) / p.linSlope;
}

inline double cameraLogFromLinear(double x, const CameraLog &p)
{
    const double ls = linearSlopeOf(p);
    const double lb = logBreak(p);
    const double lo = lb - ls * p.linBreak;
    if (x <= p.linBreak)
        return ls * x + lo;
    return p.logSlope * std::log(p.linSlope * x + p.linOffset) / std::log(p.base) + p.logOffset;
}

// Parameters sourced from the OCIO ACES built-in transforms.
inline const CameraLog kACEScct  { 2.0, 1.0/17.52, 9.72/17.52, 1.0, 0.0, 0.0078125, 0.0 };
inline const CameraLog kARRILogC3{ 10.0, 0.2471896383, 0.3855369987,
                                   1.0/0.18, 0.0522722750,
                                   ((1.0/9.0) - 0.0522722750) / (1.0/0.18), 0.0 };
inline const CameraLog kARRILogC4{ 2.0, 0.0647954196341293, -0.295908392682586,
                                   2231.82630906769, 64.0, -0.0180569961199113, 0.0 };
inline const CameraLog kSLog3    { 10.0, 261.5/1023.0, 420.0/1023.0,
                                   1.0/(0.18+0.01), 0.01/(0.18+0.01), 0.01125,
                                   ((171.2102946929 - 95.0) / 0.01125) / 1023.0 };
inline const CameraLog kVLog     { 10.0, 0.241514, 0.598206, 1.0, 0.00873, 0.01, 0.0 };
inline const CameraLog kLog3G10  { 10.0, 0.224282, 0.0, 155.975327,
                                   0.01 * 155.975327 + 1.0, -0.01, 0.0 };

// --- sRGB piecewise (IEC 61966-2-1). ---
inline double srgbToLinear(double c)
{
    const double a = std::fabs(c);
    const double r = a <= 0.04045 ? a / 12.92 : std::pow((a + 0.055) / 1.055, 2.4);
    return c < 0.0 ? -r : r;
}
inline double srgbFromLinear(double c)
{
    const double a = std::fabs(c);
    const double r = a <= 0.0031308 ? a * 12.92 : 1.055 * std::pow(a, 1.0 / 2.4) - 0.055;
    return c < 0.0 ? -r : r;
}

// --- Pure display gamma (texture encodings g1.8 / g2.2 / g2.4). ---
template <int Num, int Den>
inline double gammaToLinear(double c) { return signedPow(c, double(Num) / double(Den)); }
template <int Num, int Den>
inline double gammaFromLinear(double c) { return signedPow(c, double(Den) / double(Num)); }

// --- Rec.709 camera OETF (ITU-R BT.709). ---
inline double rec709ToLinear(double v)
{
    const double a = std::fabs(v);
    const double r = a < 0.081 ? a / 4.5 : std::pow((a + 0.099) / 1.099, 1.0 / 0.45);
    return v < 0.0 ? -r : r;
}
inline double rec709FromLinear(double l)
{
    const double a = std::fabs(l);
    const double r = a < 0.018 ? 4.5 * a : 1.099 * std::pow(a, 0.45) - 0.099;
    return l < 0.0 ? -r : r;
}

// --- ACEScc (pure log, no linear toe; SMPTE has an explicit dark branch). ---
inline double acesccToLinear(double v)
{
    if (v < (9.72 - 15.0) / 17.52)
        return (std::pow(2.0, v * 17.52 - 9.72) - std::pow(2.0, -16.0)) * 2.0;
    return std::pow(2.0, v * 17.52 - 9.72);
}
inline double acesccFromLinear(double lin)
{
    if (lin <= 0.0)
        return (std::log2(std::pow(2.0, -16.0)) + 9.72) / 17.52;
    if (lin < std::pow(2.0, -15.0))
        return (std::log2(std::pow(2.0, -16.0) + lin * 0.5) + 9.72) / 17.52;
    return (std::log2(lin) + 9.72) / 17.52;
}

// --- Canon Log 2 (explicit piecewise, includes the *0.9 scene scaling). ---
inline double canonLog2ToLinear(double in)
{
    double out = in < 0.092864125
        ? -(std::pow(10.0, (0.092864125 - in) / 0.24136077) - 1.0) / 87.099375
        :  (std::pow(10.0, (in - 0.092864125) / 0.24136077) - 1.0) / 87.099375;
    return out * 0.9;
}
inline double canonLog2FromLinear(double lin)
{
    const double x = lin / 0.9;
    return x < 0.0
        ? 0.092864125 - std::log10(-x * 87.099375 + 1.0) * 0.24136077
        : 0.092864125 + std::log10( x * 87.099375 + 1.0) * 0.24136077;
}

// --- Canon Log 3 (explicit piecewise with a central linear segment). ---
inline double canonLog3ToLinear(double in)
{
    double out;
    if (in < 0.097465473)
        out = -(std::pow(10.0, (0.12783901 - in) / 0.36726845) - 1.0) / 14.98325;
    else if (in <= 0.15277891)
        out = (in - 0.12512219) / 1.9754798;
    else
        out = (std::pow(10.0, (in - 0.12240537) / 0.36726845) - 1.0) / 14.98325;
    return out * 0.9;
}
inline double canonLog3FromLinear(double lin)
{
    const double x = lin / 0.9;
    if (x < -0.014)
        return 0.12783901 - std::log10(-x * 14.98325 + 1.0) * 0.36726845;
    if (x <= 0.014)
        return x * 1.9754798 + 0.12512219;
    return 0.12240537 + std::log10(x * 14.98325 + 1.0) * 0.36726845;
}

// --- Blackmagic Film Generation 5. ---
inline double bmdFilmToLinear(double y)
{
    constexpr double A = 0.08692876065491224, B = 0.005494072432257808,
                     C = 0.5300133392291939,  D = 8.283605932402494,
                     E = 0.09246575342465753, LIN_CUT = 0.005;
    constexpr double LOG_CUT = D * LIN_CUT + E;
    return y < LOG_CUT ? (y - E) / D : std::exp((y - C) / A) - B;
}
inline double bmdFilmFromLinear(double x)
{
    constexpr double A = 0.08692876065491224, B = 0.005494072432257808,
                     C = 0.5300133392291939,  D = 8.283605932402494,
                     E = 0.09246575342465753, LIN_CUT = 0.005;
    return x < LIN_CUT ? D * x + E : A * std::log(x + B) + C;
}

// --- DaVinci Intermediate. ---
inline double davinciIntToLinear(double v)
{
    constexpr double A = 0.0075, B = 7.0, C = 0.07329248,
                     M = 10.44426855, LOG_CUT = 0.02740668;
    return v <= LOG_CUT ? v / M : std::pow(2.0, v / C - B) - A;
}
inline double davinciIntFromLinear(double l)
{
    constexpr double A = 0.0075, B = 7.0, C = 0.07329248,
                     M = 10.44426855, LIN_CUT = 0.00262409;
    return l <= LIN_CUT ? l * M : (std::log2(l + A) + B) * C;
}

inline double linearToLinear(double x) { return x; }

// Wrappers giving every camera-log encoding a uniform double(*)(double) shape.
inline double acescctTo(double v)   { return cameraLogToLinear(v, kACEScct); }
inline double acescctFrom(double v) { return cameraLogFromLinear(v, kACEScct); }
inline double arriLogC3To(double v)   { return cameraLogToLinear(v, kARRILogC3); }
inline double arriLogC3From(double v) { return cameraLogFromLinear(v, kARRILogC3); }
inline double arriLogC4To(double v)   { return cameraLogToLinear(v, kARRILogC4); }
inline double arriLogC4From(double v) { return cameraLogFromLinear(v, kARRILogC4); }
inline double slog3To(double v)   { return cameraLogToLinear(v, kSLog3); }
inline double slog3From(double v) { return cameraLogFromLinear(v, kSLog3); }
inline double vlogTo(double v)    { return cameraLogToLinear(v, kVLog); }
inline double vlogFrom(double v)  { return cameraLogFromLinear(v, kVLog); }
inline double log3g10To(double v)   { return cameraLogToLinear(v, kLog3G10); }
inline double log3g10From(double v) { return cameraLogFromLinear(v, kLog3G10); }

} // namespace detail

/** @brief A pair of transfer functions: code<->scene-linear. */
struct Transfer {
    double (*toLinear)(double);
    double (*fromLinear)(double);
};

// ---------------------------------------------------------------------------
// Camera gamut primaries (xy), from the OCIO ACES built-in transforms.
// ---------------------------------------------------------------------------
constexpr Primaries kRec709    = { 0.64,   0.33,   0.30,   0.60,   0.15,   0.06,   0.3127, 0.3290 };
constexpr Primaries kRec2020   = { 0.708,  0.292,  0.170,  0.797,  0.131,  0.046,  0.3127, 0.3290 };
constexpr Primaries kP3D65     = { 0.680,  0.320,  0.265,  0.690,  0.150,  0.060,  0.3127, 0.3290 };
constexpr Primaries kAWG3      = { 0.684,  0.313,  0.221,  0.848,  0.0861,-0.102,  0.3127, 0.3290 };
constexpr Primaries kAWG4      = { 0.7347, 0.2653, 0.1424, 0.8576, 0.0991,-0.0308, 0.3127, 0.3290 };
constexpr Primaries kSGamut3   = { 0.730,  0.280,  0.140,  0.855,  0.100, -0.050,  0.3127, 0.3290 };
constexpr Primaries kSGamut3Cine = { 0.766, 0.275, 0.225,  0.800,  0.089, -0.087,  0.3127, 0.3290 };
constexpr Primaries kCanonCGamut = { 0.740, 0.270, 0.170,  1.140,  0.080, -0.100,  0.3127, 0.3290 };
constexpr Primaries kVGamut    = { 0.730,  0.280,  0.165,  0.840,  0.100, -0.030,  0.3127, 0.3290 };
constexpr Primaries kREDWideGamut = { 0.780308, 0.304253, 0.121595, 1.493994, 0.095612, -0.084589, 0.3127, 0.3290 };
constexpr Primaries kBMDWideGamut = { 0.7177215, 0.3171181, 0.2280410, 0.8615690, 0.1005841, -0.0820452, 0.3127, 0.3290 };
constexpr Primaries kDaVinciWideGamut = { 0.8000, 0.3130, 0.1682, 0.9877, 0.0790, -0.1155, 0.3127, 0.3290 };

// Sony Venice gamuts: OCIO ships precomputed RGB->AP0 matrices (no primaries).
constexpr Matrix33 kVeniceSGamut3_to_AP0 = { {
    {  0.7933297411,  0.0890786256,  0.1175916333 },
    {  0.0155810585,  1.0327123069, -0.0482933654 },
    { -0.0188647478,  0.0127694121,  1.0060953358 }
} };
constexpr Matrix33 kVeniceSGamut3Cine_to_AP0 = { {
    {  0.6742570921,  0.2205717359,  0.1051711720 },
    { -0.0093136061,  1.1059588614, -0.0966452553 },
    { -0.0382090673, -0.0179383766,  1.0561474439 }
} };

// ---------------------------------------------------------------------------
// The supported colourspaces (scene-referred working + camera/texture spaces).
// ---------------------------------------------------------------------------
enum class Colourspace {
    ACES2065_1,             // AP0, the reference
    ACEScg,
    ACEScc,
    ACEScct,
    lin_p3d65,
    lin_rec2020,
    lin_rec709_srgb,
    g18_rec709_tx,
    g22_rec709_tx,
    g24_rec709_tx,
    g22_ap1_tx,
    srgb_tx,
    srgb_encoded_ap1_tx,
    srgb_encoded_p3d65_tx,
    camera_rec709,
    lin_arri_wide_gamut_3,
    arri_logc3_ei800,
    lin_arri_wide_gamut_4,
    arri_logc4,
    lin_bmd_widegamut_gen5,
    bmdfilm_widegamut_gen5,
    davinci_intermediate_widegamut,
    lin_davinci_widegamut,
    lin_cinemagamut_d55,
    canonlog2_cinemagamut_d55,
    canonlog3_cinemagamut_d55,
    lin_vgamut,
    vlog_vgamut,
    lin_redwidegamutrgb,
    log3g10_redwidegamutrgb,
    lin_sgamut3,
    slog3_sgamut3,
    lin_sgamut3cine,
    slog3_sgamut3cine,
    lin_venice_sgamut3,
    slog3_venice_sgamut3,
    lin_venice_sgamut3cine,
    slog3_venice_sgamut3cine,
    Count
};

/** @brief Everything needed to convert a colourspace to/from ACES2065-1. */
struct ColourspaceInfo {
    Matrix33 toAP0;     // linear RGB (this space's gamut) -> AP0
    Matrix33 fromAP0;   // AP0 -> linear RGB (this space's gamut)
    Transfer tf;        // code <-> scene-linear
};

namespace detail {

inline Transfer linearTF()  { return { linearToLinear, linearToLinear }; }

inline ColourspaceInfo make(const Matrix33 &toAP0, const Transfer &tf)
{
    return { toAP0, inverse(toAP0), tf };
}

// Build the table once, on first use (no static-init-order concerns; the
// gamut matrices are compile-time constants).
inline const std::array<ColourspaceInfo, std::size_t(Colourspace::Count)> &table()
{
    using A = Adaptation;
    static const std::array<ColourspaceInfo, std::size_t(Colourspace::Count)> t = [] {
        std::array<ColourspaceInfo, std::size_t(Colourspace::Count)> a{};
        auto set = [&](Colourspace cs, const ColourspaceInfo &info) {
            a[std::size_t(cs)] = info;
        };

        const Transfer lin = linearTF();
        const Transfer srgb     = { srgbToLinear, srgbFromLinear };
        const Transfer g18      = { gammaToLinear<9,5>,  gammaFromLinear<9,5>  };
        const Transfer g22      = { gammaToLinear<11,5>, gammaFromLinear<11,5> };
        const Transfer g24      = { gammaToLinear<12,5>, gammaFromLinear<12,5> };
        const Transfer rec709   = { rec709ToLinear, rec709FromLinear };
        const Transfer cc       = { acesccToLinear, acesccFromLinear };
        const Transfer cct      = { acescctTo, acescctFrom };
        const Transfer logc3    = { arriLogC3To, arriLogC3From };
        const Transfer logc4    = { arriLogC4To, arriLogC4From };
        const Transfer slog3    = { slog3To, slog3From };
        const Transfer clog2    = { canonLog2ToLinear, canonLog2FromLinear };
        const Transfer clog3    = { canonLog3ToLinear, canonLog3FromLinear };
        const Transfer vlog     = { vlogTo, vlogFrom };
        const Transfer log3g10  = { log3g10To, log3g10From };
        const Transfer bmd      = { bmdFilmToLinear, bmdFilmFromLinear };
        const Transfer dvi      = { davinciIntToLinear, davinciIntFromLinear };

        // Gamut matrices to AP0.  AP1 needs no white adaptation; Rec.709/2020/
        // P3 and Panasonic/RED use Bradford; the other camera gamuts use CAT02
        // (matching the OCIO ACES built-in transforms).
        const Matrix33 ap0   = kIdentity33;
        const Matrix33 ap1   = toAP0Matrix(kAP1, A::None);
        const Matrix33 p3    = toAP0Matrix(kP3D65, A::Bradford);
        const Matrix33 r2020 = toAP0Matrix(kRec2020, A::Bradford);
        const Matrix33 r709  = toAP0Matrix(kRec709, A::Bradford);
        const Matrix33 awg3  = toAP0Matrix(kAWG3, A::CAT02);
        const Matrix33 awg4  = toAP0Matrix(kAWG4, A::CAT02);
        const Matrix33 bmdg  = toAP0Matrix(kBMDWideGamut, A::CAT02);
        const Matrix33 dvwg  = toAP0Matrix(kDaVinciWideGamut, A::CAT02);
        const Matrix33 cgamut= toAP0Matrix(kCanonCGamut, A::CAT02);
        const Matrix33 vgamut= toAP0Matrix(kVGamut, A::Bradford);
        const Matrix33 rwg   = toAP0Matrix(kREDWideGamut, A::Bradford);
        const Matrix33 sg3   = toAP0Matrix(kSGamut3, A::CAT02);
        const Matrix33 sg3c  = toAP0Matrix(kSGamut3Cine, A::CAT02);

        set(Colourspace::ACES2065_1, make(ap0, lin));
        set(Colourspace::ACEScg,     make(ap1, lin));
        set(Colourspace::ACEScc,     make(ap1, cc));
        set(Colourspace::ACEScct,    make(ap1, cct));

        set(Colourspace::lin_p3d65,        make(p3, lin));
        set(Colourspace::lin_rec2020,      make(r2020, lin));
        set(Colourspace::lin_rec709_srgb,  make(r709, lin));
        set(Colourspace::g18_rec709_tx,    make(r709, g18));
        set(Colourspace::g22_rec709_tx,    make(r709, g22));
        set(Colourspace::g24_rec709_tx,    make(r709, g24));
        set(Colourspace::g22_ap1_tx,       make(ap1, g22));
        set(Colourspace::srgb_tx,          make(r709, srgb));
        set(Colourspace::srgb_encoded_ap1_tx,   make(ap1, srgb));
        set(Colourspace::srgb_encoded_p3d65_tx, make(p3, srgb));
        set(Colourspace::camera_rec709,    make(r709, rec709));

        set(Colourspace::lin_arri_wide_gamut_3, make(awg3, lin));
        set(Colourspace::arri_logc3_ei800,      make(awg3, logc3));
        set(Colourspace::lin_arri_wide_gamut_4, make(awg4, lin));
        set(Colourspace::arri_logc4,            make(awg4, logc4));

        set(Colourspace::lin_bmd_widegamut_gen5, make(bmdg, lin));
        set(Colourspace::bmdfilm_widegamut_gen5, make(bmdg, bmd));
        set(Colourspace::davinci_intermediate_widegamut, make(dvwg, dvi));
        set(Colourspace::lin_davinci_widegamut,  make(dvwg, lin));

        set(Colourspace::lin_cinemagamut_d55,       make(cgamut, lin));
        set(Colourspace::canonlog2_cinemagamut_d55, make(cgamut, clog2));
        set(Colourspace::canonlog3_cinemagamut_d55, make(cgamut, clog3));

        set(Colourspace::lin_vgamut,  make(vgamut, lin));
        set(Colourspace::vlog_vgamut, make(vgamut, vlog));

        set(Colourspace::lin_redwidegamutrgb,     make(rwg, lin));
        set(Colourspace::log3g10_redwidegamutrgb, make(rwg, log3g10));

        set(Colourspace::lin_sgamut3,      make(sg3, lin));
        set(Colourspace::slog3_sgamut3,    make(sg3, slog3));
        set(Colourspace::lin_sgamut3cine,  make(sg3c, lin));
        set(Colourspace::slog3_sgamut3cine,make(sg3c, slog3));

        set(Colourspace::lin_venice_sgamut3,       make(kVeniceSGamut3_to_AP0, lin));
        set(Colourspace::slog3_venice_sgamut3,     make(kVeniceSGamut3_to_AP0, slog3));
        set(Colourspace::lin_venice_sgamut3cine,   make(kVeniceSGamut3Cine_to_AP0, lin));
        set(Colourspace::slog3_venice_sgamut3cine, make(kVeniceSGamut3Cine_to_AP0, slog3));

        return a;
    }();
    return t;
}

} // namespace detail

/** @brief Look up the conversion data for a colourspace. */
inline const ColourspaceInfo &info(Colourspace cs)
{
    return detail::table()[std::size_t(cs)];
}

/** @brief Convert an RGB triplet in @p from to ACES2065-1 (AP0). */
inline RGB toACES2065_1(RGB c, Colourspace from)
{
    const ColourspaceInfo &i = info(from);
    const RGB lin = { i.tf.toLinear(c.r), i.tf.toLinear(c.g), i.tf.toLinear(c.b) };
    return i.toAP0 * lin;
}

/** @brief Convert an ACES2065-1 (AP0) triplet to the colourspace @p to. */
inline RGB fromACES2065_1(RGB aces, Colourspace to)
{
    const ColourspaceInfo &i = info(to);
    const RGB lin = i.fromAP0 * aces;
    return { i.tf.fromLinear(lin.r), i.tf.fromLinear(lin.g), i.tf.fromLinear(lin.b) };
}

/** @brief Convert an RGB triplet directly between two colourspaces. */
inline RGB convert(RGB c, Colourspace from, Colourspace to)
{
    if (from == to) return c;
    return fromACES2065_1(toACES2065_1(c, from), to);
}

/** @brief Map an OFX colourspace identifier string (e.g. the kOfxColourspace*
values from the config header, such as "slog3_sgamut3") to a Colourspace.
@returns true and sets @p out on success; false if the name is not one of the
spaces handled here. */
inline bool colourspaceFromName(const char *name, Colourspace &out)
{
    struct Entry { const char *name; Colourspace cs; };
    static const Entry entries[] = {
        { "ACES2065-1", Colourspace::ACES2065_1 },
        { "ACEScg", Colourspace::ACEScg },
        { "ACEScc", Colourspace::ACEScc },
        { "ACEScct", Colourspace::ACEScct },
        { "lin_p3d65", Colourspace::lin_p3d65 },
        { "lin_rec2020", Colourspace::lin_rec2020 },
        { "lin_rec709_srgb", Colourspace::lin_rec709_srgb },
        { "g18_rec709_tx", Colourspace::g18_rec709_tx },
        { "g22_rec709_tx", Colourspace::g22_rec709_tx },
        { "g24_rec709_tx", Colourspace::g24_rec709_tx },
        { "g22_ap1_tx", Colourspace::g22_ap1_tx },
        { "srgb_tx", Colourspace::srgb_tx },
        { "srgb_encoded_ap1_tx", Colourspace::srgb_encoded_ap1_tx },
        { "srgb_encoded_p3d65_tx", Colourspace::srgb_encoded_p3d65_tx },
        { "camera_rec709", Colourspace::camera_rec709 },
        { "lin_arri_wide_gamut_3", Colourspace::lin_arri_wide_gamut_3 },
        { "arri_logc3_ei800", Colourspace::arri_logc3_ei800 },
        { "lin_arri_wide_gamut_4", Colourspace::lin_arri_wide_gamut_4 },
        { "arri_logc4", Colourspace::arri_logc4 },
        { "lin_bmd_widegamut_gen5", Colourspace::lin_bmd_widegamut_gen5 },
        { "bmdfilm_widegamut_gen5", Colourspace::bmdfilm_widegamut_gen5 },
        { "davinci_intermediate_widegamut", Colourspace::davinci_intermediate_widegamut },
        { "lin_davinci_widegamut", Colourspace::lin_davinci_widegamut },
        { "lin_cinemagamut_d55", Colourspace::lin_cinemagamut_d55 },
        { "canonlog2_cinemagamut_d55", Colourspace::canonlog2_cinemagamut_d55 },
        { "canonlog3_cinemagamut_d55", Colourspace::canonlog3_cinemagamut_d55 },
        { "lin_vgamut", Colourspace::lin_vgamut },
        { "vlog_vgamut", Colourspace::vlog_vgamut },
        { "lin_redwidegamutrgb", Colourspace::lin_redwidegamutrgb },
        { "log3g10_redwidegamutrgb", Colourspace::log3g10_redwidegamutrgb },
        { "lin_sgamut3", Colourspace::lin_sgamut3 },
        { "slog3_sgamut3", Colourspace::slog3_sgamut3 },
        { "lin_sgamut3cine", Colourspace::lin_sgamut3cine },
        { "slog3_sgamut3cine", Colourspace::slog3_sgamut3cine },
        { "lin_venice_sgamut3", Colourspace::lin_venice_sgamut3 },
        { "slog3_venice_sgamut3", Colourspace::slog3_venice_sgamut3 },
        { "lin_venice_sgamut3cine", Colourspace::lin_venice_sgamut3cine },
        { "slog3_venice_sgamut3cine", Colourspace::slog3_venice_sgamut3cine },
    };
    for (const Entry &e : entries) {
        if (std::strcmp(e.name, name) == 0) { out = e.cs; return true; }
    }
    return false;
}

} // namespace colour
} // namespace ofx

#endif // _ofxColourConvert_h_
