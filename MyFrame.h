#include <wx/wx.h>
#include <wx-3.0/wx/grid.h>
#include <bits/stdc++.h>
#include <wx-3.0/wx/tokenzr.h>
#include "mathplot.h"
#include <gsl/gsl_vector.h>
#include <gsl/gsl_multifit.h>
#include <gsl/gsl_multifit_nlin.h>
#include "Graph.h"

class MyFrame : public wxFrame {
    public:
        MyFrame(const wxString& title);

    private:
        wxGrid* grid = NULL;

        void OnOpen(wxCommandEvent& event);
        void PlotGraph(wxCommandEvent& event);
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
    menuFile->Append(wxID_OPEN);    
    menuFile->Append(ID_PLOT, "&Plotting\tCtrl-P");    
    menuFile->Append(wxID_EXIT);

 
    wxMenu *menuHelp = new wxMenu;
    menuHelp->Append(wxID_ABOUT);
 
    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(menuFile, "&File");
    menuBar->Append(menuHelp, "&Help");
 
    SetMenuBar(menuBar);
 
    // Add event to menu
    Bind(wxEVT_MENU, &MyFrame::OnOpen, this, wxID_OPEN);
    Bind(wxEVT_MENU, &MyFrame::PlotGraph, this, ID_PLOT);
    Bind(wxEVT_MENU, &MyFrame::OnExit, this, wxID_EXIT);
    Bind(wxEVT_MENU, &MyFrame::OnAbout, this, wxID_ABOUT);


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

// Exitting method
void MyFrame::OnExit(wxCommandEvent& event)
{
    Close(true);
}
// About mathplotfit
void MyFrame::OnAbout(wxCommandEvent& event) {
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

void MyFrame::PlotGraph(wxCommandEvent& event) {
    std::vector<double> x = {0, 1, 2, 3, 4, 5};
    std::vector<double> y = {1, 3, 5, 7, 9, 11};

    Graph* graph = new Graph(x, y);
}