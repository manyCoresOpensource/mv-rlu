enum {
    NUM_SOCKET = 2,
    NUM_PHYSICAL_CPU_PER_SOCKET = 12,
    SMT_LEVEL = 2,
};

const int OS_CPU_ID[NUM_SOCKET][NUM_PHYSICAL_CPU_PER_SOCKET][SMT_LEVEL] = {
    { /* socket id: 0 */
        { /* physical cpu id: 0 */
          0, 24,     },
        { /* physical cpu id: 1 */
          1, 25,     },
        { /* physical cpu id: 2 */
          2, 26,     },
        { /* physical cpu id: 3 */
          3, 27,     },
        { /* physical cpu id: 4 */
          4, 28,     },
        { /* physical cpu id: 5 */
          5, 29,     },
        { /* physical cpu id: 8 */
          6, 30,     },
        { /* physical cpu id: 9 */
          7, 31,     },
        { /* physical cpu id: 10 */
          8, 32,     },
        { /* physical cpu id: 11 */
          9, 33,     },
        { /* physical cpu id: 12 */
          10, 34,     },
        { /* physical cpu id: 13 */
          11, 35,     },
    },
    { /* socket id: 1 */
        { /* physical cpu id: 0 */
          12, 36,     },
        { /* physical cpu id: 1 */
          13, 37,     },
        { /* physical cpu id: 2 */
          14, 38,     },
        { /* physical cpu id: 3 */
          15, 39,     },
        { /* physical cpu id: 4 */
          16, 40,     },
        { /* physical cpu id: 5 */
          17, 41,     },
        { /* physical cpu id: 8 */
          18, 42,     },
        { /* physical cpu id: 9 */
          19, 43,     },
        { /* physical cpu id: 10 */
          20, 44,     },
        { /* physical cpu id: 11 */
          21, 45,     },
        { /* physical cpu id: 12 */
          22, 46,     },
        { /* physical cpu id: 13 */
          23, 47,     },
    },
};
