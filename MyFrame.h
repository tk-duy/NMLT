#include <wx/wx.h>
#include <wx-3.0/wx/grid.h>
#include <iostream>
#include <fstream>
#include <wx-3.0/wx/tokenzr.h>
// #include <wx-3.0/wx/string.h>
#include <string.h>

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
MyFrame::MyFrame(const wxString& title) //Re-define constructor
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
 
    // CreateStatusBar();
    // SetStatusText("Welcome to wxWidgets!");

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

void MyFrame::Plotting(wxCommandEvent& event)
{
    wxLogMessage("Ploting a graph by wxWidgets!");
}

void MyFrame::OnExit(wxCommandEvent& event)
{
    Close(true);
}
 
void MyFrame::OnAbout(wxCommandEvent& event)
{
    wxMessageBox("This is a wxWidgets program",
                    "About MathPlotFit", wxOK | wxICON_INFORMATION);
}
 
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
