'use strict';

/**
 * 1x1 pixel gif.
 */
module.exports = new Buffer([
    // G     I     F     8     9     a
    0x47, 0x49, 0x46, 0x38, 0x39, 0x61,

    // 1 pixel width
    0x01, 0x00,

    // 1 pixel height
    0x01, 0x00,

    // bit 0:    Global Color Table Flag (GCTF)
    // bit 1..3: Color Resolution
    // bit 4:    Sort Flag to Global Color Table
    // bit 5..7: Size of Global Color Table: 2^(1+n)
    0x00,

    // Background Color Index
    0x00,

    // Pixel Aspect Ratio
    0x00,

    // Global Color Table (0 entries in our case)

    // Blocks

    // Image Separator
    0x2c,

    // Image Left Position
    0x00, 0x00,

    // Image Top Position
    0x00, 0x00,

    // Image Width
    0x01, 0x00,

    // Image Height
    0x01, 0x00,

    // bit 0:    Local Color Table Flag (LCTF)
    // bit 1:    Interlace Flag
    // bit 2:    Sort Flag
    // bit 2..3: Reserved
    // bit 4..7: Size of Local Color Table: 2^(1+n)
    0x00,

    // LZW Minimum Code Size
    0x02,

    // Block Size
    0x02,

    // Image Data (2 bytes)
    0x4c, 0x01,

    // Block Terminator
    0x00,

    // Trailer
    0x3b
]);

