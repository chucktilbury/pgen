#ifndef _FILEIO_H_
#define _FILEIO_H_

#include <fstream>
#include <string>

using namespace std;

class File {

    public:
    File(const string& name);
    ~File();
    int consume_char();

    int get_char() {
        if(file.is_open())
            return crnt_char;
        else
            return EOF;
    }

    const string& get_file_name() {
        return fname;
    }

    bool is_open() {
        return file.is_open();
    }

    const string& fname;
    int line_no;
    int col_no;

    private:
    fstream file;
    int crnt_char;
};


#endif /* _FILEIO_H_ */
