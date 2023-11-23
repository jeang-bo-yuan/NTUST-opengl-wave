
#include "UBO.h"

UBO::UBO(GLsizeiptr size, GLenum usage)
    : m_UBO_id(0), m_usage(usage), m_size(size)
{
    glGenBuffers(1, &m_UBO_id);
    this->resize(size, usage);
}

void UBO::resize(GLsizeiptr size, GLenum usage)
{
    glNamedBufferData(m_UBO_id, size, nullptr, usage);
}

void UBO::BufferData(void *data)
{
    glNamedBufferData(m_UBO_id, m_size, data, m_usage);
}

void UBO::BufferSubData(GLintptr offset, GLsizeiptr size, const void *data)
{
    glNamedBufferSubData(m_UBO_id, offset, size, data);
}

void UBO::bind_to(GLuint binding)
{
    glBindBufferBase(GL_UNIFORM_BUFFER, binding, m_UBO_id);
}

