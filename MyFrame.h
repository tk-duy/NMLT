#ifndef MYFRAME_H
#define MYFRAME_H

#include <wx/wx.h>
#include <wx-3.0/wx/grid.h>
#include <bits/stdc++.h>
#include <wx-3.0/wx/tokenzr.h>
#include "mathplot.h"
#include <gsl/gsl_vector.h>
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
        std::pair<std::vector<double>, std::vector<double>> SelectedData();
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

// About MathPlotFit
void MyFrame::OnAbout(wxCommandEvent& event) {
    wxMessageBox("MathPlotFit is a tool for data visualization and processing.",
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

// Method of plot a graph
void MyFrame::PlotGraph(wxCommandEvent& event) {
    auto data = SelectedData();
    std::vector<double> x = data.first;
    std::vector<double> y = data.second;

    Graph* graph = new Graph(x, y);
}

std::pair<std::vector<double>, std::vector<double>> MyFrame::SelectedData() {
    std::vector<double> x, y;

    // Get the number of rows and columns in the grid
    int numRows = grid->GetNumberRows();
    int numCols = grid->GetNumberCols();

    // Iterate over all cells in the grid
    for (int row = 0; row < numRows; ++row) {
        for (int col = 0; col < numCols; ++col) {
            // Check if the cell is selected
            if (grid->IsInSelection(row, col)) {
                // Get the value of the selected cell
                wxString value = grid->GetCellValue(row, col);

                // Convert the value to double and store it in the appropriate vector
                double val;
                value.ToDouble(&val);
                if (x.size() == y.size()) {
                    x.push_back(val);
                } else {
                    y.push_back(val);
                }
            }
        }
    }

    return std::make_pair(x, y);
}


#endif