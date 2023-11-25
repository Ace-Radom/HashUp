#include"renalog.h"

rena::renalog_capture::~renalog_capture(){
    this -> _oss << " [" << this -> _file
                 << " L:" << this -> _line << "]";
    __global_logger__ -> push(
        this -> _severity ,
        this -> _host ,
        this -> _tp ,
        this -> _oss.str()
    );
    return;
}
