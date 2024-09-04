#include "Containers.h"

#include <vector>
#include "GL/glew.h"
#include "GL/gl.h"

using namespace utils;

FBOContainer::FBOContainer() = default;

FBOContainer::~FBOContainer()
{
    if (isInitialized)
        DeleteFrameBuffer();
}

void FBOContainer::Init(glm::ivec2 resolution)
{
    // Mark as initialized
    isInitialized = true;
    
    // Set the resolution
    this->currentResolution = resolution;
    
    CreateFrameBuffer(resolution);
}

void FBOContainer::Bind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
}

glm::ivec2 FBOContainer::GetResolution() const
{
    return currentResolution;
}

void FBOContainer::SetResolution(glm::ivec2 resolution)
{
    if (!isInitialized)
    {
        Init(resolution);
        return;
    }
    
    // Check if the resolution is the same
    if (this->currentResolution == resolution)
        return;
    
    // Set the resolution
    this->currentResolution = resolution;
    
    DeleteFrameBuffer();
    CreateFrameBuffer(resolution);
}

unsigned int FBOContainer::GetColorTextureID() const
{
    return colorTexture;
}

void FBOContainer::CreateFrameBuffer(glm::ivec2 resolution)
{
    // Generate and bind the framebuffer
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    // Generate, bind and initialize the color texture
    glGenTextures(1, &colorTexture);
    glBindTexture(GL_TEXTURE_2D, colorTexture);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, resolution.x, resolution.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    // Bind the color texture to the framebuffer as a color attachment at position 0
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, colorTexture, 0);

    // Generate, bind and initialize the depth texture
    glGenTextures(1, &depthTexture);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, resolution.x, resolution.y, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    // Bind the depth texture to the framebuffer as a depth attachment
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture, 0);

    // Set the color texture as the draw texture
    std::vector<GLenum> draw_textures;
    draw_textures.push_back(GL_COLOR_ATTACHMENT0);
    glDrawBuffers(draw_textures.size(), &draw_textures[0]);

    // Check the status of the framebuffer
    glCheckFramebufferStatus(GL_FRAMEBUFFER);

    // Bind the default framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FBOContainer::DeleteFrameBuffer()
{
    // Safety check
    if (fbo == 0)
        return;
    
    // Delete the old FBO and textures
    glDeleteFramebuffers(1, &fbo);
    glDeleteTextures(1, &colorTexture);
    glDeleteTextures(1, &depthTexture);
}
