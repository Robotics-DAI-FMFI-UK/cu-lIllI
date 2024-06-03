#ifndef _MOVEMENT_SEQUENCE_PARSER_H_
#define _MOVEMENT_SEQUENCE_PARSER_H_

#include <inttypes.h>

#include "movement_sequence.h"


/**
 * @class movement_sequence_parser
 * @brief Parses data into movement sequence.
 */
class movement_sequence_parser 
{
  private:

    const char *last_error;

  public:
  /**
   * Parses a 16-bit unsigned integer from the provided data pointer in little-endian format.
   * The data pointer is incremented by 2 bytes after reading the integer.
   *
   * @param data A double pointer to the byte array from which the 16-bit unsigned integer is read.
   * @return The parsed 16-bit unsigned integer.
  */
  static uint16_t get_uint16_t(uint8_t **data);

  /**
   * Parses a 24-bit unsigned integer from the provided data pointer in little-endian format.
   * The data pointer is incremented by 3 bytes after reading the integer.
   *
   * @param data A double pointer to the byte array from which the 24-bit unsigned integer is read.
   * @return The parsed 24-bit unsigned integer.
  */
  static uint32_t get_uint24_t(uint8_t **data);

  /**
   * Parses two 12-bit unsigned integers from the provided data pointer in little-endian format.
   * The data pointer is incremented by 3 bytes after reading the integers.
   *
   * @param data A double pointer to the byte array from which the 12-bit unsigned integers are read.
   * @param v1 Output pointer to the first 12-bit unsigned integer.
   * @param v2 Output pointer to the second 12-bit unsigned integer.
  */
  static void get_two_uint12_t(uint8_t **data, uint16_t *v1, uint16_t *v2);

  /**
   * @brief Loads movement sequence
   * @param ms Pointer to the movement sequence object to load the data into.
   * @param data Pointer to the data array containing the sequence information.
   * @return int 0 if successful, otherwise an error code.
   */
  int load(movement_sequence *ms, uint8_t *data);

  /**
   * @brief Retrieves the last error encountered during parsing.
   * @return char* String describing the last error.
   */
  const char *get_last_error();
};

#endif