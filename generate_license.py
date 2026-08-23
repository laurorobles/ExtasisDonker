#!/usr/bin/env python3
"""
Extasis Donker v1.0 — Official License Key Generator
Use this tool to generate individual serials, gift keys, or batch export for Bandcamp / Gumroad / Plugin Boutique.
"""

import sys
import argparse
import random

SALT_1 = 0xB894E28F4A6C39D7
SALT_2 = 0x7F5D81C32E96B44B
MASK_64 = 0xFFFFFFFFFFFFFFFF
MASK_16 = 0xFFFF

def generate_serial_for_seed(val1: int) -> str:
    val1 = val1 & MASK_16
    if val1 == 0:
        val1 = 1
    seed = ((val1 << 32) | val1) & MASK_64
    
    expected2 = (((seed ^ SALT_1) * 0x39E4F2B) & MASK_64) >> 16
    expected2 &= MASK_16
    
    rot = (((seed << 11) & MASK_64) | (seed >> 21)) & MASK_64
    expected3 = (((rot ^ SALT_2) * 0x18C3FA4D) & MASK_64) >> 16
    expected3 &= MASK_16
    
    expected4 = ((val1 ^ expected2 ^ expected3 ^ 0xCAFE) * 0x228BA3) & MASK_16
    
    return f"EXTD-{val1:04X}-{expected2:04X}-{expected3:04X}-{expected4:04X}".upper()

def validate_serial(serial: str) -> bool:
    s = serial.strip().upper().replace("-", "").replace(" ", "").replace("\t", "").replace("\r", "").replace("\n", "")
    if len(s) != 20:
        return False
    if not s.startswith("EXTD"):
        return False
    try:
        val1 = int(s[4:8], 16)
        val2 = int(s[8:12], 16)
        val3 = int(s[12:16], 16)
        val4 = int(s[16:20], 16)
        
        seed = ((val1 << 32) | val1) & MASK_64
        expected2 = (((seed ^ SALT_1) * 0x39E4F2B) & MASK_64) >> 16
        expected2 &= MASK_16
        
        rot = (((seed << 11) & MASK_64) | (seed >> 21)) & MASK_64
        expected3 = (((rot ^ SALT_2) * 0x18C3FA4D) & MASK_64) >> 16
        expected3 &= MASK_16
        
        expected4 = ((val1 ^ expected2 ^ expected3 ^ 0xCAFE) * 0x228BA3) & MASK_16
        
        return (val2 == expected2 and val3 == expected3 and val4 == expected4)
    except Exception:
        return False

def main():
    parser = argparse.ArgumentParser(description="Extasis Donker License Key Generator")
    parser.add_argument("-n", "--count", type=int, default=1, help="Number of licenses to generate (default: 1)")
    parser.add_argument("-v", "--validate", type=str, help="Validate an existing license key")
    parser.add_argument("-o", "--output", type=str, help="Output CSV/TXT file path")
    args = parser.parse_args()

    if args.validate:
        is_valid = validate_serial(args.validate)
        if is_valid:
            print(f"✅ Key '{args.validate.strip()}' is VALID.")
            sys.exit(0)
        else:
            print(f"❌ Key '{args.validate.strip()}' is INVALID.")
            sys.exit(1)

    serials = []
    for _ in range(args.count):
        seed = random.randint(1, 0xFFFE)
        serials.append(generate_serial_for_seed(seed))

    if args.output:
        with open(args.output, "w") as f:
            for s in serials:
                f.write(s + "\n")
        print(f"✅ Generated {args.count} license(s) to '{args.output}'.")
    else:
        for s in serials:
            print(s)

if __name__ == "__main__":
    main()
