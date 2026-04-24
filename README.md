# c-embedded-training

A growing collection of small, focused C modules designed to strengthen embedded-systems fundamentals without requiring any hardware. This repository serves as a practical training ground for building confidence with low-level patterns, deterministic behavior, and embedded-friendly design.

The goal is simple: write clear, correct, modular C code that reflects the mindset of real embedded development. Each component is intentionally scoped, easy to understand, and built to be extended over time.

---

## Purpose

This project exists to explore and practice the core ideas that show up again and again in embedded systems:

- deterministic execution  
- fixed-size data structures  
- predictable memory behavior  
- fixed-point arithmetic  
- modular architecture  
- test-driven development  

Everything here is written with an embedded engineer’s perspective, but without tying anything to a specific microcontroller or hardware platform. It’s all about the fundamentals.

---

## Current Modules

### **Fixed-Point Math Library**
A modular, extensible fixed-point arithmetic framework supporting multiple formats (Q15, Q31, Q7.8).  
Includes:

- format-specific conversion and arithmetic  
- unified `fixed_point_t` abstraction  
- saturation and truncation behavior  
- comprehensive math-correctness test suites  

This module mirrors the kinds of numeric work done in DSP, control loops, and embedded signal processing.

---

### **Ring Buffer**
A lightweight, allocation-free circular buffer suitable for embedded environments.  
Features:

- constant-time push/pop  
- configurable element size  
- no dynamic memory  
- clean, test-backed API  

Useful as a foundational building block for drivers, streaming interfaces, and ISR-safe queues.

---
### **JSON Parser** 
(SAX‑Style, Embedded‑Friendly)

A pair of embedded‑friendly JSON parsers built around a clean SAX‑style event interface.
Both variants emphasize deterministic behavior, fixed memory usage, and correctness over general‑purpose flexibility.

Core Features (shared by both parsers):
- Event‑driven design: 
    Emits callbacks for object/array boundaries, keys, strings, numbers, booleans, and null values. Ideal for low‑memory, incremental, or real‑time processing.
- Deterministic state machine:   
    Explicit grammar enforcement using well‑defined parser states (EXPECT_VALUE, EXPECT_KEY, EXPECT_COLON, EXPECT_COMMA_OR_END).
    Rejects malformed JSON early and predictably.
- Zero dynamic memory:  
    Uses a fixed‑depth context stack for nested structures.
    No heap usage, no hidden allocations, and no surprises.
- Convenience API:  
    A simple one‑shot function wraps initialization and parsing for straightforward use
- Comprehensive test suite:  
    Covers basic objects, arrays, nested structures, mixed types, and error cases.
    Ensures correctness and stability before future extensions.

### Standard JSON Parser (One‑Shot)
A simple, single‑buffer parser for cases where the entire JSON document is available at once.
Provides a convenience API for straightforward use in embedded applications.

### Streaming JSON Parser (Incremental, Chunk‑Safe)
A new module that enables JSON parsing from arbitrary input chunks—ideal for sockets, UART streams, DMA buffers, or any environment where data arrives incrementally.

Streaming‑Specific Capabilities:
- Chunk‑safe tokenizer
    Handles strings, numbers, and literals even when split across chunk boundaries. Correctly decodes escape sequences (\", \\) across chunks.
- Stable token memory
    Tokens are emitted from internal fixed buffers, never from transient input slices.
- Incremental parsing loop
    feed() returns NEED_MORE, OK, or DONE, allowing the caller to drive parsing at their own pace.
- Identical callback interface
    The streaming parser uses the same event callbacks as the one‑shot parser, making it easy to switch between them.

### Current Scope
The streaming parser is functionally complete and fully tested for correctness across chunk boundaries.

Future enhancements (planned as separate PRs):
- Full JSON escape set (\n, \t, \r, \b, \f, \/)
- Unicode escape decoding (\uXXXX)
- Additional error reporting and fuzz testing

---

## Philosophy

This repository treats embedded engineering as a craft. Each module is an artifact: small, intentional, and designed to teach or reinforce a specific pattern. The emphasis is on clarity, correctness, and maintainability.

As the project grows, it will continue to accumulate modules that explore topics such as:

- state machines  
- cooperative schedulers  
- lookup tables  
- fixed-size allocators  
- protocol parsers  
- digital filters  
- hardware abstraction patterns  

The idea is to build a library of reusable embedded concepts, one focused module at a time.


---

## Outlook

This repository will expand steadily as new concepts are explored. The long-term goal is to create a well-organized, well-tested collection of embedded-friendly C modules that reflect real-world engineering practices—while staying approachable and fun to work on.

More modules, more patterns, and more embedded fundamentals will be added over time.

