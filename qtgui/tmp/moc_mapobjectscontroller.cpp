/****************************************************************************
** Meta object code from reading C++ file 'mapobjectscontroller.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.3.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../mapobjectscontroller.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mapobjectscontroller.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.3.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_MapObjectsController_t {
    QByteArrayData data[16];
    char stringdata[239];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MapObjectsController_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MapObjectsController_t qt_meta_stringdata_MapObjectsController = {
    {
QT_MOC_LITERAL(0, 0, 20),
QT_MOC_LITERAL(1, 21, 20),
QT_MOC_LITERAL(2, 42, 0),
QT_MOC_LITERAL(3, 43, 3),
QT_MOC_LITERAL(4, 47, 20),
QT_MOC_LITERAL(5, 68, 15),
QT_MOC_LITERAL(6, 84, 15),
QT_MOC_LITERAL(7, 100, 12),
QT_MOC_LITERAL(8, 113, 18),
QT_MOC_LITERAL(9, 132, 16),
QT_MOC_LITERAL(10, 149, 23),
QT_MOC_LITERAL(11, 173, 14),
QT_MOC_LITERAL(12, 188, 6),
QT_MOC_LITERAL(13, 195, 23),
QT_MOC_LITERAL(14, 219, 14),
QT_MOC_LITERAL(15, 234, 4)
    },
    "MapObjectsController\0edgeSelectionChanged\0"
    "\0num\0nodeSelectionChanged\0geometryClicked\0"
    "const Geometry*\0widgetClosed\0"
    "signalAppIsClosing\0removeAllWidgets\0"
    "edgeSelectionHasChanged\0EdgeMapObject*\0"
    "object\0nodeSelectionHasChanged\0"
    "NodeMapObject*\0node"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MapObjectsController[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   54,    2, 0x06 /* Public */,
       4,    1,   57,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    1,   60,    2, 0x09 /* Protected */,
       7,    1,   63,    2, 0x09 /* Protected */,
       8,    0,   66,    2, 0x0a /* Public */,
       9,    0,   67,    2, 0x0a /* Public */,
      10,    1,   68,    2, 0x0a /* Public */,
      13,    1,   71,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Int, QMetaType::Int,    3,
    QMetaType::Int, QMetaType::Int,    3,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 6,    2,
    QMetaType::Void, QMetaType::QObjectStar,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 11,   12,
    QMetaType::Void, 0x80000000 | 14,   15,

       0        // eod
};

void MapObjectsController::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        MapObjectsController *_t = static_cast<MapObjectsController *>(_o);
        switch (_id) {
        case 0: { int _r = _t->edgeSelectionChanged((*reinterpret_cast< int(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 1: { int _r = _t->nodeSelectionChanged((*reinterpret_cast< int(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 2: _t->geometryClicked((*reinterpret_cast< const Geometry*(*)>(_a[1]))); break;
        case 3: _t->widgetClosed((*reinterpret_cast< QObject*(*)>(_a[1]))); break;
        case 4: _t->signalAppIsClosing(); break;
        case 5: _t->removeAllWidgets(); break;
        case 6: _t->edgeSelectionHasChanged((*reinterpret_cast< EdgeMapObject*(*)>(_a[1]))); break;
        case 7: _t->nodeSelectionHasChanged((*reinterpret_cast< NodeMapObject*(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef int (MapObjectsController::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&MapObjectsController::edgeSelectionChanged)) {
                *result = 0;
            }
        }
        {
            typedef int (MapObjectsController::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&MapObjectsController::nodeSelectionChanged)) {
                *result = 1;
            }
        }
    }
}

const QMetaObject MapObjectsController::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_MapObjectsController.data,
      qt_meta_data_MapObjectsController,  qt_static_metacall, 0, 0}
};


const QMetaObject *MapObjectsController::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MapObjectsController::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MapObjectsController.stringdata))
        return static_cast<void*>(const_cast< MapObjectsController*>(this));
    return QObject::qt_metacast(_clname);
}

int MapObjectsController::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 8;
    }
    return _id;
}

// SIGNAL 0
int MapObjectsController::edgeSelectionChanged(int _t1)
{
    int _t0 = int();
    void *_a[] = { const_cast<void*>(reinterpret_cast<const void*>(&_t0)), const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
    return _t0;
}

// SIGNAL 1
int MapObjectsController::nodeSelectionChanged(int _t1)
{
    int _t0 = int();
    void *_a[] = { const_cast<void*>(reinterpret_cast<const void*>(&_t0)), const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
    return _t0;
}
QT_END_MOC_NAMESPACE
