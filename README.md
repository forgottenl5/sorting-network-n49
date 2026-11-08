# Sorting network for 49 inputs: 364 comparators, 22 layers

A sorting network for **49 inputs** that uses **364 comparators** in **22 layers**.

As of 7 November 2026, the best size listed for 49 inputs on Bert Dobbelaere's
[list of sorting networks](https://bertdobbelaere.github.io/sorting_networks_extended.html)
(page dated 29 October 2026) is **365 comparators** (at 21 layers). This network is one comparator smaller.
It was submitted to the list's maintainer on 7 November 2026.

| | comparators | layers |
|---|---|---|
| Listed best size (Van Voorhis (4,4) type) | 365 | 21 |
| Listed best depth | 369 | 19 |
| **This network** | **364** | **22** |

## Files

| File | Contents |
|---|---|
| `n49_364_d22_layers.txt` | The network, one layer per line, in the same format as the list: `[(a,b),(c,d),...]` |
| `n49_364_d22_pairs.txt` | The same network for the verifier: first line `49`, then one comparator `a b` per line, in order |
| `xverify.cpp` | Exact verifier (C++, no dependencies) |

Comparators are 0-indexed wire pairs `(a,b)` with `a < b`; each one puts the smaller value on wire `a`.

## How to check it

```
g++ -O2 -o xverify xverify.cpp
./xverify n49_364_d22_pairs.txt
```

Expected output (about 15 seconds on one core):

```
n=49 comparators=364 prefix=87 groups=4 product of reachable states=1.152e+08
checked 115182000 reachable prefix states (covers all 2^49 inputs): SORTS ALL INPUTS (0 failing)
```

**Why this is a complete proof, not a sample.** By the 0-1 principle, a comparator network sorts every input
if and only if it sorts every 0-1 input, here 2^49 of them. The verifier takes the longest opening part of
the network that splits into independent groups of at most 16 wires, and lists every output state each group
can reach by trying all inputs to that group. Every state the whole opening part can reach is a combination of
the groups' states, so checking all 115,182,000 combinations through the rest of the network (64 at a time,
bit-sliced) covers all 2^49 inputs exactly.

The verifier was also run on copies with one comparator removed. It rejected them (for example 111,753
failing states), so it does catch broken networks.

## How it was found

[SorterHunter](https://github.com/bertdobbelaere/SorterHunter) (Dobbelaere's evolutionary search), run on a
rented 8-core cloud machine for about 34 hours in total over two runs, costing about USD 3. The search kept
the opening layers of the listed Van Voorhis (4,4) type network fixed and evolved the rest. A first run found
364 comparators at 24 layers; a second run reached the same size at 22 layers.

## Checksums (SHA-256)

```
175eb340af69cc326cd11382e9cb948d5b13065eb9457e2cba0c87a3f85e4987  n49_364_d22_layers.txt
ef25068de27d9c314437e14e86f37cb6e81b4424a3613ebf3d115c2a8ffdbdc2  n49_364_d22_pairs.txt
```

## Credit

Found and verified by forgottenl5, with Claude (Anthropic) as an AI assistant for the search setup and the verifier.
