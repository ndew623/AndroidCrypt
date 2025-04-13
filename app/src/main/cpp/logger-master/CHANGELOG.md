# Change Log

v1.0.3

- Changes required to build on OpenBSD

v1.0.2

- Updated to the latest Simple Test Framework (STF)

v1.0.1

- Changed NullOStream so that NullBuffer is a member and changed logger.cpp
  so that CreateNullLogger() contains a static variable of type NullBuffer
  These changes avoid issues with unpredictable static initialization order
- Lock mutex before notifying waiting threads
- Changed use of LoggerBuffer so that each instance is member object, not a
  unique_ptr; functionally, this is the same, but this change avoids heap
  allocations and use of pointers to store allocated objects

v1.0.0

- Initial Release
