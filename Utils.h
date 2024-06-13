#ifndef UTILS_H
#define UTILS_H

#include <wx/wx.h>

wxImage ResizeImage(const wxImage& image, int width, int height);
wxImage RotateImage(const wxImage& image, double angle);

#endif
