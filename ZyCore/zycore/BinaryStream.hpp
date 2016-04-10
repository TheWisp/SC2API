/**
 * This file is part of the zyan core library (zyantific.com).
 * 
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Joel Höner (athre0z)
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software 
 * and associated documentation files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge, publish, distribute, 
 * sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all copies or 
 * substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING 
 * BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef _ZYCORE_BINARYSTREAM_HPP_
#define _ZYCORE_BINARYSTREAM_HPP_

#ifdef ZYCORE_HEADER_ONLY
#   error "This file cannot be used in header-only mode."
#endif // ZYCORE_HEADER_ONLY

#include "zycore/Utils.hpp"
#include "zycore/Exceptions.hpp"

#include <vector>
#include <cassert>
#include <string>

namespace zycore
{

// ============================================================================================== //
// [BaseBinaryStream]                                                                             //
// ============================================================================================== //

/**
 * @brief   Base class for binary streams.
 * @see     IBinaryStream
 * @see     OBinaryStream
 * @see     BinarySteam
 */
class BaseBinaryStream : public NonCopyable
{
public:
    using Buffer = std::vector<uint8_t>;
    using StreamSize = Buffer::size_type;
    using StreamOffs = StreamSize;
protected:
    Buffer *m_buffer;
public:
    /**
     * @brief   Constructor.
     * @param   buffer The buffer to work on.
     */
    BaseBinaryStream(Buffer *buffer);

    /**
     * @brief   Destructor.
     */
    virtual ~BaseBinaryStream() {}
};

// ============================================================================================== //
// [IBinaryStream]                                                                                //
// ============================================================================================== //

/**
 * @brief   Input stream for parsing binary data.
 * 
 * Reading data accesses perform boundary checks. All methods are guaranteed to throw a 
 * @c OutOfBounds in case a requested read operation exceeds the managed buffer's boundaries.
 */
class IBinaryStream : public virtual BaseBinaryStream
{
protected:
    StreamOffs m_rpos = 0;

    /**
     * @internal
     * @brief   Validates if the given offset and length lie outside the buffers boundaries.
     * @param   offs    The offset to check
     * @param   len     The length to add.
     */
    void validateOffset(StreamOffs offs, StreamSize len) const;
public:
    /**
     * @copydoc BaseBinaryStream::BaseBinaryStream
     */
    IBinaryStream(Buffer *buffer);

    /**
     * @brief   Destructor.
     */
    virtual ~IBinaryStream() {}

    /**
     * @brief   Gets the read offset.
     * @return  The read offset.
     */
    StreamOffs rpos() const;

    /**
     * @brief   Sets the read offset.
     * @param   pos The read offset.
     * @return  This instance.
     */
    IBinaryStream& rpos(StreamOffs pos);

    /**
     * @brief   Extracts a potion of the buffer into a new buffer.
     * @param   pos The position to start extracting.
     * @param   len The length of the data to extract.
     * @return  A new buffer containing the requested buffer region.
     */
    Buffer sub(StreamOffs pos, StreamSize len) const;
    
    /**
     * @brief   Extracts an ANSI string from the buffer.
     * @param   pos             The position to start reading the string.
     * @param   maxLen          The maximum length of the string. 0 for infinite.
     */
    std::string extractString8(StreamOffs pos = 0, StreamSize maxLen = 0) const;
        
    /**
     * @brief   Extracts a wide string from the buffer.
     * @param   pos             The position to start reading the string
     * @param   maxLen          The maximum length of the string (in characters). 0 for infinite.
     */
    std::wstring extractString16(StreamOffs pos = 0, StreamSize maxLen = 0) const;
    
    /**
     * @brief   Generates a hex dump using the buffer's data.
     *
     * @param   pos   The position.
     * @param   len   The length.
     * @return  The hex dump.
     *
     * Example return (200 bytes):
     * @code
     *   0x0000 00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f ................
     *   0x0010 10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f ................
     *   0x0020 20 21 22 23 24 25 26 27 28 29 2a 2b 2c 2d 2e 2f  !"#$%&'()*+,-./
     *   0x0030 30 31 32 33 34 35 36 37 38 39 3a 3b 3c 3d 3e 3f 0123456789:;<=>?
     *   0x0040 40 41 42 43 44 45 46 47 48 49 4a 4b 4c 4d 4e 4f @ABCDEFGHIJKLMNO
     *   0x0050 50 51 52 53 54 55 56 57 58 59 5a 5b 5c 5d 5e 5f PQRSTUVWXYZ[\]^_
     *   0x0060 60 61 62 63 64 65 66 67 68 69 6a 6b 6c 6d 6e 6f `abcdefghijklmno
     *   0x0070 70 71 72 73 74 75 76 77 78 79 7a 7b 7c 7d 7e 7f pqrstuvwxyz{|}~.
     *   0x0080 80 81 82 83 84 85 86 87 88 89 8a 8b 8c 8d 8e 8f ................
     *   0x0090 90 91 92 93 94 95 96 97 98 99 9a 9b 9c 9d 9e 9f ................
     *   0x00a0 a0 a1 a2 a3 a4 a5 a6 a7 a8 a9 aa ab ac ad ae af ................
     *   0x00b0 b0 b1 b2 b3 b4 b5 b6 b7 b8 b9 ba bb bc bd be bf ................
     *   0x00c0 c0 c1 c2 c3 c4 c5 c6 c7                         ........
     * @endcode
     */
    std::string hexDump(StreamOffs pos, StreamSize len) const;

    /**
     * @overload
     * The hex-dump contains the whole buffer from 0 to buffer's size.
     */
    std::string hexDump() const;

    /**
     * @brief   Retrieves a constant pointer of a location inside the buffer.
     * @tparam  T       The pointer's type.
     * @param   pos     The position.
     * @return  The desired pointer.
     */
    template<typename T> const T* constPtr(StreamOffs pos = 0) const;

    /**
     * @brief   Stream extraction operator.
     * @tparam  T       The type of data to extract.
     * @param   data   The reference to extract into.
     * @return  This instance.
     */
    template<typename T> IBinaryStream& operator >> (T& data);

    /**
     * @brief   Reads data from the stream rawly.
     * @param   pos     The position to read from.
     * @param   len     The length.
     * @param   buf     The buffer to read into.
     */
    void rawRead(StreamOffs pos, StreamSize len, uint8_t* buf) const;

    /// @overload
    template<typename T> T rawRead(StreamOffs pos) const;
};

// ============================================================================================== //
// [OBinaryStream]                                                                                //
// ============================================================================================== //

/**
 * @brief   Output stream for binary data.
 *          
 * In case a write operation would exceed the buffer's size, the size is automatically advanced to 
 * fit the new requirements. If an operation would grow the buffer beyond it's @c max_size, a 
 * @c OutOfBounds exception will be raised.
 */
class OBinaryStream : public virtual BaseBinaryStream
{
protected:
    StreamOffs  m_wpos = 0;
    StreamSize  m_blockSize;

    /**
     * @internal
     * @brief   Grows the buffer if the given pos + len does not fit into the buffer anymore.
     * @param   pos The position.
     * @param   len The length.
     */
    void growIfRequired(StreamOffs pos, StreamSize len) const;
public:
    /**
     * @copydoc BaseBinaryStream::BaseBinaryStream
     * @param   blockSize   Sets the block size for reallocation operations.
     */
    OBinaryStream(Buffer *buffer, StreamSize blockSize = 256);

    /**
     * @brief   Destructor.
     */
    virtual ~OBinaryStream() {}

    /**
     * @brief   Addition assignment operator.
     * @param   appendFrom  The buffer to append.
     * @return  This instance.
     * The given buffer is appended to the end of the managed buffer, not at the current write 
     * offset.
     */
    OBinaryStream& operator += (const Buffer& appendFrom);

    /**
     * @brief   Gets the write offset.
     * @return  The current write offset.
     */
    StreamOffs wpos() const;

    /**
     * @brief   Sets the write offset.
     * @param   pos The write offset.
     * @return  This instance.
     */
    OBinaryStream& wpos(StreamOffs pos);

    /**
     * @brief   Aligns the write offset to a given value.
     * @param   alignment   The alignment.
     * @return  This instance.
     */
    OBinaryStream& alignWpos(StreamSize alignment);

    /**
     * @brief   Appends a buffer to the managed buffer.
     * @copydetails operator+=
     */
    OBinaryStream& append(const Buffer& appendFrom);

    /**
     * @brief   Clears the managed buffer.
     * @return  This instance.
     */
    OBinaryStream& clear();

    /**
     * @brief   Clears a fragment of the managed buffer.
     * @param   pos The position to start clearing.
     * @param   len The length of the fragment to clear.
     * @return  This instance.
     */
    OBinaryStream& clear(StreamOffs pos, StreamSize len);

    /**
     * @brief   Fills the managed buffer with the given value.
     * @param   value   The value.
     * @return  This instance.
     */
    OBinaryStream& fill(uint8_t value);

    /**
     * @brief   Fills a fragment of the managed buffer with the given value.
     * @param   pos     The position to start filling.
     * @param   len     The length of the fragment to fill.
     * @param   value   The value.
     * @return  This instance.
     */
    OBinaryStream& fill(StreamOffs pos, StreamSize len, uint8_t value);

    /**
     * @brief   Retrieves a writable pointer to 
     * @tparam  T   The pointer type to retrieve.
     * @param   pos The position.
     * @return  The desired pointer.
     */
    template<typename T> T* ptr(StreamOffs pos = 0);

    /**
     * @brief   Stream insertion operator.
     * @param   data    The data to append at @c wpos.
     * @return  This instance.
     */
    template<typename T> OBinaryStream& operator << (const T& data);

    /**
     * @brief   Stream insertion operator.
     * @param   buffer  The buffer to append at @c wpos.
     * @return  This instance.
     */
    OBinaryStream& operator << (const Buffer &buffer);

    /**
     * @brief   Writes memory into the managed buffer rawly.
     * @param   pos     The position to write to.
     * @param   len     The length.
     * @param   src     The source of data.
     */
    void rawWrite(StreamOffs pos, StreamSize len, const uint8_t* src);

    /// @overload
    template<typename T> void rawWrite(StreamOffs pos, const T& data);
};

// ============================================================================================== //
// [BinaryStream]                                                                                 //
// ============================================================================================== //

/**
 * @brief Combined input and output stream for binary data.
 * @copydetails zycore::IBinaryStream
 * @copydetails zycore::OBinaryStream
 */
class BinaryStream : public IBinaryStream, public OBinaryStream 
{
public:
    /// @copydoc OBinaryStream::OBinaryStream
    explicit BinaryStream(Buffer* buffer, StreamSize blockSize = 256)
        : IBinaryStream(buffer)
        , OBinaryStream(buffer, blockSize)
        , BaseBinaryStream(buffer)
    {}
};

// ============================================================================================== //
// Implementation of inline and template functions [BaseBinaryStream]                             //
// ============================================================================================== //

inline BaseBinaryStream::BaseBinaryStream(Buffer* buffer)
    : m_buffer(buffer)
{
    assert(buffer);
}

// ============================================================================================== //
// Implementation of inline and template functions [IBinaryStream]                                //
// ============================================================================================== //

inline IBinaryStream::IBinaryStream(Buffer* buffer)
    : BaseBinaryStream(buffer)
{}

inline void IBinaryStream::validateOffset(StreamOffs offs, StreamSize len) const
{
    if (offs + len > m_buffer->size())
    {
        throw OutOfBounds("the requested offset is out of bounds");
    }
}

inline auto IBinaryStream::rpos() const -> StreamOffs
{
    return m_rpos;
}

inline IBinaryStream& IBinaryStream::rpos(StreamOffs pos)
{
    // TODO: validate rpos here?
    m_rpos = pos;
    return *this;
}

inline auto IBinaryStream::sub(StreamOffs pos, StreamSize len) const -> Buffer
{
    validateOffset(pos, len);
    return Buffer(m_buffer->begin() + pos, m_buffer->begin() + pos + len);
}

template<typename T> inline
const T* IBinaryStream::constPtr(StreamOffs pos) const
{
    validateOffset(pos, sizeof(T));
    return reinterpret_cast<T*>(m_buffer->data() + pos);
}

template<typename T> inline
IBinaryStream& IBinaryStream::operator >> (T& data)
{
    data = *constPtr<T>(m_rpos);
    m_rpos += sizeof(T);
    return *this;
}

inline void IBinaryStream::rawRead(StreamOffs pos, StreamSize len, uint8_t* buf) const
{
    validateOffset(pos, len);
    std::copy(m_buffer->begin() + pos, m_buffer->begin() + pos + len, buf);
}

template<typename T> inline
T IBinaryStream::rawRead(StreamOffs pos) const
{
    return *constPtr<T>(pos);
}

inline std::string IBinaryStream::hexDump() const
{
    return hexDump(0, m_buffer->size());
}

// ============================================================================================== //
// Implementation of inline and template functions [OBinaryStream]                                //
// ============================================================================================== //

inline OBinaryStream::OBinaryStream(Buffer* buffer, StreamSize blockSize)
    : BaseBinaryStream(buffer)
    , m_blockSize(blockSize)
{}

inline void OBinaryStream::growIfRequired(StreamOffs pos, StreamSize len) const
{
    StreamOffs end = pos + len;

    // Does it fit without any alteration? Fine.
    if (end <= m_buffer->size())
    {
        return;
    }

    // Does it fit into the capacity?
    if (end <= m_buffer->capacity())
    {
        m_buffer->resize(end);
        return;
    }

    // Nope, realloc. Does requested position exceed maximum allowed size?
    if (end > m_buffer->max_size())
    {
        throw OutOfBounds("tried to grow buffer beyond max_size");
    }

    // Grow buffer.
    m_buffer->reserve((end + m_blockSize - 1) / m_blockSize * m_blockSize);
    m_buffer->resize(end);
}

inline OBinaryStream& OBinaryStream::operator += (const Buffer& appendFrom)
{
    append(appendFrom);
    return *this;
}

inline auto OBinaryStream::wpos() const -> StreamOffs
{
    return m_wpos;
}

inline OBinaryStream& OBinaryStream::wpos(StreamOffs pos)
{
    m_wpos = pos;
    return *this;
}

inline OBinaryStream& OBinaryStream::alignWpos(StreamSize alignment)
{
    m_wpos = (m_wpos + alignment - 1) * alignment;
    return *this;
}

inline OBinaryStream& OBinaryStream::append(const Buffer& appendFrom)
{
    growIfRequired(m_wpos, appendFrom.size());
    std::copy(appendFrom.begin(), appendFrom.end(), 
        m_buffer->begin() + m_buffer->size());
    return *this;
}

inline OBinaryStream& OBinaryStream::clear(StreamOffs pos, StreamSize len)
{
    fill(pos, len, 0);
    return *this;
}

inline OBinaryStream& OBinaryStream::clear()
{
    return clear(0, m_buffer->size());
}

inline OBinaryStream& OBinaryStream::fill(StreamOffs pos, StreamSize len, uint8_t value)
{
    growIfRequired(pos, len);
    std::fill(m_buffer->begin() + pos, m_buffer->begin() + pos + len, value);
    return *this;
}

inline OBinaryStream& OBinaryStream::fill(uint8_t value)
{
    return fill(0, m_buffer->size(), value);
}

template<typename T> inline 
T* OBinaryStream::ptr(StreamOffs pos)
{
    growIfRequired(pos, sizeof(T));
    return reinterpret_cast<T*>(m_buffer->data() + pos);
}

template<typename T> inline 
OBinaryStream& OBinaryStream::operator << (const T &data)
{
    rawWrite(m_wpos, data);
    m_wpos += sizeof(T);
    return *this;
}

inline OBinaryStream& OBinaryStream::operator << (const Buffer &buffer)
{
    growIfRequired(m_wpos, buffer.size());
    std::copy(buffer.cbegin(), buffer.cend(), m_buffer->begin() + m_wpos);
    m_wpos += buffer.size();
    return *this;
}

inline void OBinaryStream::rawWrite(StreamOffs pos, StreamSize len, const uint8_t* src)
{
    growIfRequired(pos, len);
    std::copy(src, src + len, m_buffer->begin() + pos);
}

template<typename T> inline 
void OBinaryStream::rawWrite(StreamOffs pos, const T& data)
{
    *ptr<T>(pos) = data;
}

// ============================================================================================== //

} // namespace zycore

#endif // _ZYCORE_BINARYSTREAM_HPP_
