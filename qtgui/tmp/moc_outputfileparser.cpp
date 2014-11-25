/****************************************************************************
** Meta object code from reading C++ file 'outputfileparser.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.3.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../outputfileparser.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'outputfileparser.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.3.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_OutputFileParser_t {
    QByteArrayData data[7];
    char stringdata[56];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_OutputFileParser_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_OutputFileParser_t qt_meta_stringdata_OutputFileParser = {
    {
QT_MOC_LITERAL(0, 0, 16),
QT_MOC_LITERAL(1, 17, 5),
QT_MOC_LITERAL(2, 23, 0),
QT_MOC_LITERAL(3, 24, 14),
QT_MOC_LITERAL(4, 39, 5),
QT_MOC_LITERAL(5, 45, 4),
QT_MOC_LITERAL(6, 50, 5)
    },
    "OutputFileParser\0error\0\0parseCompleted\0"
    "parse\0path\0tstep"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_OutputFileParser[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   29,    2, 0x06 /* Public */,
       3,    0,   32,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    2,   33,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::Int,    5,    6,

       0        // eod
};

void OutputFileParser::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        OutputFileParser *_t = static_cast<OutputFileParser *>(_o);
        switch (_id) {
        case 0: _t->error((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 1: _t->parseCompleted(); break;
        case 2: _t->parse((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (OutputFileParser::*_t)(QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&OutputFileParser::error)) {
                *result = 0;
            }
        }
        {
            typedef void (OutputFileParser::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&OutputFileParser::parseCompleted)) {
                *result = 1;
            }
        }
    }
}

const QMetaObject OutputFileParser::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_OutputFileParser.data,
      qt_meta_data_OutputFileParser,  qt_static_metacall, 0, 0}
};


const QMetaObject *OutputFileParser::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *OutputFileParser::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_OutputFileParser.stringdata))
        return static_cast<void*>(const_cast< OutputFileParser*>(this));
    return QObject::qt_metacast(_clname);
}

int OutputFileParser::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void OutputFileParser::error(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void OutputFileParser::parseCompleted()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}
QT_END_MOC_NAMESPACE
