# Config.pri file version 2.0. Auto-generated by IDE. Any changes made by user will be lost!
BASEDIR = $$quote($$_PRO_FILE_PWD_)

device {
    CONFIG(debug, debug|release) {
        profile {
            CONFIG += \
                config_pri_assets \
                config_pri_source_group1
        } else {
            CONFIG += \
                config_pri_assets \
                config_pri_source_group1
        }

    }

    CONFIG(release, debug|release) {
        !profile {
            CONFIG += \
                config_pri_assets \
                config_pri_source_group1
        }
    }
}

simulator {
    CONFIG(debug, debug|release) {
        !profile {
            CONFIG += \
                config_pri_assets \
                config_pri_source_group1
        }
    }
}

config_pri_source_group1 {
    SOURCES += \
        $$quote($$BASEDIR/src/HeadlessCommunication/HeadlessCommunication.cpp) \
        $$quote($$BASEDIR/src/Logger/Logger.cpp) \
        $$quote($$BASEDIR/src/Settings/Settings.cpp) \
        $$quote($$BASEDIR/src/main.cpp) \
        $$quote($$BASEDIR/src/service.cpp)

    HEADERS += \
        $$quote($$BASEDIR/src/HeadlessCommunication/HeadlessCommunication.h) \
        $$quote($$BASEDIR/src/Logger/HeapUsage/HeapUsage.h) \
        $$quote($$BASEDIR/src/Logger/Logger.h) \
        $$quote($$BASEDIR/src/Settings/Settings.h) \
        $$quote($$BASEDIR/src/common.hpp) \
        $$quote($$BASEDIR/src/helpers.hpp) \
        $$quote($$BASEDIR/src/service.hpp)
}

CONFIG += precompile_header

PRECOMPILED_HEADER = $$quote($$BASEDIR/precompiled.h)

lupdate_inclusion {
    SOURCES += \
        $$quote($$BASEDIR/../src/*.c) \
        $$quote($$BASEDIR/../src/*.c++) \
        $$quote($$BASEDIR/../src/*.cc) \
        $$quote($$BASEDIR/../src/*.cpp) \
        $$quote($$BASEDIR/../src/*.cxx) \
        $$quote($$BASEDIR/../src/HeadlessCommunication/*.c) \
        $$quote($$BASEDIR/../src/HeadlessCommunication/*.c++) \
        $$quote($$BASEDIR/../src/HeadlessCommunication/*.cc) \
        $$quote($$BASEDIR/../src/HeadlessCommunication/*.cpp) \
        $$quote($$BASEDIR/../src/HeadlessCommunication/*.cxx) \
        $$quote($$BASEDIR/../src/Logger/*.c) \
        $$quote($$BASEDIR/../src/Logger/*.c++) \
        $$quote($$BASEDIR/../src/Logger/*.cc) \
        $$quote($$BASEDIR/../src/Logger/*.cpp) \
        $$quote($$BASEDIR/../src/Logger/*.cxx) \
        $$quote($$BASEDIR/../src/Logger/HeapUsage/*.c) \
        $$quote($$BASEDIR/../src/Logger/HeapUsage/*.c++) \
        $$quote($$BASEDIR/../src/Logger/HeapUsage/*.cc) \
        $$quote($$BASEDIR/../src/Logger/HeapUsage/*.cpp) \
        $$quote($$BASEDIR/../src/Logger/HeapUsage/*.cxx) \
        $$quote($$BASEDIR/../src/Settings/*.c) \
        $$quote($$BASEDIR/../src/Settings/*.c++) \
        $$quote($$BASEDIR/../src/Settings/*.cc) \
        $$quote($$BASEDIR/../src/Settings/*.cpp) \
        $$quote($$BASEDIR/../src/Settings/*.cxx) \
        $$quote($$BASEDIR/../assets/*.qml) \
        $$quote($$BASEDIR/../assets/*.js) \
        $$quote($$BASEDIR/../assets/*.qs)

    HEADERS += \
        $$quote($$BASEDIR/../src/*.h) \
        $$quote($$BASEDIR/../src/*.h++) \
        $$quote($$BASEDIR/../src/*.hh) \
        $$quote($$BASEDIR/../src/*.hpp) \
        $$quote($$BASEDIR/../src/*.hxx)
}

TRANSLATIONS = $$quote($${TARGET}.ts)