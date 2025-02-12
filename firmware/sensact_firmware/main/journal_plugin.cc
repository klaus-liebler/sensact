#include "journal_plugin.hh"
#include <array>

std::array<webmanager::MessageLogEntry, webmanager::STORAGE_LENGTH> webmanager::JournalPlugin::messageLog;
webmanager::JournalPlugin* webmanager::JournalPlugin::singleton{nullptr};