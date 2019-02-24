//=============================================================================
// Copyright (C) 2011-2019 The pmp-library developers
//
// This file is part of the Polygon Mesh Processing Library.
// Distributed under the terms of the MIT license, see LICENSE.txt for details.
//
// SPDX-License-Identifier: MIT
//=============================================================================
#pragma once
//=============================================================================

#include <pmp/visualization/GL.h>
#include <pmp/MatVec.h>

//=============================================================================

namespace pmp {

//=============================================================================

//! \addtogroup visualization visualization
//! @{

//=============================================================================

//! shader class for easy handling of the shader
class Shader
{
public:
    //! default constructor
    Shader();

    //! default destructor
    ~Shader();

    //! is shader valid (ID != 0)
    bool is_valid() const { return pid_ != 0; }

    //! get source from strings, compile, and link vertex and fragment shader,
    //! \param vshader string with the adress to the vertex shader
    //! \param fshader string with the adress to the fragment shader
    bool source(const char* vshader, const char* fshader);

    //! load (from file), compile, and link vertex and fragment shader,
    //! \param vfile string with the adress to the vertex shader
    //! \param ffile string with the adress to the fragment shader
    bool load(const char* vfile, const char* ffile);

    //! enable/bind this shader program
    void use();

    //! disable/unbind this shader program
    void disable();

    //! bind attribute to location
    void bind_attribute(const char* name, GLuint index);

    //! upload float uniform
    //! \param name string of the uniform name
    //! \param value the value for the uniform
    void set_uniform(const char* name, float value);

    //! upload int uniform
    //! \param name string of the uniform name
    //! \param value the value for the uniform
    void set_uniform(const char* name, int value);

    //! upload vec3 uniform
    //! \param name string of the uniform name
    //! \param vec the value for the uniform
    void set_uniform(const char* name, const vec3& vec);

    //! upload vec4 uniform
    //! \param name string of the uniform name
    //! \param vec the value for the uniform
    void set_uniform(const char* name, const vec4& vec);

    //! upload mat3 uniform
    //! \param name string of the uniform name
    //! \param mat the value for the uniform

    void set_uniform(const char* name, const mat3& mat);
    //! upload mat4 uniform
    //! \param name string of the uniform name
    //! \param mat the value for the uniform
    void set_uniform(const char* name, const mat4& mat);

private:
    //! deletes all shader and frees GPU shader capacities
    void cleanup();

    //! load shader from file, return as string
    bool load(const char* filename, std::string& source);

    //! compile a vertex/fragmend shader
    //! \param shader source
    //! \param type the type of the shader (vertex, fragment)
    GLint compile(const char* source, GLenum type);

    //! loads a vertex/fragmend shader from a file and compiles it
    //! \param filename the location and name of the shader
    //! \param type the type of the shader (vertex, geometry, fragment)
    GLint load_and_compile(const char* filename, GLenum type);

    //! relink: use this after setting/changing attrib location
    bool link();

private:
    //! id of the linked shader program
    GLint pid_;

    //! id of the vertex shader
    GLint vid_;

    //! id of the fragmend shader
    GLint fid_;
};

//=============================================================================
//! @}
//=============================================================================
} // namespace pmp
//=============================================================================