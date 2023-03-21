#pragma once
#include "../Utils.h"
#include "../BaseGameData.h"

namespace net
{
	class INetworkState
	{
	public:
		virtual char* INetworkStateDtor(char a2) = 0;
		virtual void networkResize() = 0;
		virtual void ValueExtractorVirtual(void* a2) = 0;
		virtual void ValueExtractorVirtual2(void* a2) = 0;
		virtual __int64 OSP_WriteJSONTemplate(__int64* a2, char a3) = 0;
		virtual __int64 WriteTemplateBufferedMemory(__int64 a2, char a3) = 0;
		virtual __int64 WriteTemplateNoVersionsFileStreamBaseFile(void* a2, bool a3) = 0;
		virtual __int64 WriteTemplateFileStreamBaseFile(__int64 a2, char a3) = 0;
		virtual void UnregisterObjectStream(void* a2, bool a3) = 0;
		virtual void UnregisterObjectStream2(void* a2, bool a3) = 0;
		virtual void UnregisterObjectStream3(void* a2, bool a3) = 0;
		virtual __int64 OSP_ReadJSONTemplate(__int64 a2, char a3) = 0;
		virtual __int64 OSP_ReadJSONTemplate2(__int64 a2, char a3) = 0;
		virtual __int64 OSP_ReadJSONTemplate3(__int64 a2, char a3) = 0;
		virtual __int64 ReadTemplateBufferedMemory(void* a2, char a3) = 0;
		virtual __int64 ReadTemplateNoVersionsFileStreamBaseFile(void* a2, bool a3) = 0;
		virtual __int64 ReadTemplateFileStreamBaseFile(void* a2, char a3) = 0;
		virtual void UnregisterObjectStream5(void* a2, bool a3) = 0;
		virtual void UnregisterObjectStream6(void* a2, bool a3) = 0;
		virtual char* getTypeNameVirtual() = 0;
		virtual unsigned int getTypeIDVirtual() = 0;
		virtual void* getTypeInfoVirtual() = 0;
		virtual __int64 ClassDataCollector(void* a2) = 0;
		virtual __int64 ClassDataCollector2(void* a2) = 0;
		virtual void* NZP_SizeOf(void* a2) = 0;
		virtual __int64 ClientValidatorNet(void* a2) = 0;
		virtual __int64 ClientValidatorNet2(void* a2) = 0;
		virtual __int64 ClientValidatorNet3(void* a2) = 0;
		virtual __int64 NZP_RecreateDeltaUpdater() = 0;
		virtual __int64 NZP_RecreateDeltaUpdater2() = 0;
		virtual void* NZP_SizeOf2(void* a2) = 0;
		virtual __int64 NZP_DefaultDeltaUpdater() = 0;
		virtual __int64 NZP_DefaultDeltaUpdater2() = 0;
		virtual void* NZP_SizeOf3(void* a2) = 0;
		virtual __int64 NZP_StreamDeltaUpdater() = 0;
		virtual __int64 NZP_StreamDeltaUpdater2() = 0;
		virtual void* NZP_SizeOf4(void* a2) = 0;
		virtual __int64 virtualProcessNetworkVariables16() = 0;
		virtual __int64 virtualProcessNetworkVariables17() = 0;
		virtual void* virtualProcessNetworkVariables18(void* a2) = 0;
		virtual __int64 virtualProcessNetworkVariables19() = 0;
		virtual __int64 virtualProcessNetworkVariables20() = 0;
		virtual __int64 virtualProcessNetworkVariables21(__int64 a2) = 0;
		virtual __int64 virtualProcessNetworkVariables22() = 0;
		virtual __int64 virtualProcessNetworkVariables23() = 0;
		virtual void* virtualProcessNetworkVariables24(void* a2) = 0;
		virtual __int64 virtualProcessNetworkVariables25(void* a2) = 0;
		virtual __int64 virtualProcessNetworkVariables26(void* a2) = 0;
		virtual void* virtualProcessNetworkVariables27(void* a2) = 0;
		virtual __int64 virtualProcessNetworkVariables28() = 0;
		virtual __int64 virtualProcessNetworkVariables29() = 0;
		virtual __int64 virtualProcessNetworkVariables30(__int64 a2) = 0;
		virtual __int64 virtualProcessNetworkVariables31() = 0;
		virtual __int64 virtualProcessNetworkVariables32() = 0;
		virtual __int64 virtualProcessNetworkVariables33(__int64 a2) = 0;
		virtual __int64 virtualProcessNetworkVariables34() = 0;
		virtual __int64 virtualProcessNetworkVariables35() = 0;
		virtual void* virtualProcessNetworkVariables36(void* a2) = 0;
		virtual __int64 virtualProcessNetworkVariables37() = 0;
		virtual __int64 virtualProcessNetworkVariables38() = 0;
		virtual void* virtualProcessNetworkVariables39(void* a2) = 0;
		virtual __int64 virtualProcessNetworkVariables40() = 0;
		virtual __int64 virtualProcessNetworkVariables41() = 0;
		virtual __int64 virtualProcessNetworkVariables42(__int64 a2) = 0;
		virtual __int64 virtualProcessNetworkVariables43() = 0;
		virtual __int64 virtualProcessNetworkVariables44() = 0;
		virtual void* virtualProcessNetworkVariables45(void* a2) = 0;
		virtual __int64 virtualProcessNetworkVariables46() = 0;
		virtual __int64 virtualProcessNetworkVariables47() = 0;
		virtual void* virtualProcessNetworkVariables48(void* a2) = 0;

		int networkState;
		int field_0C;
	};
}

namespace r
{
	class GameUpdateObject;
	class ComponentBase;
	class GameObjectState;
	class ComponentStateBase;
	class GameUpdateObjectState;

	struct BoundBox {
		float points[6];
	};

	class GameUpdateObject
	{
	public:
		virtual void Dtor() = 0;
		virtual void updateFromState() = 0;
		virtual void getUsedObjectIndices() = 0;
		virtual void getFlowNodeTypeInfo() = 0;
		virtual void virtualProcessVariables2() = 0;
		virtual void virtualProcessVariables3() = 0;
		virtual void virtualProcessVariables4() = 0;
		virtual void virtualProcessVariables5() = 0;
		virtual void virtualProcessVariables6() = 0;
		virtual void virtualProcessVariables7() = 0;
		virtual void virtualProcessVariables8() = 0;
		virtual void virtualProcessVariables9() = 0;
		virtual void virtualProcessVariables10() = 0;
		virtual void virtualProcessVariables11() = 0;
		virtual void virtualProcessVariables12() = 0;
		virtual void virtualProcessVariables13() = 0;
		virtual void virtualProcessVariables14() = 0;
		virtual void virtualProcessVariables15() = 0;
		virtual void virtualProcessVariables16() = 0;
		virtual void virtualProcessVariables17() = 0;
		virtual void virtualProcessVariables18() = 0;
		virtual char* getTypeNameVirtual() = 0;
		virtual int64_t getTypeIDVirtual() = 0;
		virtual int64_t* getTypeInfoVirtual() = 0;

		r::ComponentStateBase* state;
		int field_10;
		char updates;
		__int8 padding[3];
		__int8 field_20;
		__int8 padding2[7];
	};

	class ComponentBase : public r::GameUpdateObject
	{
	public:
		virtual void gameLoad() = 0;
		virtual void gameUnload() = 0;
		virtual void parentInitialized() = 0;
		virtual void parentUninitialized() = 0;
		virtual void streamerSetup() = 0;
		virtual void gameSetupOnStreamerThread() = 0;
		virtual void gameSetup() = 0;
		virtual void postLoad() = 0;
		virtual void gameShutdown() = 0;
		virtual void gameShutdownOnStreamerThread() = 0;
		virtual void streamerShutdown() = 0;
	};

	class GameUpdateObjectState : public net::INetworkState
	{
	public:
		virtual void setNetworkUpdateState(char a2) = 0;
		virtual void setClientSidePrediction(char a2) = 0;
		virtual bool isLockedForStreaming() = 0;

		char networkUpdateState;
		unsigned __int64 globalID;
		r::GameUpdateObject* gameUpdateObject;
		r::ComponentStateBase* componentStateBase;
	};

	class GameObjectState : public r::GameUpdateObjectState
	{
	public:
		virtual bool hide(int a2, unsigned int a3) = 0;
		virtual void hierarchyLoad() = 0;
		virtual void gameLoad() = 0;
		virtual void gameUnload() = 0;
		virtual void load() = 0;
		virtual void save() = 0;
		virtual void preSave() = 0;
		virtual void postLoad() = 0;
		virtual void* getTransform() = 0;
		virtual void* getPosition() = 0;
		virtual float getInstantiationDistance() = 0;
		virtual void setTransform() = 0;
		virtual void getResourceDependencies(int64_t a2) = 0;
		virtual int64_t getType() = 0;
		virtual void instantiateComponentState() = 0;
		virtual void addToIterationList() = 0;
		virtual void removeFromIterationList() = 0;
		virtual char* getName() = 0;
		virtual void setDirtyNetworkState() = 0;
		virtual int64_t getCustomTags() = 0;
		virtual ptr* instantiateState(int64_t a2, uint64_t a3) = 0;
		virtual void handleHideReasonChange(uint64_t a2) = 0;

		int field_30;
		unsigned __int8 refCount;
		char field_35;
		void* contentEntityBasePtr;
		r::GameObjectState* gameObjectStatePtr;
		r::GameObjectState** ptrToSelfToGameObjectState1;
		r::GameObjectState** ptrToSelfToGameObjectState2;
		__int64* field_58;
		__int8 field_60;
		__int8 field_61;
		unsigned __int16 field_62;
		int field_64;
		r::ComponentStateBase** componentList;
		unsigned int componentCount;
		int field_74;
		char field_78;
		char padding[7];
		float distance;
		int field_84;
		__int64 field_88;
	};

	class ComponentStateBase : public r::GameUpdateObjectState
	{
	public:
		virtual void getFlowNode() = 0;
		virtual void getFlowNodeTypeInfo() = 0;
		virtual int64_t* instantiate(int64_t a2, uint64_t a3) = 0;
		virtual void uninstantiate(r::ComponentBase base) = 0;
		virtual void gameLoad() = 0;
		virtual void gameUnload() = 0;
		virtual void onOwnerInitialized() = 0;
		virtual void parentInitialized() = 0;
		virtual void parentUninitialized() = 0;
		virtual int64_t onHide() = 0;

		unsigned int isHidden;
		int field34;
		__int64* contentPtr;
		__int64* field40;
		unsigned int onEnter;
		int field4C;
		char field50;
		char field50_padding[7];
		int field58;
		int field5C;
	};
}

namespace coregame
{
	class TriggerComponentState;
	class TriggerComponent;

	class TriggerComponent : public r::ComponentBase
	{
	public:
		__int64* genericEntityState_D0;
		__int64 field28;
		__int64* field30;
		int field38;
		int field3C;
		int field40;
		int field44;
		int field48;
		int field4C;
		char tmp;
		int field54;
		int field58;
		int field5C;
		int field60;
		int field64;
		int field68;
		int field6C;
		int field70;
		int field74;
		int field78;
		int field7C;
		int field80;
		int field84;
		int field88;
		int field8C;
		__int64 field90;
		int field98;
		__int16 field9C;
		__int16 field9C_Padding;
		__int64 fieldA0;
		__int64 fieldA8;
		__int64* fieldB0;
		__int64* fieldB8;
		int fieldC0;
		int fieldC4;
		char fieldC8;
		__int64 fieldD0;
		int fieldD8;
		int fieldDC;
		char fieldE0;
		void* trigger;
		r::BoundBox boundbox;

		coregame::TriggerComponentState* ComponentBaseState()
		{
			return (coregame::TriggerComponentState*)this->state;
		}
	};

	class TriggerComponentState : public r::ComponentStateBase
	{
	public:
		virtual void load(int64_t* bufferedMemoryStream) = 0;
		virtual void save(int64_t* bufferedMemoryStream) = 0;
		virtual void variableUpdate() = 0;
		virtual void getResourceDependencies(int64_t a2) = 0;

		int networkState;
		int field_0C;
		char networkUpdateState;
		unsigned __int64 globalID;
		r::GameUpdateObject* gameUpdateObject;
		r::ComponentStateBase* componentStateBase;
		char field30;
		char field30_padding[7];
		__int64* contentPtr;
		__int64* field40;
		int field48;
		int field4C;
		char field50;
		char field50_padding[7];
		char field58;
		float field5C;
		float field60;
		float field64;
		float field68;
		char padding[4];
	};
}