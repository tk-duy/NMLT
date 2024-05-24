#ifndef SHEET_H
#define SHEET_H

#include <wx/wx.h>
#include <wx-3.0/wx/grid.h>
#include <bits/stdc++.h>
#include <wx-3.0/wx/tokenzr.h>
#include "mathplot.h"
#include <gsl/gsl_vector.h>
#include <gsl/gsl_multifit_nlin.h>
#include "Graph.h"

class Sheet : public wxFrame {
    private:
        wxGrid* grid = NULL;

    public:
        Sheet(const wxString& title);
        void OnOpen(wxCommandEvent& event);
        void Save(wxCommandEvent& event);
        void PlotGraph(wxCommandEvent& event);
        void OnExit(wxCommandEvent& event);
        void OnAbout(wxCommandEvent& event);
        void LoadFileToGrid(const wxString& filePath);
        std::pair<std::vector<double>, std::vector<double>> SelectedData();
        void Export(wxGrid* grid, const wxString& filepath);
};

enum
{
    ID_SAVE,
    ID_PLOT,
};

// Implementing functions
//Re-define constructor
Sheet::Sheet(const wxString& title) 
    : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(640, 960)) {
    wxPanel* panel = new wxPanel(this, wxID_ANY);

    //Add menu Bar
    wxMenu *menuFile = new wxMenu;
    menuFile->Append(wxID_OPEN);
    menuFile->Append(ID_SAVE, "&Save as...\tCtrl-S");
    menuFile->Append(ID_PLOT, "&Plotting\tCtrl-P");    
    menuFile->Append(wxID_EXIT);

 
    wxMenu *menuHelp = new wxMenu;
    menuHelp->Append(wxID_ABOUT);
 
    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(menuFile, "&File");
    menuBar->Append(menuHelp, "&Help");
 
    SetMenuBar(menuBar);
 
    // Add event to menu
    Bind(wxEVT_MENU, &Sheet::OnOpen, this, wxID_OPEN);
    Bind(wxEVT_MENU, &Sheet::Save, this, ID_SAVE);
    Bind(wxEVT_MENU, &Sheet::PlotGraph, this, ID_PLOT);
    Bind(wxEVT_MENU, &Sheet::OnExit, this, wxID_EXIT);
    Bind(wxEVT_MENU, &Sheet::OnAbout, this, wxID_ABOUT);



    //Add a grid
    Sheet::grid = new wxGrid(panel, wxID_ANY);
    grid->CreateGrid(100, 50); // Create a 100x50 grid
    grid->SetDefaultCellBackgroundColour(*wxWHITE);
    grid->SetDefaultCellTextColour(*wxBLACK);

    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(grid, 1, wxEXPAND | wxALL, 10);

    panel->SetSizerAndFit(sizer);
}

// File openning method
void Sheet::OnOpen(wxCommandEvent& event) {
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

// Save method
void Sheet::Save(wxCommandEvent& event) {
    wxFileDialog saveFileDialog(grid->GetParent(), _("Save CSV file"), "", "",
                                "CSV files (*.csv)|*.csv|TXT files (*.txt)|*.txt", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

    if (saveFileDialog.ShowModal() == wxID_CANCEL) {
        return; // the user changed idea...
    }

    wxString filePath = saveFileDialog.GetPath();
    Export(grid, filePath);
}


// Exitting method
void Sheet::OnExit(wxCommandEvent& event)
{
    Close(true);
}

// About MathPlotFit
void Sheet::OnAbout(wxCommandEvent& event) {
    wxMessageBox("MathPlotFit is a tool for data visualization and analysis.",
                    "About MathPlotFit", wxOK | wxICON_INFORMATION);
}

// Method of load file to grid 
void Sheet::LoadFileToGrid(const wxString& filePath) {
    std::ifstream file(filePath.ToStdString());
    if (!file.is_open()) {
        wxMessageBox("Failed to open file!", "Error", wxOK | wxICON_ERROR);
        return;
    }

    std::string line;
    int numRows = grid->GetNumberRows();
    int numCols = grid->GetNumberCols();

    for (int row = 0; row < numRows; ++row) {
        for (int col = 0; col < numCols; ++col) {
            if(grid->GetCellValue(row, col).IsEmpty()) continue;
            grid->SetCellValue(row, col, "");
        }
    }

    int row = 0;
    while (std::getline(file, line) && row < numRows) {
        wxStringTokenizer tokenizer(line, ",");
        int col = 0;
        while (tokenizer.HasMoreTokens() && col < numCols) {
            wxString token = tokenizer.GetNextToken();
            grid->SetCellValue(row, col, token);
            col++;
        }
        row++;
    }
    file.close();  
}

// Method of plot a graph
void Sheet::PlotGraph(wxCommandEvent& event) {
    auto data = SelectedData();
    std::vector<double> x = data.first;
    std::vector<double> y = data.second;
    try
    {
        /* Check if vectors is empty: */
        if(x.empty() || y.empty())
            throw std::runtime_error("Data is invalid!");
        else if(x.size() != y.size())
            throw std::runtime_error("Data is not consistent!");
        else 
            Graph* graph = new Graph(x, y);
    }
    catch(const std::exception& e)
    {
        wxMessageBox("The selected data is invalid (empty or not consistent), please try again.",
                        "Plotting error", wxOK | wxICON_INFORMATION);
        return;
    }
}

// To select data from the sheet
std::pair<std::vector<double>, std::vector<double>> Sheet::SelectedData() {
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
                if(value.IsEmpty()) {return std::make_pair(x, y);} // To select by columns
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

// Method of export file
void Sheet::Export(wxGrid* grid, const wxString& filePath) {
    int numRows = grid->GetNumberRows();
    int numCols = grid->GetNumberCols();
    
    std::ofstream file;
    file.open(filePath.ToStdString());

    if (!file.is_open()) {
        wxLogError("Cannot open file '%s' for writing.", filePath);
        return;
    }

    for (int row = 0; row < numRows; ++row) {
        std::stringstream ss;
        for (int col = 0; col < numCols; ++col) {
            wxString value = grid->GetCellValue(row, col);
            if(value.IsEmpty()) continue;
            ss << value.ToStdString();
            if (col < numCols - 1) {
                ss << ",";
            }
        }
        file << ss.str() << "\n";
    }

    file.close();

    if (!file.good()) {
        wxLogError("Error occurred while writing to file '%s'.", filePath);
    } else {
        wxLogMessage("Successfully exported data to '%s'.", filePath);
    }
}

#endif
