
#include "utl/bit/utl_bit_ceil.h"
#include "utl/bit/utl_bit_floor.h"
#include "utl/bit/utl_bit_width.h"
#include "utl/bit/utl_countl_one.h"
#include "utl/bit/utl_countl_zero.h"
#include "utl/bit/utl_countr_one.h"
#include "utl/bit/utl_countr_zero.h"
#include "utl/bit/utl_has_single_bit.h"
#include "utl/bit/utl_popcount.h"

// utl_bit_ceil
static_assert(utl::bit_ceil(0x0u) == 0x1u);
static_assert(utl::bit_ceil(0x1u) == 0x1u);
static_assert(utl::bit_ceil(0x2u) == 0x2u);
static_assert(utl::bit_ceil(0x3u) == 0x4u);
static_assert(utl::bit_ceil(0x5u) == 0x8u);

// utl_bit_floor
static_assert(utl::bit_floor(0x0u) == 0x0u);
static_assert(utl::bit_floor(0x1u) == 0x1u);
static_assert(utl::bit_floor(0x2u) == 0x2u);
static_assert(utl::bit_floor(0x3u) == 0x2u);
static_assert(utl::bit_floor(0x5u) == 0x4u);

// utl_bit_width
static_assert(utl::bit_width(0x0u) == 0u);
static_assert(utl::bit_width(0x1u) == 1u);
static_assert(utl::bit_width(0x2u) == 2u);
static_assert(utl::bit_width(0x3u) == 2u);
static_assert(utl::bit_width(0x5u) == 3u);

// utl_countl_one
static_assert(utl::countl_one<unsigned char>(0xF0u) == 4u);
static_assert(utl::countl_one<unsigned char>(0xFFu) == 8u);
static_assert(utl::countl_one(0x7Fu) == 0u);
static_assert(utl::countl_one<unsigned char>(0xAAu) == 1u);
static_assert(utl::countl_one(0x00u) == 0u);

// utl_countl_zero
static_assert(utl::countl_zero<unsigned char>(0x0Fu) == 4u);
static_assert(utl::countl_zero<unsigned char>(0x00u) == 8u);
static_assert(utl::countl_zero<unsigned char>(0xFFu) == 0u);
static_assert(utl::countl_zero<unsigned char>(0x55u) == 1u);
static_assert(utl::countl_zero<unsigned char>(0x80u) == 0u);

// utl_countr_one
static_assert(utl::countr_one(0xF0u) == 0u);
static_assert(utl::countr_one(0x0Fu) == 4u);
static_assert(utl::countr_one(0xFFu) == 8u);
static_assert(utl::countr_one(0x55u) == 1u);
static_assert(utl::countr_one(0x80u) == 0u);

// utl_countr_zero
static_assert(utl::countr_zero(0x0Fu) == 0u);
static_assert(utl::countr_zero(0xF0u) == 4u);
static_assert(utl::countr_zero<unsigned char>(0x00u) == 8u);
static_assert(utl::countr_zero(0x54u) == 2u);
static_assert(utl::countr_zero(0x80u) == 7u);

// utl_has_single_bit
static_assert(utl::has_single_bit(0x10u) == true);
static_assert(utl::has_single_bit(0x40u) == true);
static_assert(utl::has_single_bit(0xF0u) == false);
static_assert(utl::has_single_bit(0x00u) == false);
static_assert(utl::has_single_bit(0x01u) == true);
static_assert(utl::has_single_bit(0x22u) == false);

// utl_popcount
static_assert(utl::popcount(0xF0u) == 4u);
static_assert(utl::popcount(0x00u) == 0u);
static_assert(utl::popcount(0xFFu) == 8u);
static_assert(utl::popcount(0xAAu) == 4u);
static_assert(utl::popcount(0x81u) == 2u);
