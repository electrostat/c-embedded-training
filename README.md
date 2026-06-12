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
### JSON Writer
(Deterministic, Embedded‑Friendly Serialization)

A zero‑allocation, streaming JSON writer designed for deterministic behavior and embedded constraints.
This module complements the JSON parsers by providing a clean, state‑driven way to produce JSON with full control over formatting and error handling.

Core Features:
- Deterministic state machine  
    Explicit writer states (START, KEY, VALUE, AFTER_VALUE) ensure predictable behavior and early detection of invalid sequences.
- Zero dynamic memory  
    All state is stored internally; output is emitted through a user‑supplied callback. No heap usage, no hidden buffers.
- Scope‑tracked object/array construction  
    Uses a fixed‑depth stack to manage nested objects and arrays with overflow protection.
- Pretty‑printing support  
    Optional indentation and newlines for human‑readable output, with correct JSON formatting rules.
- Deferred array opening  
    Arrays emit [ only when the first value is written, ensuring no partial output is produced if an array is invalidated before completion.
- Transactional reset semantics  
    json_writer_reset() restores the writer to a clean state after an error, allowing safe reuse without leaking partial output.
- Comprehensive test suite  
    Covers nested structures, mixed types, invalid sequences, depth overflow, raw injection, and reset behavior.

This module is ideal for:
- Logging and telemetry in embedded systems
- Structured output over UART, sockets, or DMA streams
- Deterministic serialization for state machines
- Producing JSON alongside the SAX‑style parser for round‑trip tests

---
### State Machine
(Deterministic, Table‑Driven, Embedded‑Friendly)

A lightweight, zero‑allocation state machine framework designed for deterministic behavior and clarity in embedded systems.
This module provides a clean, table‑driven architecture with explicit state transitions, predictable execution, and a fully opaque context for safe encapsulation.

Core Features:
- Opaque context (sm_context_t)
    Internal state is hidden behind a private struct, ensuring clean API boundaries and preventing accidental misuse.
- Deterministic dispatch logic
    All transitions are defined in a fixed transition table. No dynamic memory, no runtime allocation, no surprises.
- Safe event handling  
    Full bounds checking for invalid or negative events. Prevents undefined behavior and guarantees predictable outcomes.
- Optional transition handlers  
    Each transition may include a guard or side‑effect callback, enabling flexible behavior without complicating the core logic.
- Comprehensive test harness  
    Includes a private header for white‑box testing, Makefile integration, and unit tests covering initialization, valid transitions, invalid events, and guard behavior.
- Clean, extensible design  
    Ideal for protocol engines, control loops, UI navigation, or any embedded workflow requiring deterministic state transitions.

This module is ideal for:
- Control logic in embedded systems
- Protocol parsing and command handling
- UI/menu navigation
- Deterministic workflow engines
- Any system requiring predictable, testable state transitions


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

