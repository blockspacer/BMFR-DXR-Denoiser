#pragma once
#include "../SharedUtils/RenderPass.h"
#include "../SharedUtils/SimpleVars.h"
#include "../SharedUtils/FullscreenLaunch.h"
#include "../SharedUtils/FullscreenLaunch.h"


class BlockwiseMultiOrderFeatureRegression : public ::RenderPass, inherit_shared_from_this<::RenderPass, BlockwiseMultiOrderFeatureRegression>
{
public:
    using SharedPtr = std::shared_ptr<BlockwiseMultiOrderFeatureRegression>;
    using SharedConstPtr = std::shared_ptr<const BlockwiseMultiOrderFeatureRegression>;

    static SharedPtr create(const std::string &bufferToAccumulate = ResourceManager::kOutputChannel) { return SharedPtr(new BlockwiseMultiOrderFeatureRegression(bufferToAccumulate)); }
    virtual ~BlockwiseMultiOrderFeatureRegression() = default;

protected:
    BlockwiseMultiOrderFeatureRegression(const std::string &bufferToAccumulate);

    // Implementation of SimpleRenderPass interface
    bool initialize(RenderContext* pRenderContext, ResourceManager::SharedPtr pResManager) override;
    void initScene(RenderContext* pRenderContext, Scene::SharedPtr pScene) override;
    void execute(RenderContext* pRenderContext) override;
    void renderGui(Gui* pGui) override;
    void resize(uint32_t width, uint32_t height) override;

    // Override some functions that provide information to the RenderPipeline class
    bool appliesPostprocess() override { return true; }
    bool hasAnimation() override { return false; }

    // Information about the rendering texture we will denoise
    std::string                   mDenoiseChannel;

    // State for our accumulation shader
    //FullscreenLaunch::SharedPtr   mpDenoiseShader;
    GraphicsState::SharedPtr      mpGfxState;
    Fbo::SharedPtr                mpInternalFbo;

    // We stash a copy of our current scene.
    Scene::SharedPtr              mpScene;

	// BMFR passes
	FullscreenLaunch::SharedPtr         mpReprojection;
	ComputeProgram::SharedPtr			mpRegression;
	ComputeState::SharedPtr				mpCPState;
	ComputeVars::SharedPtr				mpRegressionVars;
	FullscreenLaunch::SharedPtr         mpAtrous;
	FullscreenLaunch::SharedPtr         mpModulate;
	FullscreenLaunch::SharedPtr         mpFilterMoments;
	FullscreenLaunch::SharedPtr         mpCombineUnfiltered;

	// Intermediate framebuffers
	Fbo::SharedPtr            mpPingPongFbo[2];
	Fbo::SharedPtr            mpFilteredPastFbo;
	Fbo::SharedPtr            mpCurReprojFbo;
	Fbo::SharedPtr            mpPrevReprojFbo;
	Fbo::SharedPtr            mpOutputFbo;

	// Textures expected by BMFR code
	struct {
		Texture::SharedPtr    curPos;
		Texture::SharedPtr    curNorm;
		Texture::SharedPtr    curNoisy;
		Texture::SharedPtr    curSpp;

		Texture::SharedPtr    prevPos;
		Texture::SharedPtr    prevNorm;
		Texture::SharedPtr    prevNoisy;

		Texture::SharedPtr    prevSpp;

		Texture::SharedPtr    tmp_data;
		Texture::SharedPtr    out_data;

		Texture::SharedPtr    accept_bools;
	} mInputTex;


    //determine whether we want to show denoise result or not
    bool                          mDoDenoise = true;


private:
	bool mNeedFboClear;

	void clearFbos(RenderContext* pCtx);
	void accumulate_noisy_data(RenderContext* pRenderContext);
	void fit_noisy_color(RenderContext* pRenderContext);
	// How many frames have we accumulated so far?
	uint32_t mAccumCount = 0;
	int	cbData[4];
};