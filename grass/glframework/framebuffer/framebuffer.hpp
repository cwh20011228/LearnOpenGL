#include "../core.h"
#include "../texture.h"

class FrameBuffer
{
public:
	FrameBuffer(unsigned int width,unsigned int height);
	~FrameBuffer();
public:
	unsigned int mWidth{ 0 };
	unsigned int mHeight{ 0 };

	unsigned int mFBO{ 0 };
	Texture* mColorAttachment{ nullptr };
	Texture* mDepthStencilAttachment{ nullptr };
};