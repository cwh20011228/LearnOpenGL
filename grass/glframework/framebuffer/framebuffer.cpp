#include "framebuffer.hpp"

FrameBuffer::FrameBuffer(unsigned int width, unsigned int height)
{
	mWidth = width;
	mHeight = height;

	// 1. ����FBO�����Ұ�
	glGenFramebuffers(1, &mFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, mFBO);

	// 2. ������ɫ���������Ҽ���fbo
	mColorAttachment = Texture::createColorAttachment(mWidth,mHeight,0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mColorAttachment->getTexture(), 0);

	// 3. ����DepthStencil����������fbo
	mDepthStencilAttachment = Texture::createDepthStencilAttachment(mWidth, mHeight, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, mDepthStencilAttachment->getTexture(), 0);

	// ��鵱ǰ������fbo�Ƿ�����
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cerr << "ERROR: FrameBuffer is not complete!" << std::endl;
	}

	// ���ǰ��fbo(�����ǰ󶨵�Ĭ�ϵ�fbo��)
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