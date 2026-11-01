# RAW decompressor

Build the standalone utility with a C++23 compiler and standard library supporting `<print>` and `<format>`. It needs no game SDK:

```sh
clang++ -std=c++23 -O2 tools/RawDecompressor/main.cpp -o /tmp/RawDecompressor
/tmp/RawDecompressor input.raw [--csv] [--packets]
```

A file argument defaults to packet export. A directory argument recursively processes `.raw` and `.RAW` files and defaults to CSV export. Explicit flags select the outputs, so `--csv --packets` enables both. `--help` prints usage.

- Packets are written beside the input in `<input-stem>/packet_001.bin`, numbered in stream order. Each contains the declared number of decoded bytes, including its type DWORD.
- Type35 CSVs are written beside the input as `<input-stem>_packet_001_type35_dump.csv`. Detection reads the full little-endian type DWORD. Each CSV contains at most 64 complete 32-byte actor records. Partial records and bytes beyond record 64 are omitted from CSV, but retained in packet export. A 1796-byte type35 payload supplies 56 rows. Retained game-arena bytes are not part of the file and are never synthesized.
- Existing output paths are overwritten. Distinct type35 packets have distinct CSV names. Inputs with the same stem in the same directory share output paths.
- RAW framing uses a 14-byte header, big-endian decoded/body sizes, and a required `FFFFFFFF` terminator. The decoder is copied verbatim from `RawLoader::DecompressBuffer` in `src/RawLoader.cpp`; it assumes valid compressed data and sufficient output space, without checking buffer bounds, back-references or the decoded size. Malformed compressed data can cause out-of-bounds memory access. Unconsumed bytes within the declared compressed body and bytes after the stream terminator are ignored. The other six header bytes are opaque; checksums are not verified.
- Detected input, packet-framing, filesystem and output-write errors return a nonzero exit code. Directory processing stops at the first failure. Extraction is not transactional: earlier outputs remain, and a failed write can leave a partial file. An empty directory or terminator-only stream succeeds without outputs.
