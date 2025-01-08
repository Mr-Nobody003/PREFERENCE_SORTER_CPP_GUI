#include <wx/wx.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <wx/filedlg.h>
#include <wx/msgdlg.h>
#include <wx/textdlg.h>
#include <wx/string.h>
using namespace std;

long long ne = 0, ns = 0;

struct ELECTIVE
{
    string name;
    long long t_cap;
};

struct Student_alloted
{
    string enrollment;
    string name;
    string alloted_subject;
};

vector<ELECTIVE> ELECTIVES;
vector<Student_alloted> ALLOTMENTS;
vector<vector<Student_alloted>> Sorted_Allotments;

void checkVectorSizes()
{
    std::cout << "Number of electives (ne): " << ne << std::endl;
    std::cout << "Number of students (ns): " << ns << std::endl;
    std::cout << "ELECTIVES vector size: " << ELECTIVES.size() << std::endl;
    std::cout << "Sorted_Allotments vector size: " << Sorted_Allotments.size() << std::endl;
}

class MyFrame : public wxFrame
{
public:
    MyFrame() : wxFrame(NULL, wxID_ANY, "Elective Allotment", wxDefaultPosition, wxSize(600, 400))
    {
        wxPanel *panel = new wxPanel(this, wxID_ANY);

        // Input Controls for electives
        new wxStaticText(panel, wxID_ANY, "Number of Electives", wxPoint(10, 10));
        electiveCountInput = new wxTextCtrl(panel, wxID_ANY, "", wxPoint(150, 10), wxSize(50, 30));

        new wxStaticText(panel, wxID_ANY, "Number of Students", wxPoint(10, 50));
        studentCountInput = new wxTextCtrl(panel, wxID_ANY, "", wxPoint(150, 50), wxSize(50, 30));

        // Buttons to input and process data
        inputButton = new wxButton(panel, wxID_ANY, "Input Electives", wxPoint(10, 100));
        Bind(wxEVT_BUTTON, &MyFrame::OnInputElectives, this, inputButton->GetId());

        processButton = new wxButton(panel, wxID_ANY, "Process Allotment", wxPoint(150, 100));
        Bind(wxEVT_BUTTON, &MyFrame::OnProcessAllotment, this, processButton->GetId());

        // Save Button
        saveButton = new wxButton(panel, wxID_ANY, "Save Alloted Data", wxPoint(300, 100));
        Bind(wxEVT_BUTTON, &MyFrame::OnSaveAllotment, this, saveButton->GetId());

        // Output display
        outputDisplay = new wxTextCtrl(panel, wxID_ANY, "", wxPoint(10, 150), wxSize(550, 200), wxTE_MULTILINE | wxTE_READONLY);
    }

private:
    wxTextCtrl *electiveCountInput;
    wxTextCtrl *studentCountInput;
    wxButton *inputButton;
    wxButton *processButton;
    wxButton *saveButton;
    wxTextCtrl *outputDisplay;
    // Function to handle input of electives
    void OnInputElectives(wxCommandEvent &event)
    {
        // Input number of electives and students
        wxString electiveCount = electiveCountInput->GetValue();
        wxString studentCount = studentCountInput->GetValue();

        ne = wxAtoi(electiveCount); // Number of electives
        ns = wxAtoi(studentCount);  // Number of students

        // Ensure that we got valid values for ne and ns
        if (ne <= 0 || ns <= 0)
        {
            wxMessageBox("Invalid number of electives or students.", "Error", wxICON_ERROR);
            return;
        }

        // Resize vectors based on the number of electives and students
        ELECTIVES.resize(ne);
        Sorted_Allotments.resize(ne);
        ALLOTMENTS.resize(ns); // Ensure we resize ALLOTMENTS vector to ns

        checkVectorSizes(); // Debug output to verify vector sizes

        // Prompt user to input names and capacities for electives
        wxTextEntryDialog *dialog = new wxTextEntryDialog(this, "Enter the names of the electives separated by commas:", "Elective Names");
        if (dialog->ShowModal() == wxID_OK)
        {
            wxString electiveNames = dialog->GetValue();

            // Convert wxString to std::string for tokenization
            std::string names = electiveNames.ToStdString();
            std::istringstream stream(names);
            std::string token;
            std::vector<std::string> namesArray;

            // Tokenize string using comma as delimiter
            while (std::getline(stream, token, ','))
            {
                namesArray.push_back(token);
            }

            // Check if the number of tokens matches the number of electives (ne)
            if (namesArray.size() == ne)
            {
                for (size_t i = 0; i < namesArray.size(); ++i)
                {
                    ELECTIVES[i].name = namesArray[i];

                    wxString message = wxString::Format("Enter the capacity for elective: %s", namesArray[i]);
                    wxTextEntryDialog capacityDialog(this, message, "Elective Capacity");
                    if (capacityDialog.ShowModal() == wxID_OK)
                    {
                        ELECTIVES[i].t_cap = wxAtoi(capacityDialog.GetValue());
                    }

                    // Debugging output to check if electives are correctly updated
                    std::cout << "Elective: " << ELECTIVES[i].name << ", Capacity: " << ELECTIVES[i].t_cap << std::endl;
                }

                outputDisplay->AppendText("Elective names and capacities entered.\n");
                // Now open the file dialog to select the input file
                OpenFileDialog();
            }
            else
            {
                wxMessageBox("The number of electives does not match the number entered.", "Error", wxICON_ERROR);
            }
        }
        dialog->Destroy();
    }
    // Function to open a file dialog to select the input file
    void OpenFileDialog()
    {
        wxFileDialog openFileDialog(this, "Open File", "", "", "Text files (*.txt)|*.txt", wxFD_OPEN | wxFD_FILE_MUST_EXIST);

        if (openFileDialog.ShowModal() == wxID_OK)
        {
            wxString filePath = openFileDialog.GetPath();
            std::cout << "Selected file: " << filePath << std::endl;

            // Process file after selecting
            input_file_handler(filePath.ToStdString()); // Convert to std::string
        }
    }
    // Function to handle input file processing
    void input_file_handler(const std::string &inputFileName)
    {
        std::ifstream in(inputFileName);
        if (!in.is_open())
        {
            wxMessageBox("Error opening input file: " + wxString(inputFileName), "Error", wxICON_ERROR);
            return;
        }

        std::string line;
        long long i = 0;

        while (std::getline(in, line) && i < ns)
        {
            std::istringstream info(line);
            std::string word, enroll, name;
            std::vector<std::string> choices(ne, ""); // Holds preferences
            long long mc = 0;                         // INDEX of preferences

            while (info >> word)
            {
                if (enroll_checker(word))
                {
                    enroll = word;
                    ALLOTMENTS[i].enrollment = enroll;
                }
                else if (name_checker(word))
                {
                    if (!name.empty())
                        name += " ";
                    name += word;
                    ALLOTMENTS[i].name = name;
                }
                else if (choice_checker(word))
                {
                    if (mc < ne)
                    {
                        // CHECK FOR DUPLICATE CHOICES
                        if (find(choices.begin(), choices.end(), word) != choices.end())
                        {
                            std::cout << "Duplicate choice in line: " << line << std::endl;
                            i++;
                            continue;
                        }
                        choices[mc] = word;
                        mc++;
                    }
                }
            }

            // Validate choices
            if (mc < ne)
            {
                std::cout << "Invalid or missing choice in line: " << line << std::endl;
                i++;
                continue;
            }

            // Allot subject
            ALLOTMENTS[i].alloted_subject = alloter(choices);

            // Debug: Print initial allotments
            std::cout << ALLOTMENTS[i].enrollment << "\t" << ALLOTMENTS[i].name << "\t"
                      << ALLOTMENTS[i].alloted_subject << std::endl;

            i++;
        }

        in.close();
    }
    // Function to handle displaying the processed allotments
    void OnProcessAllotment(wxCommandEvent &event)
    {
        if (ALLOTMENTS.empty())
        {
            // If no data has been processed yet, inform the user
            outputDisplay->AppendText("No data has been processed yet. Please load the input file first.\n");
            return;
        }

        // Display the processed allotments
        outputDisplay->Clear(); // Clear previous content
        outputDisplay->AppendText("Processed Allotments:\n");
        for (const auto &allotment : ALLOTMENTS)
        {
            outputDisplay->AppendText(wxString::Format("%s - %s - %s\n",
                                                       allotment.enrollment, allotment.name, allotment.alloted_subject));
        }
        outputDisplay->AppendText("Displaying previously processed data.\n");
    }
    // Function to handle saving the allotments to a file
    void OnSaveAllotment(wxCommandEvent &event)
    {
        // Open file dialog to select where to save the output
        wxFileDialog saveFileDialog(this, "Save Allotment File", "", "",
                                    "Text files (*.txt)|*.txt", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

        if (saveFileDialog.ShowModal() == wxID_OK)
        {
            wxString filePath = saveFileDialog.GetPath();

            // Call the function to save data
            SaveDataToFile(filePath.ToStdString());
        }
    }
    // Function to save allotments sorted by subject, then enrollment
    void SaveDataToFile(const std::string &outputFileName)
    {
        std::ofstream outFile(outputFileName);

        if (!outFile.is_open())
        {
            wxMessageBox("Error saving the file.", "Error", wxICON_ERROR);
            return;
        }

        // Write the header for the output
        outFile << "Subject\tEnrollment\tName\n";
        outFile << "----------------------------------------\n";

        // Sort the allotments by subject, then enrollment
        std::sort(ALLOTMENTS.begin(), ALLOTMENTS.end(), [](const Student_alloted &a, const Student_alloted &b)
                  {
                      if (a.alloted_subject == b.alloted_subject)
                      {
                          return a.enrollment < b.enrollment; // Sort by enrollment within the same subject
                      }
                      return a.alloted_subject < b.alloted_subject; // Sort by subject
                  });

        // Write the sorted data to the file
        std::string currentSubject;
        for (const auto &student : ALLOTMENTS)
        {
            // Add a subject header when the subject changes
            if (student.alloted_subject != currentSubject)
            {
                currentSubject = student.alloted_subject;
                outFile << "\nSubject: " << currentSubject << "\n";
            }

            // Write student details under the current subject
            outFile << student.enrollment << "\t" << student.name << "\n";
        }

        outFile.close();
        wxMessageBox("Allotments saved successfully.", "Success", wxICON_INFORMATION);
    }

    string alloter(vector<string> &choices)
    {
        for (const auto &choice : choices)
        {
            std::cout << "Checking choice: " << choice << std::endl;

            auto it = find_if(ELECTIVES.begin(), ELECTIVES.end(), [&choice](const ELECTIVE &elective)
                              { return elective.name == choice; });

            if (it != ELECTIVES.end())
            {
                std::cout << "Elective found: " << it->name << " with capacity: " << it->t_cap << std::endl;

                if (it->t_cap > 0)
                {
                    it->t_cap--;
                    std::cout << "Allotted: " << it->name << " (Remaining capacity: " << it->t_cap << ")" << std::endl;
                    return it->name;
                }
                else
                {
                    std::cout << "No capacity left for: " << it->name << std::endl;
                }
            }
            else
            {
                std::cout << "Elective not found for choice: " << choice << std::endl;
            }
        }

        std::cout << "All subjects are full for this student." << std::endl;
        return "All subjects are full";
    }

    bool choice_checker(const string &subject)
    {
        for (const auto &elective : ELECTIVES)
        {
            if (elective.name == subject)
            {
                return true;
            }
        }
        return false;
    }

    bool enroll_checker(const string &text)
    {
        return text.length() == 8 && isdigit(text[0]) && isdigit(text[1]);
    }

    bool name_checker(const string &text)
    {
        return isalpha(text[0]) && !choice_checker(text);
    }
};

// wxWidgets Application
class MyApp : public wxApp
{
public:
    virtual bool OnInit()
    {
        MyFrame *frame = new MyFrame();
        frame->Show(true);
        return true;
    }
};

wxIMPLEMENT_APP(MyApp);
