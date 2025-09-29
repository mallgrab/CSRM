#include "ShapeEngine.h"
#include "LootDrop.h" // TODO: including cause we want GetComponentByTypeId funcs, should refactor it into its own cpp file later

#define shapeEngineInstance getInstance() //change to a class pointer that caches the instance pointer?
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
	drawTriangles_t drawTrianglesFunc;
	drawTriangles1_t drawTriangles1Func;
	getTextLineHeight_t getTextLineHeightFunc;
	getTextLineHeight1_t getTextLineHeight1Func;
	getTextSize_t getTextSizeFunc;
	getTextSize2_t getTextSize2Func;
	getTextSize3_t getTextSize3Func;
	setColor_t setColorFunc;
	setColor1_t setColor1Func;
	setScissorRect_t setScissorRectFunc;
	setScissorRect1_t setScissorRect1Func;
	setStroke_t setStrokeFunc;
	setStroke1_t setStroke1Func;

	void ShapeEngine(unsigned int a1, unsigned int a2)
	{
		ShapeEngineFunc(shapeEngineInstance, a1, a2);
	}

	void ShapeEngine()
	{
		ShapeEngine1Func(shapeEngineInstance);
	}

	void drawInstancedLines(unsigned __int64 a1, Vector2* a2, int a3, XMFLOAT4X3 a4, Vector4 a5)
	{
		drawInstancedLinesFunc(shapeEngineInstance, a1, a2, a3, a4, a5);
	}

	void drawInstancedLines(unsigned __int64 a1, Vector3* a2, int a3, XMFLOAT4X3 a4, Vector4 a5)
	{
		drawInstancedLines1Func(shapeEngineInstance, a1, a2, a3, a4, a5);
	}

	void drawInstancedTriangles(unsigned __int64 a1, Vector2* a2, int a3, XMFLOAT4X3 a4, Vector4 a5)
	{
		drawInstancedTrianglesFunc(shapeEngineInstance, a1, a2, a3, a4, a5);
	}

	void drawInstancedTriangles(unsigned __int64 a1, Vector3* a2, int a3, XMFLOAT4X3 a4, Vector4 a5)
	{
		drawInstancedTriangles1Func(shapeEngineInstance, a1, a2, a3, a4, a5);
	}

	void drawLine(Vector2* start, Vector2* end)
	{
		drawLineFunc(shapeEngineInstance, start, end);
	}

	void drawLine(Vector3* start, Vector3* end)
	{
		drawLine1Func(shapeEngineInstance, start, end);
	}

	void drawLine(Vector4* start, Vector4* end)
	{
		drawLine2Func(shapeEngineInstance, start, end);
	}

	void drawLines(Vector2* lineData, int lineCount, XMFLOAT4X3* matrix)
	{
		drawLinesFunc(shapeEngineInstance, lineData, lineCount, matrix);
	}

	void drawLines(Vector3* lineData, int lineCount, XMFLOAT4X3* matrix)
	{
		drawLines1Func(shapeEngineInstance, lineData, lineCount, matrix);
	}

	void drawPoint(Vector2* point)
	{
		drawPointFunc(shapeEngineInstance, point);
	}

	void drawPoint(Vector3* point)
	{
		drawPoint1Func(shapeEngineInstance, point);
	}

	void drawPolygon(Vector2* a1, int a2, Vector3* a3, uint64_t* a4)
	{
		drawPolygonFunc(shapeEngineInstance, a1, a2, a3, a4);
	}

	void drawPolygon(Vector3* a1, int a2, Vector3* a3, uint64_t* a4)
	{
		drawPolygon1Func(shapeEngineInstance, a1, a2, a3, a4);
	}

	void drawPolygon(Vector4* a1, int a2, Vector3* a3, uint64_t* a4)
	{
		drawPolygon2Func(shapeEngineInstance, a1, a2, a3, a4);
	}

	void drawSprite(Vector2 a1, Vector2 a2, rend::ShapeEngine::Pivot pivot, float a4, Vector3* a5, Vector3 a6, Vector3 a7)
	{
		drawSpriteFunc(shapeEngineInstance, a1, a2, pivot, a4, a5, a6, a7);
	}

	void drawSprite(Vector3 a1, Vector2 a2, Vector3 a3, rend::ShapeEngine::Pivot pivot, float a5, Vector3* a6, Vector3 a7, Vector3 a8)
	{
		drawSprite1Func(shapeEngineInstance, a1, a2, a3, pivot, a5, a6, a7, a8);
	}

	void drawText(Vector2 *pos, const char *str, rend::ShapeEngine::Pivot pivot)
	{
		drawTextFunc(shapeEngineInstance, pos, str, pivot);
	}

	void drawTriangles(Vector2* posCoords, Vector3* texCoords, int pointCount, XMFLOAT4X3* a4)
	{
		drawTrianglesFunc(shapeEngineInstance, posCoords, texCoords, pointCount, a4);
	}

	void drawTriangles(Vector3* posCoords, Vector3* texCoords, int pointCount, XMFLOAT4X3* a4)
	{
		drawTriangles1Func(shapeEngineInstance, posCoords, texCoords, pointCount, a4);
	}

	float getTextLineHeight()
	{
		return getTextLineHeightFunc(shapeEngineInstance);
	}

	float getTextLineHeight(void* a1, float a2)
	{
		return getTextLineHeight1Func(shapeEngineInstance, a1, a2);
	}

	Vector2 getTextSize(char* str)
	{ //this is crashing for som reason
		return getTextSizeFunc(shapeEngineInstance, str);
	}

	float getTextSize()
	{
		return getTextSize2Func(shapeEngineInstance);
	}

	void* getTextSize(void* a1, float a2, float a3, char* str)
	{
		return getTextSize3Func(shapeEngineInstance, a1, a2, a3, str);
	}

	void SetFont(void* instance, void* rendFont) //function we hook, must pass instance here
	{
		rendFontPtr = rendFont;
		setFontFunc(instance, rendFont);
	}

	void setColor(Vector4* color)
	{
		setColorFunc(shapeEngineInstance, color);
	}

	void setColor(uint64_t icolor)
	{
		setColor1Func(shapeEngineInstance, icolor);
	}

	void setScissorRect(Vector2 a1, Vector2 a2)
	{
		setScissorRectFunc(shapeEngineInstance, a1, a2);
	}

	void setScissorRect(int a1, int a2, int a3, int a4)
	{
		setScissorRect1Func(shapeEngineInstance, a1, a2, a3, a4);
	}

	void setStroke(bool a1, float a2, Vector4* a3)
	{
		setStrokeFunc(shapeEngineInstance, a1, a2, a3);
	}

	void setStroke1(bool a1, float a2, Vector4* color)
	{
		setStroke1Func(shapeEngineInstance, a1, a2, color);
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
			drawTrianglesFunc = (drawTriangles_t)GetProcAddress(module, "?drawTriangles@ShapeEngine@rend@@QEAAXPEBV?$Vector2Template@M@m@@PEBV?$Vector3Template@M@4@HAEBV?$Matrix4x3Template@M@4@@Z");
			drawTriangles1Func = (drawTriangles1_t)GetProcAddress(module, "?drawTriangles@ShapeEngine@rend@@QEAAXPEBV?$Vector3Template@M@m@@0HAEBV?$Matrix4x3Template@M@4@@Z");
			getTextLineHeightFunc = (getTextLineHeight_t)GetProcAddress(module, "?getTextLineHeight@ShapeEngine@rend@@QEBAMXZ");
			getTextLineHeight1Func = (getTextLineHeight1_t)GetProcAddress(module, "?getTextLineHeight@ShapeEngine@rend@@SAMPEAVFont@2@M@Z");
			getTextSizeFunc = (getTextSize_t)GetProcAddress(module, "?getTextSize@ShapeEngine@rend@@QEBA?AV?$Vector2Template@M@m@@PEBD@Z");
			getTextSize2Func = (getTextSize2_t)GetProcAddress(module, "?getTextSize@ShapeEngine@rend@@QEBAMXZ");
			getTextSize3Func = (getTextSize3_t)GetProcAddress(module, "?getTextSize@ShapeEngine@rend@@SA?AV?$Vector2Template@M@m@@PEAVFont@2@MMPEBD@Z");
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

#include <DirectXMath.h>
#include "GameData.h"

HMODULE coregameModule = nullptr;
HMODULE rlModule = nullptr;
HMODULE basemodule = nullptr;

using getOwner_t = uint64_t * (__fastcall*)(void* component);
getOwner_t getOwner;

using getName_t = const char* (__fastcall*)(void* gameobjectstate);
getName_t getName;

triggerScriptCache currentSelectedTrigger;
uint64_t globalIDCache = 0;
Vector3 tmpPosition = Vector3(0,0,0);
Vector3 tmpScale = Vector3(0,0,0);
Vector3 tmpbbox_a = Vector3(0,0,0);
Vector3 tmpbbox_b = Vector3(0,0,0);

void DrawCube(Vector3 scale, Vector3 translate, Vector3 rotation)
{
	Vector4 red = Vector4(1.0f, 0.0f, 0.0f, cfg->triggerOpacity);
	Vector4 orange = Vector4(1.0f, 0.5f, 0.0f, cfg->triggerOpacity);
	Vector4 green = Vector4(0.3f, 1.0f, 0.3f, cfg->triggerOpacity);
	Vector4 blue = Vector4(0.1f, 1.0f, 1.0f, cfg->triggerOpacity);
	Vector4 purple = Vector4(0.7f, 0.3f, 0.7f, cfg->triggerOpacity);
	Vector4 white = Vector4(1.0f, 1.0f, 1.0f, cfg->triggerOpacity);

	Vector3 posCoords[36];
	Vector3 texCoords[36];

	posCoords[0] = Vector3(1.0f, 0.0f, 1.0f);
	posCoords[1] = Vector3(1.0f, -1.0f, 0.0f);
	posCoords[2] = Vector3(1.0f, 0.0f, 0.0f);

	posCoords[3] = Vector3(1.0f, -1.0f, 1.0f);
	posCoords[4] = Vector3(1.0f, -1.0f, 0.0f);
	posCoords[5] = Vector3(1.0f, 0.0f, 1.0f);

	posCoords[6] = Vector3(1.0f, 0.0f, 1.0f);
	posCoords[7] = Vector3(1.0f, 0.0f, 0.0f);
	posCoords[8] = Vector3(0.0f, 0.0f, 0.0f);

	posCoords[9] = Vector3(1.0f, 0.0f, 1.0f);
	posCoords[10] = Vector3(0.0f, 0.0f, 0.0f);
	posCoords[11] = Vector3(0.0f, 0.0f, 1.0f);

	posCoords[12] = Vector3(0.0f, -1.0f, 1.0f);
	posCoords[13] = Vector3(1.0f, -1.0f, 1.0f);
	posCoords[14] = Vector3(0.0f, 0.0f, 1.0f);

	posCoords[15] = Vector3(0.0f, 0.0f, 1.0f);
	posCoords[16] = Vector3(1.0f, -1.0f, 1.0f);
	posCoords[17] = Vector3(1.0f, 0.0f, 1.0f);

	posCoords[18] = Vector3(1.0f, 0.0f, 0.0f);
	posCoords[19] = Vector3(1.0f, -1.0f, 0.0f);
	posCoords[20] = Vector3(0.0f, 0.0f, 0.0f);

	posCoords[21] = Vector3(0.0f, 0.0f, 0.0f);
	posCoords[22] = Vector3(1.0f, -1.0f, 0.0f);
	posCoords[23] = Vector3(0.0f, -1.0f, 0.0f);

	posCoords[24] = Vector3(0.0f, 0.0f, 0.0f);
	posCoords[25] = Vector3(0.0f, -1.0f, 0.0f);
	posCoords[26] = Vector3(0.0f, 0.0f, 1.0f);

	posCoords[27] = Vector3(0.0f, 0.0f, 1.0f);
	posCoords[28] = Vector3(0.0f, -1.0f, 0.0f);
	posCoords[29] = Vector3(0.0f, -1.0f, 1.0f);

	posCoords[30] = Vector3(0.0f, -1.0f, 0.0f);
	posCoords[31] = Vector3(1.0f, -1.0f, 0.0f);
	posCoords[32] = Vector3(1.0f, -1.0f, 1.0f);

	posCoords[33] = Vector3(0.0f, -1.0f, 1.0f);
	posCoords[34] = Vector3(0.0f, -1.0f, 0.0f);
	posCoords[35] = Vector3(1.0f, -1.0f, 1.0f);

	DirectX::XMFLOAT4X3 modifiers = DirectX::XMFLOAT4X3();
	DirectX::XMVECTOR xm_scale;
	DirectX::XMVECTOR xm_translation;
	DirectX::XMVECTOR xm_rotation;
	DirectX::XMVECTOR xm_empty = DirectX::XMVECTOR();

	xm_translation.m128_f32[0] = translate.x;
	xm_translation.m128_f32[1] = translate.y;
	xm_translation.m128_f32[2] = translate.z;

	xm_scale.m128_f32[0] = scale.x;
	xm_scale.m128_f32[1] = scale.y;
	xm_scale.m128_f32[2] = scale.z;

	xm_rotation.m128_f32[0] = rotation.x;
	xm_rotation.m128_f32[1] = rotation.y;
	xm_rotation.m128_f32[2] = rotation.z;

	DirectX::XMStoreFloat4x3(&modifiers, DirectX::XMMatrixAffineTransformation(xm_scale, xm_empty, xm_rotation, xm_translation));

	ShapeEngine::setColor(&blue);
	ShapeEngine::drawTriangles(posCoords, texCoords, 36, &modifiers);
}

// TODO: port this so we draw triggers this way
// TODO: also draw death triggers as just boxes, with depth/culling enabled
void DrawTrigger(Vector3 a, Vector3 b)
{
	Vector3 modifiedScale;
	Vector3 modifiedTranslate;

	float width = 0.002f;

	// top lines
	modifiedScale.x = (float)abs(a.x - b.x) * width;
	modifiedScale.y = (float)abs(a.y - b.y) * width;
	modifiedScale.z = (float)abs(a.z - b.z);

	DrawCube(modifiedScale, { a.x, b.y, a.z }, Vector3(0, 0, 0));

	modifiedScale.x = (float)abs(a.x - b.x) * width;
	modifiedScale.y = (float)abs(a.y - b.y) * width;
	modifiedScale.z = (float)abs(a.z - b.z);

	DrawCube(modifiedScale, { b.x, b.y, a.z }, Vector3(0, 0, 0));

	modifiedScale.x = (float)abs(a.x - b.x);
	modifiedScale.y = (float)abs(a.y - b.y) * width;
	modifiedScale.z = (float)abs(a.z - b.z) * width;

	DrawCube(modifiedScale, { a.x, b.y, a.z }, Vector3(0, 0, 0));

	modifiedScale.x = (float)abs(a.x - b.x) * (1.0f + width);
	modifiedScale.y = (float)abs(a.y - b.y) * width;
	modifiedScale.z = (float)abs(a.z - b.z) * width;

	DrawCube(modifiedScale, { a.x, b.y, b.z }, Vector3(0, 0, 0));


	// vertical lines
	modifiedScale.x = (float)abs(a.x - b.x) * width;
	modifiedScale.y = (float)abs(a.y - b.y);
	modifiedScale.z = (float)abs(a.z - b.z) * width;

	DrawCube(modifiedScale, { a.x, b.y, b.z }, Vector3(0, 0, 0));

	modifiedScale.x = (float)abs(a.x - b.x) * width;
	modifiedScale.y = (float)abs(a.y - b.y);
	modifiedScale.z = (float)abs(a.z - b.z) * width;

	DrawCube(modifiedScale, { b.x, b.y, b.z }, Vector3(0, 0, 0));

	modifiedScale.x = (float)abs(a.x - b.x) * width;
	modifiedScale.y = (float)abs(a.y - b.y);
	modifiedScale.z = (float)abs(a.z - b.z) * width;

	DrawCube(modifiedScale, { b.x, b.y, a.z }, Vector3(0, 0, 0));

	modifiedScale.x = (float)abs(a.x - b.x) * width;
	modifiedScale.y = (float)abs(a.y - b.y);
	modifiedScale.z = (float)abs(a.z - b.z) * width;

	DrawCube(modifiedScale, { a.x, b.y, a.z }, Vector3(0, 0, 0));


	// bottom lines
	modifiedScale.x = (float)abs(a.x - b.x) * width;
	modifiedScale.y = (float)abs(a.y - b.y) * width;
	modifiedScale.z = (float)abs(a.z - b.z);

	DrawCube(modifiedScale, { a.x, a.y, a.z }, Vector3(0, 0, 0));

	modifiedScale.x = (float)abs(a.x - b.x) * width;
	modifiedScale.y = (float)abs(a.y - b.y) * width;
	modifiedScale.z = (float)abs(a.z - b.z);

	DrawCube(modifiedScale, { b.x, a.y, a.z }, Vector3(0, 0, 0));

	modifiedScale.x = (float)abs(a.x - b.x);
	modifiedScale.y = (float)abs(a.y - b.y) * width;
	modifiedScale.z = (float)abs(a.z - b.z) * width;

	DrawCube(modifiedScale, { a.x, a.y, a.z }, Vector3(0, 0, 0));

	modifiedScale.x = (float)abs(a.x - b.x) * (1.0f + width);
	modifiedScale.y = (float)abs(a.y - b.y) * width;
	modifiedScale.z = (float)abs(a.z - b.z) * width;

	DrawCube(modifiedScale, { a.x, a.y, b.z }, Vector3(0, 0, 0));
}

// TODO: ccw culling makes the triggers look fucked in depth mode fix later
void RenderDrawCall()
{
	if (!cfg->drawTriggers)
			return;

	Vector3 posCoords[3] = { {0,0,0},{0,0,0},{0,0,0} };
	Vector3 posCoords2[3] = { {0,0,0},{0,0,0},{0,0,0} };

	Vector4 red = Vector4(1.0f, 0.0f, 0.0f, cfg->triggerOpacity);
	Vector4 orange = Vector4(1.0f, 0.5f, 0.0f, cfg->triggerOpacity);
	Vector4 green = Vector4(0.3f, 1.0f, 0.3f, cfg->triggerOpacity);
	Vector4 blue = Vector4(0.1f, 1.0f, 1.0f, cfg->triggerOpacity);
	Vector4 purple = Vector4(0.7f, 0.3f, 0.7f, cfg->triggerOpacity);
	Vector4 white = Vector4(1.0f, 1.0f, 1.0f, cfg->triggerOpacity);

	static bool init = false;

	if (!init)
	{
		coregameModule = GetModuleHandle(L"coregame_rmdwin7_f.dll");
		rlModule = GetModuleHandle(L"rl_rmdwin7_f.dll");
		basemodule = GetModuleHandle(nullptr);

		getOwner = (getOwner_t)GetProcAddress(rlModule, "?getOwner@ComponentBase@r@@QEBAPEAVGameObject@2@XZ");
		getName = (getName_t)GetProcAddress(rlModule, "?getName@GameObjectState@r@@UEBAPEBDXZ");

		init = true;
	}

	struct posNscale {
		Vector3 pos;
		Vector3 scale;
		int draw;
		int blend;
		int depth;
		int filter;
	};
	posNscale posScale = {};
	posScale.scale = Vector3(1.0f,1.0f,1.0f);
	posScale.draw = 1;
	posScale.depth = 1;
	posScale.filter = 1; // max 3
	posScale.blend = 3;

	//below tested in GetViewMatrix
	DirectX::XMFLOAT4X3 modifiers = DirectX::XMFLOAT4X3();
	DirectX::XMMATRIX tmp = DirectX::XMMatrixIdentity();
	DirectX::XMVECTOR empty = DirectX::XMVECTOR();

	DirectX::XMVECTOR scale;
	DirectX::XMVECTOR translation;
	DirectX::XMVECTOR rotation;

	DirectX::XMMatrixDecompose(&scale, &rotation, &translation, tmp);

	if (!cfg->ui->data->GetPlayerPos())
		return;

	Vector3 texCoords[3] = { { 0.1f, 0.1f, 0.0f }, { 0.1f, 0.1f, 0.0f }, { 0.1f, 0.1f, 0.0f } };
	
	Matrix4* viewMatrix = cfg->ui->data->GetViewMatrix();

	ShapeEngine::setTransform(ShapeEngine::getInstance(), *viewMatrix);

	ShapeEngine::setDrawMode(ShapeEngine::getInstance(), posScale.draw);

	if (posScale.blend > 5)
		ShapeEngine::setBlendMode(ShapeEngine::getInstance(), 5);
	else
		ShapeEngine::setBlendMode(ShapeEngine::getInstance(), posScale.blend);

	// setting to 1 lets it get clipped properly
	// 0 is drawing it regardless
	// draw mode has to be 0 tho for it to work
	ShapeEngine::setDepthMode(ShapeEngine::getInstance(), posScale.depth);
	ShapeEngine::setFilterMode(ShapeEngine::getInstance(), posScale.filter);
	ShapeEngine::setTexture(ShapeEngine::getInstance(), nullptr);

	DrawTrigger(tmpbbox_a, tmpbbox_b);

	posScale.pos = Vector3(0.0f,0.0f,0.0f);
	translation.m128_f32[0] = 0.0f;
	translation.m128_f32[1] = 0.0f;
	translation.m128_f32[2] = 0.0f;

	scale.m128_f32[0] = 1.0f;
	scale.m128_f32[1] = 1.0f;
	scale.m128_f32[2] = 1.0f;
	DirectX::XMStoreFloat4x3(&modifiers, DirectX::XMMatrixAffineTransformation(scale, empty, rotation, translation));

	auto triggers = TriggerGetVector();
	for (int i = 0; i < triggers.size(); i++)
	{
		if (triggers[i] == nullptr)
			continue;

		if (!cfg->drawTriggers)
			return;
	
		
	if (triggers[cfg->triggerToDraw] != nullptr)
		cfg->triggerGlobalID = triggers[cfg->triggerToDraw]->state->globalID;

	float width = 0.02f;
	float rx = 0.0f, ry = 0.0f, rz = 0.0f, lx = 0.0f, ly = 0.0f, lz = 0.0f;
	
	if (i == cfg->triggerToDraw)
	{
		tmpbbox_a = triggers[i]->boundbox.a;
		tmpbbox_b = triggers[i]->boundbox.b;

		uint64_t* triggerEntity = getOwner(triggers[i]);
		if (triggerEntity == nullptr)
			continue;

		uint64_t* triggerEntityState = *(uint64_t**)((char*)triggerEntity + 0x8);

		char* ScriptComponentStateIDPtr = (char*)basemodule + 0x11ADBBC;
		uint64_t ScriptComponentStateID = *(uint64_t*)ScriptComponentStateIDPtr;
		char* ScriptComponentStateComponent = (char*)GameObjectState_GetComponentByTypeId((uint64_t)triggerEntityState, ScriptComponentStateID);

		if (ScriptComponentStateComponent != nullptr)
		{
			char* ScriptContent = *(char**)(ScriptComponentStateComponent + 0x48);
			char* ScriptPtr = *(char**)(ScriptContent + 0x88);
			uint32_t ScriptStringSize = (uint32_t)*(char**)(ScriptContent + 0x90);
			char* ScriptTypePtr = *(char**)(ScriptContent + 0x120);

			if (currentSelectedTrigger.globalID != triggers[i]->state->globalID)
			{
				currentSelectedTrigger.globalID = triggers[i]->state->globalID;

				char* scriptString = (char*)malloc(sizeof(char) * ScriptStringSize);
				memcpy(scriptString, ScriptPtr, sizeof(char) * ScriptStringSize);
				
				for (int i = 0; i < ScriptStringSize; i++)
					if (scriptString[i] == '\0')
						scriptString[i] = '\n';

				scriptString[ScriptStringSize-1] = '\0';
				currentSelectedTrigger.script = scriptString;

				free(scriptString);
			}
		}
		else
			currentSelectedTrigger.script = "";

		ShapeEngine::setColor(&blue);
	}
	else
	{
		// we could call physics::Trigger::isPointInside instead of relying on the game to set the enter state
		// since some triggers are progression triggers and they will only get enabled once

		// also being able to disable all triggers and manually activate them would be another feature we should add
		if (triggers[i]->state->onEnter)
			ShapeEngine::setColor(&green);
		else
		{
			if (cfg->hideTriggerIfNotEntered)
				continue;
		
			ShapeEngine::setColor(&orange);

			uint64_t* triggerEntity = getOwner(triggers[i]);
			if (triggerEntity == nullptr)
				continue;

			uint64_t* triggerEntityState = *(uint64_t**)((char*)triggerEntity + 0x8);

			char* ScriptComponentStateIDPtr = (char*)basemodule + 0x11ADBBC;
			uint64_t ScriptComponentStateID = *(uint64_t*)ScriptComponentStateIDPtr;
			char* ScriptComponentStateComponent = (char*)GameObjectState_GetComponentByTypeId((uint64_t)triggerEntityState, ScriptComponentStateID);

			if (ScriptComponentStateComponent != nullptr)
			{
				char* ScriptContent = *(char**)(ScriptComponentStateComponent + 0x48);
				char* ScriptPtr = *(char**)(ScriptContent + 0x88);
				uint32_t ScriptStringSize = (uint32_t) * (char**)(ScriptContent + 0x90);
				char* ScriptTypePtr = *(char**)(ScriptContent + 0x120);

				char* result = (char*)memmem(ScriptPtr, ScriptStringSize * sizeof(char), "DealKillingDamage", sizeof("DealKillingDamage"));

				if (result != nullptr)
					ShapeEngine::setColor(&red);
				else
					ShapeEngine::setColor(&purple);
			}
		}
	}





	rx = triggers[i]->boundbox.a.x;
	ry = triggers[i]->boundbox.a.y;
	rz = triggers[i]->boundbox.a.z;
						
	lx = triggers[i]->boundbox.a.x;
	ly = triggers[i]->boundbox.a.y;
	lz = triggers[i]->boundbox.b.z;

	posCoords[0].x = rx;
	posCoords[0].y = -width + ry;
	posCoords[0].z = rz;

	posCoords[1].x = lx;
	posCoords[1].y = -width + ly;
	posCoords[1].z = lz;

	posCoords[2].x = lx;
	posCoords[2].y = width + ly;
	posCoords[2].z = lz;

	posCoords2[0].x = rx;
	posCoords2[0].y = -width + ry;
	posCoords2[0].z = rz;
				 
	posCoords2[1].x = rx;
	posCoords2[1].y = width + ry;
	posCoords2[1].z = rz;
				 
	posCoords2[2].x = lx;
	posCoords2[2].y = width + ly;
	posCoords2[2].z = lz;

	ShapeEngine::drawTriangles(posCoords, texCoords, 3, &modifiers);
	ShapeEngine::drawTriangles(posCoords2, texCoords, 3, &modifiers);


	rx = triggers[i]->boundbox.a.x;
	ry = triggers[i]->boundbox.a.y;
	rz = triggers[i]->boundbox.b.z;
					
	lx = triggers[i]->boundbox.b.x;
	ly = triggers[i]->boundbox.a.y;
	lz = triggers[i]->boundbox.b.z;


	posCoords[0].x = rx;
	posCoords[0].y = -width + ry;
	posCoords[0].z = rz;

	posCoords[1].x = lx;
	posCoords[1].y = -width + ly;
	posCoords[1].z = lz;

	posCoords[2].x = lx;
	posCoords[2].y = width + ly;
	posCoords[2].z = lz;

	posCoords2[0].x = rx;
	posCoords2[0].y = -width + ry;
	posCoords2[0].z = rz;

	posCoords2[1].x = rx;
	posCoords2[1].y = width + ry;
	posCoords2[1].z = rz;

	posCoords2[2].x = lx;
	posCoords2[2].y = width + ly;
	posCoords2[2].z = lz;

	ShapeEngine::drawTriangles(posCoords, texCoords, 3, &modifiers);
	ShapeEngine::drawTriangles(posCoords2, texCoords, 3, &modifiers);

	rx = triggers[i]->boundbox.b.x;
	ry = triggers[i]->boundbox.a.y;
	rz = triggers[i]->boundbox.a.z;
					
	lx = triggers[i]->boundbox.a.x;
	ly = triggers[i]->boundbox.a.y;
	lz = triggers[i]->boundbox.a.z;

	posCoords[0].x = rx;
	posCoords[0].y = -width + ry;
	posCoords[0].z = rz;

	posCoords[1].x = lx;
	posCoords[1].y = -width + ly;
	posCoords[1].z = lz;

	posCoords[2].x = lx;
	posCoords[2].y = width + ly;
	posCoords[2].z = lz;

	posCoords2[0].x = rx;
	posCoords2[0].y = -width + ry;
	posCoords2[0].z = rz;

	posCoords2[1].x = rx;
	posCoords2[1].y = width + ry;
	posCoords2[1].z = rz;

	posCoords2[2].x = lx;
	posCoords2[2].y = width + ly;
	posCoords2[2].z = lz;

	ShapeEngine::drawTriangles(posCoords, texCoords, 3, &modifiers);
	ShapeEngine::drawTriangles(posCoords2, texCoords, 3, &modifiers);

	rx = triggers[i]->boundbox.b.x;
	ry = triggers[i]->boundbox.a.y;
	rz = triggers[i]->boundbox.b.z;

	lx = triggers[i]->boundbox.b.x;
	ly = triggers[i]->boundbox.a.y;
	lz = triggers[i]->boundbox.a.z;

	posCoords[0].x = rx;
	posCoords[0].y = -width + ry;
	posCoords[0].z = rz;

	posCoords[1].x = lx;
	posCoords[1].y = -width + ly;
	posCoords[1].z = lz;

	posCoords[2].x = lx;
	posCoords[2].y = width + ly;
	posCoords[2].z = lz;

	posCoords2[0].x = rx;
	posCoords2[0].y = -width + ry;
	posCoords2[0].z = rz;

	posCoords2[1].x = rx;
	posCoords2[1].y = width + ry;
	posCoords2[1].z = rz;

	posCoords2[2].x = lx;
	posCoords2[2].y = width + ly;
	posCoords2[2].z = lz;

	ShapeEngine::drawTriangles(posCoords, texCoords, 3, &modifiers);
	ShapeEngine::drawTriangles(posCoords2, texCoords, 3, &modifiers);

	rx = triggers[i]->boundbox.b.x;
	ry = triggers[i]->boundbox.a.y;
	rz = triggers[i]->boundbox.b.z;

	lx = triggers[i]->boundbox.b.x;
	ly = triggers[i]->boundbox.a.y;
	lz = triggers[i]->boundbox.a.z;


	posCoords[0].x = rx;
	posCoords[0].y = -width + ry;
	posCoords[0].z = rz;

	posCoords[1].x = lx;
	posCoords[1].y = -width + ly;
	posCoords[1].z = lz;

	posCoords[2].x = lx;
	posCoords[2].y = width + ly;
	posCoords[2].z = lz;

	posCoords2[0].x = rx;
	posCoords2[0].y = -width + ry;
	posCoords2[0].z = rz;

	posCoords2[1].x = rx;
	posCoords2[1].y = width + ry;
	posCoords2[1].z = rz;

	posCoords2[2].x = lx;
	posCoords2[2].y = width + ly;
	posCoords2[2].z = lz;

	ShapeEngine::drawTriangles(posCoords, texCoords, 3, &modifiers);
	ShapeEngine::drawTriangles(posCoords2, texCoords, 3, &modifiers);






	rx = triggers[i]->boundbox.a.x;
	ry = triggers[i]->boundbox.b.y;
	rz = triggers[i]->boundbox.a.z;

	lx = triggers[i]->boundbox.a.x;
	ly = triggers[i]->boundbox.b.y;
	lz = triggers[i]->boundbox.b.z;

	posCoords[0].x = rx;
	posCoords[0].y = -width + ry;
	posCoords[0].z = rz;

	posCoords[1].x = lx;
	posCoords[1].y = -width + ly;
	posCoords[1].z = lz;

	posCoords[2].x = lx;
	posCoords[2].y = width + ly;
	posCoords[2].z = lz;

	posCoords2[0].x = rx;
	posCoords2[0].y = -width + ry;
	posCoords2[0].z = rz;

	posCoords2[1].x = rx;
	posCoords2[1].y = width + ry;
	posCoords2[1].z = rz;

	posCoords2[2].x = lx;
	posCoords2[2].y = width + ly;
	posCoords2[2].z = lz;

	ShapeEngine::drawTriangles(posCoords, texCoords, 3, &modifiers);
	ShapeEngine::drawTriangles(posCoords2, texCoords, 3, &modifiers);


	rx = triggers[i]->boundbox.a.x;
	ry = triggers[i]->boundbox.b.y;
	rz = triggers[i]->boundbox.b.z;

	lx = triggers[i]->boundbox.b.x;
	ly = triggers[i]->boundbox.b.y;
	lz = triggers[i]->boundbox.b.z;


	posCoords[0].x = rx;
	posCoords[0].y = -width + ry;
	posCoords[0].z = rz;

	posCoords[1].x = lx;
	posCoords[1].y = -width + ly;
	posCoords[1].z = lz;

	posCoords[2].x = lx;
	posCoords[2].y = width + ly;
	posCoords[2].z = lz;

	posCoords2[0].x = rx;
	posCoords2[0].y = -width + ry;
	posCoords2[0].z = rz;

	posCoords2[1].x = rx;
	posCoords2[1].y = width + ry;
	posCoords2[1].z = rz;

	posCoords2[2].x = lx;
	posCoords2[2].y = width + ly;
	posCoords2[2].z = lz;

	ShapeEngine::drawTriangles(posCoords, texCoords, 3, &modifiers);
	ShapeEngine::drawTriangles(posCoords2, texCoords, 3, &modifiers);

	rx = triggers[i]->boundbox.b.x;
	ry = triggers[i]->boundbox.b.y;
	rz = triggers[i]->boundbox.a.z;

	lx = triggers[i]->boundbox.a.x;
	ly = triggers[i]->boundbox.b.y;
	lz = triggers[i]->boundbox.a.z;

	posCoords[0].x = rx;
	posCoords[0].y = -width + ry;
	posCoords[0].z = rz;

	posCoords[1].x = lx;
	posCoords[1].y = -width + ly;
	posCoords[1].z = lz;

	posCoords[2].x = lx;
	posCoords[2].y = width + ly;
	posCoords[2].z = lz;

	posCoords2[0].x = rx;
	posCoords2[0].y = -width + ry;
	posCoords2[0].z = rz;

	posCoords2[1].x = rx;
	posCoords2[1].y = width + ry;
	posCoords2[1].z = rz;

	posCoords2[2].x = lx;
	posCoords2[2].y = width + ly;
	posCoords2[2].z = lz;

	ShapeEngine::drawTriangles(posCoords, texCoords, 3, &modifiers);
	ShapeEngine::drawTriangles(posCoords2, texCoords, 3, &modifiers);

	rx = triggers[i]->boundbox.b.x;
	ry = triggers[i]->boundbox.b.y;
	rz = triggers[i]->boundbox.b.z;

	lx = triggers[i]->boundbox.b.x;
	ly = triggers[i]->boundbox.b.y;
	lz = triggers[i]->boundbox.a.z;

	posCoords[0].x = rx;
	posCoords[0].y = -width + ry;
	posCoords[0].z = rz;

	posCoords[1].x = lx;
	posCoords[1].y = -width + ly;
	posCoords[1].z = lz;

	posCoords[2].x = lx;
	posCoords[2].y = width + ly;
	posCoords[2].z = lz;

	posCoords2[0].x = rx;
	posCoords2[0].y = -width + ry;
	posCoords2[0].z = rz;

	posCoords2[1].x = rx;
	posCoords2[1].y = width + ry;
	posCoords2[1].z = rz;

	posCoords2[2].x = lx;
	posCoords2[2].y = width + ly;
	posCoords2[2].z = lz;

	ShapeEngine::drawTriangles(posCoords, texCoords, 3, &modifiers);
	ShapeEngine::drawTriangles(posCoords2, texCoords, 3, &modifiers);

	rx = triggers[i]->boundbox.b.x;
	ry = triggers[i]->boundbox.b.y;
	rz = triggers[i]->boundbox.b.z;

	lx = triggers[i]->boundbox.b.x;
	ly = triggers[i]->boundbox.b.y;
	lz = triggers[i]->boundbox.a.z;


	posCoords[0].x = rx;
	posCoords[0].y = -width + ry;
	posCoords[0].z = rz;

	posCoords[1].x = lx;
	posCoords[1].y = -width + ly;
	posCoords[1].z = lz;

	posCoords[2].x = lx;
	posCoords[2].y = width + ly;
	posCoords[2].z = lz;

	posCoords2[0].x = rx;
	posCoords2[0].y = -width + ry;
	posCoords2[0].z = rz;

	posCoords2[1].x = rx;
	posCoords2[1].y = width + ry;
	posCoords2[1].z = rz;

	posCoords2[2].x = lx;
	posCoords2[2].y = width + ly;
	posCoords2[2].z = lz;

	ShapeEngine::drawTriangles(posCoords, texCoords, 3, &modifiers);
	ShapeEngine::drawTriangles(posCoords2, texCoords, 3, &modifiers);







	// up down lines

	rx = triggers[i]->boundbox.b.x;
	ry = triggers[i]->boundbox.b.y;
	rz = triggers[i]->boundbox.a.z;

	lx = triggers[i]->boundbox.b.x;
	ly = triggers[i]->boundbox.a.y;
	lz = triggers[i]->boundbox.a.z;

	posCoords[0].x = -width + rx;
	posCoords[0].y = ry;
	posCoords[0].z = rz;

	posCoords[1].x = -width + lx;
	posCoords[1].y = ly;
	posCoords[1].z = lz;

	posCoords[2].x = width + lx;
	posCoords[2].y = ly;
	posCoords[2].z = lz;

	posCoords2[0].x = rx - width;
	posCoords2[0].y = ry;
	posCoords2[0].z = rz;

	posCoords2[1].x = rx + width;
	posCoords2[1].y = ry;
	posCoords2[1].z = rz;

	posCoords2[2].x = lx + width;
	posCoords2[2].y = ly;
	posCoords2[2].z = lz;

	ShapeEngine::drawTriangles(posCoords, texCoords, 3, &modifiers);
	ShapeEngine::drawTriangles(posCoords2, texCoords, 3, &modifiers);




	rx = triggers[i]->boundbox.a.x;
	ry = triggers[i]->boundbox.b.y;
	rz = triggers[i]->boundbox.a.z;

	lx = triggers[i]->boundbox.a.x;
	ly = triggers[i]->boundbox.a.y;
	lz = triggers[i]->boundbox.a.z;

	posCoords[0].x = -width + rx;
	posCoords[0].y = ry;
	posCoords[0].z = rz;

	posCoords[1].x = -width + lx;
	posCoords[1].y = ly;
	posCoords[1].z = lz;

	posCoords[2].x = width + lx;
	posCoords[2].y = ly;
	posCoords[2].z = lz;

	posCoords2[0].x = rx - width;
	posCoords2[0].y = ry;
	posCoords2[0].z = rz;

	posCoords2[1].x = rx + width;
	posCoords2[1].y = ry;
	posCoords2[1].z = rz;

	posCoords2[2].x = lx + width;
	posCoords2[2].y = ly;
	posCoords2[2].z = lz;

	ShapeEngine::drawTriangles(posCoords, texCoords, 3, &modifiers);
	ShapeEngine::drawTriangles(posCoords2, texCoords, 3, &modifiers);





	rx = triggers[i]->boundbox.b.x;
	ry = triggers[i]->boundbox.b.y;
	rz = triggers[i]->boundbox.b.z;

	lx = triggers[i]->boundbox.b.x;
	ly = triggers[i]->boundbox.a.y;
	lz = triggers[i]->boundbox.b.z;

	posCoords[0].x = -width + rx;
	posCoords[0].y = ry;
	posCoords[0].z = rz;

	posCoords[1].x = -width + lx;
	posCoords[1].y = ly;
	posCoords[1].z = lz;

	posCoords[2].x = width + lx;
	posCoords[2].y = ly;
	posCoords[2].z = lz;

	posCoords2[0].x = rx - width;
	posCoords2[0].y = ry;
	posCoords2[0].z = rz;

	posCoords2[1].x = rx + width;
	posCoords2[1].y = ry;
	posCoords2[1].z = rz;

	posCoords2[2].x = lx + width;
	posCoords2[2].y = ly;
	posCoords2[2].z = lz;

	ShapeEngine::drawTriangles(posCoords, texCoords, 3, &modifiers);
	ShapeEngine::drawTriangles(posCoords2, texCoords, 3, &modifiers);




	rx = triggers[i]->boundbox.a.x;
	ry = triggers[i]->boundbox.a.y;
	rz = triggers[i]->boundbox.b.z;

	lx = triggers[i]->boundbox.a.x;
	ly = triggers[i]->boundbox.b.y;
	lz = triggers[i]->boundbox.b.z;

	posCoords[0].x = -width + rx;
	posCoords[0].y = ry;
	posCoords[0].z = rz;

	posCoords[1].x = -width + lx;
	posCoords[1].y = ly;
	posCoords[1].z = lz;

	posCoords[2].x = width + lx;
	posCoords[2].y = ly;
	posCoords[2].z = lz;

	posCoords2[0].x = rx - width;
	posCoords2[0].y = ry;
	posCoords2[0].z = rz;

	posCoords2[1].x = rx + width;
	posCoords2[1].y = ry;
	posCoords2[1].z = rz;

	posCoords2[2].x = lx + width;
	posCoords2[2].y = ly;
	posCoords2[2].z = lz;

	ShapeEngine::drawTriangles(posCoords, texCoords, 3, &modifiers);
	ShapeEngine::drawTriangles(posCoords2, texCoords, 3, &modifiers);
	}
}