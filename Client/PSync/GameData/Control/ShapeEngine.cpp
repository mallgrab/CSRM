#include "ShapeEngine.h"

namespace ShapeEngine
{
	void* rendFontPtr = nullptr;

	clearBuffers_t clearBuffers;
	createGlobalResources_t createGlobalResources;
	deleteGlobalResources_t deleteGlobalResources;
	drawCircle_t drawCircle;
	drawRectangle_t drawRectangle;
	drawSpriteWithFilmGrain_t drawSpriteWithFilmGrain;
	drawUser_t drawUser;
	empty_t empty;
	freeRenderTarget_t freeRenderTarget;
	getAspectRatio_t getAspectRatio;
	getBlendMode_t getBlendMode;
	getColor_t getColor;
	getDepthMode_t getDepthMode;
	getDrawMode_t getDrawMode;
	getDropShadow_t getDropShadow;
	getFilterMode_t getFilterMode;
	getFont_t getFont;
	getInstance_t getInstance;
	getTextAreaLineHeight_t getTextAreaLineHeight;
	getTexture_t getTexture;
	getTextureMode_t getTextureMode;
	hasInstance_t hasInstance;
	isRendererAdministered_t isRendererAdministered;
	postEffectInit_t postEffectInit;
	pushToRenderer_t pushToRenderer;
	registerGlobalInstance_t registerGlobalInstance;
	render_t render;
	setAspectRatio_t setAspectRatio;
	setBlendMode_t setBlendMode;
	setCubeSamplingMode_t setCubeSamplingMode;
	setDepthMode_t setDepthMode;
	setDrawMode_t setDrawMode;
	setDropShadow_t setDropShadow;
	setFilterMode_t setFilterMode;
	setFont_t setFontFunc;
	setPointSize_t setPointSize;
	setRendererAdministered_t setRendererAdministered;
	setTextSize_t setTextSize;
	setTexture_t setTexture;
	setTextureMode_t setTextureMode;
	setTransform_t setTransform;
	setViewportTransform_t setViewportTransform;
	unregisterGlobalInstance_t unregisterGlobalInstance;
	ShapeEngine_t ShapeEngineFunc;
	ShapeEngine1_t ShapeEngine1Func;
	drawInstancedLines_t drawInstancedLinesFunc;
	drawInstancedLines1_t drawInstancedLines1Func;
	drawInstancedTriangles_t drawInstancedTrianglesFunc;
	drawInstancedTriangles1_t drawInstancedTriangles1Func;
	drawLine_t drawLineFunc;
	drawLine1_t drawLine1Func;
	drawLine2_t drawLine2Func;
	drawLines_t drawLinesFunc;
	drawLines1_t drawLines1Func;
	drawPoint_t drawPointFunc;
	drawPoint1_t drawPoint1Func;
	drawPolygon_t drawPolygonFunc;
	drawPolygon1_t drawPolygon1Func;
	drawPolygon2_t drawPolygon2Func;
	drawSprite_t drawSpriteFunc;
	drawSprite1_t drawSprite1Func;
	drawText_t drawTextFunc;
	drawText1_t drawText1Func;
	drawTriangles_t drawTrianglesFunc;
	drawTriangles1_t drawTriangles1Func;
	getTextLineHeight_t getTextLineHeightFunc;
	getTextLineHeight1_t getTextLineHeight1Func;
	getTextSize_t getTextSizeFunc;
	getTextSize1_t getTextSize1Func;
	getTextSize2_t getTextSize2Func;
	getTextSize3_t getTextSize3Func;
	getTextSize4_t getTextSize4Func;
	setColor_t setColorFunc;
	setColor1_t setColor1Func;
	setScissorRect_t setScissorRectFunc;
	setScissorRect1_t setScissorRect1Func;
	setStroke_t setStrokeFunc;
	setStroke1_t setStroke1Func;

	void ShapeEngine(unsigned int a1, unsigned int a2)
	{
		ShapeEngineFunc(a1, a2);
	}

	void ShapeEngine()
	{
		ShapeEngine1Func();
	}

	void drawInstancedLines(unsigned __int64 a1, Vector2* a2, int a3, XMFLOAT4X3 a4, Vector4 a5)
	{
		drawInstancedLinesFunc(a1, a2, a3, a4, a5);
	}

	void drawInstancedLines(unsigned __int64 a1, Vector3* a2, int a3, XMFLOAT4X3 a4, Vector4 a5)
	{
		drawInstancedLines1Func(a1, a2, a3, a4, a5);
	}

	void drawInstancedTriangles(unsigned __int64 a1, Vector2* a2, int a3, XMFLOAT4X3 a4, Vector4 a5)
	{
		drawInstancedTrianglesFunc(a1, a2, a3, a4, a5);
	}

	void drawInstancedTriangles(unsigned __int64 a1, Vector3* a2, int a3, XMFLOAT4X3 a4, Vector4 a5)
	{
		drawInstancedTriangles1Func(a1, a2, a3, a4, a5);
	}

	void drawLine(void* ShapeEngineInstance, Vector2* a1, Vector2* a2)
	{
		drawLineFunc(ShapeEngineInstance, a1, a2);
	}

	void drawLine(void* ShapeEngineInstance, Vector3 a1, Vector3 a2)
	{
		drawLine1Func(ShapeEngineInstance, a1, a2);
	}

	void drawLine(void* ShapeEngineInstance, Vector4 a1, Vector4 a2)
	{
		drawLine2Func(ShapeEngineInstance, a1, a2);
	}

	void drawLines(void* ShapeEngineInstance, Vector2* lineData, int lineCount, XMFLOAT4X3* matrix)
	{
		drawLinesFunc(ShapeEngineInstance, lineData, lineCount, matrix);
	}

	void drawLines(void* ShapeEngineInstance, Vector3* a1, int a2, XMFLOAT4X3* a3)
	{
		drawLines1Func(ShapeEngineInstance, a1, a2, a3);
	}

	void drawPoint(void* ShapeEngineInstance, Vector2* a1)
	{
		drawPointFunc(ShapeEngineInstance, a1);
	}

	void drawPoint(void* ShapeEngineInstance, Vector3* a1)
	{
		drawPoint1Func(ShapeEngineInstance, a1);
	}

	void drawPolygon(void* ShapeEngineInstance, Vector2* a1, int a2, Vector3* a3, uint64_t* a4)
	{
		drawPolygonFunc(ShapeEngineInstance, a1, a2, a3, a4);
	}

	void drawPolygon(Vector3* a1, int a2, Vector3* a3, uint64_t* a4)
	{
		drawPolygon1Func(a1, a2, a3, a4);
	}

	void drawPolygon(Vector4* a1, int a2, Vector3* a3, uint64_t* a4)
	{
		drawPolygon2Func(a1, a2, a3, a4);
	}

	void drawSprite(Vector2 a1, Vector2 a2, rend::ShapeEngine::Pivot a3, float a4, Vector3* a5, Vector3 a6, Vector3 a7)
	{
		drawSpriteFunc(a1, a2, a3, a4, a5, a6, a7);
	}

	void drawSprite(Vector3 a1, Vector2 a2, Vector3 a3, rend::ShapeEngine::Pivot a4, float a5, Vector3* a6, Vector3 a7, Vector3 a8)
	{
		drawSprite1Func(a1, a2, a3, a4, a5, a6, a7, a8);
	}

	void drawText(Vector2 a1, char* a2, rend::ShapeEngine::Pivot a3)
	{
		drawTextFunc(a1, a2, a3);
	}

	void drawText(Vector2 a1, wchar_t* a2, rend::ShapeEngine::Pivot a3)
	{
		drawText1Func(a1, a2, a3);
	}

	void drawTriangles(void* ShapeEngineInstance, Vector2* posCoords, Vector3* texCoords, int pointCount, XMFLOAT4X3* a4)
	{
		drawTrianglesFunc(ShapeEngineInstance, posCoords, texCoords, pointCount, a4);
	}

	void drawTriangles(void* ShapeEngineInstance, Vector3* posCoords, Vector3* texCoords, int pointCount, XMFLOAT4X3* a4)
	{
		drawTriangles1Func(ShapeEngineInstance, posCoords, texCoords, pointCount, a4);
	}

	float getTextLineHeight()
	{
		return getTextLineHeightFunc();
	}

	float getTextLineHeight(void* a1, float a2)
	{
		return getTextLineHeight1Func(a1, a2);
	}

	Vector2 getTextSize(char* a1)
	{
		return getTextSizeFunc(a1);
	}

	Vector2 getTextSize(wchar_t* a1)
	{
		return getTextSize1Func(a1);
	}

	float getTextSize()
	{
		return getTextSize2Func();
	}

	void* getTextSize(void* a1, float a2, float a3, char* a4)
	{
		return getTextSize3Func(a1, a2, a3, a4);
	}

	void* getTextSize(void* a1, float a2, float a3, wchar_t* a4)
	{
		return getTextSize4Func(a1, a2, a3, a4);
	}

	void setColor(void* ShapeEngineInstance, Vector4* a1)
	{
		setColorFunc(ShapeEngineInstance, a1);
	}

	void SetFont(void* instance, void* rendFont)
	{
		rendFontPtr = rendFont;
		setFontFunc(instance, rendFont);
	}

	void setColor(void* ShapeEngineInstance, uint64_t a1)
	{
		setColor1Func(ShapeEngineInstance, a1);
	}

	void setScissorRect(Vector2 a1, Vector2 a2)
	{
		setScissorRectFunc(a1, a2);
	}

	void setScissorRect(int a1, int a2, int a3, int a4)
	{
		setScissorRect1Func(a1, a2, a3, a4);
	}

	void setStroke(void* ShapeEngineInstance, bool a1, float a2, Vector4* a3)
	{
		setStrokeFunc(ShapeEngineInstance, a1, a2, a3);
	}

	void setStroke(void* ShapeEngineInstance, bool a1, float a2, uint64_t* a3)
	{
		setStroke1Func(ShapeEngineInstance, a1, a2, a3);
	}

	void InstallHooks(LPCWSTR dllName)
	{
		HINSTANCE module = GetModuleHandle(dllName);

		if (module != NULL)
		{
			clearBuffers = (clearBuffers_t)GetProcAddress(module, "?clearBuffers@ShapeEngine@rend@@QEAAXXZ");
			createGlobalResources = (createGlobalResources_t)GetProcAddress(module, "?createGlobalResources@ShapeEngine@rend@@SAXXZ");
			deleteGlobalResources = (deleteGlobalResources_t)GetProcAddress(module, "?deleteGlobalResources@ShapeEngine@rend@@SAXXZ");
			drawCircle = (drawCircle_t)GetProcAddress(module, "?drawCircle@ShapeEngine@rend@@QEAAXAEBV?$Vector2Template@M@m@@MMMM0@Z");
			drawRectangle = (drawRectangle_t)GetProcAddress(module, "?drawRectangle@ShapeEngine@rend@@QEAAXAEBV?$Vector2Template@M@m@@0@Z");
			drawSpriteWithFilmGrain = (drawSpriteWithFilmGrain_t)GetProcAddress(module, "?drawSpriteWithFilmGrain@ShapeEngine@rend@@QEAAXAEBV?$Vector2Template@M@m@@0W4Pivot@12@MPEBV?$Vector3Template@M@4@AEBV64@3@Z");
			drawUser = (drawUser_t)GetProcAddress(module, "?drawUser@ShapeEngine@rend@@QEAAXV?$FastDelegate0@X@fastdelegate@@@Z");
			empty = (empty_t)GetProcAddress(module, "?empty@ShapeEngine@rend@@QEBA_NXZ");
			freeRenderTarget = (freeRenderTarget_t)GetProcAddress(module, "?freeRenderTarget@ShapeEngine@rend@@QEAAXPEAVTexture2D@d3d@@@Z");
			getAspectRatio = (getAspectRatio_t)GetProcAddress(module, "?getAspectRatio@ShapeEngine@rend@@QEBAMXZ");
			getBlendMode = (getBlendMode_t)GetProcAddress(module, "?getBlendMode@ShapeEngine@rend@@QEBA?AW4BlendMode@12@XZ");
			getColor = (getColor_t)GetProcAddress(module, "?getColor@ShapeEngine@rend@@QEBA?AVColor@g@@XZ");
			getDepthMode = (getDepthMode_t)GetProcAddress(module, "?getDepthMode@ShapeEngine@rend@@QEBA?AW4DepthMode@12@XZ");
			getDrawMode = (getDrawMode_t)GetProcAddress(module, "?getDrawMode@ShapeEngine@rend@@QEBA?AW4DrawMode@12@XZ");
			getDropShadow = (getDropShadow_t)GetProcAddress(module, "?getDropShadow@ShapeEngine@rend@@QEBA_NXZ");
			getFilterMode = (getFilterMode_t)GetProcAddress(module, "?getFilterMode@ShapeEngine@rend@@QEBA?AW4FilterMode@12@XZ");
			getFont = (getFont_t)GetProcAddress(module, "?getFont@ShapeEngine@rend@@QEBAPEAVFont@2@XZ");
			getInstance = (getInstance_t)GetProcAddress(module, "?getInstance@ShapeEngine@rend@@SAPEAV12@XZ");
			getTextAreaLineHeight = (getTextAreaLineHeight_t)GetProcAddress(module, "?getTextAreaLineHeight@ShapeEngine@rend@@SAMPEAVFont@2@M@Z");
			getTexture = (getTexture_t)GetProcAddress(module, "?getTexture@ShapeEngine@rend@@QEBAPEAVBaseTexture@d3d@@XZ");
			getTextureMode = (getTextureMode_t)GetProcAddress(module, "?getTextureMode@ShapeEngine@rend@@QEBA?AW4TextureMode@12@XZ");
			hasInstance = (hasInstance_t)GetProcAddress(module, "?hasInstance@ShapeEngine@rend@@SA_NXZ");
			isRendererAdministered = (isRendererAdministered_t)GetProcAddress(module, "?isRendererAdministered@ShapeEngine@rend@@QEBA_NW4ThreadID@r@@@Z");
			postEffectInit = (postEffectInit_t)GetProcAddress(module, "?postEffectInit@ShapeEngine@rend@@SAXXZ");
			pushToRenderer = (pushToRenderer_t)GetProcAddress(module, "?pushToRenderer@ShapeEngine@rend@@QEAAXW4ThreadID@r@@@Z");
			registerGlobalInstance = (registerGlobalInstance_t)GetProcAddress(module, "?registerGlobalInstance@ShapeEngine@rend@@SAXAEAV12@@Z");
			render = (render_t)GetProcAddress(module, "?render@ShapeEngine@rend@@QEAAXW4DrawMode@12@@Z");
			setAspectRatio = (setAspectRatio_t)GetProcAddress(module, "?setAspectRatio@ShapeEngine@rend@@QEAAXM@Z");
			setBlendMode = (setBlendMode_t)GetProcAddress(module, "?setBlendMode@ShapeEngine@rend@@QEAAXW4BlendMode@12@@Z");
			setCubeSamplingMode = (setCubeSamplingMode_t)GetProcAddress(module, "?setCubeSamplingMode@ShapeEngine@rend@@QEAAXW4CubeSamplingMode@12@@Z");
			setDepthMode = (setDepthMode_t)GetProcAddress(module, "?setDepthMode@ShapeEngine@rend@@QEAAXW4DepthMode@12@@Z");
			setDrawMode = (setDrawMode_t)GetProcAddress(module, "?setDrawMode@ShapeEngine@rend@@QEAAXW4DrawMode@12@@Z");
			setDropShadow = (setDropShadow_t)GetProcAddress(module, "?setDropShadow@ShapeEngine@rend@@QEAAX_N@Z");
			setFilterMode = (setFilterMode_t)GetProcAddress(module, "?setFilterMode@ShapeEngine@rend@@QEAAXW4FilterMode@12@@Z");
			setFontFunc = (setFont_t)GetProcAddress(module, "?setFont@ShapeEngine@rend@@QEAAXPEAVFont@2@@Z");
			setPointSize = (setPointSize_t)GetProcAddress(module, "?setPointSize@ShapeEngine@rend@@QEAAXM@Z");
			setRendererAdministered = (setRendererAdministered_t)GetProcAddress(module, "?setRendererAdministered@ShapeEngine@rend@@QEAAXW4ThreadID@r@@_N@Z");
			setTextSize = (setTextSize_t)GetProcAddress(module, "?setTextSize@ShapeEngine@rend@@QEAAXM@Z");
			setTexture = (setTexture_t)GetProcAddress(module, "?setTexture@ShapeEngine@rend@@QEAAXPEAVBaseTexture@d3d@@@Z");
			setTextureMode = (setTextureMode_t)GetProcAddress(module, "?setTextureMode@ShapeEngine@rend@@QEAAXW4TextureMode@12@@Z");
			setTransform = (setTransform_t)GetProcAddress(module, "?setTransform@ShapeEngine@rend@@QEAAXAEBV?$Matrix4Template@M@m@@@Z");
			setViewportTransform = (setViewportTransform_t)GetProcAddress(module, "?setViewportTransform@ShapeEngine@rend@@QEAAXXZ");
			unregisterGlobalInstance = (unregisterGlobalInstance_t)GetProcAddress(module, "?unregisterGlobalInstance@ShapeEngine@rend@@SAXAEAV12@@Z");
			ShapeEngineFunc = (ShapeEngine_t)GetProcAddress(module, "??0ShapeEngine@rend@@QEAA@II@Z");
			ShapeEngine1Func = (ShapeEngine1_t)GetProcAddress(module, "??1ShapeEngine@rend@@QEAA@XZ");
			drawInstancedLinesFunc = (drawInstancedLines_t)GetProcAddress(module, "?drawInstancedLines@ShapeEngine@rend@@QEAAX_KPEBV?$Vector2Template@M@m@@HAEBV?$Matrix4x3Template@M@4@AEBV?$Vector4Template@M@4@@Z");
			drawInstancedLines1Func = (drawInstancedLines1_t)GetProcAddress(module, "?drawInstancedLines@ShapeEngine@rend@@QEAAX_KPEBV?$Vector3Template@M@m@@HAEBV?$Matrix4x3Template@M@4@AEBV?$Vector4Template@M@4@@Z");
			drawInstancedTrianglesFunc = (drawInstancedTriangles_t)GetProcAddress(module, "?drawInstancedTriangles@ShapeEngine@rend@@QEAAX_KPEBV?$Vector2Template@M@m@@HAEBV?$Matrix4x3Template@M@4@AEBV?$Vector4Template@M@4@@Z");
			drawInstancedTriangles1Func = (drawInstancedTriangles1_t)GetProcAddress(module, "?drawInstancedTriangles@ShapeEngine@rend@@QEAAX_KPEBV?$Vector3Template@M@m@@HAEBV?$Matrix4x3Template@M@4@AEBV?$Vector4Template@M@4@@Z");
			drawLineFunc = (drawLine_t)GetProcAddress(module, "?drawLine@ShapeEngine@rend@@QEAAXAEBV?$Vector2Template@M@m@@0@Z");
			drawLine1Func = (drawLine1_t)GetProcAddress(module, "?drawLine@ShapeEngine@rend@@QEAAXAEBV?$Vector3Template@M@m@@0@Z");
			drawLine2Func = (drawLine2_t)GetProcAddress(module, "?drawLine@ShapeEngine@rend@@QEAAXAEBV?$Vector4Template@M@m@@0@Z");
			drawLinesFunc = (drawLines_t)GetProcAddress(module, "?drawLines@ShapeEngine@rend@@QEAAXPEBV?$Vector2Template@M@m@@HAEBV?$Matrix4x3Template@M@4@@Z");
			drawLines1Func = (drawLines1_t)GetProcAddress(module, "?drawLines@ShapeEngine@rend@@QEAAXPEBV?$Vector3Template@M@m@@HAEBV?$Matrix4x3Template@M@4@@Z");
			drawPointFunc = (drawPoint_t)GetProcAddress(module, "?drawPoint@ShapeEngine@rend@@QEAAXAEBV?$Vector2Template@M@m@@@Z");
			drawPoint1Func = (drawPoint1_t)GetProcAddress(module, "?drawPoint@ShapeEngine@rend@@QEAAXAEBV?$Vector3Template@M@m@@@Z");
			drawPolygonFunc = (drawPolygon_t)GetProcAddress(module, "?drawPolygon@ShapeEngine@rend@@QEAAXPEBV?$Vector2Template@M@m@@HPEBV?$Vector3Template@M@4@PEBVColor@g@@@Z");
			drawPolygon1Func = (drawPolygon1_t)GetProcAddress(module, "?drawPolygon@ShapeEngine@rend@@QEAAXPEBV?$Vector3Template@M@m@@H0PEBVColor@g@@@Z");
			drawPolygon2Func = (drawPolygon2_t)GetProcAddress(module, "?drawPolygon@ShapeEngine@rend@@QEAAXPEBV?$Vector4Template@M@m@@HPEBV?$Vector3Template@M@4@PEBVColor@g@@@Z");
			drawSpriteFunc = (drawSprite_t)GetProcAddress(module, "?drawSprite@ShapeEngine@rend@@QEAAXAEBV?$Vector2Template@M@m@@0W4Pivot@12@MPEBV?$Vector3Template@M@4@AEBV64@3@Z");
			drawSprite1Func = (drawSprite1_t)GetProcAddress(module, "?drawSprite@ShapeEngine@rend@@QEAAXAEBV?$Vector3Template@M@m@@AEBV?$Vector2Template@M@4@0W4Pivot@12@MPEBV34@00@Z");
			drawTextFunc = (drawText_t)GetProcAddress(module, "?drawText@ShapeEngine@rend@@QEAAXAEBV?$Vector2Template@M@m@@PEBDW4Pivot@12@@Z");
			drawText1Func = (drawText1_t)GetProcAddress(module, "?drawText@ShapeEngine@rend@@QEAAXAEBV?$Vector2Template@M@m@@PEB_WW4Pivot@12@@Z");
			drawTrianglesFunc = (drawTriangles_t)GetProcAddress(module, "?drawTriangles@ShapeEngine@rend@@QEAAXPEBV?$Vector2Template@M@m@@PEBV?$Vector3Template@M@4@HAEBV?$Matrix4x3Template@M@4@@Z");
			drawTriangles1Func = (drawTriangles1_t)GetProcAddress(module, "?drawTriangles@ShapeEngine@rend@@QEAAXPEBV?$Vector3Template@M@m@@0HAEBV?$Matrix4x3Template@M@4@@Z");
			getTextLineHeightFunc = (getTextLineHeight_t)GetProcAddress(module, "?getTextLineHeight@ShapeEngine@rend@@QEBAMXZ");
			getTextLineHeight1Func = (getTextLineHeight1_t)GetProcAddress(module, "?getTextLineHeight@ShapeEngine@rend@@SAMPEAVFont@2@M@Z");
			getTextSizeFunc = (getTextSize_t)GetProcAddress(module, "?getTextSize@ShapeEngine@rend@@QEBA?AV?$Vector2Template@M@m@@PEBD@Z");
			getTextSize1Func = (getTextSize1_t)GetProcAddress(module, "?getTextSize@ShapeEngine@rend@@QEBA?AV?$Vector2Template@M@m@@PEB_W@Z");
			getTextSize2Func = (getTextSize2_t)GetProcAddress(module, "?getTextSize@ShapeEngine@rend@@QEBAMXZ");
			getTextSize3Func = (getTextSize3_t)GetProcAddress(module, "?getTextSize@ShapeEngine@rend@@SA?AV?$Vector2Template@M@m@@PEAVFont@2@MMPEBD@Z");
			getTextSize4Func = (getTextSize4_t)GetProcAddress(module, "?getTextSize@ShapeEngine@rend@@SA?AV?$Vector2Template@M@m@@PEAVFont@2@MMPEB_W@Z");
			setColorFunc = (setColor_t)GetProcAddress(module, "?setColor@ShapeEngine@rend@@QEAAXAEBV?$Vector4Template@M@m@@@Z");
			setColor1Func = (setColor1_t)GetProcAddress(module, "?setColor@ShapeEngine@rend@@QEAAXAEBVColor@g@@@Z");
			setScissorRectFunc = (setScissorRect_t)GetProcAddress(module, "?setScissorRect@ShapeEngine@rend@@QEAAXAEBV?$Vector2Template@M@m@@0@Z");
			setScissorRect1Func = (setScissorRect1_t)GetProcAddress(module, "?setScissorRect@ShapeEngine@rend@@QEAAXHHHH@Z");
			setStrokeFunc = (setStroke_t)GetProcAddress(module, "?setStroke@ShapeEngine@rend@@QEAAX_NMAEBV?$Vector4Template@M@m@@@Z");
			setStroke1Func = (setStroke1_t)GetProcAddress(module, "?setStroke@ShapeEngine@rend@@QEAAX_NMAEBVColor@g@@@Z");

			// TODO: we get a font ptr from the first call for now
			// later we can initialize our own font, but for now this will do
			void* setFontFuncPtr = (void*)GetProcAddress(module, "?setFont@ShapeEngine@rend@@QEAAXPEAVFont@2@@Z");
			if (MH_CreateHook(setFontFuncPtr, &SetFont, reinterpret_cast<LPVOID*>(&setFontFunc)) != MH_OK) throw;
			if (MH_EnableHook(setFontFuncPtr) != MH_OK) throw;
		}
	}
}