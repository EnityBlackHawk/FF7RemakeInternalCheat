#pragma once
#include <iostream>
#include <vector>
#include <Windows.h>
#include <chrono>

namespace Cheat
{

	typedef uint8_t Permition;
	typedef uint8_t Char;

	struct OriginalValues
	{
		int32_t CloudAtk;
		int32_t CloudMagAtk;
		int32_t TiffaAtk;
		int32_t TiffaMagAtk;
		int32_t BarretAtk;
		int32_t BarretMagAtk;
		int32_t AerithAtk;
		int32_t AerithMagAtk;
	};

	inline auto baseModule = reinterpret_cast<std::uintptr_t>(GetModuleHandle("ff7remake_.exe"));
	inline auto startTime = std::chrono::high_resolution_clock::now();
	inline bool     auto_heal;
	inline bool		IsHooked;
	inline Permition AllowToChangeMoney;
	inline OriginalValues originals;

	inline DWORD	hookAdress;
	inline DWORD    jmpBackAddy;

	inline int32_t* money;
	inline int32_t* cloudLife;
	inline int32_t* cloudMaxLife;
	inline int32_t* cloudMagic;
	inline int32_t* cloudMaxMagic;
	inline int32_t* cloudAtk;
	inline int32_t* cloudMagAtk;
	

	inline int32_t* tiffaLife;
	inline int32_t* tiffaMaxLife;
	inline int32_t* tiffaMagic;
	inline int32_t* tiffaMaxMagic;
	inline int32_t* tiffaAtk;
	inline int32_t* tiffaMagAtk;

	inline int32_t* barretLife;
	inline int32_t* barretMaxLife;
	inline int32_t* barretMagic;
	inline int32_t* barretMaxMagic;
	inline int32_t* barretAtk;
	inline int32_t* barretMagAtk;

	inline int32_t* aerithLife;
	inline int32_t* aerithMaxLife;
	inline int32_t* aerithMagic;
	inline int32_t* aerithMaxMagic;
	inline int32_t* aerithAtk;
	inline int32_t* aerithMagAtk;

	inline int		totalOfHealing = 0;

	enum eChars
	{
		ALL = 0,
		CLOUD = 1,
		TIFFA = 2,
		BARRET = 3,
		AERITH = 4,
	};

	enum eModes
	{
		RESTORE = 0,
		HEALTH = 1,
		PM = 2,
		ATK = 3,
		MAGIC_ATK = 4,
	};

	enum ePermitions
	{
		ALLOWED = 0,
		NOT_ALLOWED = 1,
		ASK_AWAYS = 2,
		ASK_EXCEPT_DECREASE = 3,
		ASK_EXCEPT_INCREASE = 4,
		INCREASE_ONLY = 5,
		DECREASE_ONLY = 6,
	};

	inline const char* PermitionsMessages[] =
	{
		"Allowed",
		"Inclease only",
		"Ask (aways)",
		"Ask (Except declease)",
		"Ask (Except inclease)",
		"Not allowed",
		"Declease only",
	};


	void InitCheat() noexcept;
	void Main() noexcept;
	void ShowInfo(bool WithCls) noexcept;
	void ShowAdvInfo() noexcept;
	void ShowAddrsInfo() noexcept;
	void ManualWrite() noexcept;
	void Write(Char character, int mode, int value) noexcept;
	void EndMessage() noexcept;
	void EditMoneyPermition() noexcept;
	void SaveOrignalValues() noexcept;
	void LoadOriginalValues(Char) noexcept;


	
}