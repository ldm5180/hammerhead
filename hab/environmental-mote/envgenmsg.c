/**
 * This file is automatically generated by mig. DO NOT EDIT THIS FILE.
 * This file implements the functions for encoding and decoding the
 * 'env_general_msg' message type. See envgenmsg.h for more details.
 */

#include <message.h>
#include "envgenmsg.h"

uint16_t ENVGENMSG_node_id_get(tmsg_t *msg)
{
  return tmsg_read_ube(msg, 0, 16);
}

void ENVGENMSG_node_id_set(tmsg_t *msg, uint16_t value)
{
  tmsg_write_ube(msg, 0, 16, value);
}

uint16_t ENVGENMSG_volt_get(tmsg_t *msg)
{
  return tmsg_read_ube(msg, 16, 16);
}

void ENVGENMSG_volt_set(tmsg_t *msg, uint16_t value)
{
  tmsg_write_ube(msg, 16, 16, value);
}

uint16_t ENVGENMSG_temp_get(tmsg_t *msg)
{
  return tmsg_read_ube(msg, 32, 16);
}

void ENVGENMSG_temp_set(tmsg_t *msg, uint16_t value)
{
  tmsg_write_ube(msg, 32, 16, value);
}

uint16_t ENVGENMSG_photo_get(tmsg_t *msg)
{
  return tmsg_read_ube(msg, 48, 16);
}

void ENVGENMSG_photo_set(tmsg_t *msg, uint16_t value)
{
  tmsg_write_ube(msg, 48, 16, value);
}
