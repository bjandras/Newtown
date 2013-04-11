
TARGET = newtown
TEMPLATE = app

QT += opengl

LIBS += -lGLU

INCLUDEPATH = .

SOURCES += \
    app/mainwindow.cpp \
    app/toolbox.cpp \
    app/main.cpp \
    app/util.cpp \
    app/view.cpp \
    app/scene.cpp \
    app/glwidget.cpp \
    app/fielditem.cpp \
    app/graphitem.cpp \
    app/model.cpp \
    demo/demo.cpp \
    demo/transformdemo.cpp \
    core/parameters.cpp \
    core/field.cpp \
    core/fieldpainter.cpp \
    core/mapimage.cpp \
    core/border.cpp \
    core/edge.cpp \
    core/tracer.cpp \
    core/tracer_data.cpp \
    core/tracer_params.cpp \
    core/tracer_tracing.cpp \
    core/seeder.cpp \
    core/grapher.cpp \
    core/region.cpp \
    core/city.cpp \
    core/district.cpp \
    core/block.cpp \
    core/volumebox.cpp \
    math/funcs.cpp \
    math/vector2f.cpp \
    math/point2f.cpp \
    math/polygon.cpp \
    math/rect.cpp \
    math/tensor.cpp \
    math/graph.cpp \
    base/bitmatrix.cpp
HEADERS += app/mainwindow.h \
    app/toolbox.h \
    app/util.h \
    app/view.h \
    app/scene.h \
    app/glwidget.h \
    app/fielditem.h \
    app/graphitem.h \
    app/model.h \
    demo/transformdemo.h \
    core/parameters.h \
    core/parameters.hh \
    core/point.hh \
    core/point.h \
    core/field.h \
    core/fieldpainter.h \
    core/mapimage.h \
    core/border.h \
    core/edge.hh \
    core/edge.h \
    core/tracer.hh \
    core/tracer.h \
    core/seeder.hh \
    core/seeder.h \
    core/grapher.hh \
    core/grapher.h \
    core/region.h \
    core/region.hh \
    core/city.h \
    core/city.hh \
    core/district.h \
    core/district.hh \
    core/block.h \
    core/block.hh \
    core/volumebox.h \
    core/volumebox.hh \
    math/funcs.h \
    math/vector2f.hh \
    math/vector2f.h \
    math/point2f.hh \
    math/point2f.h \
    math/polygon.h \
    math/polygon.hh \
    math/rect.h \
    math/rect.hh \
    math/tensor.hh \
    math/tensor.h \
    math/graph.h \
    math/graph.hh \
    base/matrix.hh \
    base/matrix.h \
    base/bitmatrix.h \
    base/bitmatrix.hh \
    base/config.h \
    base/names.h

FORMS += \
    ui/mainwindow.ui \
    ui/toolbox.ui \
    ui/fieldpainterview.ui

RESOURCES += data/resources.qrc

OBJECTS_DIR = ./build
MOC_DIR = ./build
RCC_DIR = ./build
UI_DIR = ./build
