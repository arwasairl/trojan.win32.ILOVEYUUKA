#include <ntddk.h>

#include "typedefs.h"
#include "bluescreen.h"

PBSOD_INFORMATION g_BsodInformation = nullptr;

EXTERN_C NTSTATUS DriverEntry(PDRIVER_OBJECT, PUNICODE_STRING)
{
	g_BsodInformation = static_cast<PBSOD_INFORMATION>(
		ExAllocatePool2(POOL_FLAG_NON_PAGED, sizeof(BSOD_INFORMATION), 0x21)
		);
	if (g_BsodInformation == NULL)
	{
		DbgPrint("[%ws::%d] ExAllocatePool failed: %08x\n", __FUNCTIONW__, __LINE__, STATUS_INSUFFICIENT_RESOURCES);
		return STATUS_FAILED_DRIVER_ENTRY;
	}
	else
	{
		RtlSecureZeroMemory(g_BsodInformation, sizeof(BSOD_INFORMATION));
	}

	///
	/// Set the emoticon and new error message to be something Walter-like
	/// 
	g_BsodInformation->lol = RTL_CONSTANT_STRING(L":)");
	g_BsodInformation->Grouch = RTL_CONSTANT_STRING(L"FUCK YOU! I LOVE YUUKA KAZAMI!");

	///
	/// Anything after ~96 characters overwrites the next buffer, which is the percentage counter so you'll have 
	/// 100 sippee cu
	/// L"We're just collecting some error info, and then you can go and cry to yo mama with your precious sippee cup."
	/// 
	g_BsodInformation->Insult = RTL_CONSTANT_STRING(L"YOUR COMPUTER AND ALL OF ITS DATA IS FUCKED!! HAHAHA! THIS IS WHAT YOU GET WHEN YOU EAT SUNFLOWER SEEDS!");

	///
	/// Resolve all the variables needed to dynamically locate the emoji and color variables
	/// 
	if (!NT_SUCCESS(bluescreen::initialize()))
	{
		return STATUS_FAILED_DRIVER_ENTRY;
	}

	///
	/// Overwrite the sad face emoji to be something else
	/// 
	if (!NT_SUCCESS(bluescreen::OverwriteSadFace()))
	{
		return STATUS_FAILED_DRIVER_ENTRY;
	}

	///
	/// Overwrite the error messages with something mean
	/// 
	if (!NT_SUCCESS(bluescreen::OverwriteErrorMessage(
		g_BsodInformation->BsodMessageOne,
		&g_BsodInformation->Grouch
	)))
	{
		return STATUS_FAILED_DRIVER_ENTRY;
	}

	if (!NT_SUCCESS(bluescreen::OverwriteErrorMessage(
		g_BsodInformation->BsodMessageTwo,
		&g_BsodInformation->Insult
	)))
	{
		return STATUS_FAILED_DRIVER_ENTRY;
	}

	///
	/// Set the BSOD to be a dark red color
	/// 
	if (!NT_SUCCESS(bluescreen::ChangeBsodColor(0xff8b0000)))
	{
		return STATUS_FAILED_DRIVER_ENTRY;
	}

	if (g_BsodInformation)
	{
		ExFreePool(g_BsodInformation);
	}

	///
	/// No need to completely load/unload the driver as it's just modifying 
	/// some variables. An error message that comes up when loading this driver
	/// is normal. 
	/// 
	return STATUS_FAILED_DRIVER_ENTRY;
}


/// EOF