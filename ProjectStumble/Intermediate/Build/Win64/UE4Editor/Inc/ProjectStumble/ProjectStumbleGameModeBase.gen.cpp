// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "ProjectStumble/ProjectStumbleGameModeBase.h"
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4883)
#endif
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeProjectStumbleGameModeBase() {}
// Cross Module References
	PROJECTSTUMBLE_API UClass* Z_Construct_UClass_AProjectStumbleGameModeBase_NoRegister();
	PROJECTSTUMBLE_API UClass* Z_Construct_UClass_AProjectStumbleGameModeBase();
	ENGINE_API UClass* Z_Construct_UClass_AGameModeBase();
	UPackage* Z_Construct_UPackage__Script_ProjectStumble();
// End Cross Module References
	void AProjectStumbleGameModeBase::StaticRegisterNativesAProjectStumbleGameModeBase()
	{
	}
	UClass* Z_Construct_UClass_AProjectStumbleGameModeBase_NoRegister()
	{
		return AProjectStumbleGameModeBase::StaticClass();
	}
	struct Z_Construct_UClass_AProjectStumbleGameModeBase_Statics
	{
		static UObject* (*const DependentSingletons[])();
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UE4CodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_AProjectStumbleGameModeBase_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_AGameModeBase,
		(UObject* (*)())Z_Construct_UPackage__Script_ProjectStumble,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_AProjectStumbleGameModeBase_Statics::Class_MetaDataParams[] = {
		{ "Comment", "/**\n * \n */" },
		{ "HideCategories", "Info Rendering MovementReplication Replication Actor Input Movement Collision Rendering Utilities|Transformation" },
		{ "IncludePath", "ProjectStumbleGameModeBase.h" },
		{ "ModuleRelativePath", "ProjectStumbleGameModeBase.h" },
		{ "ShowCategories", "Input|MouseInput Input|TouchInput" },
	};
#endif
	const FCppClassTypeInfoStatic Z_Construct_UClass_AProjectStumbleGameModeBase_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<AProjectStumbleGameModeBase>::IsAbstract,
	};
	const UE4CodeGen_Private::FClassParams Z_Construct_UClass_AProjectStumbleGameModeBase_Statics::ClassParams = {
		&AProjectStumbleGameModeBase::StaticClass,
		"Game",
		&StaticCppClassTypeInfo,
		DependentSingletons,
		nullptr,
		nullptr,
		nullptr,
		UE_ARRAY_COUNT(DependentSingletons),
		0,
		0,
		0,
		0x009002ACu,
		METADATA_PARAMS(Z_Construct_UClass_AProjectStumbleGameModeBase_Statics::Class_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UClass_AProjectStumbleGameModeBase_Statics::Class_MetaDataParams))
	};
	UClass* Z_Construct_UClass_AProjectStumbleGameModeBase()
	{
		static UClass* OuterClass = nullptr;
		if (!OuterClass)
		{
			UE4CodeGen_Private::ConstructUClass(OuterClass, Z_Construct_UClass_AProjectStumbleGameModeBase_Statics::ClassParams);
		}
		return OuterClass;
	}
	IMPLEMENT_CLASS(AProjectStumbleGameModeBase, 1695117069);
	template<> PROJECTSTUMBLE_API UClass* StaticClass<AProjectStumbleGameModeBase>()
	{
		return AProjectStumbleGameModeBase::StaticClass();
	}
	static FCompiledInDefer Z_CompiledInDefer_UClass_AProjectStumbleGameModeBase(Z_Construct_UClass_AProjectStumbleGameModeBase, &AProjectStumbleGameModeBase::StaticClass, TEXT("/Script/ProjectStumble"), TEXT("AProjectStumbleGameModeBase"), false, nullptr, nullptr, nullptr);
	DEFINE_VTABLE_PTR_HELPER_CTOR(AProjectStumbleGameModeBase);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
