
#include <glbinding/functions.h>

#include <glow/Program.h>
#include <glow/VertexArrayObject.h>
#include <glow/FrameBufferObject.h>
#include <glow/Texture.h>

#include <glowutils/Camera.h>
#include <glowbase/File.h>
#include <glowutils/ScreenAlignedQuad.h>
#include <glowutils/glowutils.h>

#include "FragmentShaderParticles.h"


using namespace glow;
using namespace glowutils;
using namespace glm;


FragmentShaderParticles::FragmentShaderParticles(
    const std::vector<vec4> & positions
,   const std::vector<vec4> & velocities
,   const Texture & forces
,   const Camera & camera)
: AbstractParticleTechnique(positions, velocities, forces, camera)
, m_width(10)
, m_height(10)
{
}

FragmentShaderParticles::~FragmentShaderParticles()
{
}

void FragmentShaderParticles::initialize()
{
    // Create textures to store the particle data
    m_texPositions = new glow::Texture(gl::TEXTURE_2D);
    m_texPositions->setParameter(gl::TEXTURE_MIN_FILTER, static_cast<gl::GLint>(gl::NEAREST));
    m_texPositions->setParameter(gl::TEXTURE_MAG_FILTER, static_cast<gl::GLint>(gl::NEAREST));
    m_texPositions->setParameter(gl::TEXTURE_WRAP_S, static_cast<gl::GLint>(gl::CLAMP_TO_EDGE));
    m_texPositions->setParameter(gl::TEXTURE_WRAP_T, static_cast<gl::GLint>(gl::CLAMP_TO_EDGE));
    m_texPositions->setParameter(gl::TEXTURE_WRAP_R, static_cast<gl::GLint>(gl::CLAMP_TO_EDGE));

    m_texVelocities = new glow::Texture(gl::TEXTURE_2D);
    m_texVelocities->setParameter(gl::TEXTURE_MIN_FILTER, static_cast<gl::GLint>(gl::NEAREST));
    m_texVelocities->setParameter(gl::TEXTURE_MAG_FILTER, static_cast<gl::GLint>(gl::NEAREST));
    m_texVelocities->setParameter(gl::TEXTURE_WRAP_S, static_cast<gl::GLint>(gl::CLAMP_TO_EDGE));
    m_texVelocities->setParameter(gl::TEXTURE_WRAP_T, static_cast<gl::GLint>(gl::CLAMP_TO_EDGE));
    m_texVelocities->setParameter(gl::TEXTURE_WRAP_R, static_cast<gl::GLint>(gl::CLAMP_TO_EDGE));

    // Fill buffers with data
    reset();

    // Create empty vertex array object
    m_vao = new VertexArrayObject();

    // Create frame buffer object for update
    m_fboUpdate = new FrameBufferObject();
    m_fboUpdate->attachTexture2D(gl::COLOR_ATTACHMENT0, m_texPositions);
    m_fboUpdate->attachTexture2D(gl::COLOR_ATTACHMENT1, m_texVelocities);
    m_fboUpdate->setDrawBuffers({gl::COLOR_ATTACHMENT0, gl::COLOR_ATTACHMENT1});
    m_fboUpdate->unbind();

    // Create screen aligned quad for particle update
    m_quadUpdate = new ScreenAlignedQuad(
        Shader::fromFile(gl::FRAGMENT_SHADER, "data/gpu-particles/particle.frag"),
        m_texPositions );
    m_quadUpdate->program()->setUniform("vertices",   0);
    m_quadUpdate->program()->setUniform("velocities", 1);
    m_quadUpdate->program()->setUniform("forces",     2);

    // Create frame buffer object for rendering
    m_fbo = new FrameBufferObject();

    m_colorBuffer = new Texture(gl::TEXTURE_2D);
    m_colorBuffer->setParameter(gl::TEXTURE_MIN_FILTER, static_cast<gl::GLint>(gl::NEAREST));
    m_colorBuffer->setParameter(gl::TEXTURE_MAG_FILTER, static_cast<gl::GLint>(gl::NEAREST));
    m_colorBuffer->setParameter(gl::TEXTURE_WRAP_S, static_cast<gl::GLint>(gl::CLAMP_TO_EDGE));
    m_colorBuffer->setParameter(gl::TEXTURE_WRAP_T, static_cast<gl::GLint>(gl::CLAMP_TO_EDGE));
    m_colorBuffer->setParameter(gl::TEXTURE_WRAP_R, static_cast<gl::GLint>(gl::CLAMP_TO_EDGE));

    m_fbo->attachTexture2D(gl::COLOR_ATTACHMENT0, m_colorBuffer);
    m_fbo->setDrawBuffers({ gl::COLOR_ATTACHMENT0 });
    m_fbo->unbind();

    // Create screen aligned quads for clear and rendering
    m_clear = new ScreenAlignedQuad(
        Shader::fromFile(gl::FRAGMENT_SHADER, "data/gpu-particles/clear.frag") );
    m_quad  = new ScreenAlignedQuad(m_colorBuffer);

    // Create draw program
    m_drawProgram = new Program();
    m_drawProgram->attach(
        Shader::fromFile(gl::VERTEX_SHADER,   "data/gpu-particles/points_fragment.vert")
    ,   Shader::fromFile(gl::GEOMETRY_SHADER, "data/gpu-particles/points.geom")
    ,   Shader::fromFile(gl::FRAGMENT_SHADER, "data/gpu-particles/points.frag"));
}

void FragmentShaderParticles::reset()
{
    // Choose appropriate width and height for the current number of particles
    int size = static_cast<int>(m_positions.size());
    m_width  = static_cast<int>(std::sqrt(static_cast<float>(size)));
    m_height = m_width;
    int remain = size - (m_height * m_width);
    m_height += remain / m_width + (remain % m_width == 0 ? 0 : 1);

    // Read positions and velocities into textures
    m_texPositions ->image2D(0, gl::RGBA32F, m_width, m_height, 0, gl::RGBA, gl::FLOAT, m_positions .data());
    m_texVelocities->image2D(0, gl::RGBA32F, m_width, m_height, 0, gl::RGBA, gl::FLOAT, m_velocities.data());
}

void FragmentShaderParticles::step(const float elapsed)
{
    // Simulate particles via fragment shader
    // Use positions and velocities textures for both input and output at the same time

    m_fboUpdate->bind();
    m_texPositions->bindActive(gl::TEXTURE0);
    m_texVelocities->bindActive(gl::TEXTURE1);
    m_forces.bindActive(gl::TEXTURE2);
    m_quadUpdate->program()->setUniform("elapsed", elapsed);

    gl::Viewport(0, 0, m_width, m_height);
    m_quadUpdate->draw();
    gl::Viewport(0, 0, m_camera.viewport().x, m_camera.viewport().y);

    m_fboUpdate->unbind();
}

void FragmentShaderParticles::draw(const float elapsed)
{
    // Disable depth buffer
    gl::Disable(gl::DEPTH_TEST);

    // Activate FBO
    m_fbo->bind();

    // Clear color buffer
    gl::Enable(gl::BLEND);
    gl::BlendFunc(gl::ZERO, gl::ONE_MINUS_SRC_COLOR);
    m_clear->program()->setUniform("elapsed", elapsed);
    m_clear->draw();

    // Draw particles
    gl::BlendFunc(gl::SRC_ALPHA, gl::ONE);

    m_drawProgram->setUniform("viewProjection", m_camera.viewProjection());
    m_texPositions->bindActive(gl::TEXTURE0);
    m_drawProgram->setUniform("vertices", 0);
    m_texVelocities->bindActive(gl::TEXTURE1);
    m_drawProgram->setUniform("velocities", 1);
    m_drawProgram->setUniform("texWidth", m_width);
    m_drawProgram->use();

    m_vao->bind();
    m_vao->drawArrays(gl::POINTS, 0, m_numParticles);
    m_vao->unbind();

    m_texPositions->unbind();

    m_drawProgram->release();

    gl::Disable(gl::BLEND);

    m_fbo->unbind();

    m_quad->draw();

    // Re-enable depth buffer
    gl::Enable(gl::DEPTH_TEST);
}

void FragmentShaderParticles::resize()
{
    // Update size of color buffer and ascept ratio

    m_drawProgram->setUniform("aspect", m_camera.aspectRatio());

    m_colorBuffer->image2D(0, gl::RGB16F, m_camera.viewport().x, m_camera.viewport().y, 0, gl::RGB, gl::FLOAT, nullptr);

    m_fbo->bind();
    gl::Clear(gl::COLOR_BUFFER_BIT);
    m_fbo->unbind();
}
