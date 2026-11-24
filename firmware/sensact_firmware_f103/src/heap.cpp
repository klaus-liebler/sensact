#include "heap.hpp"

#include <contracts>
#include <cstdint>
#include <new>

#ifndef SENSACT_HEAP_BYTES
#define SENSACT_HEAP_BYTES 8192
#endif

namespace {

// Blockkopf: Groesse inkl. Kopf (Vielfaches von 8), belegt ja/nein. Die Bloecke liegen lueckenlos.
struct Block {
    std::uint32_t size;
    std::uint32_t used;
};
static_assert(sizeof(Block) == 8);

alignas(8) std::byte arena[SENSACT_HEAP_BYTES];
bool initialized = false;
bool sealed = false;
std::size_t in_use_bytes = 0;
std::size_t peak_bytes = 0;

Block* at(std::size_t offset) noexcept { return reinterpret_cast<Block*>(arena + offset); }

void init() noexcept {
    *at(0) = {SENSACT_HEAP_BYTES, 0};
    initialized = true;
}

void* allocate(std::size_t size) noexcept {
    if (!initialized) init();
    contract_assert(!sealed);  // eine Allokation im Betrieb: s. heap.hpp
    const std::size_t need = ((size + 7) & ~std::size_t{7}) + sizeof(Block);
    for (std::size_t off = 0; off < SENSACT_HEAP_BYTES; off += at(off)->size) {
        Block* b = at(off);
        if (b->used || b->size < need) continue;
        if (b->size - need >= 2 * sizeof(Block)) {  // teilen
            *at(off + need) = {static_cast<std::uint32_t>(b->size - need), 0};
            b->size = static_cast<std::uint32_t>(need);
        }
        b->used = 1;
        in_use_bytes += b->size;
        if (in_use_bytes > peak_bytes) peak_bytes = in_use_bytes;
        return b + 1;
    }
    contract_assert(false);  // Bereich voll: SENSACT_HEAP_BYTES erhoehen
    return nullptr;
}

void release(void* p) noexcept {
    if (p == nullptr) return;
    Block* b = static_cast<Block*>(p) - 1;
    b->used = 0;
    in_use_bytes -= b->size;
    // freie Nachbarn zusammenfassen (die Liste ist kurz: nur beim Start benutzt)
    for (std::size_t off = 0; off < SENSACT_HEAP_BYTES;) {
        Block* cur = at(off);
        const std::size_t next = off + cur->size;
        if (!cur->used && next < SENSACT_HEAP_BYTES && !at(next)->used) {
            cur->size += at(next)->size;
            continue;
        }
        off = next;
    }
}

}  // namespace

namespace heap {
void seal() noexcept { sealed = true; }
std::size_t capacity() noexcept { return SENSACT_HEAP_BYTES; }
std::size_t in_use() noexcept { return in_use_bytes; }
std::size_t peak() noexcept { return peak_bytes; }
}  // namespace heap

void* operator new(std::size_t size) { return allocate(size); }
void* operator new[](std::size_t size) { return allocate(size); }
void* operator new(std::size_t size, const std::nothrow_t&) noexcept { return allocate(size); }
void* operator new[](std::size_t size, const std::nothrow_t&) noexcept { return allocate(size); }
void operator delete(void* p) noexcept { release(p); }
void operator delete[](void* p) noexcept { release(p); }
void operator delete(void* p, std::size_t) noexcept { release(p); }
void operator delete[](void* p, std::size_t) noexcept { release(p); }
