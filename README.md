# sqz
Data compressor geared toward retrocomputing

# background on compression approaches

## Prefix-free binary coding

Here, we're only looking at "Prefix-free Binary codes," where
the encoded sequence can be read one bit at a time until
finding a known sequence, at which point the matching source
symbol can be output.

Such a coding is valuable when it uses shorter encodings for
values that are more common, and longer encodings for values
that are less common.

The entropy of the source data sets of a lower bound for the
size of the coded sequence, based on the probabilities of the
source symbols, assuming a perfectly random shuffle. "Entropy
coding" refers to codings that approach that lower bound.

Some such codings are suitable for inputs containing arbitrary
large integers as source symbols. Others have a limited range of
source symbols. Encoding arbirary large integers can't rely on
lookup tables, and has to use algorithms instead.

"Universal codes" are codes that can efficiently encode all
positive integers as long as small numbers are more common
than largers ones, provably remaining within a fixed ratio
to optimal coding. They can also be useful when dealing
with small amounts of source data, because their algorithmic
nature saves the need for data-specific lookup tables.

Some prefix-free binary codes worth mentioning, 

-Plain binary code, technically a prefix-free binary code,
optimal with 2^n equally probable source symbols. Parametrized,
not suitable for arbitrary large numbers.

-Trucated binary code, a generalized version of the above,
optimal with n equally probably source symbols, even when n is
not a power of two. Parametrized, not suitable for arbitrary
large numbers.

-Unary code. Optimal when probabilities of large numbers
decrease steeply enough (faster than golden-ratio geometric.)

-Golomb code. A parametrized family of generalizations of unary
codes, optimal for geometric distributions. 1966.

-Elias gamma code. Related to floating point, with exponent
in unary and mantissa just large enough to store an integer.
1975.

-Elias delta code. Similar to Elias gamma, but the exponent
is store in Elias gamma instead of unary. 1975.

-Elias omega code. Pushing the reasoning to the next level,
where the exponent is stored in unterminated Elias omega,
such that large numbers (greater than 16) store the number
of bits of the number of bits of the exponent, and so on.
works well for distributions that contain extremely large
symbols. 1975. Strong similarities to Levenshtein code, 1966.

-Even-Rodeh code. Similar to Elias omega, but uses more
bits in the initial group. 1978.

-Fibonacci code. Uses the fact that any integer is the unique
sum of non-consecutive Fibonacci numbers, stores the positions
of those numbers with an extra 1 at the end.

-Huffman code. Optimal prefix-free binary code for data with
a finite number of symbols, which is known in advance. Requires
to store a decoding tree, which can be large and is suitable for
appropriate coding, optionally after being canonicalized is
appropriate. 1962.

## Lempel-Ziv compression

All LZ variants work from the same underlying principle:
recognize sequences of input symbols that have been used
before, and reference previous instances instead of including
them explicitly again.

LZ has two main families. LZ1/LZ77 (1977) can re-use any
sequence of input symbols regardless of how it was generated,
while LZ2/LZ78 (1978) can only re-use a sequence exactly as
it was output before. As a result, when re-using data, LZ77
must include both the location and the size of the data
being re-used, whereas LZ78 only needs a single offset into
a dictionary.

The original LZ77 and LZ78 versions both assumed that the
encoding would use exactly one back-reference followed by
one new symbol. Later variants have more flexibility in that
domain.

Those algorithms were created at a time when large data
would most probably be processed through streaming and would
not fit in RAM. LZ77 does not allow arbitrary references
to past data, and encodes the position of back-references
relative to the current decoding point, working through a
sliding window. However, for in-RAM decoding, references
can also conceivably be made relative to the beginning of the
output. LZ78's limits are less immediately obvious, but
it's possible not to add new dictionary entries when the
dictionary is full. Comparing the two approaches, LZ77's
restriction limit it to finding data to re-use in what
has already been output, while LZ78's limits constrain it
to the earlier part of the input, such that data that
has different repetition patterns across its length will
still compress with a limited-memory LZ77, but will
struggle with a limited-memory LZ78.

Note that, in LZ78, a long sequence that repeats exactly is
unlikely to have been output in one command the first time,
such that there will not be a dictionary entry for it already.
In turn, when that sequence repeats, the beginning of the
sequence will find a dictionary entry, then output a new
character, such that the next pattern search will not have
the same alignment as the original, and will not be able to
re-use the exact same dictionary entries.

LZW (1984) builds on LZ78, but only includes dictionary
references, without new symbols. The dictionary is seeded
with all possible symbols, so that new symbols can be output
from those dictionary references, while new dictionary entries
are created by using the first symbol that couldn't be
matched from the dictionary (i.e. the first symbol from the
next dictionary entry in the stream).

Evolutions of LZW added the ability to clear the dictionary,
and used variable-width dictionary references such that
dictionary entries remain short when the dictionary is mostly
empty.

A noteworthy variant is LZT, which purges the dictionary
with an LRU approach. LZMW builds on LZW by concatenating
entire dictionary entries instead of adding single characters.
LZAP is more aggressive, building all possible partial
concatenations.

Some thoughts: it might be possible to slightly improve LZW
by using truncated binary encoding instead of plain binary
encoding. The choice of when to clear the dictionary is
probably quite expensive to optimize. It's not clear how
well a Huffman stage can follow the dictionary coding, though
in that case there's no point using variable-width codes.

The main derivative of LZ77 is LZSS, which introduces two
kinds of blocks: a pure back-reference with no new symbol
added, and a variable-length literal block.

From there, there's a breadth of other variants, many adding
a Huffman stage after the LZ stage, with either a fixed or
a variable code, while others accept a lower compression
ratio for better compression or decompression speed.

Beyond LZSS, other noteworthy variants of LZ77 include
LZR (using pointers into arbitrary locations of the data),
LZH (Huffman coding of the offsets only), LZB (varying
offset sizes to match input size).

Some thoughts: just like LZW removes the need to store new
symbols in the compressed stream, by seeding the dictionary
with all possible symbols, LZ77 can work without new symbols
by seeding all possible symbols at well-known offset locations,
with the understanding that this requires some care. However,
at a half-way point between LZ77 and LZSS, where there exist
two types of blocks, back-references and single literals,
a Huffman code as a later stage will be at least as effective
as emulating the symbol table at well-know offsets.

Finally, RLE can be seen as a form of LZSS where the only
allowed offset is 1. Packbits uses RLE, and Degas Elite
uses Packbits over blocks of 40 bytes.

## Other compression approaches

Byte pair encoding (BPE) (1994) recognizes the most common
character pairs, and replaces them with symbols that are
otherwise unused.

Tunstall coding (1967) takes a unique approach to entropy
coding: instead varying the length of codes to match
uneven probabilities, it evens out probabilities by storing
symbol pairs (or longer) into fixed-length codes, essentially
turning Huffman on its head.

## Data transformations

Finally, there exist transformations that are intended to
condition data to make it easier to compress.

Symbol separation processes the input such that symbols
are organized in a way that makes semantic sense. This
could involve extracting individual codepoints from a UTF-8
stream, or pixels from a chunked of bitplane storage.

Delta encoding stores differences between symbols. Plain
encoding stores plain differences, alternating encoding
stores alternating differences and xor encoding uses xor
instead of differences; the point of the latter two variants
is to better encode runs that alternate between two values.

The Burrows-Wheeler transform (BWT) (1983-1994) transforms
the ata, adding one terminator symbol into the input and
reversibly reordering the input into a form that is more
likely to have local repetitions.

The Move-To-Front transform (MTF) (1980-1986) is another
transform, which does not change the size of the data, and
which stores information about how recently each symbol has
been used.

It turns out that applying MTF after BWT can make the data
greatly more compressible.

It's worth noting that all of those data transformations,
unlike entropy codes and dictionary codes, are dependent
on the ordering of the symbols. Some situations allow
changing that ordering (e.g. paletted bitmap data), giving
an additional dimension of possibilities.

For data that is stored across multiple dimensions, other
possibilities exist: delta encoding along multiple dimensions,
as well as linearizing data in different orders: in rows,
in columns, or along fractal space-filling curves (Hilbert,
Lebesgue, Moore, Peano).
