
#pragma once

#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>
#include <print>
#include <cstring>
#include <cerrno>

using namespace std;

typedef enum log_level_t {
    OFF = 0,
    DEBUG = 10,
    INFO = 20,
    WARNING = 30,
    ERROR = 40,
    FATAL = 50,
} log_level_t;

class Logger {

public:
    Logger(log_level_t level, const string name):
            fname(name), log_level(level) {

        errors = 0;
        outfile.open(fname, ios::app);
        if(!outfile.is_open()) {
            cerr << "error: cannot open log file: \""<< fname <<"\": "
                    << strerror(errno) << endl;
            exit(1);
        }
    }

    Logger(log_level_t level):
            fname(""), log_level(level)  {
    }

    ~Logger() {
        outfile.close();
    }

    void log(log_level_t level, const string& msg) {

        // time_t now = time(0);
        // tm* timeinfo = localtime(&now);
        // char timestamp[20];
        // strftime(timestamp, sizeof(timestamp),
        //         "%Y-%m-%d %H:%M:%S", timeinfo);

        ostringstream log_entry;

        if(level == FATAL) {
            print(stderr, "fatal error: {}\n", msg);
            errors++;

            ostringstream log_entry;
            log_entry // << "[" << timestamp << "] "
                    << log_level_to_str(level) << ": " << msg << endl;

            // Output to log file
            if (outfile.is_open()) {
                outfile << log_entry.str();
                outfile.flush(); // Ensure immediate write to file
            }

            exit(1);
        }
        else if(level == ERROR) {
            print(stderr, "error: {}\n", msg);
            errors++;

            log_entry // << "[" << timestamp << "] "
                    << log_level_to_str(level) << ": " << msg << endl;

            // Output to log file
            if (outfile.is_open()) {
                outfile << log_entry.str();
                outfile.flush(); // Ensure immediate write to file
            }

        }
        else if(level >= log_level) {

            log_entry // << "[" << timestamp << "] "
                    << log_level_to_str(level) << ": " << msg << endl;

            // Output to console
            cout << log_entry.str();

            // Output to log file
            if (outfile.is_open()) {
                outfile << log_entry.str();
                outfile.flush(); // Ensure immediate write to file
            }
        }
    }

    int get_errors() {
        return errors;
    }

private:
    const string fname;
    ofstream outfile;
    log_level_t log_level;
    int errors;

    const string log_level_to_str(log_level_t level) {
        return (level == DEBUG)? "DEBUG" :
            (level == INFO)? "INFO" :
            (level == WARNING)? "WARNING" :
            (level == ERROR)? "ERROR" :
            (level == FATAL)? "FATAL" : "UNKNOWN";
    }
};

