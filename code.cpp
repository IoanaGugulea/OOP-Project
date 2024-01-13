#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <map>

using namespace std;

class Step {
public:
    virtual void execute() const = 0;
    virtual ~Step() {} // Virtual destructor to ensure proper cleanup
};

class FlowExecutionException : public exception {
private:
    string stepName;

public:
    FlowExecutionException(const string& step) : stepName(step) {}

    const char* what() const noexcept override {
        return ("Error during flow execution in step: " + stepName).c_str();
    }
};

class FlowNotFoundException : public exception {
private:
    string flowName;

public:
    FlowNotFoundException(const string& name) : flowName(name) {}

    const char* what() const noexcept override {
        return ("Flow not found: " + flowName).c_str();
    }
};

// Analytics information for each step
struct StepAnalytics {
    int startedCount = 0;
    int completedCount = 0;
    int skippedCount = 0;
    int errorCount = 0;
};

class Flow {
private:
    string name;
    vector<Step*> steps;
    time_t timestamp;

    // Analytics tracking for each step
    map<Step*, StepAnalytics> analytics;

    // Flow-level analytics
    int flowStartedCount = 0;
    int flowCompletedCount = 0;
    int totalErrorsAcrossFlows = 0;

public:
    Flow(const string& n) : name(n), timestamp(time(0)) {}

    const string& getName() const {
        return name;
    }

    void addStep(Step* step) {
        steps.push_back(step);
        analytics[step]; // Initialize analytics entry for the step
    }

    bool askUserToRunStep(int stepIndex) const {
        char response;
        cout << "Do you want to run step " << stepIndex + 1 << "? (y/n): ";
        cin >> response;
        return (response == 'y' || response == 'Y');
    }

    bool askUserToSkipStep(int stepIndex) const {
        char response;
        cout << "Do you want to skip step " << stepIndex + 1 << "? (y/n): ";
        cin >> response;
        return (response == 'y' || response == 'Y');
    }

    bool askUserToCompleteStep(int stepIndex) const {
        char response;
        cout << "Have you completed the action of step " << stepIndex + 1 << "? (y/n): ";
        cin >> response;
        return (response == 'y' || response == 'Y');
    }

    void run() {
        ++flowStartedCount;

        for (size_t i = 0; i < steps.size(); ++i) {
            ++analytics[steps[i]].startedCount;

            if (askUserToRunStep(i)) {
                bool completed = false;
                while (!completed) {
                    try {
                        steps[i]->execute();
                        completed = askUserToCompleteStep(i);
                        if (completed) {
                            ++analytics[steps[i]].completedCount;
                            cout << "Step " << i + 1 << " completed." << endl;
                        }
                    } catch (const FlowExecutionException& e) {
                        ++analytics[steps[i]].errorCount;
                        cerr << "Flow Execution Error: " << e.what() << endl;
                        cout << "Retrying the step..." << endl;
                    }
                }
            } else if (askUserToSkipStep(i)) {
                ++analytics[steps[i]].skippedCount;
                cout << "Step " << i + 1 << " skipped." << endl;
            } else {
                cerr << "Invalid input. Please enter 'y' or 'n'." << endl;
                --i; // Retry the same step
            }
        }

        ++flowCompletedCount;
    }

    void printAnalytics() const {
        cout << "Flow Analytics for '" << name << "':" << endl;
        cout << "a. Flow started " << flowStartedCount << " times." << endl;
        cout << "b. Flow completed " << flowCompletedCount << " times." << endl;
        cout << "c. Step-wise analytics:" << endl;

        for (const auto& entry : analytics) {
            Step* step = entry.first;
            const StepAnalytics& stepAnalytics = entry.second;

            cout << "   Step " << step << ": Started " << stepAnalytics.startedCount
                 << " times, Completed " << stepAnalytics.completedCount
                 << " times, Skipped " << stepAnalytics.skippedCount
                 << " times, Errors " << stepAnalytics.errorCount << " times." << endl;
        }

        // Calculate and print the average number of errors per flow completed
        if (flowCompletedCount > 0) {
            double avgErrorsPerFlow = static_cast<double>(totalErrorsAcrossFlows) / flowCompletedCount;
            cout << "e. Average number of errors per flow completed: " << avgErrorsPerFlow << endl;
        } else {
            cout << "e. Average number of errors per flow completed: N/A (No completed flows)" << endl;
        }
    }

    ~Flow() {
        for (Step* step : steps) {
            delete step;
        }
    }
};

class TitleStep : public Step {
private:
    string title;
    string subtitle;

public:
    TitleStep(const string& t, const string& st) : title(t), subtitle(st) {}

    void execute() const override {
        cout << "Title: " << title << endl;
        cout << "Subtitle: " << subtitle << endl;
    }
};

class TextStep : public Step {
private:
    string title;
    string content;

public:
    TextStep(const string& t, const string& c) : title(t), content(c) {}

    void execute() const override {
        cout << "Title: " << title << endl;
        cout << "Content: " << content << endl;
    }
};

class TextInputStep : public Step {
private:
    string description;
    string textInput;

public:
    TextInputStep(const string& desc, const string& input) : description(desc), textInput(input) {}

    void execute() const override {
        if (textInput.substr(textInput.find_last_of(".") + 1) == "txt") {
            ifstream file(textInput);
            if (file.is_open()) {
                stringstream buffer;
                buffer << file.rdbuf();
                cout << "Text Input (from file): " << buffer.str() << endl;
                file.close();
            } else {
                cerr << "Error: Unable to open text file '" << textInput << "'" << endl;
                throw FlowExecutionException("TextInputStep");
            }
        } else {
            cout << "Text Input: " << textInput << endl;
        }
    }
};

class CSVInputStep : public Step {
private:
    string description;
    string csvFilePath;

public:
    CSVInputStep(const string& desc, const string& path) : description(desc), csvFilePath(path) {}

    void execute() const override {
        if (csvFilePath.substr(csvFilePath.find_last_of(".") + 1) == "csv") {
            ifstream file(csvFilePath);
            if (file.is_open()) {
                string line;
                while (getline(file, line)) {
                    cout << "CSV Input: " << line << endl;
                }
                file.close();
            } else {
                cerr << "Error: Unable to open CSV file '" << csvFilePath << "'" << endl;
                throw FlowExecutionException("CSVInputStep");
            }
        } else {
            cerr << "Error: Invalid file type for CSVInputStep" << endl;
            throw FlowExecutionException("CSVInputStep");
        }
    }
};

// create a base class for the .txt and .csv files

class FileInputStep : public Step {
private:
    string description;
    string filePath;

public:
    FileInputStep(const string& desc, const string& path) : description(desc), filePath(path) {}

    void execute() const override {
        cout << "Description: " << description << endl;

        ifstream file(filePath);
        if (file.is_open()) {
            string line;
            while (getline(file, line)) {
                cout << line << endl;
            }
            file.close();
        } else {
            cerr << "Error: Unable to open file '" << filePath << "'" << endl;
            throw FlowExecutionException("FileInputStep");
        }
    }
};

class TextFileInputStep : public FileInputStep {
public:
    TextFileInputStep(const string& desc, const string& path) : FileInputStep(desc, path) {}

    void execute() const override {
        cout << "Text File Content:" << endl;
        FileInputStep::execute();
    }
};

class CSVFileInputStep : public FileInputStep {
public:
    CSVFileInputStep(const string& desc, const string& path) : FileInputStep(desc, path) {}

    void execute() const override {
        cout << "CSV File Content:" << endl;
        FileInputStep::execute();
    }
};

class DisplayStep : public Step {
private:
    const Step* previousStep;

public:
    DisplayStep(const Step* prevStep) : previousStep(prevStep) {}

    void execute() const override {
        if (previousStep) {
            const FileInputStep* fileStep = dynamic_cast<const FileInputStep*>(previousStep);
            if (fileStep) {
                fileStep->execute(); // Display file content
            } else {
                previousStep->execute();
            }
        } else {
            throw FlowExecutionException("DisplayStep");
        }
    }
};

template<typename T>
class NumberInputStep : public Step {
private:
    string description;
    T numberInput;

public:
    NumberInputStep(const string& desc, const T& input) : description(desc), numberInput(input) {}

    void execute() const override {
        cout << "Description: " << description << endl;
        cout << "Number Input: " << numberInput << endl;
    }
};

template<typename T>
class CalculusStep : public Step {
private:
    int steps;
    vector<T> inputValues;
    string operation;

public:
    CalculusStep(int s, const vector<T>& inputs, const string& op) : steps(s), inputValues(inputs), operation(op) {}

    void execute() const override {
        if (steps <= 0 || inputValues.size() < 2) {
            cerr << "Error: Invalid configuration for CalculusStep." << endl;
            return;
        }

        T result = inputValues[0];
        for (int i = 1; i < steps && i < inputValues.size(); ++i) {
            if (operation == "+") {
                result += inputValues[i];
            } else if (operation == "-") {
                result -= inputValues[i];
            } else if (operation == "*") {
                result *= inputValues[i];
            } else if (operation == "/") {
                if (inputValues[i] != 0) {
                    result /= inputValues[i];
                } else {
                    cerr << "Error: Division by zero in CalculusStep." << endl;
                    return;
                }
            } else if (operation == "min") {
                result = min(result, inputValues[i]);
            } else if (operation == "max") {
                result = max(result, inputValues[i]);
            } else {
                cerr << "Error: Unsupported operation in CalculusStep." << endl;
                return;
            }
        }

        cout << "Calculus Result: " << result << endl;
    }
};

class OutputStep : public Step {
private:
    string fileType;
    string description;
    string content;

public:
    OutputStep(const string& type, const string& desc, const string& data) : fileType(type), description(desc), content(data) {}

    void execute() const override {
        string fileName;
        cout << "Enter the output file name (without extension): ";
        cin >> fileName;

        string filePath = fileName + "." + fileType;

        ofstream outputFile(filePath);
        if (outputFile.is_open()) {
            outputFile << "Description: " << description << endl;
            outputFile << "Content: " << content << endl;
            cout << "Output written to file: " << filePath << endl;
            outputFile.close();
        } else {
            cerr << "Error: Unable to open output file '" << filePath << "'" << endl;
            throw FlowExecutionException("OutputStep");
        }
    }
};

class System {
private:
    vector<Flow*> flows;

public:
    void createFlow(const string& name) {
        flows.push_back(new Flow(name));
    }

    void deleteFlow(const string& flowName) {
        auto it = find_if(flows.begin(), flows.end(), [flowName](const Flow* flow) {
            return flow->getName() == flowName;
        });

        if (it != flows.end()) {
            Flow* flowToDelete = *it;
            flows.erase(it);
            delete flowToDelete;
            cout << "Flow '" << flowName << "' deleted from the system." << endl;
        } else {
            throw FlowNotFoundException(flowName);
        }
    }

    // Additional function to print available flows
    void printFlows() const {
        cout << "Available Flows:" << endl;
        for (const Flow* flow : flows) {
            cout << "- " << flow->getName() << endl;
        }
    }

    const vector<Flow*>& getFlows() const {
        return flows;
    }

    ~System() {
        for (Flow* flow : flows) {
            delete flow;
        }
    }
};

class EndStep : public Step {
public:
    void execute() const override {
        // This step does nothing when executed, as it represents the end of the flow.
    }
};


int main() {
    System system;

    int choice;
    do {
        cout << "Choose an option:" << endl;
        cout << "1. Create Flow" << endl;
        cout << "2. Delete Flow" << endl;
        cout << "3. Run Flow" << endl;
        cout << "4. Print Available Flows" << endl;
        cout << "5. Exit" << endl;
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1: {
                string flowName;
                cout << "Enter the name for the new flow: ";
                cin >> flowName;

                // Create a new flow
                Flow* newFlow = new Flow(flowName);

                // Add steps to the flow
                newFlow->addStep(new TitleStep("My Title", "Subtitle"));
                newFlow->addStep(new TextStep("Text Step", "Some text content"));
                newFlow->addStep(new TextInputStep("Enter some text:", "User input text"));
                newFlow->addStep(new CSVInputStep("Enter CSV file path:", "example.csv"));
                newFlow->addStep(new NumberInputStep<int>("Enter a number:", 42));
                newFlow->addStep(new CalculusStep<int>(3, {2, 3, 4}, "+"));
                newFlow->addStep(new FileInputStep("Enter file path:", "example.txt"));
                newFlow->addStep(new TextFileInputStep("Enter text file path:", "example.txt"));
                newFlow->addStep(new DisplayStep(newFlow->getSteps().back())); // Display the last step
                newFlow->addStep(new OutputStep("txt", "Output Description", "Output Content"));
                newFlow->addStep(new EndStep());

                // Add the flow to the system
                system.createFlow(flowName);
                break;
            }
            case 2: {
                string flowName;
                cout << "Enter the name of the flow to delete: ";
                cin >> flowName;
                try {
                    system.deleteFlow(flowName);
                } catch (const exception& e) {
                    cerr << "Error: " << e.what() << endl;
                }
                break;
            }
            case 3: {
                // Choose a flow to run
                system.printFlows();
                string flowToRun;
                cout << "Enter the name of the flow to run: ";
                cin >> flowToRun;

                auto it = find_if(system.getFlows().begin(), system.getFlows().end(), [flowToRun](const Flow* flow) {
                    return flow->getName() == flowToRun;
                });

                if (it != system.getFlows().end()) {
                    Flow* flowToRunPtr = *it;
                    flowToRunPtr->run();
                } else {
                    cerr << "Error: Flow '" << flowToRun << "' not found." << endl;
                }
                break;
            }
            case 4: {
                system.printFlows();
                break;
            }
            case 5:
                cout << "Exiting program." << endl;
                break;
            default:
                cout << "Invalid choice. Try again." << endl;
        }
    } while (choice != 5);

    return 0;
}
