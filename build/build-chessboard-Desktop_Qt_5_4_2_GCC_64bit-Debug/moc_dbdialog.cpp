/****************************************************************************
** Meta object code from reading C++ file 'dbdialog.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../chessboard/master/src/dbdialog.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'dbdialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.4.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_DBdialog_t {
    QByteArrayData data[8];
    char stringdata[57];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_DBdialog_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_DBdialog_t qt_meta_stringdata_DBdialog = {
    {
QT_MOC_LITERAL(0, 0, 8), // "DBdialog"
QT_MOC_LITERAL(1, 9, 8), // "setEvent"
QT_MOC_LITERAL(2, 18, 0), // ""
QT_MOC_LITERAL(3, 19, 5), // "value"
QT_MOC_LITERAL(4, 25, 7), // "getGame"
QT_MOC_LITERAL(5, 33, 3), // "txt"
QT_MOC_LITERAL(6, 37, 7), // "setGame"
QT_MOC_LITERAL(7, 45, 11) // "mySetResult"

    },
    "DBdialog\0setEvent\0\0value\0getGame\0txt\0"
    "setGame\0mySetResult"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_DBdialog[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   34,    2, 0x0a /* Public */,
       4,    1,   37,    2, 0x0a /* Public */,
       6,    1,   40,    2, 0x0a /* Public */,
       7,    0,   43,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::QString,    5,
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void,

       0        // eod
};

void DBdialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        DBdialog *_t = static_cast<DBdialog *>(_o);
        switch (_id) {
        case 0: _t->setEvent((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->getGame((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 2: _t->setGame((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->mySetResult(); break;
        default: ;
        }
    }
}

const QMetaObject DBdialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_DBdialog.data,
      qt_meta_data_DBdialog,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *DBdialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DBdialog::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_DBdialog.stringdata))
        return static_cast<void*>(const_cast< DBdialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int DBdialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
