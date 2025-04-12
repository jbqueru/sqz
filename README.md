# sqz
Data compressor geared toward retrocomputing

# Background on compression approaches

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

It's worth noting that the efficiency of such codes is not
affected by a shuffle of the source data, and therefore
do not provide any savings for repetitionsin case the data
is less random than a perfect shuffle.

Finally, since non-trivial examples of such codes don't use
the same bit count for all symbols, such that they tend to
be inappropriate as inputs for later compression stages that
won't find recognizable symbols to process, short of decoding
the prefix-free code, which negates its purpose. In practice,
that implies that most compression schemes will have a
Huffman code as the last stage, while schemes without such
a stage probably leave some easy gains unrealized.

Some prefix-free binary codes worth mentioning:

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
appropriate. 1952.

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

## Thoughts on existing schemes

### Deflate

-recognizes that very long lengths and offsets might not
be very useful, and limits offsets to 15 bits (though the
format supports 16 bits, which is what differentiates
deflate64) and lengths to 8 bits (since longer matches
could be encoded into multiple matches).

-recognizes that Huffman coding of exact offsets and lengths
is counter-productive, because values tend to be unique:
the costs in the decoding tree would outweigh the gains
in the symbol stream, especially since that would rule out
canonical Huffman trees. Instead, uses an encoding that is
loosely related to Elias coding, though not universal,
in the sense that there's some information about the number
of bits, followed by an appropriate number of verbatim bits
from the original symbol.

-intertwines two sets of Huffman symbols along with other
data (non-prefix free but whose length is known).

-has 2 options to store the Huffman tree: conventional, and
canonical + RLE.

### Bzip2

-uses a nifty RLE trick as the first stage, where the indicator
for an RLE sequence is a repetition of 4 copies of the same
symbol: the next symbol is a length. That could be parametrized,
where a shorter threshold increases the risk of runs that are
exactly at the threshold and therefore need an unused length,
while a larger threshold makes actual runs more expensive to
store. Note: the author later recognized that this is not a
useful stage.

-uses BWT on blocks between 100kB and 900kB, probably to
have a tunable compression parameter between 1 and 9 as
a tradeoff between speed and ratio. Unlike deflate, though,
that parameter has an impact on the compression format.

-recognizes that 0 is such a common symbol after BWT + MTF
that Huffman deviates too much from perfect entropy, and
specifically encodes runs of zeroes before Huffman coding.

-uses multiple Huffman code, switching between them with
an MTF scheme and unary coding.

-Huffman tables are stored in an optimized canonical form:
a two-level bitmap keeps track of which symbols are actually
used, and the rest of the canonical form is stored with deltas,
using the fact that MTF tends to store distances in order from
higher to lower frequency.

# Design notes

## Huffman

### Minimum number of symbols

-Huffman is useless with 2 input symbols.

-Huffman with 3 symbols trivially amounts to a truncated
binary encoder. It can be encoded through regular Huffman
coding, but could also use an ad-hoc implementation (which
would allow for a Huffman implementation which only handles
4 or more symbols).

-Huffman with 4 symbols is either a plain binary code, or
a 1-2-3-3 tree. This is probably the bondary where ad-hoc
implementations stop making sense. It's noteworthy that
this amounts to 2bpp bitmaps, which is a very common bit
depth.

### Storing the Huffman decoding tree

-A Huffman tree is a binary tree. For a tree representing
N distinct symbols as used in the source, there are N-1
decision nodes, each reprenting 1 bit of the input stream.

-The straightforward approach stores a tree of N-1 nodes,
where each node contains 2 references, which can either be
another node or an explicit symbol.

-It is also possible to consider 2N+1 nodes, where N leaf
nodes each reference exactly one symbol and N-1 inner nodes
each reference exactly 2 other nodes. That approach can open
possibilities to store both types of nodes separately.

-If we make the last inner node the root of the tree, which
is natural during construction, inner nodes can reference
either N-2 other inner nodes, or N symbols. That approach is
potentially useful when N is of the form 2^k+1, which can
happen e.g. with a BWT's termination symbol, or with a step
involving bijective numeration.

-The leaf nodes can be skipped entirely if the symbols are
exactly integers in the range from 0 to N-1. If that range
is offset (from A to A+N-1), the offset A can be stored as
a separate entry. If the integers are sparse, they can be
stored as a bitmap, or as hierarchical bitmaps (where the
number of levels and the width of each chunk are parameters).

### Somewhere between Huffman and Arithmetic

Taking the extreme example of a dictionary with 2 input symbols,
Huffman obviously offers no compression.

Now, taking inspiration from Arithmetic coding, we can take
the probabilities P0 and P1 of the two symbols, and make guesses
about the probabilities of pairs of symbols, where the probability
of 00 is assumed to be P0^2, that of 01 and 10 to be P0*P1, and
that of 11 to be P1^2.

However, unless the data is perfectly shuffled, it's more efficient
to run Huffman on pairs of symbols directly.

That can be extended beyond pairs of symbols as needed, based on
the number of distinct symbols and the size of the input.

### Canonical Huffman

## LZ78 family (including LZW sub-family)

The family is essentially split in two, with plain LZ78 on one side
storing unmatched symbols in the stream, while LZW seeds the dictionary
with all possible symbols such that no symbol can be unmatched. The
two approaches potentially have pros and cons, and therefore neither
should be ruled away off-hand.

LZ78 can easily be combined with a Huffman stage for the unmatched
symbols.

### Dealing with a full dictionary

There are multiple approaches to handle a full dictionary:
    -stop adding to the dictionary (the default LZW behavior)
    -clear the whole dictionary (explicit in LZW, implicit in LZC)
    -remove the LRU entry (used in LZT)
    -remove one unreferenced entry (used in BTLZ)
    -remove all unreferenced entries (used in LZJ)

LZW has a command to clear the dictionary on demand, which requires
the encoder to make a complex decision.

LZC discards the dictionary when compression becomes poor, which
requires an arbitrary parameter that needs to be pre-agreed on.

### Adding to the dictionary

There are also multiple approaches about creating new dictionary
entries:
    -with a single new character (used in most schemes, including
    plain LZ78 and LZW)
    -as a concatenation of two entries (LZMW)
    -as a concatenation of the first entry with all prefixes of a
    second entry (LZAP)
    -with a large range of substrings, which makes the set of
    possible matches resemble that of LZ77 (LZJ)

The LZW approach is a narrow subset of LZAP, where only
the smallest prefix is used, while the LZMW approach is the other
narrow subset where only the longest prefix is used.

### Thoughts on single-character adds

Adding a single character to the dictionary, as done by LZ78 and LZW,
might have some unexpected effects: if a sequence of characters tends
to repeat but is originally matched by two dictionary entries, the
first occurrence will create a slightly longer dictionary entry for
the prefix, and a greedy match at the next occurrence will match one
more symbol, using the longer recent dictionary entry but then missing
an opportunity to re-use a second entry, having matched one character
too many.

One approach is to add an entry for the matching suffix of the second
entry.

Another approach is to be slightly less greedy, and to optionally match
one fewer characters than optimal, by doing a lookahead on the next
match.

This is where LZMW has an adge: it avoids situations where a greedy
match catches exactly one symbol too many and misses the next match
opportunity

### Literal symbol runs

In their original form, both LZ77 and LZ78 alternate exactly one match
(which could be empty) and a single literal symbol.

In the LZ77 family, multiple options have been explored to break that
requirement, including LZSS and deflate. In those cases indeed, how
the symbols are decoded doesn't matter for future decoding since the
only state is the output, and those formats allow multiple matches
in a row without any literal symbol, as well as multiple literal
symbols in a row without any matches.

In the LZ78 family, building the dictionary is an important aspect.
A sequence of unmatched symbols would use entry 0 (empty) between
the symbols, but each symbol would only ever appear once in such
a context, since there would later be a dictionary entry containing
exactly that symbol, and referencing that dictionary entry is
important as it creates a two-character entry, and so on. As a result,
in LZ78, unmatched solitary symbols only ever appear once, and
attempting to reduce references to those also reduces the opportunities
to create new dictionary entries.

Instead, it might make sense to apply some partial Huffman coding to
the earlier entries in the dictionary, which are the shorter ones and
therefore are more likely to appear often. The cutoff between Huffman
and literal coding is a potential parameter, to be researched.

### Encoder/decoder implementation

While literature tends to describe the encoder and decoder for
LZ78/LZW as using a dictionary and constructing it the same way,
reality is slightly different: during encoding, it's very natural
to contruct a trie, while decoding is probably happening with a
tree that can only be navigated toward the root.

This is where LZMW becomes odd. The encoding isn't a pure trie
any more, because some nodes in the trie woudln't match any
dictionary entries, while at decoding time the data structure
evolves from a tree to a DAG.

### LZ78 fileformat

Stream style:
```
0 = LZ78-style (with literal symbols in stream)
1 = LZW-style (with dictionary initialized with all symbols)
```
TODO: might be entirely different top-level formats

Full-dictionary handling:
```
00: Clear LRU entry (like in LZT)
01: Clear one leaf entry, round-robin (like in BTLZ)
10: Clear all leaf entries (like in LZAP)
110: Clear all entries
111: Keep dictonary full, stop adding
```
TODO: if there's a clear winner, possibly move encoding to 1-2-3-4-4 bits.
TOOD: use explicit clears for all styles, or only on keep full?

New entry creation, LZW-style:
```
-0 append first character of next match (like LZW)
-10 append all of next match (like LZMW)
-11 append all prefixes of next match (like LZAP)
```

Extra entry creation, for single-symbol scenarios:
```
0: no extra entry
1: also add longest suffix of current match, if long enough
```

Literal symbol encoding (LZ78-style):
```
0: plain
1: Huffman
```

Dictionary entry encoding:
```
0: plain
10: partial Huffman, keep for whle document
11: partial Huffman, new code on dictionary reset
```

Magic values (TBD):
```
0: empty dictionary entry, EOF in LZW-style
1: clear dictionary
2: EOF in LZ78 style
```
TODO: study more.
LZW could use 0 for clear and 0-0 for EOF.
LZ78 has to have dedicated entries for clear, for EOF.
LZ78 could use max_entry + 1 for EOF, which requires early change.
Clear might only be needed when the dictionary doesn't clear itself.

Simplest approach, works in all cases
```
0 = empty
1 = EOF
2 = clear
```

Literal size for non-Huffman LZ78 case, unless known otherwise:
```
000: 3 bits
001: 4 bits
010: 5 bits
011: 6 bits
100: 7 bits
101: 8 bits
110: 9 bits
111: 10 bits
```

Max dictionary entry size:
```
00: 8 bits
01: 10 bits
10: 12 bits
11: 14 bits
```

TODO: case where symbols aren't zero-based.

# Atari ST

## Known image file formats

NEO, PI1, PI2, PI3, PC1, PC2, PC3, IFF, SPU, SPC, KID, CA1, CA2, CA3

## Squeezer image file format

Extensions: QS1, QS2, QS3, QSI

2 bits: 00-01-10 standard resolutions 11 custom

For standard color resolutions, 1 bit for palette depth
(0 = ST, 1 = STe), then the right number of bits to specify
what is present, then the palette entries, packed RGB MSB.

Encoding information:


# Other computers

Atari 8-bit: A
BBC micro: B
Amstrad CPC: C
Amiga: L (Lorraine)
Generic: Q (SQI)
TI 99/4A: N
(ST: S)
Thomson: T
C64: V (VIC)
ZX Spectrum: Z
