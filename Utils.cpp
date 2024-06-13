#include "Utils.h"

wxImage ResizeImage(const wxImage& image, int width, int height) {
    return image.Scale(width, height, wxIMAGE_QUALITY_HIGH);
}

wxImage RotateImage(const wxImage& image, double angle) {
    return image.Rotate(angle, wxPoint(image.GetWidth() / 2, image.GetHeight() / 2));
}
