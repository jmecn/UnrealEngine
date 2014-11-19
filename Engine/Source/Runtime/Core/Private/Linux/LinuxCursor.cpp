// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "CorePrivatePCH.h"
#include "LinuxApplication.h"
#include "LinuxCursor.h"
#include "LinuxWindow.h"

FLinuxCursor::FLinuxCursor()
	: 	bHidden(false)
	,	AccumulatedOffsetX(0)
	,	AccumulatedOffsetY(0)
{
	if (!FPlatformMisc::PlatformInitMultimedia()) //	will not initialize more than once
	{
		UE_LOG(LogInit, Fatal, TEXT("FLinuxCursor::FLinuxCursor() : PlatformInitMultimedia() failed, cannot construct cursor."));
		// unreachable
		return;
	}

#if DO_CHECK
	uint32 InitializedSubsystems = SDL_WasInit(SDL_INIT_EVERYTHING);
	check(InitializedSubsystems & SDL_INIT_VIDEO);
#endif // DO_CHECK

	// Load up cursors that we'll be using
	for( int32 CurCursorIndex = 0; CurCursorIndex < EMouseCursor::TotalCursorCount; ++CurCursorIndex )
	{
		CursorHandles[ CurCursorIndex ] = NULL;

		SDL_HCursor CursorHandle = NULL;
		switch( CurCursorIndex )
		{
		case EMouseCursor::None:
			// The mouse cursor will not be visible when None is used
			break;

		case EMouseCursor::Default:
			CursorHandle = SDL_CreateSystemCursor( SDL_SYSTEM_CURSOR_ARROW );
			break;

		case EMouseCursor::TextEditBeam:
			CursorHandle = SDL_CreateSystemCursor( SDL_SYSTEM_CURSOR_IBEAM );
			break;

		case EMouseCursor::ResizeLeftRight:
			CursorHandle = SDL_CreateSystemCursor( SDL_SYSTEM_CURSOR_SIZEWE );
			break;

		case EMouseCursor::ResizeUpDown:
			CursorHandle = SDL_CreateSystemCursor( SDL_SYSTEM_CURSOR_SIZENS );
			break;

		case EMouseCursor::ResizeSouthEast:
			CursorHandle = SDL_CreateSystemCursor( SDL_SYSTEM_CURSOR_SIZENWSE );
			break;

		case EMouseCursor::ResizeSouthWest:
			CursorHandle = SDL_CreateSystemCursor( SDL_SYSTEM_CURSOR_SIZENESW );
			break;

		case EMouseCursor::CardinalCross:
			CursorHandle = SDL_CreateSystemCursor( SDL_SYSTEM_CURSOR_SIZEALL );
			break;

		case EMouseCursor::Crosshairs:
			CursorHandle = SDL_CreateSystemCursor( SDL_SYSTEM_CURSOR_CROSSHAIR );
			break;

		case EMouseCursor::Hand:
			CursorHandle = SDL_CreateSystemCursor( SDL_SYSTEM_CURSOR_HAND );
			break;

		case EMouseCursor::GrabHand:
			//CursorHandle = LoadCursorFromFile((LPCTSTR)*(FString( FPlatformProcess::BaseDir() ) / FString::Printf( TEXT("%sEditor/Slate/Old/grabhand.cur"), *FPaths::EngineContentDir() )));
			//if (CursorHandle == NULL)
			//{
			//	// Failed to load file, fall back
				CursorHandle = SDL_CreateSystemCursor( SDL_SYSTEM_CURSOR_HAND );
			//}
			break;

		case EMouseCursor::GrabHandClosed:
			//CursorHandle = LoadCursorFromFile((LPCTSTR)*(FString( FPlatformProcess::BaseDir() ) / FString::Printf( TEXT("%sEditor/Slate/Old/grabhand_closed.cur"), *FPaths::EngineContentDir() )));
			//if (CursorHandle == NULL)
			//{
			//	// Failed to load file, fall back
				CursorHandle = SDL_CreateSystemCursor( SDL_SYSTEM_CURSOR_HAND );
			//}
			break;

		case EMouseCursor::SlashedCircle:
			CursorHandle = SDL_CreateSystemCursor( SDL_SYSTEM_CURSOR_NO );
			break;

		case EMouseCursor::EyeDropper:
			{
				Uint8 mask[32]={0x00, 0x07, 0x00, 0x0f, 0x00, 0x5f, 0x00, 0xfe,
								0x01, 0xfc, 0x00, 0xf8, 0x01, 0xfc, 0x02, 0xf8,
								0x07, 0xd0, 0x0f, 0x80, 0x1f, 0x00, 0x3e, 0x00,
								0x7c, 0x00, 0x78, 0x00, 0xf0, 0x00, 0x40, 0x00};

				Uint8 data[32]={0x00, 0x07, 0x00, 0x0b, 0x00, 0x53, 0x00, 0xa6,
								0x01, 0x0c, 0x00, 0xf8, 0x01, 0x7c, 0x02, 0x38,
								0x04, 0x50, 0x08, 0x80, 0x11, 0x00, 0x22, 0x00,
								0x44, 0x00, 0x48, 0x00, 0xb0, 0x00, 0x40, 0x00};

				CursorHandle = SDL_CreateCursor(data, mask, 16, 16, 0, 15);
			}
			break;

			// NOTE: For custom app cursors, use:
			//		CursorHandle = ::LoadCursor( InstanceHandle, (LPCWSTR)MY_RESOURCE_ID );

		default:
			// Unrecognized cursor type!
			check( 0 );
			break;
		}

		CursorHandles[ CurCursorIndex ] = CursorHandle;
	}

	// Set the default cursor
	SetType( EMouseCursor::Default );
}

FLinuxCursor::~FLinuxCursor()
{
	// Release cursors
	// NOTE: Shared cursors will automatically be destroyed when the application is destroyed.
	//       For dynamically created cursors, use DestroyCursor
	for( int32 CurCursorIndex = 0; CurCursorIndex < EMouseCursor::TotalCursorCount; ++CurCursorIndex )
	{
		switch( CurCursorIndex )
		{
			case EMouseCursor::None:
			case EMouseCursor::Default:
			case EMouseCursor::TextEditBeam:
			case EMouseCursor::ResizeLeftRight:
			case EMouseCursor::ResizeUpDown:
			case EMouseCursor::ResizeSouthEast:
			case EMouseCursor::ResizeSouthWest:
			case EMouseCursor::CardinalCross:
			case EMouseCursor::Crosshairs:
			case EMouseCursor::Hand:
			case EMouseCursor::GrabHand:
			case EMouseCursor::GrabHandClosed:
			case EMouseCursor::SlashedCircle:
				// Standard shared cursors don't need to be destroyed
				break;
			case EMouseCursor::EyeDropper:
				SDL_FreeCursor(CursorHandles[CurCursorIndex]);
				break;

			default:
				// Unrecognized cursor type!
				check( 0 );
				break;
		}
	}
}

FVector2D FLinuxCursor::GetPosition() const
{
	int CursorX, CursorY;

	SDL_GetGlobalMouseState(&CursorX, &CursorY);
	CursorX += AccumulatedOffsetX;
	CursorY += AccumulatedOffsetY;

	return FVector2D( CursorX, CursorY );
}

void FLinuxCursor::SetPosition( const int32 X, const int32 Y )
{
	int WndX, WndY;

	SDL_HWindow WndFocus = SDL_GetMouseFocus();

	SDL_GetWindowPosition( WndFocus, &WndX, &WndY );	//	get top left
	SDL_WarpMouseInWindow( NULL, X - WndX, Y - WndY );
}

void FLinuxCursor::AddOffset(const int32 DX, const int32 DY)
{
	AccumulatedOffsetX += DX; 
	AccumulatedOffsetY += DY;
}

void FLinuxCursor::ResetOffset()
{
	AccumulatedOffsetX = AccumulatedOffsetY = 0;
}

void FLinuxCursor::SetType( const EMouseCursor::Type InNewCursor )
{
	checkf( InNewCursor < EMouseCursor::TotalCursorCount, TEXT("Invalid cursor(%d) supplied"), InNewCursor );
	if(InNewCursor == EMouseCursor::None)
	{
		bHidden = true;
		SDL_ShowCursor(SDL_DISABLE);
		SDL_SetCursor(CursorHandles[0]);
	}
	else
	{
		bHidden = false;
		SDL_ShowCursor(SDL_ENABLE);
		SDL_SetCursor(CursorHandles[InNewCursor]);
	}
}

void FLinuxCursor::GetSize( int32& Width, int32& Height ) const
{
	Width = 16;
	Height = 16;
}

void FLinuxCursor::Show( bool bShow )
{
	if( bShow )
	{
		// Show mouse cursor.
		bHidden = false;
		SDL_ShowCursor(SDL_ENABLE);
	}
	else
	{
		// Disable the cursor.
		bHidden = true;
		SDL_ShowCursor(SDL_DISABLE);
	}
}

void FLinuxCursor::Lock( const RECT* const Bounds )
{
	LinuxApplication->OnMouseCursorLock( Bounds != NULL );

	// Lock/Unlock the cursor
	if ( Bounds == NULL )
	{
		CursorClipRect = FIntRect();
		SDL_SetWindowGrab( NULL, SDL_FALSE );
	}
	else
	{
		SDL_SetWindowGrab( NULL, SDL_TRUE );
		CursorClipRect.Min.X = FMath::TruncToInt(Bounds->left);
		CursorClipRect.Min.Y = FMath::TruncToInt(Bounds->top);
		CursorClipRect.Max.X = FMath::TruncToInt(Bounds->right) - 1;
		CursorClipRect.Max.Y = FMath::TruncToInt(Bounds->bottom) - 1;
	}

	FVector2D CurrentPosition = GetPosition();
	if( UpdateCursorClipping( CurrentPosition ) )
	{
		SetPosition( CurrentPosition.X, CurrentPosition.Y );
	}
}

bool FLinuxCursor::UpdateCursorClipping( FVector2D& CursorPosition )
{
	bool bAdjusted = false;

	if (CursorClipRect.Area() > 0)
	{
		if (CursorPosition.X < CursorClipRect.Min.X)
		{
			CursorPosition.X = CursorClipRect.Min.X;
			bAdjusted = true;
		}
		else if (CursorPosition.X > CursorClipRect.Max.X)
		{
			CursorPosition.X = CursorClipRect.Max.X;
			bAdjusted = true;
		}

		if (CursorPosition.Y < CursorClipRect.Min.Y)
		{
			CursorPosition.Y = CursorClipRect.Min.Y;
			bAdjusted = true;
		}
		else if (CursorPosition.Y > CursorClipRect.Max.Y)
		{
			CursorPosition.Y = CursorClipRect.Max.Y;
			bAdjusted = true;
		}
	}

	return bAdjusted;
}

bool FLinuxCursor::IsHidden()
{
	return bHidden;
}
