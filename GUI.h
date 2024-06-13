#ifndef GUI_H
#define GUI_H

#include <wx/wx.h>
#include <wx/filedlg.h>
#include <wx/image.h>
#include <wx/bitmap.h>

class GUIFrame : public wxFrame {
public:
    GUIFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

private:
    wxPanel* m_panel;
    wxStaticBitmap* m_bitmap;
    wxImage m_image;
    wxBoxSizer* m_sizer;
    wxSlider* m_zoomSlider;
    wxSlider* m_rotateSlider;
    wxButton* m_combineButton;
    wxStaticBitmap* m_squareBitmaps[5]; // Added array for five squares

    wxRect m_selectionRect; // Rectangle for selecting the zoom area
    bool m_dragging;
    wxPoint m_dragStart;

    void OnOpenFile(wxCommandEvent& event);
    void OnSaveFile(wxCommandEvent& event);
    void OnZoom(wxCommandEvent& event);
    void OnRotate(wxCommandEvent& event);
    void OnCombine(wxCommandEvent& event);
    void OnPaint(wxPaintEvent& event);
    void OnLeftDown(wxMouseEvent& event);
    void OnLeftUp(wxMouseEvent& event);
    void OnMouseMove(wxMouseEvent& event);

    wxBitmap CreateCompositeImage();
    void UpdateSquares(); // Method to update the squares
    void UpdateSelectionRect(); // Method to update the selection rectangle

    wxDECLARE_EVENT_TABLE();
};

enum {
    ID_Open = 1,
    ID_Save = 2,
    ID_Zoom = 3,
    ID_Rotate = 4,
    ID_Combine = 5
};

#endif
