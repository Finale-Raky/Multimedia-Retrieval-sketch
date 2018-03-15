//=============================================================================
// Copyright (C) 2011-2017 The pmp-library developers
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the name of the copyright holder nor the names of its
//   contributors may be used to endorse or promote products derived from
//   this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//=============================================================================
#pragma once
//=============================================================================

#include <pmp/gl/gl.h>
#include <pmp/gl/Shader.h>
#include <pmp/MatVec.h>
#include <pmp/PointSet.h>

//=============================================================================

namespace pmp {

//=============================================================================

//! \addtogroup gl gl
//! @{

//=============================================================================

//! Class for rendering a point set using OpenGL
class PointSetGL : public PointSet
{
public:
    //! Constructor
    PointSetGL();

    //! default destructor
    ~PointSetGL();

    //! draw the points
    void draw(const mat4& projectionMatrix, const mat4& modelviewMatrix,
              const std::string drawMode);

    //! update all opengl buffers for efficient core profile rendering
    void updateOpenGLBuffers();

private:
    //! OpenGL buffers
    GLuint m_vertexArrayObject;
    GLuint m_vertexBuffer;
    GLuint m_normalBuffer;
    GLuint m_colorBuffer;

    //! buffer sizes
    GLsizei m_nVertices;

    //! shaders
    Shader m_phongShader;

    //! material properties
    vec3  m_frontColor, m_backColor;
    float m_ambient, m_diffuse, m_specular, m_shininess;
};

//=============================================================================
//! @}
//=============================================================================
} // namespace
//=============================================================================
