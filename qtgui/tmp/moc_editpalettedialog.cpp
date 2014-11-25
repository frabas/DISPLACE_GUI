/****************************************************************************
** Meta object code from reading C++ file 'editpalettedialog.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.3.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../editpalettedialog.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'editpalettedialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.3.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_EditPaletteDialog_t {
    QByteArrayData data[14];
    char stringdata[247];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_EditPaletteDialog_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_EditPaletteDialog_t qt_meta_stringdata_EditPaletteDialog = {
    {
QT_MOC_LITERAL(0, 0, 17),
QT_MOC_LITERAL(1, 18, 14),
QT_MOC_LITERAL(2, 33, 0),
QT_MOC_LITERAL(3, 34, 24),
QT_MOC_LITERAL(4, 59, 5),
QT_MOC_LITERAL(5, 65, 22),
QT_MOC_LITERAL(6, 88, 4),
QT_MOC_LITERAL(7, 93, 22),
QT_MOC_LITERAL(8, 116, 21),
QT_MOC_LITERAL(9, 138, 20),
QT_MOC_LITERAL(10, 159, 31),
QT_MOC_LITERAL(11, 191, 17),
QT_MOC_LITERAL(12, 209, 17),
QT_MOC_LITERAL(13, 227, 19)
    },
    "EditPaletteDialog\0paletteChanged\0\0"
    "on_palette_doubleClicked\0index\0"
    "on_minVal_valueChanged\0arg1\0"
    "on_maxVal_valueChanged\0on_nrCol_valueChanged\0"
    "on_step_valueChanged\0"
    "on_specialPalette_doubleClicked\0"
    "on_btLoad_clicked\0on_btSave_clicked\0"
    "on_name_textChanged"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_EditPaletteDialog[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   64,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       3,    1,   65,    2, 0x08 /* Private */,
       5,    1,   68,    2, 0x08 /* Private */,
       7,    1,   71,    2, 0x08 /* Private */,
       8,    1,   74,    2, 0x08 /* Private */,
       9,    1,   77,    2, 0x08 /* Private */,
      10,    1,   80,    2, 0x08 /* Private */,
      11,    0,   83,    2, 0x08 /* Private */,
      12,    0,   84,    2, 0x08 /* Private */,
      13,    1,   85,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, QMetaType::QModelIndex,    4,
    QMetaType::Void, QMetaType::Double,    6,
    QMetaType::Void, QMetaType::Double,    6,
    QMetaType::Void, QMetaType::Int,    6,
    QMetaType::Void, QMetaType::Double,    6,
    QMetaType::Void, QMetaType::QModelIndex,    4,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    6,

       0        // eod
};

void EditPaletteDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        EditPaletteDialog *_t = static_cast<EditPaletteDialog *>(_o);
        switch (_id) {
        case 0: _t->paletteChanged(); break;
        case 1: _t->on_palette_doubleClicked((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 2: _t->on_minVal_valueChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 3: _t->on_maxVal_valueChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 4: _t->on_nrCol_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->on_step_valueChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 6: _t->on_specialPalette_doubleClicked((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 7: _t->on_btLoad_clicked(); break;
        case 8: _t->on_btSave_clicked(); break;
        case 9: _t->on_name_textChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (EditPaletteDialog::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&EditPaletteDialog::paletteChanged)) {
                *result = 0;
            }
        }
    }
}

const QMetaObject EditPaletteDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_EditPaletteDialog.data,
      qt_meta_data_EditPaletteDialog,  qt_static_metacall, 0, 0}
};


const QMetaObject *EditPaletteDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *EditPaletteDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_EditPaletteDialog.stringdata))
        return static_cast<void*>(const_cast< EditPaletteDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int EditPaletteDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 10;
    }
    return _id;
}

// SIGNAL 0
void EditPaletteDialog::paletteChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
