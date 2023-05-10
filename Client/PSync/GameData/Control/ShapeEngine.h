#pragma once
#include <DirectXMath.h>
using namespace DirectX;

#include "../Utils.h"
#include "../BaseGameData.h"
#include "../ocular/kiero/minhook/include/MinHook.h"
#include "RemedyLib.h"

namespace rend
{
	// TODO:
	// Add enums for the arguments you pass to setDepthmode / filter / etc
	// As of now if you go out of range on some of the modes the game crashes

	namespace ShapeEngine
	{
		typedef enum
		{ //need to do more thorough investigation as to how these work
			tmp1, //unknown? doesn't show..?
			tmp2, //top left?
			tmp3, //also top left?
			tmp4, //left aligned?
			tmp5, //center aligned?
			tmp6, //right aligned?
			tmp7, //center right?
			tmp8, //lower left?
			tmp9,
		} Pivot;

		typedef uint64_t DrawMode, BlendMode, CubeSamplingMode, DepthMode, FilterMode, TextureMode;
	}
}

// Code example for drawing a triangle at the player's location
// posScale is just a temporary struct with int's inside, any draw modes variables are regular int's, no enums were defined.
/*
struct posNscale {
	Vector3 pos;
	Vector3 scale;
	int draw;
	int blend;
	int depth;
	int filter;
};
posNScale posScale = {0};

//below tested in GetViewMatrix
	DirectX::XMFLOAT4X3 modifiers = DirectX::XMFLOAT4X3();
	DirectX::XMMATRIX tmp = DirectX::XMMatrixIdentity();
	DirectX::XMVECTOR empty = DirectX::XMVECTOR();

	DirectX::XMVECTOR scale;
	DirectX::XMVECTOR translation;
	DirectX::XMVECTOR rotation;

	DirectX::XMMatrixDecompose(&scale, &rotation, &translation, tmp);

	translation.m128_f32[0] = GetPlayerPos()->x + posScale.pos.x;
	translation.m128_f32[1] = GetPlayerPos()->y + posScale.pos.y;
	translation.m128_f32[2] = GetPlayerPos()->z + posScale.pos.z;

	translation.m128_f32[0] = posScale.pos.x;
	translation.m128_f32[1] = posScale.pos.y;
	translation.m128_f32[2] = posScale.pos.z;

	scale.m128_f32[0] = posScale.scale.x;
	scale.m128_f32[1] = posScale.scale.y;
	scale.m128_f32[2] = posScale.scale.z;

	DirectX::XMStoreFloat4x3(&modifiers, DirectX::XMMatrixAffineTransformation(scale, empty, rotation, translation));

	Vector3 texCoords[3] = { { 0.1f, 0.1f, 0.0f }, { 0.1f, 0.1f, 0.0f }, { 0.1f, 0.1f, 0.0f } };
	Vector3 posCoords[3] = { {-0.5f, -0.5f, 0.0f}, {0.5f, -0.5f, 0.0f}, {0.0f, 0.5f, 0.0f} };
	Vector4 colors = { 1.0f, 0.0f, 1.0f, 0.5f };
	viewMatrix = *reinterpret_cast<Matrix4*>(ptrViewMatrixAddr);

	ShapeEngine::setTransform(ShapeEngine::getInstance(), viewMatrix);
	//ShapeEngine::setViewportTransform(ShapeEngine::getInstance());
	ShapeEngine::setColor(ShapeEngine::getInstance(), &colors);

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
	ShapeEngine::drawTriangles(ShapeEngine::getInstance(), posCoords, texCoords, 3, &modifiers);
*/

// How to do rotation manupulation with the directx math library
/*
	// for rotation
	auto rotationResultQuaternion = DirectX::XMQuaternionRotationAxis({ 0.0f, 1.0f, 0.0f, 0.0f }, DirectX::XMConvertToRadians((float)GetTickCount64()));
	auto result = DirectX::XMQuaternionMultiply(rotation, rotationResultQuaternion);
*/

namespace ShapeEngine
{
	extern void* rendFontPtr;

	using clearBuffers_t = void(__fastcall*)(void* ShapeEngineInstance);
	using createGlobalResources_t = void(__fastcall*)(void* ShapeEngineInstance);
	using deleteGlobalResources_t = void(__fastcall*)(void* ShapeEngineInstance);
	using drawCircle_t = void(__fastcall*)(void* ShapeEngineInstance, Vector2 a1, float a2, float a3, float a4, float a5, Vector2 a6);
	using drawRectangle_t = void(__fastcall*)(void* ShapeEngineInstance, Vector2 a1, Vector2 a2);
	using drawSpriteWithFilmGrain_t = void(__fastcall*)(void* ShapeEngineInstance, Vector2 a1, Vector2 a2, rend::ShapeEngine::Pivot pivot, float a4, Vector3* a5, Vector3 a6, Vector3 a7);
	using drawUser_t = void(__fastcall*)(void* ShapeEngineInstance, void* a1);
	using empty_t = bool(__fastcall*)(void* ShapeEngineInstance);
	using freeRenderTarget_t = void(__fastcall*)(void* ShapeEngineInstance, void* d3dTexture2D);
	using getAspectRatio_t = float(__fastcall*)(void* ShapeEngineInstance);
	using getBlendMode_t = int64_t /*enum*/(__fastcall*)(void* ShapeEngineInstance);
	using getColor_t = uint64_t(__fastcall*)(void* ShapeEngineInstance);
	using getDepthMode_t = int64_t /*enum*/(__fastcall*)(void* ShapeEngineInstance);
	using getDrawMode_t = int64_t /*enum*/(__fastcall*)(void* ShapeEngineInstance);
	using getDropShadow_t = bool(__fastcall*)(void* ShapeEngineInstance);
	using getFilterMode_t = int64_t /*enum*/(__fastcall*)(void* ShapeEngineInstance);
	using getFont_t = void*(__fastcall*)(void* ShapeEngineInstance);
	using getInstance_t = void* (__fastcall*)();
	using getTextAreaLineHeight_t = float(__fastcall*)(void* ShapeEngineInstance, void* a1, float a2);
	using getTexture_t = void*(__fastcall*)(void* ShapeEngineInstance);
	using getTextureMode_t = int64_t /*enum*/(__fastcall*)(void* ShapeEngineInstance);
	using hasInstance_t = bool(__fastcall*)(void* ShapeEngineInstance);
	using isRendererAdministered_t = bool(__fastcall*)(void* ShapeEngineInstance, r::ThreadID a1);
	using postEffectInit_t = void(__fastcall*)(void* ShapeEngineInstance);
	using pushToRenderer_t = void(__fastcall*)(void* ShapeEngineInstance, r::ThreadID a1);
	using registerGlobalInstance_t = void(__fastcall*)(void* ShapeEngineInstance);
	using render_t = void(__fastcall*)(void* ShapeEngineInstance, rend::ShapeEngine::DrawMode a1);
	using setAspectRatio_t = void(__fastcall*)(void* ShapeEngineInstance, float a1);
	using setBlendMode_t = void(__fastcall*)(void* ShapeEngineInstance, rend::ShapeEngine::BlendMode a1);
	using setCubeSamplingMode_t = void(__fastcall*)(void* ShapeEngineInstance, rend::ShapeEngine::CubeSamplingMode a1);
	using setDepthMode_t = void(__fastcall*)(void* ShapeEngineInstance, rend::ShapeEngine::DepthMode a1);
	using setDrawMode_t = void(__fastcall*)(void* ShapeEngineInstance, rend::ShapeEngine::DrawMode a1);
	using setDropShadow_t = void(__fastcall*)(void* ShapeEngineInstance, bool a1);
	using setFilterMode_t = void(__fastcall*)(void* ShapeEngineInstance, rend::ShapeEngine::FilterMode a1);
	using setFont_t = void(__fastcall*)(void* ShapeEngineInstance, void* a2);
	using setPointSize_t = void(__fastcall*)(void* ShapeEngineInstance, float size);
	using setRendererAdministered_t = void(__fastcall*)(void* ShapeEngineInstance, r::ThreadID a1, bool a2);
	using setTextSize_t = void(__fastcall*)(void* ShapeEngineInstance, float size);
	using setTexture_t = void(__fastcall*)(void* ShapeEngineInstance, void* d3dBaseTexture);
	using setTextureMode_t = void(__fastcall*)(void* ShapeEngineInstance, rend::ShapeEngine::TextureMode a1);
	using setTransform_t = void(__fastcall*)(void* ShapeEngineInstance, Matrix4 matrix);
	using setViewportTransform_t = void(__fastcall*)(void* ShapeEngineInstance);
	using unregisterGlobalInstance_t = void(__fastcall*)(void* ShapeEngineInstance);

	extern clearBuffers_t clearBuffers;
	extern createGlobalResources_t createGlobalResources;
	extern deleteGlobalResources_t deleteGlobalResources;
	extern drawCircle_t drawCircle;
	extern drawRectangle_t drawRectangle;
	extern drawSpriteWithFilmGrain_t drawSpriteWithFilmGrain;
	extern drawUser_t drawUser;
	extern empty_t empty;
	extern freeRenderTarget_t freeRenderTarget;
	extern getAspectRatio_t getAspectRatio;
	extern getBlendMode_t getBlendMode;
	extern getColor_t getColor;
	extern getDepthMode_t getDepthMode;
	extern getDrawMode_t getDrawMode;
	extern getDropShadow_t getDropShadow;
	extern getFilterMode_t getFilterMode;
	extern getFont_t getFont;
	extern getInstance_t getInstance;
	extern getTextAreaLineHeight_t getTextAreaLineHeight;
	extern getTexture_t getTexture;
	extern getTextureMode_t getTextureMode;
	extern hasInstance_t hasInstance;
	extern isRendererAdministered_t isRendererAdministered;
	extern postEffectInit_t postEffectInit;
	extern pushToRenderer_t pushToRenderer;
	extern registerGlobalInstance_t registerGlobalInstance;
	extern render_t render;
	extern setAspectRatio_t setAspectRatio;
	extern setBlendMode_t setBlendMode;
	extern setCubeSamplingMode_t setCubeSamplingMode;
	extern setDepthMode_t setDepthMode;
	extern setDrawMode_t setDrawMode;
	extern setDropShadow_t setDropShadow;
	extern setFilterMode_t setFilterMode;
	extern setFont_t setFont;
	extern setPointSize_t setPointSize;
	extern setRendererAdministered_t setRendererAdministered;
	extern setTextSize_t setTextSize;
	extern setTexture_t setTexture;
	extern setTextureMode_t setTextureMode;
	extern setTransform_t setTransform;
	extern setViewportTransform_t setViewportTransform;
	extern unregisterGlobalInstance_t unregisterGlobalInstance;

	using ShapeEngine_t = void(__fastcall*)(void* ShapeEngineInstance, unsigned int a1, unsigned int a2);
	void ShapeEngine(unsigned int a1, unsigned int a2);

	using ShapeEngine1_t = void(__fastcall*)(void* ShapeEngineInstance);
	void ShapeEngine();

	using drawInstancedLines_t = void(__fastcall*)(void* ShapeEngineInstance, unsigned __int64 a1, Vector2* a2, int a3, XMFLOAT4X3 a4, Vector4 a5);
	void drawInstancedLines(unsigned __int64 a1, Vector2* a2, int a3, XMFLOAT4X3 a4, Vector4 a5);

	using drawInstancedLines1_t = void(__fastcall*)(void* ShapeEngineInstance, unsigned __int64 a1, Vector3* a2, int a3, XMFLOAT4X3 a4, Vector4 a5);
	void drawInstancedLines(unsigned __int64 a1, Vector3* a2, int a3, XMFLOAT4X3 a4, Vector4 a5);

	using drawInstancedTriangles_t = void(__fastcall*)(void* ShapeEngineInstance, unsigned __int64 a1, Vector2* a2, int a3, XMFLOAT4X3 a4, Vector4 a5);
	void drawInstancedTriangles(unsigned __int64 a1, Vector2* a2, int a3, XMFLOAT4X3 a4, Vector4 a5);

	using drawInstancedTriangles1_t = void(__fastcall*)(void* ShapeEngineInstance, unsigned __int64 a1, Vector3* a2, int a3, XMFLOAT4X3 a4, Vector4 a5);
	void drawInstancedTriangles(unsigned __int64 a1, Vector3* a2, int a3, XMFLOAT4X3 a4, Vector4 a5);

	using drawLine_t = void(__fastcall*)(void* ShapeEngineInstance, Vector2* start, Vector2* end);
	void drawLine(Vector2* start, Vector2* end);

	using drawLine1_t = void(__fastcall*)(void* ShapeEngineInstance, Vector3* start, Vector3* end);
	void drawLine(Vector3* start, Vector3* end);

	using drawLine2_t = void(__fastcall*)(void* ShapeEngineInstance, Vector4* start, Vector4* end);
	void drawLine(Vector4* start, Vector4* end);

	using drawLines_t = void(__fastcall*)(void* ShapeEngineInstance, Vector2* lineData, int lineCount, XMFLOAT4X3* matrix);
	void drawLines(Vector2* lineData, int lineCount, XMFLOAT4X3* matrix);

	using drawLines1_t = void(__fastcall*)(void* ShapeEngineInstance, Vector3* lineData, int lineCount, XMFLOAT4X3* matrix);
	void drawLines(Vector3* lineData, int lineCount, XMFLOAT4X3* matrix);

	using drawPoint_t = void(__fastcall*)(void* ShapeEngineInstance, Vector2* point);
	void drawPoint(Vector2* point);

	using drawPoint1_t = void(__fastcall*)(void* ShapeEngineInstance, Vector3* point);
	void drawPoint(Vector3* point);

	using drawPolygon_t = void(__fastcall*)(void* ShapeEngineInstance, Vector2* a1, int a2, Vector3* a3, uint64_t* a4);
	void drawPolygon(Vector2* a1, int a2, Vector3* a3, uint64_t* a4);

	using drawPolygon1_t = void(__fastcall*)(void* ShapeEngineInstance, Vector3* a1, int a2, Vector3* a3, uint64_t* a4);
	void drawPolygon(Vector3* a1, int a2, Vector3* a3, uint64_t* a4);

	using drawPolygon2_t = void(__fastcall*)(void* ShapeEngineInstance, Vector4* a1, int a2, Vector3* a3, uint64_t* a4);
	void drawPolygon(Vector4* a1, int a2, Vector3* a3, uint64_t* a4);

	using drawSprite_t = void(__fastcall*)(void* ShapeEngineInstance, Vector2 a1, Vector2 a2, rend::ShapeEngine::Pivot pivot, float a4, Vector3* a5, Vector3 a6, Vector3 a7);
	void drawSprite(Vector2 a1, Vector2 a2, rend::ShapeEngine::Pivot pivot, float a4, Vector3* a5, Vector3 a6, Vector3 a7);

	using drawSprite1_t = void(__fastcall*)(void* ShapeEngineInstance, Vector3 a1, Vector2 a2, Vector3 a3, rend::ShapeEngine::Pivot pivot, float a5, Vector3* a6, Vector3 a7, Vector3 a8);
	void drawSprite(Vector3 a1, Vector2 a2, Vector3 a3, rend::ShapeEngine::Pivot pivot, float a5, Vector3* a6, Vector3 a7, Vector3 a8);

	using drawText_t = void(__fastcall*)(void *ShapeEngineInstance, Vector2 *pos, const char* str, rend::ShapeEngine::Pivot pivot);
	void drawText(Vector2 *pos, const char* str, rend::ShapeEngine::Pivot pivot);

	using drawTriangles_t = void(__fastcall*)(void* ShapeEngineInstance, Vector2* posCoords, Vector3* texCoords, int pointCount, XMFLOAT4X3* matrix);
	void drawTriangles(Vector2* posCoords, Vector3* texCoords, int pointCount, XMFLOAT4X3* matrix);

	using drawTriangles1_t = void(__fastcall*)(void* ShapeEngineInstance, Vector3* posCoords, Vector3* texCoords, int pointCount, XMFLOAT4X3* matrix);
	void drawTriangles(Vector3* posCoords, Vector3* texCoords, int pointCount, XMFLOAT4X3* matrix);

	using getTextLineHeight_t = float(__fastcall*)(void* ShapeEngineInstance);
	float getTextLineHeight();
	
	using getTextLineHeight1_t = float(__fastcall*)(void* ShapeEngineInstance, void* a1, float a2);
	float getTextLineHeight(void* a1, float a2);

	using getTextSize_t = Vector2(__fastcall*)(void* ShapeEngineInstance, char* str);
	Vector2 getTextSize(char* str);

	using getTextSize2_t = float(__fastcall*)(void* ShapeEngineInstance);
	float getTextSize();

	using getTextSize3_t = void* (__fastcall*)(void* ShapeEngineInstance, void* rendFont, float a2, float a3, char* str);
	void* getTextSize(void* a1, float a2, float a3, char* str);

	using setColor_t = void(__fastcall*)(void* ShapeEngineInstance, Vector4* color);
	void setColor(Vector4* color);

	using setColor1_t = void(__fastcall*)(void* ShapeEngineInstance, uint64_t icolor);
	void setColor(uint64_t icolor);

	using setScissorRect_t = void(__fastcall*)(void* ShapeEngineInstance, Vector2 a1, Vector2 a2);
	void setScissorRect(Vector2 a1, Vector2 a2);

	using setScissorRect1_t = void(__fastcall*)(void* ShapeEngineInstance, int a1, int a2, int a3, int a4);
	void setScissorRect(int a1, int a2, int a3, int a4);

	using setStroke_t = void(__fastcall*)(void* ShapeEngineInstance, bool a1, float a2, Vector4* a3);
	void setStroke(bool a1, float a2, Vector4* a3);

	using setStroke1_t = void(__fastcall*)(void* ShapeEngineInstance, bool a1, float a2, uint64_t* a3);
	void setStroke(bool a1, float a2, uint64_t* a3);

	void InstallHooks(LPCWSTR dllName);
}