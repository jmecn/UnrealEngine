// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.


#include "AbilitySystemEditorPrivatePCH.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetArrayLibrary.h"
#include "GameplayTask.h"
#include "AbilityTask.h"
#include "KismetCompiler.h"
#include "BlueprintEditorUtils.h"
#include "K2Node_LatentAbilityCall.h"
#include "GameplayAbilityGraphSchema.h"
#include "K2Node_EnumLiteral.h"
#include "BlueprintFunctionNodeSpawner.h"
#include "BlueprintActionDatabaseRegistrar.h"

#define LOCTEXT_NAMESPACE "K2Node"

UK2Node_LatentAbilityCall::UK2Node_LatentAbilityCall(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	if (HasAnyFlags(RF_ClassDefaultObject) == true)
	{
		UK2Node_LatentGameplayTaskCall::RegisterSpecializedTaskNodeClass(GetClass());
	}
}

bool UK2Node_LatentAbilityCall::IsHandling(TSubclassOf<UGameplayTask> TaskClass) const
{
	return TaskClass && TaskClass->IsChildOf(UAbilityTask::StaticClass());
}

bool UK2Node_LatentAbilityCall::IsCompatibleWithGraph(UEdGraph const* TargetGraph) const
{
	bool bIsCompatible = false;
	
	EGraphType GraphType = TargetGraph->GetSchema()->GetGraphType(TargetGraph);
	bool const bAllowLatentFuncs = (GraphType == GT_Ubergraph || GraphType == GT_Macro);
	
	if (bAllowLatentFuncs)
	{
		UBlueprint* MyBlueprint = FBlueprintEditorUtils::FindBlueprintForGraph(TargetGraph);
		if (MyBlueprint && MyBlueprint->GeneratedClass)
		{
			if (MyBlueprint->GeneratedClass->IsChildOf(UGameplayAbility::StaticClass()))
			{
				bIsCompatible = true;
			}
		}
	}
	
	return bIsCompatible;
}

void UK2Node_LatentAbilityCall::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	// these nested loops are combing over the same classes/functions the
	// FBlueprintActionDatabase does; ideally we save on perf and fold this in
	// with FBlueprintActionDatabase, but we want to keep the modules separate
	for (TObjectIterator<UClass> ClassIt; ClassIt; ++ClassIt)
	{
		UClass* Class = *ClassIt;
		if (!Class->IsChildOf<UAbilityTask>() || Class->HasAnyClassFlags(CLASS_Abstract))
		{
			continue;
		}
		
		for (TFieldIterator<UFunction> FuncIt(Class, EFieldIteratorFlags::ExcludeSuper); FuncIt; ++FuncIt)
		{
			UFunction* Function = *FuncIt;
			if (!Function->HasAnyFunctionFlags(FUNC_Static))
			{
				continue;
			}

			// to keep from needlessly instantiating a UBlueprintNodeSpawner, first   
			// check to make sure that the registrar is looking for actions of this type
			// (could be regenerating actions for a specific asset, and therefore the 
			// registrar would only accept actions corresponding to that asset)
			if (!ActionRegistrar.IsOpenForRegistration(Function))
			{
				continue;
			}

			UObjectProperty* ReturnProperty = Cast<UObjectProperty>(Function->GetReturnProperty());
			// see if the function is a static factory method for online proxies
			bool const bIsProxyFactoryMethod = (ReturnProperty != nullptr) && ReturnProperty->PropertyClass->IsChildOf<UAbilityTask>();
			
			if (bIsProxyFactoryMethod)
			{
				UBlueprintNodeSpawner* NodeSpawner = UBlueprintFunctionNodeSpawner::Create(Function);
				check(NodeSpawner != nullptr);
				NodeSpawner->NodeClass = GetClass();
				
				auto CustomizeAcyncNodeLambda = [](UEdGraphNode* NewNode, bool bIsTemplateNode, TWeakObjectPtr<UFunction> FunctionPtr)
				{
					UK2Node_LatentAbilityCall* AsyncTaskNode = CastChecked<UK2Node_LatentAbilityCall>(NewNode);
					if (FunctionPtr.IsValid())
					{
						UFunction* Func = FunctionPtr.Get();
						UObjectProperty* ReturnProp = CastChecked<UObjectProperty>(Func->GetReturnProperty());
						
						AsyncTaskNode->ProxyFactoryFunctionName = Func->GetFName();
						AsyncTaskNode->ProxyFactoryClass        = Func->GetOuterUClass();
						AsyncTaskNode->ProxyClass               = ReturnProp->PropertyClass;
					}
				};
				
				TWeakObjectPtr<UFunction> FunctionPtr = Function;
				NodeSpawner->CustomizeNodeDelegate = UBlueprintNodeSpawner::FCustomizeNodeDelegate::CreateStatic(CustomizeAcyncNodeLambda, FunctionPtr);
				
				// @TODO: since this can't be folded into FBlueprintActionDatabase, we
				//        need a way to associate these spawners with a certain class
				ActionRegistrar.AddBlueprintAction(Function, NodeSpawner);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE
