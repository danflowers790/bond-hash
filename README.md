# bond-hash

simulate Linux bond hashing for L3 + L4

hash result = (srcIP xor dstIP) xor (srcPort xor dstPort)  modulo (number of slaves in Bond)

