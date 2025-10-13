# Logger

This library implements a logging object that can produce logs to the specified
destination (console, syslog, or file) in a thread-safe manner.

The logging object is called `Logger` and exists in the namespace
`Terra::Logger`.  The examples in this file assume
`using namespace Terra::Logger` was used for brevity.

## Logging to syslog

To perform logging via syslog, one can create a `Logger` object this way:

```cpp
// Creating a Logger object
Terra::LoggerPointer logger = std::make_shared<Terra::Logger>("app");

// Example log message
logger->Log("This is a test message");
```

Here, "app" is the application identifier that will be used in logged messages.
If this string is empty, `nullptr` will be passed as the identifier causing
syslog to use the name of the executable.

Generally, syslog is only available on Unix-type systems.

## Logging to Streams

One may log to any C++ output stream, including `std::ofstream`,
`std::cout`, `std::clog`, `std::ostringstream` or and other valid output
stream.  While the streaming to a output string may have limited utility,
the point is that any valid `std::ostream` may be used.  The following shows
how this might be used:

```cpp
std::ofstream log_file("/tmp/app.log", std::ios::binary);
LoggerPointer logger = std::make_shared<Logger>(log_file);

logger->Log("This is a test message");
```

## Logging to the Nowhere

Sometimes, it is desirable to enable or disable logging programatically.
One way to do that is to not create a parent Logger and to pass `nullptr` or an
unassigned `LoggerPointer` object to child Logger objects.  Another approach is
to create a parent Logger that specified the stream `NullOStream`, which
is an object defined in this library.

Example usage:

```cpp
NullOStream null_stream;
LoggerPointer logger = std::make_shared<Logger>(null_stream);
```

If using the latter approach, note that the `NullOstream` object must persist
as long as the parent Logger exists.

## Log Levels

The log levels used by `Logger` are a subset of those defined by syslog.  They
are defined in the file `log_level.h` as follows.

* Critical
* Error
* Warning
* Notice
* Info
* Debug

Each log line submitted to the `Logger` carries with it a `LogLevel` value.
The `Logger` that receives the message will determine if it should emit
(or forward to a parent `Logger`) given the `Logger` object's logging level.
For example, if the `Logger` is configured to log only critical messages,
then debug, info, or other messages of lesser importance will not be emitted.

## Logging Messages

To log messages, one may call the `Log()` functions in `Logger` or use the
streaming objects that are members of the `Logger` class.  The following are
all valid examples that each log a message.

```cpp
logger->Log("This is an info message");
logger->Log(LogLevel::Warning, "This is a warning message");
logger->critical << "This is a critical message" << std::flush;
LOGGER_NOTICE(logger, "This is a notice message");
```

Note that the last example shows the use of the macros defined in
`logger_macros.h` and are not automatically included when including
the `logger.h` header file.

When using the streaming members (like the `critical` member in the above
example), it is important to use `std::flush` to flush the contents of the
internal string buffer after submitting the entire log message.  Failure to do
so will result in warning messages in the logging output and will negatively
affect performance in multithreaded applications.

## Root Logger and Child Loggers

Different components of the application may each have their own logging objects
that are independent of one another.  However, it is also possible to create
a child logging object that is given a shared pointer to its parent `Logger`.
The benefit of doing that is that each sub-component of the application may
be given a short string designation that helps make the logging output
clearer.  For example, suppose an INFO message is logged from a `Logger`
having a component name "CCC" that is a child of a `Logger` having a component
name "BBB", that is in turn a child of a `Logger` object having a component
name "AAA".  That `Logger` would also be a child of the root logger, which has
no component name.  An example log message from the `Logger` having
the component name "CCC" as described might be something like the following.

```text
yyyy-mm-ddThh:mm:sss [INFO] [AAA] [BBB] [CCC] This is a log message
```

Log messages from the root `Logger` object will not have a component name
in brackets.

Here, "[INFO]" indicates the `LogLevel` value in text form.

## Timestamp on Log Messages

At the start of the logged message is the current timestamp.  By default,
the precision of the `Logger` is milliseconds.  However, one may specify
microseconds by calling the `SetTimePrecision()` function.

Log messages sent to syslog do not contain a timestamp since syslog timestamps
messages itself.  So the timestamps only appear when using a `std::ostream`
destination.
