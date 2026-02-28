#pragma once
 // goofy lightweight random
uint8_t rand(uint8_t num) {
    static uint8_t seed = 42;
    seed ^= seed << 3;
    seed ^= seed >> 5;
    seed ^= seed << 1;
    return seed % num;
}