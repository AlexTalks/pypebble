#include <format>
#include <iostream>

#include "pebble.h"

int main() {
  const std::string dbName = "tmp";
  std::cout << "Pebble Test: Starting" << std::endl;
  try {
    cockroachdb::pebble::DB* db = cockroachdb::pebble::DB::Open(dbName);
    std::cout << "Opened " << dbName << std::endl;

    db->Set("hello", "world", true);
    db->Set("name", "alex", true);

    std::string val = db->Get("hello");
    std::cout << "Got val=" << val << " for key="
              << "hello" << std::endl;

    int64_t numFiles = db->NumFiles();
    std::cout << "DB Num Files: " << numFiles << std::endl;

    db->Close();
    std::cout << "DB closed" << std::endl;
  } catch (const std::runtime_error& e) {
    std::cerr << "Exception: " << e.what() << std::endl;
    exit(-1);
  }

  std::cout << "Exiting." << std::endl;
  return 0;
}