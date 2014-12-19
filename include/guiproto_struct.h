#ifndef GUIPROTO_STRUCT_H
#define GUIPROTO_STRUCT_H

/** @brief Message type, defines how should be interpreted the next data block */
struct gui_msgtype {
    enum { CURRSTEP,
           LAST }

         type;
};

struct gui_currstep {
    unsigned int tstep;
};

struct gui_movevessel {
    unsigned int vid;
};

#endif // GUIPROTO_STRUCT_H
