include(../../../common-xlets.pri)

HEADERS     = *.h
SOURCES     = *.cpp
TRANSLATIONS += $$ROOT_DIR/i18n/agentsnext_en.ts
TRANSLATIONS += $$ROOT_DIR/i18n/agentsnext_fr.ts
TRANSLATIONS += $$ROOT_DIR/i18n/agentsnext_nl.ts

TARGET      = agentsnextplugin

RESOURCES = agentsnext.qrc
