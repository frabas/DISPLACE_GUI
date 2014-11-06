/****************************************************************************
** Meta object code from reading C++ file 'simulator.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.3.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../simulator.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'simulator.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.3.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Simulator_t {
    QByteArrayData data[26];
    char stringdata[317];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Simulator_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Simulator_t qt_meta_stringdata_Simulator = {
    {
QT_MOC_LITERAL(0, 0, 9),
QT_MOC_LITERAL(1, 10, 3),
QT_MOC_LITERAL(2, 14, 0),
QT_MOC_LITERAL(3, 15, 19),
QT_MOC_LITERAL(4, 35, 22),
QT_MOC_LITERAL(5, 58, 8),
QT_MOC_LITERAL(6, 67, 8),
QT_MOC_LITERAL(7, 76, 21),
QT_MOC_LITERAL(8, 98, 11),
QT_MOC_LITERAL(9, 110, 8),
QT_MOC_LITERAL(10, 119, 3),
QT_MOC_LITERAL(11, 123, 1),
QT_MOC_LITERAL(12, 125, 1),
QT_MOC_LITERAL(13, 127, 6),
QT_MOC_LITERAL(14, 134, 4),
QT_MOC_LITERAL(15, 139, 5),
QT_MOC_LITERAL(16, 145, 17),
QT_MOC_LITERAL(17, 163, 16),
QT_MOC_LITERAL(18, 180, 5),
QT_MOC_LITERAL(19, 186, 22),
QT_MOC_LITERAL(20, 209, 8),
QT_MOC_LITERAL(21, 218, 20),
QT_MOC_LITERAL(22, 239, 22),
QT_MOC_LITERAL(23, 262, 23),
QT_MOC_LITERAL(24, 286, 7),
QT_MOC_LITERAL(25, 294, 22)
    },
    "Simulator\0log\0\0processStateChanged\0"
    "QProcess::ProcessState\0oldstate\0"
    "newstate\0simulationStepChanged\0"
    "vesselMoved\0laststep\0idx\0x\0y\0course\0"
    "fuel\0state\0outputFileUpdated\0"
    "nodesStatsUpdate\0error\0QProcess::ProcessError\0"
    "finished\0QProcess::ExitStatus\0"
    "readyReadStandardError\0readyReadStandardOutput\0"
    "started\0subprocessStateChanged"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Simulator[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       6,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   74,    2, 0x06 /* Public */,
       3,    2,   77,    2, 0x06 /* Public */,
       7,    1,   82,    2, 0x06 /* Public */,
       8,    7,   85,    2, 0x06 /* Public */,
      16,    2,  100,    2, 0x06 /* Public */,
      17,    1,  105,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      18,    1,  108,    2, 0x08 /* Private */,
      20,    2,  111,    2, 0x08 /* Private */,
      22,    0,  116,    2, 0x08 /* Private */,
      23,    0,  117,    2, 0x08 /* Private */,
      24,    0,  118,    2, 0x08 /* Private */,
      25,    1,  119,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, 0x80000000 | 4, 0x80000000 | 4,    5,    6,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::Float, QMetaType::Float, QMetaType::Float, QMetaType::Float, QMetaType::Int,    9,   10,   11,   12,   13,   14,   15,
    QMetaType::Void, QMetaType::QString, QMetaType::Int,    2,    2,
    QMetaType::Void, QMetaType::QString,    2,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 19,    2,
    QMetaType::Void, QMetaType::Int, 0x80000000 | 21,    2,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 4,    2,

       0        // eod
};

void Simulator::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Simulator *_t = static_cast<Simulator *>(_o);
        switch (_id) {
        case 0: _t->log((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 1: _t->processStateChanged((*reinterpret_cast< QProcess::ProcessState(*)>(_a[1])),(*reinterpret_cast< QProcess::ProcessState(*)>(_a[2]))); break;
        case 2: _t->simulationStepChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->vesselMoved((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< float(*)>(_a[3])),(*reinterpret_cast< float(*)>(_a[4])),(*reinterpret_cast< float(*)>(_a[5])),(*reinterpret_cast< float(*)>(_a[6])),(*reinterpret_cast< int(*)>(_a[7]))); break;
        case 4: _t->outputFileUpdated((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 5: _t->nodesStatsUpdate((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 6: _t->error((*reinterpret_cast< QProcess::ProcessError(*)>(_a[1]))); break;
        case 7: _t->finished((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QProcess::ExitStatus(*)>(_a[2]))); break;
        case 8: _t->readyReadStandardError(); break;
        case 9: _t->readyReadStandardOutput(); break;
        case 10: _t->started(); break;
        case 11: _t->subprocessStateChanged((*reinterpret_cast< QProcess::ProcessState(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (Simulator::*_t)(QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Simulator::log)) {
                *result = 0;
            }
        }
        {
            typedef void (Simulator::*_t)(QProcess::ProcessState , QProcess::ProcessState );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Simulator::processStateChanged)) {
                *result = 1;
            }
        }
        {
            typedef void (Simulator::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Simulator::simulationStepChanged)) {
                *result = 2;
            }
        }
        {
            typedef void (Simulator::*_t)(int , int , float , float , float , float , int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Simulator::vesselMoved)) {
                *result = 3;
            }
        }
        {
            typedef void (Simulator::*_t)(QString , int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Simulator::outputFileUpdated)) {
                *result = 4;
            }
        }
        {
            typedef void (Simulator::*_t)(QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Simulator::nodesStatsUpdate)) {
                *result = 5;
            }
        }
    }
}

const QMetaObject Simulator::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Simulator.data,
      qt_meta_data_Simulator,  qt_static_metacall, 0, 0}
};


const QMetaObject *Simulator::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Simulator::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Simulator.stringdata))
        return static_cast<void*>(const_cast< Simulator*>(this));
    return QObject::qt_metacast(_clname);
}

int Simulator::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 12)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 12;
    }
    return _id;
}

// SIGNAL 0
void Simulator::log(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Simulator::processStateChanged(QProcess::ProcessState _t1, QProcess::ProcessState _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void Simulator::simulationStepChanged(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void Simulator::vesselMoved(int _t1, int _t2, float _t3, float _t4, float _t5, float _t6, int _t7)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)), const_cast<void*>(reinterpret_cast<const void*>(&_t5)), const_cast<void*>(reinterpret_cast<const void*>(&_t6)), const_cast<void*>(reinterpret_cast<const void*>(&_t7)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void Simulator::outputFileUpdated(QString _t1, int _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void Simulator::nodesStatsUpdate(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}
QT_END_MOC_NAMESPACE
