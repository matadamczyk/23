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
    : wxFrame(NULL, wxID_ANY, title, pos, size), m_dragging(false), m_rotationAngle(0.0), m_zoomFactor(1.0) {

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

    m_bitmap = new wxStaticBitmap(m_panel, wxID_ANY, wxNullBitmap);
    m_sizer->Add(m_bitmap, 1, wxEXPAND | wxALL, 5);

    // Pięć kwadratów na dole
    wxGridSizer* squaresSizer = new wxGridSizer(2, 3, 5, 5); // 2 wiersze, 3 kolumny
    for (int i = 0; i < 5; ++i) {
        m_squareBitmaps[i] = new wxStaticBitmap(m_panel, wxID_ANY, wxNullBitmap);
        squaresSizer->Add(m_squareBitmaps[i], 1, wxEXPAND | wxALL, 5);
    }
    squaresSizer->Add(new wxStaticText(m_panel, wxID_ANY, ""), 1, wxEXPAND); // Puste pole
    m_sizer->Add(squaresSizer, 0, wxEXPAND | wxALL, 5);

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
    m_zoomFactor = m_zoomSlider->GetValue() / 100.0;
    UpdateImage();
}

void GUIFrame::OnRotate(wxCommandEvent& event) {
    m_rotationAngle = wxDegToRad(m_rotateSlider->GetValue());
    UpdateImage();
}

void GUIFrame::OnCombine(wxCommandEvent& event) {
    UpdateImage();
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

void GUIFrame::UpdateImage() {
    if (!m_image.IsOk()) return;

    // Obliczanie nowych wymiarów i przesunięcia dla powiększenia i obrotu
    double centerX = m_image.GetWidth() / 2.0;
    double centerY = m_image.GetHeight() / 2.0;
    double newWidth = m_image.GetWidth() * m_zoomFactor;
    double newHeight = m_image.GetHeight() * m_zoomFactor;
    double offsetX = (newWidth - m_image.GetWidth()) / 2.0;
    double offsetY = (newHeight - m_image.GetHeight()) / 2.0;

    wxAffineMatrix2D matrix;
    matrix.Translate(-centerX, -centerY);    // Przesunięcie do środka
    matrix.Rotate(m_rotationAngle);         // Obrót
    matrix.Scale(m_zoomFactor, m_zoomFactor); // Skalowanie
    matrix.Translate(centerX, centerY);    // Przesunięcie z powrotem

    // Tworzenie nowego obrazu i rysowanie z transformacją
    wxImage transformedImage(newWidth, newHeight);
    wxBitmap transformedBitmap(transformedImage);
    {
        wxGraphicsContext* gc = wxGraphicsContext::Create(wxMemoryDC(transformedBitmap));
        wxGraphicsMatrix matrix; // Utworzenie obiektu wxGraphicsMatrix
        matrix.Translate(-centerX, -centerY);    // Przesunięcie do środka
        matrix.Rotate(m_rotationAngle);         // Obrót
        matrix.Scale(m_zoomFactor, m_zoomFactor); // Skalowanie
        matrix.Translate(centerX, centerY);    // Przesunięcie z powrotem
        gc->SetTransform(matrix); // Przekazanie obiektu wxGraphicsMatrix
        gc->DrawBitmap(m_image, -offsetX, -offsetY, m_image.GetWidth(), m_image.GetHeight());
        delete gc;
    }

    m_bitmap->SetBitmap(wxBitmap(transformedImage));
    UpdateSquares();
}

void GUIFrame::UpdateSquares() {
    if (!m_image.IsOk()) return;

    int squareSize = 100; // Size of the square bitmaps

    // Get the transformed image from the main bitmap
    wxBitmap currentBitmap = m_bitmap->GetBitmap();
    wxImage transformedImage = currentBitmap.ConvertToImage();

    // Calculate the center of the transformed image
    int centerX = transformedImage.GetWidth() / 2;
    int centerY = transformedImage.GetHeight() / 2;

    // Calculate the selection rectangle based on the center and square size
    wxRect selectionRect(centerX - squareSize / 4, centerY - squareSize / 4, squareSize / 2, squareSize / 2);

    // Ensure the selection rectangle is within the image bounds
    if (selectionRect.GetLeft() < 0) selectionRect.SetLeft(0);
    if (selectionRect.GetTop() < 0) selectionRect.SetTop(0);
    if (selectionRect.GetRight() > transformedImage.GetWidth()) selectionRect.SetRight(transformedImage.GetWidth());
    if (selectionRect.GetBottom() > transformedImage.GetHeight()) selectionRect.SetBottom(transformedImage.GetHeight());

    wxImage selectedArea = transformedImage.GetSubImage(selectionRect);

    // Update the square bitmaps with the selected area and interpolated images
    m_squareBitmaps[0]->SetBitmap(wxBitmap(selectedArea.Scale(squareSize, squareSize, wxIMAGE_QUALITY_NORMAL)));
    m_squareBitmaps[1]->SetBitmap(wxBitmap(HermiteInterpolation(selectedArea).Scale(squareSize, squareSize)));
    m_squareBitmaps[2]->SetBitmap(wxBitmap(MitchellInterpolation(selectedArea).Scale(squareSize, squareSize)));
    m_squareBitmaps[3]->SetBitmap(wxBitmap(BSplineInterpolation(selectedArea).Scale(squareSize, squareSize)));
    m_squareBitmaps[4]->SetBitmap(wxBitmap(LanczosInterpolation(selectedArea).Scale(squareSize, squareSize)));

    m_panel->Refresh();
}
