#include <wx/wx.h>
#include <wx-3.0/wx/grid.h>
#include <bits/stdc++.h>
#include <fstream>
#include <wx-3.0/wx/tokenzr.h>
// #include <wx-3.0/wx/string.h>
#include <string.h>
#include "mathplot.h"

class MyFrame : public wxFrame {
    public:
        MyFrame(const wxString& title);

    private:
        wxGrid* grid = NULL;

        void OnOpen(wxCommandEvent& event);
        void Plotting(wxCommandEvent& event);
        void OnExit(wxCommandEvent& event);
        void OnAbout(wxCommandEvent& event);
        void LoadFileToGrid(const wxString& filePath);
};

enum
{
    ID_PLOT,
};

// Implementing functions
//Re-define constructor
MyFrame::MyFrame(const wxString& title) 
    : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(800, 600)) {
    wxPanel* panel = new wxPanel(this, wxID_ANY);

    //Add menu Bar
    wxMenu *menuFile = new wxMenu;
    menuFile->Append(ID_PLOT, "&Plotting...\tCtrl-P");    
    menuFile->Append(wxID_OPEN);
    // menuFile->AppendSeparator();
    menuFile->Append(wxID_EXIT);

 
    wxMenu *menuHelp = new wxMenu;
    menuHelp->Append(wxID_ABOUT);
 
    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(menuFile, "&File");
    menuBar->Append(menuHelp, "&Help");
 
    SetMenuBar( menuBar );
 
    // Add event to menu
    Bind(wxEVT_MENU, &MyFrame::OnOpen, this, wxID_OPEN);
    Bind(wxEVT_MENU, &MyFrame::Plotting, this, ID_PLOT);
    Bind(wxEVT_MENU, &MyFrame::OnAbout, this, wxID_ABOUT);
    Bind(wxEVT_MENU, &MyFrame::OnExit, this, wxID_EXIT);

    //Add a grid
    MyFrame::grid = new wxGrid(panel, wxID_ANY);
    grid->CreateGrid(300, 300); // Create a 5x5 grid
    grid->SetDefaultCellBackgroundColour(*wxWHITE);
    grid->SetDefaultCellTextColour(*wxBLACK);

    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(grid, 1, wxEXPAND | wxALL, 10);

    panel->SetSizerAndFit(sizer);
}
// File openning method
void MyFrame::OnOpen(wxCommandEvent& event) {
    // Create a file dialog
    wxFileDialog wxFileDialog(this, "Open File", "", "", 
                            "Text Files (*.txt)|*.txt|CSV Files (*.csv)|*.csv", 
                            wxFD_OPEN | wxFD_FILE_MUST_EXIST);

    // Show the file
    if (wxFileDialog.ShowModal() == wxID_CANCEL)
        return; 

    wxString filePath = wxFileDialog.GetPath();
    LoadFileToGrid(filePath);
}
// Plotting method
void MyFrame::Plotting(wxCommandEvent& event) {
    std::vector<double> xData = {-5, -4, -3, -2, -1, 0, 1, 2, 3, 4, 5};
    std::vector<double> yData = {-5, -4, -2, -1, -4, 4, 1, 1, 7, 2, 5};
    auto minX = *std::min_element(xData.begin(), xData.end());
    auto maxX = *std::max_element(xData.begin(), xData.end());
    auto minY = *std::min_element(yData.begin(), yData.end());
    auto maxY = *std::max_element(yData.begin(), yData.end());

    wxFrame* plotFrame = new wxFrame(NULL, wxID_ANY, "Graph Plot",wxDefaultPosition, wxSize(1280,720));
    mpWindow* plotWindows = new mpWindow(plotFrame, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER);

    // Create a new plot and add it to the plot window
    mpFXYVector* plot = new mpFXYVector();
    plot->SetData(xData, yData);
    plot->SetContinuity(false);

    // Set line thickness for the plot
    // Create a pen with blue color and thickness 2
    wxPen pen(wxColour(255, 0, 0), 5, wxPENSTYLE_SOLID); 
     // Set the pen for the plot
    plot->SetPen(pen);

    plotWindows->Fit(minX, maxX, minY, maxY);
    plotWindows->AddLayer(plot);
    plotWindows->UpdateAll();

    // Add scale
    mpScaleX *sx = new mpScaleX( wxT("x axis"), 1, false);
    sx->SetAlign(mpALIGN_CENTER);
    mpScaleY *sy = new mpScaleY( wxT("y axis"), 1, false);
    sy->SetAlign(mpALIGN_CENTER);
    plotWindows->AddLayer(sx);
    plotWindows->AddLayer(sy);

    plotWindows->EnableDoubleBuffer(true);
    plotWindows->LockAspect();
    // Set plot window as the main window
    plotFrame->Show(true);    
}
// Exitting method
void MyFrame::OnExit(wxCommandEvent& event)
{
    Close(true);
}
// About mathplotfit
void MyFrame::OnAbout(wxCommandEvent& event)
{
    wxMessageBox("MathPlotFit is a tool for data processing.",
                    "About MathPlotFit", wxOK | wxICON_INFORMATION);
}
// Method of load file to grid 
void MyFrame::LoadFileToGrid(const wxString& filePath) {
    std::ifstream file(filePath.ToStdString());
    if (!file.is_open()) {
        wxMessageBox("Failed to open file!", "Error", wxOK | wxICON_ERROR);
        return;
    }

    std::string line;
    int row = 0;
    while (std::getline(file, line) && row < MyFrame::grid->GetNumberRows()) {
        wxStringTokenizer tokenizer(line, ",");
        int col = 0;
        while (tokenizer.HasMoreTokens() && col < MyFrame::grid->GetNumberCols()) {
            wxString token = tokenizer.GetNextToken();
            grid->SetCellValue(row, col, token);
            col++;
        }
        row++;
    }
    file.close();  
}
