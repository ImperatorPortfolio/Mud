#ifndef ZEROPOINT_COMM_INPUT_WEAK_H
#define ZEROPOINT_COMM_INPUT_WEAK_H

/*
 * comm.c still carries the inherited canonical line parser.  Keep that
 * implementation available as a weak fallback so input_line.c can provide
 * the authoritative parser without replacing the large legacy comm.c file.
 */
#if defined(__GNUC__) || defined(__clang__)
struct descriptor_data;
void read_from_buffer( struct descriptor_data *d ) __attribute__((weak));
#endif

#endif /* ZEROPOINT_COMM_INPUT_WEAK_H */
