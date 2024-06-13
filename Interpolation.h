#ifndef INTERPOLATION_H
#define INTERPOLATION_H

#include <wx/wx.h>

/**
 * Performs Hermite interpolation on the input image.
 *
 * @param image The input image.
 * @return The interpolated image.
 */
wxImage HermiteInterpolation(const wxImage& image);

/**
 * Performs Mitchell-Netravali interpolation on the input image.
 *
 * @param image The input image.
 * @param B The B parameter of the Mitchell-Netravali filter (default: 1/3).
 * @param C The C parameter of the Mitchell-Netravali filter (default: 1/3).
 * @return The interpolated image.
 */
wxImage MitchellInterpolation(const wxImage& image, double B = 1.0 / 3.0, double C = 1.0 / 3.0);

/**
 * Performs B-spline interpolation on the input image.
 *
 * @param image The input image.
 * @return The interpolated image.
 */
wxImage BSplineInterpolation(const wxImage& image);

/**
 * Performs Lanczos interpolation on the input image.
 *
 * @param image The input image.
 * @param a The Lanczos filter parameter (default: 3).
 * @return The interpolated image.
 */
wxImage LanczosInterpolation(const wxImage& image, int a = 3);

#endif 
