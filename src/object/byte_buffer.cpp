#include "object/byte_buffer.hpp"

#include <iostream>

byte_buffer::byte_buffer()
{
  chunk_list_.push_back(buffer_chunk());
}

byte_buffer::~byte_buffer()
{}

void byte_buffer::append(const void *bytes, byte_buffer::size_type size)
{
  std::cout << "appending " << size << " bytes to list (current size: " << this->size() << ") ... ";
  const char *ptr = (const char*)bytes;
  size_type bytes_written = 0;
  while (chunk_list_.back().available() < size) {
    buffer_chunk &chunk = chunk_list_.back();
    // copy first part of bytes
    std::copy(ptr+bytes_written, ptr+bytes_written+chunk.available(), &chunk.data[chunk.write_cursor]);
    // adjust remaining size
    size -= chunk.available();
    // adjust written bytes
    bytes_written += chunk.available();
    // adjust cursor
    chunk.write_cursor += chunk.available();
    // append new buffer chunk
    chunk_list_.push_back(buffer_chunk());
  }
  if (size > 0) {
    buffer_chunk &chunk = chunk_list_.back();
    // append remaining bytes
    std::copy(ptr+bytes_written, ptr+bytes_written+size, &chunk.data[chunk.write_cursor]);
    chunk.write_cursor += size;
  }
  std::cout << "finished (new size: " << this->size() << ")\n";
}

void byte_buffer::release(void *bytes, byte_buffer::size_type size)
{
  std::cout << "releasing " << size << " bytes from list (current size: " << this->size() << ") ... ";  
  char *ptr = (char*)bytes;
  size_type bytes_read = 0;
  while (chunk_list_.front().used() <= size) {
    buffer_chunk &chunk = chunk_list_.front();
    // available bytes are not enough
    // copy first part
    std::copy(chunk.data.begin()+chunk.read_cursor, chunk.data.begin()+chunk.read_cursor+chunk.used(), ptr+bytes_read);
    // adjust remaining size
    size -= chunk.used();
    // adjust bytes read
    bytes_read += chunk.used();
    // pop back
    chunk_list_.pop_front();
    // get next chunk
  }
  if (size > 0) {
    buffer_chunk &chunk = chunk_list_.front();
    // copy rest of bytes
    std::copy(chunk.data.begin()+chunk.read_cursor, chunk.data.begin()+chunk.read_cursor+size, ptr+bytes_read);
    // adjust read cursor
    chunk.read_cursor += size;
  }
  if (chunk_list_.empty()) {
    chunk_list_.push_back(buffer_chunk());
  }
  std::cout << "finished (new size: " << this->size() << ")\n";
}

byte_buffer::size_type byte_buffer::size() const
{
  if (chunk_list_.size() == 1) {
    return BUF_SIZE - chunk_list_.front().available();
  } else {
    return (chunk_list_.size() * BUF_SIZE) - chunk_list_.back().available();
  }
}

void byte_buffer::clear()
{
  chunk_list_.clear();
  chunk_list_.push_back(buffer_chunk());
}
