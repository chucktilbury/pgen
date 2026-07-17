
#pragma once

#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>
#include <print>
#include <cstring>
#include <cerrno>
#include <vector>
#include <format>

using namespace std;

class Logger {

public:
    enum {
        SILENT = 0,
        DEBUG = 10,
        ENTER = 11,
        RETURN = 12,
        TRACE = 13,
        INFO = 20,
        WARNING = 30,
        ERROR = 40,
        FATAL = 50,
        ALL = 500,
    } ;

    Logger(int level) {
        push_level(level);
    }

    void info(const string& msg) {
        if(peek_level() <= INFO) {
            pad();
            print(stdout, "INFO: {}\n", msg);
        }
    }

    void warning(const string& msg) {
        if(peek_level() <= WARNING) {
            print(stderr, "warning: {}\n", msg);
            warnings++;
        }
    }

    void error(const string& msg) {
        if(peek_level() <= ERROR) {
            print(stderr, "error: {}\n", msg);
            errors++;
        }
    }

    void fatal(const string& msg) {
        print(stderr, "fatal error: {}\n", msg);
        errors++;
        exit(1);
    }

    void set_level(int lev) {
        //print(stderr, "set level: {}\n", lev);
        push_level(lev);
    }

    int get_errors() {
        return errors;
    }

    int get_warnings() {
        return warnings;
    }

    void push_level(int lev) {
        lev_stack.push_back(lev);
    }

    void pop_level() {
        lev_stack.pop_back();
    }

    int peek_level() {
        return lev_stack.back();
    }

    int get_level() {
        return peek_level();
    }

    void inc_depth() { depth += dinc; }
    void dec_depth() { depth -= dinc; }
    void pad() { for(int i = 0; i < depth; i++) cout << ' '; }


private:
    //int log_level;
    int errors;
    int warnings;
    int depth;
    const int dinc = 2;
    vector<int> lev_stack;

};

#ifdef USE_TRACE

#define ENTER do { \
    if(logger.get_level() <= Logger::DEBUG) { \
        logger.pad(); \
        print(stdout, "ENTER: {}\n", __PRETTY_FUNCTION__); \
        logger.inc_depth(); \
    } \
} while(false)

#define RETURN(...) do { \
    if(logger.get_level() <= Logger::DEBUG) { \
        logger.dec_depth(); \
        logger.pad(); \
        print(stdout, "RETURN({}) {}\n", #__VA_ARGS__, __PRETTY_FUNCTION__); \
    } \
    return __VA_ARGS__; \
} while(false);

#define TRACE(m) do { \
    if(logger.get_level() <= Logger::DEBUG) { \
        logger.pad(); \
        print(stdout, "TRACE: {}\n", m); \
    } \
} while(false)

#define START(m) do { \
    if(logger.get_level() <= Logger::DEBUG) { \
        print(stdout, "{:-^75}\n", m); \
    } \
} while(false)

#define END(m) do { \
    if(logger.get_level() <= Logger::DEBUG) { \
        print(stdout, "{:-^75}\n", m); \
    } \
} while(false)

#else

#define ENTER
#define RETURN(...) return __VAR_ARGS__
#define TRACE(m)

#endif
