#include "Interpolation.h"
#include <cmath>
#include <algorithm>
#include <wx/image.h>

// Hermite Interpolation
wxImage HermiteInterpolation(const wxImage& image) {
    int width = image.GetWidth();
    int height = image.GetHeight();
    wxImage result(width * 2, height * 2);

    for (int y = 0; y < height * 2; ++y) {
        for (int x = 0; x < width * 2; ++x) {
            int srcX = x / 2;
            int srcY = y / 2;
            result.SetRGB(x, y, image.GetRed(srcX, srcY), image.GetGreen(srcX, srcY), image.GetBlue(srcX, srcY));
        }
    }

    return result;
}

wxImage MitchellInterpolation(const wxImage& image, double B = 1.0 / 3.0, double C = 1.0 / 3.0) {
    int width = image.GetWidth();
    int height = image.GetHeight();
    wxImage result(width * 2, height * 2);

    // Mitchell-Netravali kernel function
    auto mitchell = [B, C](double x) {
        x = std::abs(x);
        if (x < 1)
            return ((12 - 9 * B - 6 * C) * x * x * x + (-18 + 12 * B + 6 * C) * x * x + (6 - 2 * B)) / 6.0;
        else if (x < 2)
            return ((-B - 6 * C) * x * x * x + (6 * B + 30 * C) * x * x + (-12 * B - 48 * C) * x + (8 * B + 24 * C)) / 6.0;
        else
            return 0.0;
        };

    for (int y = 0; y < height * 2; ++y) {
        for (int x = 0; x < width * 2; ++x) {
            double srcX = x / 2.0;
            double srcY = y / 2.0;
            int ix = static_cast<int>(srcX);
            int iy = static_cast<int>(srcY);
            double fx = srcX - ix;
            double fy = srcY - iy;

            double r = 0, g = 0, b = 0;
            for (int dy = -2; dy <= 2; ++dy) {
                for (int dx = -2; dx <= 2; ++dx) {
                    int sx = ix + dx;
                    int sy = iy + dy;
                    if (sx >= 0 && sx < width && sy >= 0 && sy < height) {
                        double weight = mitchell(dx - fx) * mitchell(dy - fy);
                        r += weight * image.GetRed(sx, sy);
                        g += weight * image.GetGreen(sx, sy);
                        b += weight * image.GetBlue(sx, sy);
                    }
                }
            }
            result.SetRGB(x, y, std::clamp<int>(r, 0, 255), std::clamp<int>(g, 0, 255), std::clamp<int>(b, 0, 255));
        }
    }
    return result;
}

// B-Spline Interpolation (Cubic)
wxImage BSplineInterpolation(const wxImage& image) {
    int width = image.GetWidth();
    int height = image.GetHeight();
    wxImage result(width * 2, height * 2);

    // B-spline kernel function (Cubic)
    auto bspline = [](double x) {
        x = std::abs(x);
        if (x < 1)
            return (2.0 / 3.0) + (0.5) * x * x * x - x * x;
        else if (x < 2)
            return (1.0 / 6.0) * std::pow(2 - x, 3);
        else
            return 0.0;
        };

    for (int y = 0; y < height * 2; ++y) {
        for (int x = 0; x < width * 2; ++x) {
            double srcX = x / 2.0;
            double srcY = y / 2.0;
            int ix = static_cast<int>(srcX);
            int iy = static_cast<int>(srcY);
            double fx = srcX - ix;
            double fy = srcY - iy;

            double r = 0, g = 0, b = 0;
            for (int dy = -2; dy <= 2; ++dy) {
                for (int dx = -2; dx <= 2; ++dx) {
                    int sx = ix + dx;
                    int sy = iy + dy;
                    if (sx >= 0 && sx < width && sy >= 0 && sy < height) {
                        double weight = bspline(dx - fx) * bspline(dy - fy);
                        r += weight * image.GetRed(sx, sy);
                        g += weight * image.GetGreen(sx, sy);
                        b += weight * image.GetBlue(sx, sy);
                    }
                }
            }
            result.SetRGB(x, y, std::clamp<int>(r, 0, 255), std::clamp<int>(g, 0, 255), std::clamp<int>(b, 0, 255));
        }
    }
    return result;
}

// Lanczos Interpolation
wxImage LanczosInterpolation(const wxImage& image, int a = 3) {
    int width = image.GetWidth();
    int height = image.GetHeight();
    wxImage result(width * 2, height * 2);

    // Lanczos kernel function
    auto lanczos = [a](double x) {
        x = std::abs(x);
        if (x < a) {
            return (a * std::sin(M_PI * x) * std::sin(M_PI * x / a)) / (M_PI * M_PI * x * x);
        }
        else {
            return 0.0;
        }
        };

    for (int y = 0; y < height * 2; ++y) {
        for (int x = 0; x < width * 2; ++x) {
            double srcX = x / 2.0;
            double srcY = y / 2.0;
            int ix = static_cast<int>(srcX);
            int iy = static_cast<int>(srcY);
            double fx = srcX - ix;
            double fy = srcY - iy;

            double r = 0, g = 0, b = 0;
            for (int dy = -a; dy <= a; ++dy) {
                for (int dx = -a; dx <= a; ++dx) {
                    int sx = ix + dx;
                    int sy = iy + dy;
                    if (sx >= 0 && sx < width && sy >= 0 && sy < height) {
                        double weight = lanczos(dx - fx) * lanczos(dy - fy);
                        r += weight * image.GetRed(sx, sy);
                        g += weight * image.GetGreen(sx, sy);
                        b += weight * image.GetBlue(sx, sy);
                    }
                }
            }
            result.SetRGB(x, y, std::clamp<int>(r, 0, 255), std::clamp<int>(g, 0, 255), std::clamp<int>(b, 0, 255));
        }
    }
    return result;
}