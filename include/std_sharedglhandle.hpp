#pragma once

#include <memory>
#include <functional>
#include <cstddef> // f�r std::size_t

// F�r OpenGL-Typen und Funktionen
#include <GL/gl.h> // oder <GL/glew.h> oder <GL/glcorearb.h> je nach System/Projekt

// =================================================================================================

template <typename HANDLE_T>
class SharedHandle {
protected:
    struct HandleInfo {
        HANDLE_T handle;

        HandleInfo(HANDLE_T h)
            : handle(h) 
        { }
    };

    std::shared_ptr<HandleInfo> m_info;
    std::function<void(HANDLE_T)> m_releaser;
    std::function<HANDLE_T()> m_allocator;

public:
    SharedHandle() = default;

    SharedHandle(HANDLE_T handle, std::function<HANDLE_T()> allocator, std::function<void(HANDLE_T)> releaser)
        : m_info(std::make_shared<HandleInfo>(handle)), m_allocator(std::move(allocator)), m_releaser(std::move(releaser)) 
    { }

    // Copy/Move-Constructor und Assignment
    SharedHandle(const SharedHandle&) = default;

    SharedHandle(SharedHandle&&) noexcept = default;

    SharedHandle& operator=(const SharedHandle&) = default;

    SharedHandle& operator=(SharedHandle&&) noexcept = default;

    // Gleichheitsoperator
    inline bool operator==(const SharedHandle& other) const {
        if (!m_info and !other.m_info)
            return true;
        if (!m_info or !other.m_info)
            return false;
        return m_info->handle == other.m_info->handle;
    }

    inline bool operator!=(const SharedHandle& other) const {
        return not operator==(other);
    }

    bool operator==(const HANDLE_T handle) const {
        return m_info ? m_info->handle == handle : false;
    }

    inline HANDLE_T get() const { return m_info ? m_info->handle : HANDLE_T{}; }

    inline operator HANDLE_T() const { return get(); }

    inline bool IsAvailable(void) const { return get() != HANDLE_T{}; }

    inline operator bool() const { return this->IsAvailable(); }

    inline bool operator!() const { return not this->IsAvailable(); }

    inline void Release() { m_info.reset(); }

    // Claim: gibt alten frei, legt neuen an (wenn allocator gesetzt)
    HANDLE_T Claim() {
        Release();
        HANDLE_T newHandle = m_allocator();
        if (newHandle)
            m_info = std::make_shared<HandleInfo>(newHandle);
        return newHandle;
    }

    inline std::size_t RefCount() const { return m_info ? m_info.use_count() : 0; }
};

// =================================================================================================

using glBufferAllocator = void (*)(GLsizei, GLuint*);
using glBufferReleaser = void (*)(GLsizei, const GLuint*);

class SharedGLHandle : public SharedHandle<GLuint> {
public:
    SharedGLHandle() = default;

    SharedGLHandle(GLuint handle, glBufferAllocator allocator, glBufferReleaser releaser)
        : SharedHandle(
            handle, 
            [allocator]() { GLuint h; if (allocator == nullptr) h = 0; else allocator(1, &h); return h; },
            [releaser](GLuint h) { if (h and (releaser != nullptr)) releaser(1, &h); } 
          )
    { }
};

class SharedTextureHandle : public SharedGLHandle {
public:
    SharedTextureHandle(GLuint handle = 0)
        : SharedGLHandle(handle, glGenTextures, glDeleteTextures)
    { }
};

class SharedBufferHandle : public SharedGLHandle {
public:
    SharedBufferHandle(GLuint handle = 0)
        : SharedGLHandle(handle, glGenBuffers, glDeleteBuffers)
    { }
};

class SharedFramebufferHandle : public SharedGLHandle {
public:
    SharedFramebufferHandle(GLuint handle = 0)
        : SharedGLHandle(handle, glGenFramebuffers, glDeleteFramebuffers)
    { }
};

// =================================================================================================
