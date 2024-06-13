#include <wx/wx.h>
#include "GUI.h"

class MyApp : public wxApp {
public:
    virtual bool OnInit();
};

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit() {
    GUIFrame* frame = new GUIFrame("Raster Graphics Interpolation", wxPoint(50, 50), wxSize(800, 600));
    frame->Show(true);
    return true;
}
