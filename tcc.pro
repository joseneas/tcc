QT += multimedia
QT += multimediawidgets
QT += network
QT += qml
QT += quick
QT += quickcontrols2
QT += sql
QT += svg
QT += widgets

CONFIG += c++11

HEADERS += src/core/app.h \
    src/core/filedialog.h \
    src/core/notification.h \
    src/core/pluginmanager.h \
    src/core/utils.h \
    src/database/database.h \
    src/database/databasecomponent.h \
    src/network/downloadmanager.h \
    src/network/requesthttp.h \
    src/network/uploadmanager.h

SOURCES += main.cpp \
    src/core/app.cpp \
    src/core/filedialog.cpp \
    src/core/notification.cpp \
    src/core/utils.cpp \
    src/core/pluginmanager.cpp \
    src/database/database.cpp \
    src/database/databasecomponent.cpp \
    src/network/downloadmanager.cpp \
    src/network/requesthttp.cpp \
    src/network/uploadmanager.cpp

RESOURCES += qml.qrc \
    translations.qrc

TRANSLATIONS = translations/en_US.ts \
    translations/pt_BR.ts \
    translations/pt_BR.qm

OTHER_FILES += config.json \
    qtquickcontrols2.conf

plugins.files = plugins
plugins.path = /assets/
INSTALLS += plugins

linux:!android {
    QT += KConfigCore
    QT += KNotifications

    # plasma KDE required notification file
    OTHER_FILES += notification-linux-config.notifyrc

    pluginsDestinationPath = $$OUT_PWD/plugins
    # remove plugins directory before build
    exists($$pluginsDestinationPath) {
        QMAKE_PRE_LINK += rm -rf $$pluginsDestinationPath $$escape_expand(\\n\\t)
    }
    # copy plugins directory to application executable dir after build
    QMAKE_POST_LINK += $(COPY_DIR) $$PWD/plugins $$OUT_PWD
}

android: {
    QT += androidextras

    HEADERS += src/extras/androidfiledialog.h \
        src/extras/JavaCppConnect.h \
        src/extras/androidstatusbar.h

    SOURCES += src/extras/androidfiledialog.cpp \
        src/extras/androidstatusbar.cpp

    ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android

    OTHER_FILES += \
        android/AndroidManifest.xml \
        android/google-services.json \
        android/gradle/wrapper/gradle-wrapper.jar \
        android/gradlew \
        android/res/values/libs.xml \
        android/build.gradle \
        android/gradle/wrapper/gradle-wrapper.properties \
        android/gradlew.bat \

    OTHER_FILES += android/src/org/qtproject/qt5/android/bindings/*.java

    contains(ANDROID_TARGET_ARCH,armeabi-v7a) {
        ANDROID_EXTRA_LIBS = \
            $$PWD/android/libs/openssl-1.0.2/armeabi-v7a/lib/libcrypto.so \
            $$PWD/android/libs/openssl-1.0.2/armeabi-v7a/lib/libssl.so
    }
}

ios: {
    # PLEASE! Check platform notes for ios:
    # http://doc.qt.io/qt-5/platform-notes-ios.html
    QMAKE_INFO_PLIST = $$PWD/ios/Info.plist

    # PLEASE! Check platform notes for ios:
    # http://doc.qt.io/qt-5/platform-notes-ios.html
    assets_catalogs.files = plugins
    QMAKE_BUNDLE_DATA += assets_catalogs

    ios_icon.files = $$files($$PWD/ios/icons/Icon-App-*.png)
    QMAKE_BUNDLE_DATA += ios_icon

    app_launch_images.files = $$PWD/ios/Launch.xib $$files($$PWD/ios/icons/Icon-App-*.png)
    QMAKE_BUNDLE_DATA += app_launch_images

    ios_google_plist.files = $$PWD/ios/GoogleService-Info.plist
    QMAKE_BUNDLE_DATA += ios_google_plist

    QMAKE_LFLAGS += $(inherited) -ObjC -l "c++" -l "sqlite3" -l "z" -framework "AdSupport" -framework "AddressBook" -framework "CoreGraphics" -framework "FirebaseAnalytics" -framework "FirebaseInstanceID" -framework "FirebaseMessaging" -framework "GoogleIPhoneUtilities" -framework "GoogleInterchangeUtilities" -framework "GoogleSymbolUtilities" -framework "GoogleUtilities" -framework "SafariServices" -framework "StoreKit" -framework "SystemConfiguration"

    OBJECTIVE_SOURCES += ios/QtAppDelegate.mm

    Q_ENABLE_BITCODE.value = NO
    Q_ENABLE_BITCODE.name = ENABLE_BITCODE
    QMAKE_MAC_XCODE_SETTINGS += Q_ENABLE_BITCODE

    mac: LIBS += -F$$PWD/ios/Firebase/ -framework FirebaseAnalytics
    mac: LIBS += -F$$PWD/ios/Firebase/ -framework FirebaseInstanceID
    mac: LIBS += -F$$PWD/ios/Firebase/ -framework FirebaseMessaging
    mac: LIBS += -F$$PWD/ios/Firebase/ -framework GoogleInterchangeUtilities
    mac: LIBS += -F$$PWD/ios/Firebase/ -framework GoogleIPhoneUtilities
    mac: LIBS += -F$$PWD/ios/Firebase/ -framework GoogleSymbolUtilities
    mac: LIBS += -F$$PWD/ios/Firebase/ -framework GoogleUtilities

    DEPENDPATH += $$PWD/ios/Firebase
    INCLUDEPATH += $$PWD/ios/Firebase
}

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target