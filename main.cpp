// ============================================
// DIGITAL ATTENDANCE SYSTEM
// EEE227 - Capstone Project
// HND Electrical Engineering
// ============================================

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>
#include <ctime>
#include <sstream>
#include <algorithm>

using namespace std;

// ============================================
// CLASSES DEFINITIONS
// ============================================

// Student Class
class Student {
private:
    string name;
    string indexNumber;
    string department;
    int year;

public:
    // Constructors
    Student() : name(""), indexNumber(""), department(""), year(0) {}
    
    Student(string n, string idx, string dept, int yr) {
        name = n;
        indexNumber = idx;
        department = dept;
        year = yr;
    }
    
    // Getters
    string getName() const { return name; }
    string getIndexNumber() const { return indexNumber; }
    string getDepartment() const { return department; }
    int getYear() const { return year; }
    
    // Setters
    void setName(string n) { name = n; }
    void setIndexNumber(string idx) { indexNumber = idx; }
    
    // Display student info
    void display() const {
        cout << left << setw(15) << indexNumber 
             << setw(25) << name 
             << setw(15) << department 
             << "Year " << year << endl;
    }
    
    // Convert to string for file storage
    string toString() const {
        return indexNumber + "," + name + "," + department + "," + to_string(year);
    }
    
    // Create from string (file loading)
    static Student fromString(const string& data) {
        stringstream ss(data);
        string idx, name, dept, yrStr;
        
        getline(ss, idx, ',');
        getline(ss, name, ',');
        getline(ss, dept, ',');
        getline(ss, yrStr, ',');
        
        return Student(name, idx, dept, stoi(yrStr));
    }
};

// Attendance Record Class
class AttendanceRecord {
private:
    string studentIndex;
    string status; // "Present", "Absent", "Late"
    time_t timestamp;

public:
    AttendanceRecord() : studentIndex(""), status("Absent"), timestamp(time(0)) {}
    
    AttendanceRecord(string idx, string stat) {
        studentIndex = idx;
        status = stat;
        timestamp = time(0);
    }
    
    // Getters
    string getStudentIndex() const { return studentIndex; }
    string getStatus() const { return status; }
    time_t getTimestamp() const { return timestamp; }
    
    // Set status
    void setStatus(string stat) { status = stat; }
    
    // Display record
    void display(const vector<Student>& students) const {
        // Find student name
        string studentName = "Unknown";
        for (const auto& student : students) {
            if (student.getIndexNumber() == studentIndex) {
                studentName = student.getName();
                break;
            }
        }
        
        cout << left << setw(15) << studentIndex 
             << setw(25) << studentName 
             << setw(10) << status << endl;
    }
    
    // For file storage
    string toString() const {
        return studentIndex + "," + status + "," + to_string(timestamp);
    }
    
    static AttendanceRecord fromString(const string& data) {
        stringstream ss(data);
        string idx, stat, timeStr;
        
        getline(ss, idx, ',');
        getline(ss, stat, ',');
        getline(ss, timeStr, ',');
        
        return AttendanceRecord(idx, stat);
    }
};

// Attendance Session Class
class AttendanceSession {
private:
    string courseCode;
    string date;
    string startTime;
    int duration; // in hours
    vector<AttendanceRecord> records;

public:
    AttendanceSession() : courseCode(""), date(""), startTime(""), duration(1) {}
    
    AttendanceSession(string code, string d, string time, int dur) {
        courseCode = code;
        date = d;
        startTime = time;
        duration = dur;
    }
    
    // Getters
    string getCourseCode() const { return courseCode; }
    string getDate() const { return date; }
    string getStartTime() const { return startTime; }
    int getDuration() const { return duration; }
    
    // Add attendance record
    void addRecord(const AttendanceRecord& record) {
        records.push_back(record);
    }
    
    // Update attendance for a student
    bool updateAttendance(string indexNumber, string newStatus) {
        for (auto& record : records) {
            if (record.getStudentIndex() == indexNumber) {
                record.setStatus(newStatus);
                return true;
            }
        }
        return false;
    }
    
    // Check if student already has record
    bool hasStudent(string indexNumber) const {
        for (const auto& record : records) {
            if (record.getStudentIndex() == indexNumber) {
                return true;
            }
        }
        return false;
    }
    
    // Display all attendance records
    void displayAttendance(const vector<Student>& students) const {
        cout << "\n==========================================\n";
        cout << "ATTENDANCE: " << courseCode << " - " << date << " " << startTime << " (" << duration << "hr)\n";
        cout << "==========================================\n";
        cout << left << setw(15) << "Index No." << setw(25) << "Student Name" << setw(10) << "Status" << endl;
        cout << "------------------------------------------\n";
        
        for (const auto& record : records) {
            record.display(students);
        }
        
        // Display summary
        int present = 0, absent = 0, late = 0;
        for (const auto& record : records) {
            if (record.getStatus() == "Present") present++;
            else if (record.getStatus() == "Absent") absent++;
            else if (record.getStatus() == "Late") late++;
        }
        
        cout << "------------------------------------------\n";
        cout << "SUMMARY: Present: " << present << ", Absent: " << absent 
             << ", Late: " << late << ", Total: " << records.size() << endl;
        cout << "==========================================\n";
    }
    
    // Generate filename for storage
    string getFileName() const {
        return "session_" + courseCode + "_" + date + ".txt";
    }
    
    // Save session to file
    bool saveToFile() const {
        string filename = getFileName();
        ofstream file(filename);
        
        if (!file.is_open()) return false;
        
        // Save session metadata
        file << courseCode << endl;
        file << date << endl;
        file << startTime << endl;
        file << duration << endl;
        file << records.size() << endl;
        
        // Save attendance records
        for (const auto& record : records) {
            file << record.toString() << endl;
        }
        
        file.close();
        return true;
    }
    
    // Load session from file
    static AttendanceSession loadFromFile(const string& filename) {
        ifstream file(filename);
        AttendanceSession session;
        
        if (!file.is_open()) return session;
        
        string line;
        
        // Read metadata
        getline(file, session.courseCode);
        getline(file, session.date);
        getline(file, session.startTime);
        file >> session.duration;
        
        int recordCount;
        file >> recordCount;
        file.ignore(); // consume newline
        
        // Read records
        for (int i = 0; i < recordCount; i++) {
            getline(file, line);
            session.records.push_back(AttendanceRecord::fromString(line));
        }
        
        file.close();
        return session;
    }
};

// ============================================
// ATTENDANCE SYSTEM CLASS (Main Application)
// ============================================

class AttendanceSystem {
private:
    vector<Student> students;
    vector<AttendanceSession> sessions;
    const string STUDENT_FILE = "students.txt";

public:
    // Constructor - load data on startup
    AttendanceSystem() {
        loadStudents();
    }
    
    // ========== STUDENT MANAGEMENT ==========
    
    // Register new student
    void registerStudent() {
        system("cls");
        cout << "\n=== REGISTER NEW STUDENT ===\n";
        
        string name, index, dept;
        int year;
        
        cout << "Enter Student Name: ";
        cin.ignore();
        getline(cin, name);
        
        cout << "Enter Index Number: ";
        getline(cin, index);
        
        // Check if index already exists
        for (const auto& s : students) {
            if (s.getIndexNumber() == index) {
                cout << "Error: Index number already exists!\n";
                system("pause");
                return;
            }
        }
        
        cout << "Enter Department: ";
        getline(cin, dept);
        
        cout << "Enter Year (1-3): ";
        cin >> year;
        
        students.push_back(Student(name, index, dept, year));
        saveStudents();
        
        cout << "\nStudent registered successfully!\n";
        system("pause");
    }
    
    // View all students
    void viewAllStudents() {
        system("cls");
        cout << "\n=== ALL REGISTERED STUDENTS ===\n";
        
        if (students.empty()) {
            cout << "No students registered yet.\n";
            system("pause");
            return;
        }
        
        cout << left << setw(15) << "Index No." 
             << setw(25) << "Name" 
             << setw(15) << "Department" 
             << "Year" << endl;
        cout << "------------------------------------------------------------\n";
        
        for (const auto& student : students) {
            student.display();
        }
        
        cout << "\nTotal Students: " << students.size() << endl;
        system("pause");
    }
    
    // Search student by index
    void searchStudent() {
        system("cls");
        cout << "\n=== SEARCH STUDENT ===\n";
        
        if (students.empty()) {
            cout << "No students registered yet.\n";
            system("pause");
            return;
        }
        
        string index;
        cout << "Enter Index Number to search: ";
        cin >> index;
        
        bool found = false;
        for (const auto& student : students) {
            if (student.getIndexNumber() == index) {
                cout << "\nStudent Found:\n";
                cout << "----------------------------------------\n";
                cout << "Name: " << student.getName() << endl;
                cout << "Index: " << student.getIndexNumber() << endl;
                cout << "Department: " << student.getDepartment() << endl;
                cout << "Year: " << student.getYear() << endl;
                found = true;
                break;
            }
        }
        
        if (!found) {
            cout << "Student with index " << index << " not found.\n";
        }
        
        system("pause");
    }
    
    // ========== SESSION MANAGEMENT ==========
    
    // Create new lecture session
    void createSession() {
        system("cls");
        cout << "\n=== CREATE LECTURE SESSION ===\n";
        
        if (students.empty()) {
            cout << "Cannot create session: No students registered.\n";
            system("pause");
            return;
        }
        
        string courseCode, date, startTime;
        int duration;
        
        cout << "Enter Course Code: ";
        cin >> courseCode;
        
        cout << "Enter Date (YYYY_MM_DD): ";
        cin >> date;
        
        cout << "Enter Start Time (HH:MM): ";
        cin >> startTime;
        
        cout << "Enter Duration (hours): ";
        cin >> duration;
        
        AttendanceSession newSession(courseCode, date, startTime, duration);
        
        // Initialize attendance records for all students (default Absent)
        for (const auto& student : students) {
            newSession.addRecord(AttendanceRecord(student.getIndexNumber(), "Absent"));
        }
        
        sessions.push_back(newSession);
        newSession.saveToFile();
        
        cout << "\nSession created successfully! Added " << students.size() << " students.\n";
        system("pause");
    }
    
    // Mark attendance for a session
    void markAttendance() {
        system("cls");
        cout << "\n=== MARK ATTENDANCE ===\n";
        
        if (sessions.empty()) {
            cout << "No sessions available. Create a session first.\n";
            system("pause");
            return;
        }
        
        // Display available sessions
        cout << "Available Sessions:\n";
        for (size_t i = 0; i < sessions.size(); i++) {
            cout << i + 1 << ". " << sessions[i].getCourseCode() 
                 << " - " << sessions[i].getDate() 
                 << " @ " << sessions[i].getStartTime() << endl;
        }
        
        int choice;
        cout << "\nSelect session (1-" << sessions.size() << "): ";
        cin >> choice;
        
        if (choice < 1 || choice > sessions.size()) {
            cout << "Invalid choice!\n";
            system("pause");
            return;
        }
        
        AttendanceSession& session = sessions[choice - 1];
        
        // Mark attendance for each student
        for (const auto& student : students) {
            system("cls");
            cout << "\n=== MARKING ATTENDANCE ===\n";
            cout << "Session: " << session.getCourseCode() << " - " << session.getDate() << "\n\n";
            cout << "Student: " << student.getName() << " (" << student.getIndexNumber() << ")\n";
            cout << "1. Present\n";
            cout << "2. Absent\n";
            cout << "3. Late\n";
            cout << "Enter status (1-3): ";
            
            int status;
            cin >> status;
            
            string statusStr;
            switch(status) {
                case 1: statusStr = "Present"; break;
                case 2: statusStr = "Absent"; break;
                case 3: statusStr = "Late"; break;
                default: statusStr = "Absent";
            }
            
            session.updateAttendance(student.getIndexNumber(), statusStr);
        }
        
        // Save updated session
        session.saveToFile();
        
        cout << "\nAttendance marked successfully!\n";
        system("pause");
    }
    
    // Update attendance record
    void updateAttendance() {
        system("cls");
        cout << "\n=== UPDATE ATTENDANCE RECORD ===\n";
        
        if (sessions.empty()) {
            cout << "No sessions available.\n";
            system("pause");
            return;
        }
        
        // Display sessions
        for (size_t i = 0; i < sessions.size(); i++) {
            cout << i + 1 << ". " << sessions[i].getCourseCode() 
                 << " - " << sessions[i].getDate() << endl;
        }
        
        int choice;
        cout << "\nSelect session: ";
        cin >> choice;
        
        if (choice < 1 || choice > sessions.size()) {
            cout << "Invalid choice!\n";
            system("pause");
            return;
        }
        
        AttendanceSession& session = sessions[choice - 1];
        
        string index, newStatus;
        cout << "Enter student index number: ";
        cin >> index;
        
        cout << "Enter new status (Present/Absent/Late): ";
        cin >> newStatus;
        
        if (session.updateAttendance(index, newStatus)) {
            session.saveToFile();
            cout << "Attendance updated successfully!\n";
        } else {
            cout << "Student not found in this session!\n";
        }
        
        system("pause");
    }
    
    // ========== REPORTS ==========
    
    // Display attendance for a session
    void viewSessionAttendance() {
        system("cls");
        cout << "\n=== VIEW SESSION ATTENDANCE ===\n";
        
        if (sessions.empty()) {
            cout << "No sessions available.\n";
            system("pause");
            return;
        }
        
        // Display sessions
        for (size_t i = 0; i < sessions.size(); i++) {
            cout << i + 1 << ". " << sessions[i].getCourseCode() 
                 << " - " << sessions[i].getDate() << endl;
        }
        
        int choice;
        cout << "\nSelect session: ";
        cin >> choice;
        
        if (choice < 1 || choice > sessions.size()) {
            cout << "Invalid choice!\n";
            system("pause");
            return;
        }
        
        sessions[choice - 1].displayAttendance(students);
        system("pause");
    }
    
    // Load session from file
    void loadSessionFromFile() {
        system("cls");
        cout << "\n=== LOAD SESSION FROM FILE ===\n";
        
        string filename;
        cout << "Enter filename to load: ";
        cin >> filename;
        
        AttendanceSession loaded = AttendanceSession::loadFromFile(filename);
        
        if (loaded.getCourseCode().empty()) {
            cout << "Failed to load session. File may not exist.\n";
        } else {
            sessions.push_back(loaded);
            cout << "Session loaded successfully!\n";
        }
        
        system("pause");
    }
    
    // ========== FILE HANDLING ==========
    
    // Save students to file
    void saveStudents() {
        ofstream file(STUDENT_FILE);
        
        if (!file.is_open()) {
            cout << "Warning: Could not save students to file.\n";
            return;
        }
        
        for (const auto& student : students) {
            file << student.toString() << endl;
        }
        
        file.close();
    }
    
    // Load students from file
    void loadStudents() {
        ifstream file(STUDENT_FILE);
        
        if (!file.is_open()) {
            return; // File doesn't exist yet
        }
        
        string line;
        while (getline(file, line)) {
            if (!line.empty()) {
                students.push_back(Student::fromString(line));
            }
        }
        
        file.close();
    }
    
    // Load all sessions from files
    void loadAllSessions() {
        // This would need directory listing - simplified version
        // In real implementation, you'd scan directory for session_*.txt files
        cout << "Sessions are loaded on-demand via 'Load Session' option.\n";
    }
    
    // ========== MAIN MENU ==========
    
    void displayMenu() {
        cout << "\n==========================================\n";
        cout << "   DIGITAL ATTENDANCE SYSTEM - EEE227\n";
        cout << "==========================================\n";
        cout << "1. Student Management\n";
        cout << "2. Session Management\n";
        cout << "3. Mark Attendance\n";
        cout << "4. View Reports\n";
        cout << "5. File Operations\n";
        cout << "6. Exit\n";
        cout << "==========================================\n";
        cout << "Students: " << students.size() << " | Sessions: " << sessions.size() << endl;
        cout << "Enter choice: ";
    }
    
    void studentManagementMenu() {
        int choice;
        do {
            system("cls");
            cout << "\n=== STUDENT MANAGEMENT ===\n";
            cout << "1. Register New Student\n";
            cout << "2. View All Students\n";
            cout << "3. Search Student by Index\n";
            cout << "4. Back to Main Menu\n";
            cout << "Enter choice: ";
            cin >> choice;
            
            switch(choice) {
                case 1: registerStudent(); break;
                case 2: viewAllStudents(); break;
                case 3: searchStudent(); break;
                case 4: break;
                default: cout << "Invalid choice!\n"; system("pause");
            }
        } while (choice != 4);
    }
    
    void sessionManagementMenu() {
        int choice;
        do {
            system("cls");
            cout << "\n=== SESSION MANAGEMENT ===\n";
            cout << "1. Create New Lecture Session\n";
            cout << "2. Update Attendance Record\n";
            cout << "3. Back to Main Menu\n";
            cout << "Enter choice: ";
            cin >> choice;
            
            switch(choice) {
                case 1: createSession(); break;
                case 2: updateAttendance(); break;
                case 3: break;
                default: cout << "Invalid choice!\n"; system("pause");
            }
        } while (choice != 3);
    }
    
    void reportsMenu() {
        int choice;
        do {
            system("cls");
            cout << "\n=== REPORTS ===\n";
            cout << "1. View Session Attendance\n";
            cout << "2. Back to Main Menu\n";
            cout << "Enter choice: ";
            cin >> choice;
            
            switch(choice) {
                case 1: viewSessionAttendance(); break;
                case 2: break;
                default: cout << "Invalid choice!\n"; system("pause");
            }
        } while (choice != 2);
    }
    
    void fileOperationsMenu() {
        int choice;
        do {
            system("cls");
            cout << "\n=== FILE OPERATIONS ===\n";
            cout << "1. Load Session from File\n";
            cout << "2. Save All Data (Auto-saves on changes)\n";
            cout << "3. Back to Main Menu\n";
            cout << "Enter choice: ";
            cin >> choice;
            
            switch(choice) {
                case 1: loadSessionFromFile(); break;
                case 2: saveStudents(); cout << "Data saved!\n"; system("pause"); break;
                case 3: break;
                default: cout << "Invalid choice!\n"; system("pause");
            }
        } while (choice != 3);
    }
    
    void run() {
        int choice;
        do {
            system("cls");
            displayMenu();
            cin >> choice;
            
            switch(choice) {
                case 1: studentManagementMenu(); break;
                case 2: sessionManagementMenu(); break;
                case 3: markAttendance(); break;
                case 4: reportsMenu(); break;
                case 5: fileOperationsMenu(); break;
                case 6: 
                    saveStudents();
                    cout << "Thank you for using Digital Attendance System!\n";
                    break;
                default: 
                    cout << "Invalid choice! Please try again.\n";
                    system("pause");
            }
        } while (choice != 6);
    }
};

// ============================================
// MAIN FUNCTION
// ============================================

int main() {
    // Set console to handle UTF-8 (for Windows)
    system("chcp 65001 > nul");
    
    cout << "\n";
    cout << "╔════════════════════════════════════════╗\n";
    cout << "║    DIGITAL ATTENDANCE SYSTEM v1.0     ║\n";
    cout << "║    EEE227 - HND Electrical Engineering ║\n";
    cout << "║    Mid Semester Capstone Project       ║\n";
    cout << "╚════════════════════════════════════════╝\n";
    
    AttendanceSystem system;
    system.run();
    
    return 0;
}