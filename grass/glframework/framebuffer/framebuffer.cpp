#include "framebuffer.hpp"

FrameBuffer::FrameBuffer(unsigned int width, unsigned int height)
{
	mWidth = width;
	mHeight = height;

	// 1. 生成FBO对象并且绑定
	glGenFramebuffers(1, &mFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, mFBO);

	// 2. 生成颜色附件，并且加入fbo
	mColorAttachment = Texture::createColorAttachment(mWidth,mHeight,0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mColorAttachment->getTexture(), 0);

	// 3. 生成DepthStencil附件，加入fbo
	mDepthStencilAttachment = Texture::createDepthStencilAttachment(mWidth, mHeight, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, mDepthStencilAttachment->getTexture(), 0);

	// 检查当前构建的fbo是否完整
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cerr << "ERROR: FrameBuffer is not complete!" << std::endl;
	}

	// 解绑当前的fbo(这里是绑定到默认的fbo中)
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

FrameBuffer::~FrameBuffer()
{
	if (mFBO)
	{
		glDeleteFramebuffers(1, &mFBO);
	}
	if (mColorAttachment != nullptr)
	{
		delete mColorAttachment;
	}
	if (mDepthStencilAttachment != nullptr)
	{
		delete mDepthStencilAttachment;
	}
}