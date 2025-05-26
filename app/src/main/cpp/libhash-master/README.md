# Cryptographic Hashing Library

This library implements various cryptographic hashing algorithms and related
functions.  In this context, a hash value is also sometimes called a message
digest.

The algorithms contained in this library include:

* SHA-1
* SHA-224
* SHA-256
* SHA-384
* SHA-512
* HMAC (for any of the aforementioned algorithms)

## Usage Examples

Note that for all of the following examples, the hashing related objects are
within the namespace `Terra::Crypto::Hashing`.  The namespace is left out for
brevity.  In the test code, one will note the use of
`using Terra::Crypto::Hashing` also for brevity.  In commercial applications,
it does make sense to retain the namespace or at least some part of it.
Alternatively, one might define a shorter alias (e.g.,
`namespace TCH = Terra::Crypto::Hashing`).  The library uses the longer
namespace names to avoid naming conflicts in our own code and third-party
code.

### SHA-256 Computation

If the data for which a hash value it to be competed is contained in a single
string, one may compute the hash by simply doing this:

```cpp
// Create the SHA256 object and compute the hash
SHA256 sha256("abc");

// Retrieve that hash value
std::string result = sha256.Result();
```

In this example, the result is returned as a string.  The result may also be
returned as a sequence of octets.  Refer to the header file for APIs and the
implementation file for more complete documentation.

Generally, though, one will compute the hash value over data incrementally.
To do that, the code might look something like the following:

```cpp
// Creation of the SHA256 object
SHA256 sha256;

// Examples adding input incrementally
sha256.Input(some_data);
sha256.Input(more_data);

// Finalize the hash by calling Finalize() once all data is inputted
sha256.Finalize();

// Get the result, as in the previous example
std::string result = sha256.Result();
```

### Creating a Hash object

Each of the hashing algorithms is derived from the abstract base class
Hashing::Hash.  To create an instance as a unique pointer, one calls the
following function and specifying the algorithm to use:

```cpp
HashPointer hash = CreateHashObject(HashAlgorithm::SHA256);
```

### HMAC Example

One may implement an HMAC object and compute the hash similarly:

```cpp
HMAC hmac(HashAlgorithm::SHA256, key);
hmac.Input(input);
hmac.Finalize();

std::string result = hmac.Result();
```

The above example is taken from the `test_hmac.cpp` file.  Those tests and
other unit tests provide a number of additional usage examples.

## Note on Performance

Note that this code does not utilize older C-style macros, instead opting for
`constexpr` functions.  While this makes the code more readable and safe,
performance suffers unless this library is built with compiler optimizations
enabled.  If compiler optimization is not possible for a given platform,
employ C-style macros to replace various utility functions implemented in
the C++ source files to significantly increase performance.
