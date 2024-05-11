// Copyright Epic Games, Inc. All Rights Reserved.

#include "GAMHUD.h"
#include "Engine/Canvas.h"
#include "Engine/Texture2D.h"
#include "TextureResource.h"
#include "CanvasItem.h"
#include "UObject/ConstructorHelpers.h"
#include "Blueprint/UserWidget.h"

// This is an overview of how event-based user interface (UI) systems and heads-up displays (HUDs) have been
// employed to create the graphical user interface (GUI):

//    The constructor initializes a crosshair texture used for aiming. In the DrawHUD function, the crosshair is drawn at
//    the center of the screen using the crosshair texture.

//    In the constructor, a WidgetBlueprint is found using the ConstructorHelpers class, providing access to the UI elements.
//    The HeatUI WidgetBlueprint is assigned to the HUDWidgetClass variable, which will be used to create the HUD
//    widget instance.

//    In the BeginPlay function, if the HUDWidgetClass is valid, a widget instance is created using the CreateWidget function.
//    The created widget instance is then added to the viewport, ensuring it is visible to players during gameplay.

//    The GUI interaction in the code is event-based. The HUDWidgetClass is found and assigned in the constructor
//	  The UI creation and display are events that happen based on game progression.

//    The widget (HeatUI) is created and displayed as part of the UI, allowing players to see the timer and the gun's heat level.

//    The event-driven approach allows for dynamic GUI interaction. Widgets can be created, displayed, and updated based on
//    events, such as gameplay state changes, player actions, or object interactions.


AGAMHUD::AGAMHUD()
{
	// Set the crosshair texture
	static ConstructorHelpers::FObjectFinder<UTexture2D> CrosshairTexObj(TEXT("/Game/FirstPerson/Textures/FirstPersonCrosshair"));
	CrosshairTex = CrosshairTexObj.Object;

	// Find WidgetBlueprint and execute
	static ConstructorHelpers::FClassFinder<UUserWidget> HeatBarObj(TEXT("/Game/UI/HeatUI"));

	HUDWidgetClass = HeatBarObj.Class;
}


void AGAMHUD::DrawHUD()
{
	Super::DrawHUD();

	// Draw very simple crosshair

	// find center of the Canvas
	const FVector2D Center(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);

	// offset by half the texture's dimensions so that the center of the texture aligns with the center of the Canvas
	const FVector2D CrosshairDrawPosition( (Center.X),
										   (Center.Y + 20.0f));

	// draw the crosshair
	FCanvasTileItem TileItem( CrosshairDrawPosition, CrosshairTex->Resource, FLinearColor::White);
	TileItem.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem( TileItem );
}

// On Begin, Create Widget and add to Viewport
void AGAMHUD::BeginPlay()
{
	Super::BeginPlay();

	if (HUDWidgetClass != nullptr)
	{
		CurrentWidget = CreateWidget<UUserWidget>(GetWorld(), HUDWidgetClass);

		if (CurrentWidget)
		{
			CurrentWidget->AddToViewport();
		}
	}
}