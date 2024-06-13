#pragma once
#ifndef INTERPOLATION_H
#define INTERPOLATION_H

#include <wx/wx.h>

wxImage HermiteInterpolation(const wxImage& image);
wxImage MitchellInterpolation(const wxImage& image);
wxImage BSplineInterpolation(const wxImage& image);
wxImage LanczosInterpolation(const wxImage& image);

#endif
