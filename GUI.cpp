#include "GUI.h"
#include "Interpolation.h"
#include "Utils.h"

wxBEGIN_EVENT_TABLE(GUIFrame, wxFrame)
EVT_MENU(ID_Open, GUIFrame::OnOpenFile)
EVT_MENU(ID_Save, GUIFrame::OnSaveFile)
EVT_SLIDER(ID_Zoom, GUIFrame::OnZoom)
EVT_SLIDER(ID_Rotate, GUIFrame::OnRotate)
EVT_BUTTON(ID_Combine, GUIFrame::OnCombine)
EVT_PAINT(GUIFrame::OnPaint)
EVT_LEFT_DOWN(GUIFrame::OnLeftDown)
EVT_LEFT_UP(GUIFrame::OnLeftUp)
EVT_MOTION(GUIFrame::OnMouseMove)
wxEND_EVENT_TABLE()

GUIFrame::GUIFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
    : wxFrame(NULL, wxID_ANY, title, pos, size), m_dragging(false) {

    wxMenu* menuFile = new wxMenu;
    menuFile->Append(ID_Open, "&Open...\tCtrl-O");
    menuFile->Append(ID_Save, "&Save...\tCtrl-S");
    menuFile->AppendSeparator();
    menuFile->Append(wxID_EXIT);

    wxMenuBar* menuBar = new wxMenuBar;
    menuBar->Append(menuFile, "&File");
    SetMenuBar(menuBar);

    CreateStatusBar();
    SetStatusText("Welcome to Raster Graphics Interpolation!");

    m_panel = new wxPanel(this, wxID_ANY);
    m_sizer = new wxBoxSizer(wxVERTICAL);
    m_panel->SetSizer(m_sizer);

    m_zoomSlider = new wxSlider(m_panel, ID_Zoom, 100, 10, 200, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL);
    m_sizer->Add(m_zoomSlider, 0, wxALL | wxEXPAND, 5);

    m_rotateSlider = new wxSlider(m_panel, ID_Rotate, 0, -180, 180, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL);
    m_sizer->Add(m_rotateSlider, 0, wxALL | wxEXPAND, 5);

    m_combineButton = new wxButton(m_panel, ID_Combine, "Combine Transformations");
    m_sizer->Add(m_combineButton, 0, wxALL | wxCENTER, 5);

    m_bitmap = new wxStaticBitmap(m_panel, wxID_ANY, wxBitmap());
    m_sizer->Add(m_bitmap, 1, wxEXPAND | wxALL, 5);

    // Add five squares at the bottom
    wxBoxSizer* squaresSizer = new wxBoxSizer(wxHORIZONTAL);
    for (int i = 0; i < 5; ++i) {
        m_squareBitmaps[i] = new wxStaticBitmap(m_panel, wxID_ANY, wxBitmap(100, 100));
        squaresSizer->Add(m_squareBitmaps[i], 1, wxALL | wxEXPAND, 5);
    }
    m_sizer->Add(squaresSizer, 0, wxALL | wxEXPAND, 5);

    m_selectionRect = wxRect(0, 0, 50, 50); // Initial selection rectangle
}

void GUIFrame::OnOpenFile(wxCommandEvent& event) {
    wxFileDialog openFileDialog(this, _("Open BMP file"), "", "",
        "BMP files (*.bmp)|*.bmp", wxFD_OPEN | wxFD_FILE_MUST_EXIST);

    if (openFileDialog.ShowModal() == wxID_CANCEL)
        return;

    m_image.LoadFile(openFileDialog.GetPath(), wxBITMAP_TYPE_BMP);
    m_bitmap->SetBitmap(wxBitmap(m_image));
    UpdateSquares(); // Update squares when a new image is loaded
    m_panel->Refresh();
}

void GUIFrame::OnSaveFile(wxCommandEvent& event) {
    wxFileDialog saveFileDialog(this, _("Save BMP file"), "", "",
        "BMP files (*.bmp)|*.bmp", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

    if (saveFileDialog.ShowModal() == wxID_CANCEL)
        return;

    wxBitmap compositeBitmap = CreateCompositeImage();
    compositeBitmap.SaveFile(saveFileDialog.GetPath(), wxBITMAP_TYPE_BMP);
}

void GUIFrame::OnZoom(wxCommandEvent& event) {
    int zoomFactor = m_zoomSlider->GetValue();
    wxImage zoomedImage = ResizeImage(m_image, m_image.GetWidth() * zoomFactor / 100, m_image.GetHeight() * zoomFactor / 100);
    m_bitmap->SetBitmap(wxBitmap(zoomedImage));
    UpdateSquares(); // Update squares when zoom changes
    m_panel->Refresh();
}

void GUIFrame::OnRotate(wxCommandEvent& event) {
    int angle = m_rotateSlider->GetValue();
    wxImage rotatedImage = RotateImage(m_image, wxDegToRad(angle));
    m_bitmap->SetBitmap(wxBitmap(rotatedImage));
    UpdateSquares(); // Update squares when rotation changes
    m_panel->Refresh();
}

void GUIFrame::OnCombine(wxCommandEvent& event) {
    int zoomFactor = m_zoomSlider->GetValue();
    int angle = m_rotateSlider->GetValue();
    wxImage transformedImage = ResizeImage(m_image, m_image.GetWidth() * zoomFactor / 100, m_image.GetHeight() * zoomFactor / 100);
    transformedImage = RotateImage(transformedImage, wxDegToRad(angle));
    m_bitmap->SetBitmap(wxBitmap(transformedImage));
    UpdateSquares(); // Update squares when combined transformation is applied
    m_panel->Refresh();
}

void GUIFrame::OnPaint(wxPaintEvent& event) {
    wxPaintDC dc(this);
    if (m_image.IsOk()) {
        dc.SetPen(*wxRED_PEN);
        dc.SetBrush(*wxTRANSPARENT_BRUSH);
        dc.DrawRectangle(m_selectionRect);
    }
}

void GUIFrame::OnLeftDown(wxMouseEvent& event) {
    wxPoint pos = event.GetPosition();
    if (m_selectionRect.Contains(pos)) {
        m_dragging = true;
        m_dragStart = pos - m_selectionRect.GetPosition();
    }
}

void GUIFrame::OnLeftUp(wxMouseEvent& event) {
    m_dragging = false;
}

void GUIFrame::OnMouseMove(wxMouseEvent& event) {
    if (m_dragging) {
        wxPoint pos = event.GetPosition();
        m_selectionRect.SetPosition(pos - m_dragStart);
        UpdateSelectionRect();
        m_panel->Refresh();
    }
}

void GUIFrame::UpdateSelectionRect() {
    if (m_selectionRect.GetRight() > m_image.GetWidth()) {
        m_selectionRect.SetRight(m_image.GetWidth());
    }
    if (m_selectionRect.GetBottom() > m_image.GetHeight()) {
        m_selectionRect.SetBottom(m_image.GetHeight());
    }
    if (m_selectionRect.GetLeft() < 0) {
        m_selectionRect.SetLeft(0);
    }
    if (m_selectionRect.GetTop() < 0) {
        m_selectionRect.SetTop(0);
    }
    UpdateSquares();
}

wxBitmap GUIFrame::CreateCompositeImage() {
    int width = m_squareBitmaps[0]->GetBitmap().GetWidth();
    int height = m_squareBitmaps[0]->GetBitmap().GetHeight();

    wxBitmap compositeBitmap(width * 5, height);
    wxMemoryDC dc(compositeBitmap);

    for (int i = 0; i < 5; ++i) {
        dc.DrawBitmap(m_squareBitmaps[i]->GetBitmap(), width * i, 0, false);
    }

    dc.SelectObject(wxNullBitmap);
    return compositeBitmap;
}

void GUIFrame::UpdateSquares() {
    if (!m_image.IsOk()) return;

    int squareSize = 100; // Size of the square bitmaps
    wxRect selectionRect(0, 0, squareSize / 2, squareSize / 2); // Adjust this for the selection frame

    wxImage selectedArea = m_image.GetSubImage(selectionRect);
    m_squareBitmaps[0]->SetBitmap(wxBitmap(selectedArea.Scale(squareSize, squareSize, wxIMAGE_QUALITY_NORMAL)));

    m_squareBitmaps[1]->SetBitmap(wxBitmap(HermiteInterpolation(selectedArea).Scale(squareSize, squareSize)));
    m_squareBitmaps[2]->SetBitmap(wxBitmap(MitchellInterpolation(selectedArea).Scale(squareSize, squareSize)));
    m_squareBitmaps[3]->SetBitmap(wxBitmap(BSplineInterpolation(selectedArea).Scale(squareSize, squareSize)));
    m_squareBitmaps[4]->SetBitmap(wxBitmap(LanczosInterpolation(selectedArea).Scale(squareSize, squareSize)));

    m_panel->Refresh();
}
