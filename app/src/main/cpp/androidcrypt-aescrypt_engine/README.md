# AES Crypt Engine

The AES Crypt Engine is responsible for encrypting or decrypting octets
from a given source stream to a given destination stream.  The encryption stream
adheres to the AES Crypt stream format.  Likewise, it is expected that the
stream to be decrypted follows the same [stream format](stream_format.md).

The AES Crypt Engine is comprised of two primary objects:

* Encryptor - responsible for encrypting a plaintext stream
* Decryptor - responsible for decrypting a ciphertext stream

## Common Behavior

Both the Encryptor and Decryptor are created with a pointer to a Logger
object, instance identifier, and a callback function to deliver progress
updates.  All of those are optional.

When encrypting or decrypting, one provides a password, source stream, and
destination stream.  The password will be transformed into a key suitable
for encryption.  The source an destination streams are std::istream and
std::ostream object references, respectively.  This allows one to use a
consistent API to stream from various sources, including standard input,
files, or even strings.

There is also a progress interval value.  This value is the number of source
octets to consume before issuing a callback to notify whatever receiver
was specified in the constructor about the current progress.  If this value
is zero, no callbacks are issued.  This value should never be less than
16, since that is the smallest possible octet count where a callback could
be delivered.  Ideally, the caller would specify a value that would result
in movement of a progress meter, for example, on each callback.  Callbacks
that have no real value are just going to slow processing.

## Encryption

When encrypting, one has the opportunity to provide a `kdf_iterations` value
that can be used to make it more complex to guess the user's password.  See the
Key Derivation section details that more.

It is also possible to provide a vector of "extensions", which are really just
tag/value pairs that are inserted into the beginning of the file.  These
extension values are not encrypted and are not protected from modification.
They are just pieces of meta data that might be of interest to automated tools,
but should not be relied upon to be accurate if there is a chance the file
might have changed.  The reason they are not protected is that these were added
at user's request to have a place to insert or change meta information
after a file is created.

## Decryption

Decryption is performed in much the same way, though the user does not need
to provide the `kdf_iterations` value.  The Decryptor will read the
`kdf_iterations` value from the encrypted stream.

Extensions are ignored by the Decryptor.

## Key Derivation

Transforming the user-provided password to a key suitable for encryption uses
the FIPS-recommended PBKDF2 algorithm utilizing SHA-512 as the hashing
algorithm.  An important consideration is the number of hashing iterations.

If the "password" is actually a random value with at least 256 bits or more of
entropy, a single iteration would be sufficient.  If using a password, far more
iterations are recommended.

While the number of iterations is not precisely the time required to perform
a hash operation, it is a close approximation.  An iteration involves a bit more
work, but the hash is the most significant part of the time required.  So for
simplicity, we will assume for this discussion that one iteration is equal to
one hash operation.

Tests on an Nvidia 4090 GPU shows an ability to perform about 8.6 billion hashes
per second using SHA-256 and 3.2 billion hashes per second using SHA-512.  For
this reason (and similar differences on other hardware), SHA-512 is used as
the hashing algorithm.

Using a `kdf_iterations` value of 300,000 reduces the effective password
guesses to about 10,000 per second.  Likewise, a value of 600,000 reduces
password guesses to 5,000 per second.  There are likely other dedicated
machines that can operate even faster and, obviously, a cluster of GPUs working
together can reduce the time linearly with the number of GPUs working in
parallel.

For the following discussion, we consider passwords with characters chosen
from a set of 62 possible values: A-Z, a-z, and 0-9.  Obviously, using
additional characters in the selection of random values further increases the
total entropy.

Let's assume one has a "random" password of 12 characters of of the 62 possible
values.  This would represent log2(62) = 5.95 bits of entropy * 12 = 71.45 total
bits of entropy.  So a password cracking machine would need to enumerate through
a maximum of 2^71.45 (or 3.23 x 10^21) values to guess all possible values.
(One arrives at the same value via 62^12, if you prefer to look at it using that
approach.)

If using a single hash iteration and this 12 character password, it would
take about 32,000 years to test all possible values given the above data
([math](https://www.wolframalpha.com/input?i=2%5E%28log2%2862%29*12%29+%2F+3.2%C3%9710%5E9+%2F+3.154x10%5E7+years)).

However, if using a cluster of 100 GPUs, this time is reduced to less than 320
years ([math](https://www.wolframalpha.com/input?i=2%5E%28log2%2862%29*12%29+%2F+3.2%C3%9710%5E11+%2F+3.154x10%5E7+years)).
Likewise, using a cluster of 1000 brings the time down to 3.2 years.  Obviously,
that is not an acceptable value.  Thus iterations must be higher for shorter
or non-random passwords.

It's important to note that increasing the password length to 16 characters
for the above single GPU scenario increases the number of years to crack to
4.7 billions ([math](https://www.wolframalpha.com/input?i=2%5E%28log2%2862%29*16%29+%2F+3.2%C3%9710%5E11+%2F+3.154x10%5E7+years)).

Obviously, if the password is easier to guess or even more GPUs are employed,
this time can be reduced even further.  If a password is known to be exactly 12
characters, for example, then the all possible values less than that can
be removed from consideration.

If the number of iterations is increased to 300,000 for that 12-character
password, the time to "guess" increases to 10 billion years on a single GPU
since 300,000 iterations reduces the "guess" rate to about 10,000 per second
([math](https://www.wolframalpha.com/input?i=2%5E%28log2%2862%29*12%29+%2F+10000+%2F+3.154x10%5E7+years)).
The 10,000 comes from the ~3 billion hashes / second divided by 300,000.
While these numbers are not precise, understand that the measurements are also
not precise.  There is some variability, but these numbers are just to provide
a feel for the complexity and time.

Since humans have a tendency to select passwords poorly, the amount of entropy
is likely lower and the amount of guessing is reduced.  It's impossible to
assume how long it might take to crack a poorly-chosen password.  The
numbers discussed above are just the upper bound, given the known hashing
rates.  It's also important to be mindful that hashing power increases
every year.

In short, use `kdf_iterations` that makes it as difficult as possible to crack
the password, while still consuming a tolerable amount of time.  If encrypting
a single file, consuming a bit of time to perform iterations is probably fine.
If encrypting a bunch of files (e.g., using AES Crypt to back up files on
a NAS wherein each is encrypted) may not be.  For the latter, it might be
best to select a password that truly is a random sequence with more than
256 bits of entropy and reduce the KDF iterations.  For example, a 43-character
randomly selected password from a character set of 62 characters would yield
256 bits of entropy (log2(62) * 43 ~= 256).  This 256 bits of entropy would be
sufficient as input into the KDF with a single iteration, since the encryption
key is 256 bits.  Using even longer random password strings or more iterations
os fine.  The point was only to say that the iteration could safely be reduced
with sufficiently long and truly random "password" strings in order to lower
computational requirements when encrypting or decrypting.

Related to this entire discussion, NIST Special Publication 800-132 (Dec 2010)
says one should use at least 1000 iterations for passwords and goes on to
suggest that for extremely critical keys, a value of 10,000,000 might
be appropriate.  Section A.2.2 advises, "The number of iterations should be set
as high as can be tolerated for the environment, while maintaining acceptable
performance."  While this is not wrong, it's important to consider the other
factors like the randomness of the password / key and length, too.  A higher
number of iterations reduces the time to "guess" at a password, but
there is a diminishing return as the amount of entropy in the password
increases.

Note that the KDF iterations value allowed is bounded between 1 (acceptable
if the password is random data) and 5,000,000.  The upper bound is set to
prevent users from being frustrated with extremely long delays in key
derivation, and is arguably still too high.  One may modify `engine_common.h`
to increase this value, but it is not recommended just to ensure AES Crypt
files remain interoperable.  The default value of 300,000 is higher than
current industry recommendations (i.e., it is stronger than necessary, but
still introduces an acceptable delay for encrypting/decrypting individual
files).
