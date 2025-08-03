#pragma once

#include <vector>
#include <cstdint>
#include <cassert>
#include <stdexcept>

// =================================================================================================

template<typename DATA_T>
class ManagedArray {
private:
    std::vector<DATA_T> m_array;
    int32_t m_width = 0;
    int32_t m_height = 0;

public:
    // Konstruktor f�r 1D-ManagedArray
    inline ManagedArray(int32_t size = 0)
        : m_array(static_cast<size_t>(size)) {
    }

    inline ManagedArray(std::initializer_list<DATA_T> data)
        : m_array(data) 
    { }

    // Konstruktor f�r 2D-ManagedArray
    inline ManagedArray(int32_t width, int32_t height)
        : m_array(static_cast<size_t>(width)* static_cast<size_t>(height)),
        m_width(width), m_height(height) {
        assert(width * height > 0 && "Width and height must be > 0");
#if defined(_DEBUG)
        if (width * height <= 0)
            throw std::invalid_argument("ManagedArray: invalid width or height arguments (must both be > 0)");
#endif    
    }

    ManagedArray(const ManagedArray& other)
        : m_array(other.m_array) {
    }

    // Move-Konstruktor
    ManagedArray(ManagedArray&& other) noexcept
        : m_array(std::move(other.m_array)) 
    { }

    // Copy-Zuweisungsoperator
    ManagedArray& operator=(const ManagedArray& other) {
        if (this != &other)
            m_array = other.m_array;
        return *this;
    }

    // Move-Zuweisungsoperator
    ManagedArray& operator=(ManagedArray&& other) noexcept {
        if (this != &other)
            m_array = std::move(other.m_array);
        return *this;
    }

    ManagedArray& operator=(std::initializer_list<DATA_T> data) {
        m_array = data;
        return *this;
    }

    inline int32_t Capacity(void) const { return static_cast<int32_t>(m_array.capacity()); }

    // Zugriff auf L�nge
    inline int32_t Length(void) const { return static_cast<int32_t>(m_array.size()); }

    inline bool IsEmpty(void) const { return Length() == 0; }

    // Gesamte Datenmenge in Bytes
    inline int32_t DataSize() const { return Length() * static_cast<int32_t>(sizeof(DATA_T)); }

    // 1D-Indexzugriff
    inline DATA_T& operator[](int32_t i) {
#if defined(_DEBUG)
        return m_array.at(static_cast<size_t>(i));
#else
        return m_array[static_cast<size_t>(i)];
#endif
    }
    inline const DATA_T& operator[](int32_t i) const {
#if defined(_DEBUG)
        return m_array.at(static_cast<size_t>(i));
#else
        return m_array[static_cast<size_t>(i)];
#endif
    }

    // 2D-Zugriff (x, y)
    inline DATA_T& operator()(int32_t x, int32_t y) {
        assert(m_width > 0 && m_height > 0);
#if defined(_DEBUG)
        return m_array.at(static_cast<size_t>(y) * static_cast<size_t>(m_width) + static_cast<size_t>(x));
#else
        return m_array[static_cast<size_t>(y) * static_cast<size_t>(m_width) + static_cast<size_t>(x)];
#endif
    }

    inline bool IsValidIndex(int32_t i) { return (i >= 0) && (i < Length()); }

    inline bool IsValidIndex(int32_t x, int32_t y) { return (x >= 0) and (x < m_width) and (y >= 0) and (y < m_height); }

    inline int GetCheckedIndex(int32_t x, int32_t y) { return IsValidIndex(x, y) ?int(y * m_width + x) : -1; }

    inline DATA_T* operator()(int32_t x, int32_t y, bool rangeCheck) { // always checks range; parameter only to distinguish from other operator()
        int i = GetCheckedIndex(x, y);
        return (i < 0) ? nullptr : Data(i);
    }

    inline const DATA_T& operator()(int32_t x, int32_t y) const {
        assert(m_width > 0 && m_height > 0);
#if defined(_DEBUG)
        return m_array.at(static_cast<size_t>(y) * static_cast<size_t>(m_width) + static_cast<size_t>(x));
#else
        return m_array[static_cast<size_t>(y) * static_cast<size_t>(m_width) + static_cast<size_t>(x)];
#endif
    }

    void Append(const DATA_T& data) { m_array.push_back(data); }

    void Fill(DATA_T value) {
        std::fill(m_array.begin(), m_array.end(), value);
    }

    DATA_T* Append(void) { 
        m_array.emplace_back(); 
        return &m_array.back();
    }

    void Push(DATA_T data) { m_array.push_back(data); }

    DATA_T Pop(void) {
        if (m_array.empty())
            return DATA_T();
        DATA_T data = m_array.back();
        m_array.pop_back();
        return data;
    }

    template<typename... Args>
    DATA_T* Append(Args&&... args) {
        m_array.emplace_back(std::forward<Args>(args)...);
        return &m_array.back();
    }

    // Zeiger auf Rohdaten (z.B. f�r OpenGL)
    inline DATA_T* Data(int32_t i = 0) { return m_array.data() + i; }

    inline const DATA_T* Data(int32_t i = 0) const { return m_array.data() + i; }

    DATA_T* DataRow(int32_t y) {
#if defined(_DEBUG)
        if (m_width * m_height <= 0)
            throw std::invalid_argument("ManagedArray: invalid width or height arguments (must both be > 0)");
#endif    
        return Data(y * m_width);
    }

    inline void Reserve(int32_t capacity) {
        m_array.reserve(static_cast<size_t>(capacity));
    }

    // Resize-Methoden
    inline DATA_T* Resize(int32_t newSize) {
        m_array.resize(static_cast<size_t>(newSize));
        return Data();
    }

    inline DATA_T* Resize(int32_t width, int32_t height) {
        m_array.resize(static_cast<size_t>(width) * static_cast<size_t>(height));
        m_width = width; 
        m_height = height; 
        return Data();
    }

    inline void Reset(void) {
        m_array.clear();
    }

    inline void Destroy(void) {
        m_array.clear();
        m_array.shrink_to_fit();
    }

    inline auto begin() { return m_array.begin(); }

    inline auto end() { return m_array.end(); }

    inline auto begin() const { return m_array.begin(); }

    inline auto end() const { return m_array.end(); }

    inline auto rbegin() { return m_array.rbegin(); }

    inline auto rend() { return m_array.rend(); }

    inline auto rbegin() const { return m_array.rbegin(); }

    inline auto rend() const { return m_array.rend(); }

    // Typecast-Operator zu std::vector<DATA_T>
    inline operator std::vector<DATA_T>& () { return m_array; }

    inline operator const std::vector<DATA_T>& () const { return m_array; }

    template <typename Predicate>
    auto Find(Predicate compare) {
        return std::find_if(m_array.begin(), m_array.end(), compare);
    }


    template<typename KEY_T, typename COMPARE_T>
    int FindLinear(const KEY_T& key, COMPARE_T compare) const {
        int i = 0;
        for (const auto& data : m_array) {
            if (not compare(data, key))
                return i;
            ++i;
        }
        return -1;
    }


    template<typename KEY_T, typename COMPARE_T>
    int FindBinary(const KEY_T& key, COMPARE_T compare) const {
        auto it = std::lower_bound(
            m_array.begin(), m_array.end(), key,
            [&](const DATA_T& data, const KEY_T& key) {
                return compare(data, key) < 0; // a < b
            }
        );
        if (it != m_array.end() && compare(*it, key) == 0)
            return static_cast<int>(std::distance(m_array.begin(), it));
        else
            return -1;
    }
};

// =================================================================================================

class ByteArray : public ManagedArray<uint8_t> {
public:
    ByteArray(const int32_t nLength) {
        Resize(nLength);
    }
};

class ShortArray : public ManagedArray<int16_t> {
public:
    ShortArray(const int32_t nLength) {
        Resize(nLength);
    }
};

class UShortArray : public ManagedArray<uint16_t> {
public:
    UShortArray(const int32_t nLength) {
        Resize(nLength);
    }
};

class IntArray : public ManagedArray<int32_t> {
public:
    IntArray(const int32_t nLength) {
        Resize(nLength);
    }
};

class UIntArray : public ManagedArray<int32_t> {
public:
    UIntArray(const int32_t nLength) {
        Resize(nLength);
    }
};

class SizeArray : public ManagedArray<size_t> {
public:
    SizeArray(const int32_t nLength) {
        Resize(nLength);
    }
};

class FloatArray : public ManagedArray<float> {
public:
    FloatArray(const int32_t nLength) {
        Resize(nLength);
    }
};

// =================================================================================================
