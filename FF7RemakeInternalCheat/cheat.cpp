#include "cheat.h"
#include <Windows.h>
#include <MinHook.h> // Credits: https://github.com/TsudaKageyu/minhook

#pragma comment(lib, "libMinHook.x64.lib") // Credits: https://github.com/TsudaKageyu/minhook


uintptr_t damage_addrs = Cheat::baseModule + 0xAFB6C0;
uintptr_t endfunc_addrs = Cheat::baseModule + 0x4072ED0;
uintptr_t writeMoney_addrs = Cheat::baseModule + 0xB209C0;


typedef __int64(__fastcall* Damage)(unsigned __int8 a1, int a2);
Damage pDamage = nullptr;
Damage pDamageTarget = reinterpret_cast<Damage>(damage_addrs);

int64_t __fastcall NewDamage(unsigned __int8 a1, int a2)
{
	if (a2 <= 0) a2 = 1;
	return pDamage(a1, a2);
}

typedef void(__fastcall* EndFunc)(); // void __fastcall sub_7FF7914A2ED0()
EndFunc pEndFunc = nullptr;
EndFunc pEndFuncTarget = reinterpret_cast<EndFunc>(endfunc_addrs);

void __fastcall NewEndFunc()
{
	Cheat::EndMessage();
	return pEndFunc();
}
// void __fastcall sub_7FF78DF509C0(__int64 a1, int a2)
typedef void(__fastcall* writeMoney)(__int64 a1, int a2);
writeMoney pWriteMoney = nullptr;
writeMoney pWriteMoneyTarget = reinterpret_cast<writeMoney>(writeMoney_addrs);

void __fastcall NewWriteMoney(__int64 a1, int a2)
{
	if (*Cheat::money == a2) return pWriteMoneyTarget(a1, a2);

	if (Cheat::AllowToChangeMoney == Cheat::NOT_ALLOWED) return;
	else if (Cheat::AllowToChangeMoney == Cheat::ALLOWED) return pWriteMoney(a1, a2);

	if (*Cheat::money < a2) // se ganhar dinheiro
	{
		if (Cheat::AllowToChangeMoney == Cheat::INCREASE_ONLY || Cheat::AllowToChangeMoney == Cheat::ASK_EXCEPT_INCREASE) 
			return pWriteMoney(a1, a2);
		else if (Cheat::AllowToChangeMoney == Cheat::DECREASE_ONLY) return;
	}
	else if (*Cheat::money > a2) // se perder dinheiro
	{
		if (Cheat::AllowToChangeMoney == Cheat::DECREASE_ONLY || Cheat::AllowToChangeMoney == Cheat::ASK_EXCEPT_DECREASE)
			return pWriteMoney(a1, a2);
		else if (Cheat::AllowToChangeMoney == Cheat::INCREASE_ONLY) return;
	}

	int esc = MessageBoxA(NULL, "Allow edit money value ?", "FFVII", MB_YESNO);
	
	if (esc == IDYES) return pWriteMoney(a1, a2);
	else if (esc == IDNO) return;
	
}

int32_t* GetPointerAddress(uintptr_t ptr, std::vector<unsigned __int64> offsets)
{
	uintptr_t addr = *reinterpret_cast<uintptr_t*>(ptr);

	for (int i = 0; i < offsets.size(); ++i) addr += offsets[i];

	
	return reinterpret_cast<int32_t*>(addr);
}

void ShowError(const char* pMessage)
{
	MessageBox(NULL, pMessage, "FFVII", MB_OK | MB_ICONERROR);
}

void Cheat::InitCheat() noexcept
{
	auto_heal = true;
	IsHooked = false;
	AllowToChangeMoney = 0;

	money = GetPointerAddress(baseModule + 0x579D6E8, { 0x356C4 });
	cloudLife = GetPointerAddress(baseModule + 0x0579D6E8, { 0x8B0 });
	cloudMaxLife = GetPointerAddress(baseModule + 0x0579D6E8, { 0x8B4 });
	cloudMagic = GetPointerAddress(baseModule + 0x0579D6E8, { 0x8B8 });
	cloudMaxMagic = GetPointerAddress(baseModule + 0x0579D6E8, { 0x8BC });
	cloudAtk = GetPointerAddress(baseModule + 0x0579D6E8, { 0x8C8 });
	cloudMagAtk = GetPointerAddress(baseModule + 0x0579D6E8, { 0x8CC });

	tiffaLife = GetPointerAddress(baseModule + 0x0579D6E8, { 0x930 });
	tiffaMaxLife = GetPointerAddress(baseModule + 0x0579D6E8, { 0x934 });
	tiffaMagic = GetPointerAddress(baseModule + 0x0579D6E8, { 0x938 });
	tiffaMaxMagic = GetPointerAddress(baseModule + 0x0579D6E8, { 0x93C });
	tiffaAtk = GetPointerAddress(baseModule + 0x0579D6E8, { 0x948 }); //18
	tiffaMagAtk = GetPointerAddress(baseModule + 0x0579D6E8, { 0x94C }); //1C

	barretLife = GetPointerAddress(baseModule + 0x0579D6E8, { 0x8F0 });
	barretMaxLife = GetPointerAddress(baseModule + 0x0579D6E8, { 0x8F4 });
	barretMagic = GetPointerAddress(baseModule + 0x0579D6E8, { 0x8F8 });
	barretMaxMagic = GetPointerAddress(baseModule + 0x0579D6E8, { 0x8FC });
	barretAtk = GetPointerAddress(baseModule + 0x0579D6E8, { 0x908 });
	barretMagAtk = GetPointerAddress(baseModule + 0x0579D6E8, { 0x90C });

	aerithLife = GetPointerAddress(baseModule + 0x0579D6E8, { 0x970 });
	aerithMaxLife = GetPointerAddress(baseModule + 0x0579D6E8, { 0x974 });
	aerithMagic = GetPointerAddress(baseModule + 0x0579D6E8, { 0x978 });
	aerithMaxMagic = GetPointerAddress(baseModule + 0x0579D6E8, { 0x97C });
	aerithAtk = GetPointerAddress(baseModule + 0x0579D6E8, { 0x988 });
	aerithMagAtk = GetPointerAddress(baseModule + 0x0579D6E8, { 0x98C });

	MH_STATUS status = MH_Initialize();
	if (status != MH_OK)
	{
		ShowError("MinHook initialize error");
		return;
	}

	if (MH_CreateHook(reinterpret_cast<void**>(pDamageTarget), &NewDamage, reinterpret_cast<void**>(&pDamage)) != MH_OK)
	{
		ShowError("Error on hook creation: Damage");
		return;
	}
	if (MH_CreateHook(reinterpret_cast<void**>(pEndFuncTarget), &NewEndFunc, reinterpret_cast<void**>(&pEndFunc)) != MH_OK)
	{
		ShowError("Error on hook creation: EndFunction");
		return;
	}
	if (MH_CreateHook(reinterpret_cast<void**>(pWriteMoneyTarget), &NewWriteMoney, reinterpret_cast<void**>(&pWriteMoney)) != MH_OK)
	{
		ShowError("Error on hook creation: WriteMoney");
		return;
	}

	if (MH_EnableHook(reinterpret_cast<void**>(pEndFuncTarget)) != MH_OK)
	{
		ShowError("Error on activating End Funtion");
		return;
	}

	if (MH_EnableHook(reinterpret_cast<void**>(pWriteMoneyTarget)) != MH_OK)
	{
		ShowError("Error on activating WriteMoney");
		return;
	}

	if (*cloudLife == 0)
	{
		std::cout << "Aguardando load do jogo..." << std::endl;
		while (*cloudLife == 0) Sleep(300);
	}

	Cheat::SaveOrignalValues();
	Cheat::Main();
}

void Cheat::Main() noexcept
{
	ShowInfo(true);

	while (true)
	{
		if (!IsHooked && auto_heal)
		{
			if (MH_EnableHook(reinterpret_cast<void**>(pDamageTarget)) != MH_OK)
			{
				ShowError("Error on enable hook");
				return;
			}
			IsHooked = true;
		}
		else if (IsHooked && !auto_heal)
		{
			if (MH_DisableHook(reinterpret_cast<void**>(pDamageTarget)) != MH_OK)
			{
				ShowError("Error on disable hook");
				return;
			}
			IsHooked = false;
		}

		if (GetAsyncKeyState(VK_INSERT))
		{
			ManualWrite();
		}

		if (GetAsyncKeyState(VK_DELETE))
		{
			auto_heal = !auto_heal;
			ShowInfo(true);
			Sleep(1000);
		}

		if (GetAsyncKeyState(VK_END))
		{
			EndMessage();
			break;
		}

		if (GetAsyncKeyState(VK_HOME))
		{
			ShowInfo(true);
		}

		if (GetAsyncKeyState(VK_F10))
		{
			SaveOrignalValues();
		}

		if (GetAsyncKeyState(VK_F9))
		{
			LoadOriginalValues(ALL);
		}

		if (GetAsyncKeyState(VK_F8))
		{
			EditMoneyPermition();
		}

		if (GetAsyncKeyState(VK_F7))
		{
			ShowAdvInfo();
		}

		if (GetAsyncKeyState(VK_F6))
		{
			ShowAddrsInfo();
		}

		if (auto_heal)
		{
			if (*cloudLife <= 1) Write(CLOUD, HEALTH, RESTORE);

			if (*tiffaLife <= 1) Write(TIFFA, HEALTH, RESTORE);

			if (*barretLife <= 1) Write(BARRET, HEALTH, RESTORE);

			if (*aerithLife <= 1) Write(AERITH, HEALTH, RESTORE);
		}

	}
	return;
}
	


void Cheat::ShowInfo(bool WithCls = false) noexcept
{
	if (WithCls) system("cls");
	std::cout << "Auto heal: " << std::boolalpha << auto_heal << " [press del to change]" << std::endl;
	std::cout << "Money changeing permition: " << PermitionsMessages[AllowToChangeMoney];
	std::cout << " [press F8 to change]" << std::endl;
	std::cout << "====================================" << std::endl;
	std::cout << "[F10] Store original Atk and Magic atk as original values" << std::endl;
	std::cout << "[F9] Restore original Atk and Magic Atk values" << std::endl;
	std::cout << "[F7] Show values" << std::endl;
	std::cout << "[F6] Show addresses" << std::endl;
	std::cout << "[end] Uninject" << std::endl;
	std::cout << "[insert] Edit character values" << std::endl;
	std::cout << "Compile date: " << __DATE__ << std::endl;
	std::cout << "====================================" << std::endl;
}

void Cheat::ShowAdvInfo() noexcept
{
	system("cls");
	std::cout << "Final Fantasy VII Stats: Values" << std::endl;
	std::cout << "====================================" << std::endl;
	std::cout << "Money: " << std::dec << *money << std::endl;
	std::cout << "Cloud's life value: " << std::dec << *cloudLife << std::endl;
	std::cout << "Cloud's PM value: " << std::dec << *cloudMagic << std::endl;
	std::cout << "Cloud's Atk: " << std::dec << *cloudAtk << std::endl;
	std::cout << "Cloud's Magic Atk: " << std::dec << *cloudMagAtk << std::endl;
	std::cout << "Tiffa's life value: " << std::dec << *tiffaLife << std::endl;
	std::cout << "Tiffa's PM value: " << std::dec << *tiffaMagic << std::endl;
	std::cout << "Tiffa's Atk: " << std::dec << *tiffaAtk << std::endl;
	std::cout << "Tiffa's Magic Atk: " << std::dec << *tiffaMagAtk << std::endl;
	std::cout << "Barret's life value: " << std::dec << *barretLife << std::endl;
	std::cout << "Barret's PM value: " << std::dec << *barretMagic << std::endl;
	std::cout << "Aerith's life value: " << std::dec << *aerithLife << std::endl;
	std::cout << "Aerith's PM value: " << std::dec << *aerithMagic << std::endl;
	std::cout << "====================================" << std::endl;
	std::cout << "[home] Return to home" << std::endl;
	std::cout << "[F10] Store original Atk and Magic atk as original values" << std::endl;
	std::cout << "[F9] Restore original Atk and Magic Atk values" << std::endl;
	std::cout << "[F6] Show addresses" << std::endl;
	std::cout << "[F7] Show values" << std::endl;
	std::cout << "[end] Uninject" << std::endl;
	std::cout << "[insert] Edit character values" << std::endl;
	std::cout << "====================================" << std::endl;
}

void Cheat::ShowAddrsInfo() noexcept
{
	system("cls");
	std::cout << "Final Fantasy VII Stats: Address" << std::endl;
	std::cout << "====================================" << std::endl;
	std::cout << "Base module: " << std::hex << baseModule << std::endl;
	std::cout << "Money: " << std::hex << money << std::endl;
	std::cout << "Cloud's life value: " << std::hex << cloudLife << std::endl;
	std::cout << "Cloud's PM value: " << std::hex << cloudMagic << std::endl;
	std::cout << "Cloud's Atk: " << std::hex << cloudAtk << std::endl;
	std::cout << "Cloud's Magic Atk: " << std::hex << cloudMagAtk << std::endl;
	std::cout << "Tiffa's life value: " << std::hex << tiffaLife << std::endl;
	std::cout << "Tiffa's PM value: " << std::hex << tiffaMagic << std::endl;
	std::cout << "Tiffa's Atk: " << std::hex << tiffaAtk << std::endl;
	std::cout << "Tiffa's Magic Atk: " << std::hex << tiffaMagAtk << std::endl;
	std::cout << "Barret's life value: " << std::hex << barretLife << std::endl;
	std::cout << "Barret's PM value: " << std::hex << barretMagic << std::endl;
	std::cout << "Aerith's life value: " << std::hex << aerithLife << std::endl;
	std::cout << "Aerith's PM value: " << std::hex << aerithMagic << std::endl;
	std::cout << "====================================" << std::endl;
	std::cout << "[home] Return to home" << std::endl;
	std::cout << "[F10] Store original Atk and Magic atk as original values" << std::endl;
	std::cout << "[F9] Restore original Atk and Magic Atk values" << std::endl;
	std::cout << "[F7] Show values" << std::endl;
	std::cout << "[F6] Show adrressed" << std::endl;
	std::cout << "[end] Uninject" << std::endl;
	std::cout << "[insert] Edit character values" << std::endl;
	std::cout << "====================================" << std::endl;
}



void Cheat::ManualWrite() noexcept
{
	
	int esc_char;
	int esc_mode;
	int value;

	system("cls");
	std::cout << "[1] Cloud"  << '\n';
	std::cout << "[2] Tiffa"  << '\n';
	std::cout << "[3] Barret" << '\n';
	std::cout << "[4] Aerith" << '\n';

	while (true)
	{
		if (GetAsyncKeyState(VK_NUMPAD1)) { esc_char = 1; break; }
		if (GetAsyncKeyState(VK_NUMPAD2)) { esc_char = 2; break; }
		if (GetAsyncKeyState(VK_NUMPAD3)) { esc_char = 3; break; }
		if (GetAsyncKeyState(VK_NUMPAD4)) { esc_char = 4; break; }
	}
	Sleep(500);
	system("cls");
	std::cout << "[1] Life" << '\n';
	std::cout << "[2] PM" << '\n';
	std::cout << "[3] Atk" << '\n';
	std::cout << "[4] Magic Atk" << '\n';

	while (true)
	{
		if (GetAsyncKeyState(VK_NUMPAD1)) { esc_mode = 1; break; }
		if (GetAsyncKeyState(VK_NUMPAD2)) { esc_mode = 2; break; }
		if (GetAsyncKeyState(VK_NUMPAD3)) { esc_mode = 3; break; }
		if (GetAsyncKeyState(VK_NUMPAD4)) { esc_mode = 4; break; }
	}

	
	if (esc_mode < 3) Write(esc_char, esc_mode, RESTORE);
	else
	{
		fflush(stdin);
		Sleep(500);
		std::cout << "Valor: ";
		std::cin >> value;
		Write(esc_char, esc_mode, value);
	}

	ShowInfo(true);
	
}

void Cheat::Write(Char character,int mode, int value) noexcept
{
	totalOfHealing++;
	switch (mode)
	{
		case HEALTH: 
		{
			switch (character)
			{
				case CLOUD: 
				{
					*cloudLife = *cloudMaxLife;
					std::cout << "Vida de Cloud restaurada" << std::endl;
					break;
				}

				case TIFFA:
				{
					*tiffaLife = *tiffaMaxLife;
					std::cout << "Vida de Tiffa restaurada" << std::endl;
					break;
				}
				case BARRET:
				{
					*barretLife = *barretMaxLife;
					std::cout << "Vida de Barret restaurada" << std::endl;
					break;
				}
				case AERITH:
				{
					*aerithLife = *aerithMaxLife;
					std::cout << "Vida de Aerith restaurada" << std::endl;
					break;
				}
			}
			break;
		}
		case PM:
		{
			switch (character)
			{
				case CLOUD:
				{
					*cloudMagic = *cloudMaxMagic;
					std::cout << "PM de Cloud restaurada" << std::endl;
					break;
				}

				case TIFFA:
				{
					*tiffaMagic = *tiffaMaxMagic;
					std::cout << "PM de Tiffa restaurada" << std::endl;
					break;
				}
				case BARRET:
				{
					*barretMagic = *barretMaxMagic;
					std::cout << "PM de Barret restaurada" << std::endl;
					break;
				}
				case AERITH:
				{
					*aerithMagic = *aerithMaxMagic;
					std::cout << "PM de Aerith restaurada" << std::endl;
					break;
				}
			}
			break;
		}
		case ATK:
		{
			switch (character)
			{
			case CLOUD:
			{
				*cloudAtk = value;
				std::cout << "Atk de Cloud restaurada" << std::endl;
				break;
			}

			case TIFFA:
			{
				*tiffaAtk = value;
				std::cout << "Atk de Tiffa restaurada" << std::endl;
				break;
			}
			case BARRET:
			{
				*barretAtk = value;
				std::cout << "Atk de Barret restaurada" << std::endl;
				break;
			}
			case AERITH:
			{
				*aerithAtk = value;
				std::cout << "Atk de Aerith restaurada" << std::endl;
				break;
			}
			}
			break;
		}
		case MAGIC_ATK:
		{
			switch (character)
			{
			case CLOUD:
			{
				*cloudMagAtk = value;
				std::cout << "Magic Atk de Cloud restaurada" << std::endl;
				break;
			}

			case TIFFA:
			{
				*tiffaMagAtk = value;
				std::cout << "Magic Atk de Tiffa restaurada" << std::endl;
				break;
			}
			case BARRET:
			{
				*barretMagAtk = value;
				std::cout << "Magic Atk de Barret restaurada" << std::endl;
				break;
			}
			case AERITH:
			{
				*aerithMagAtk = value;
				std::cout << "Magic Atk de Aerith restaurada" << std::endl;
				break;
			}
			}
			break;
		}

	}
}

void Cheat::EndMessage() noexcept
{
	MH_DisableHook(MH_ALL_HOOKS);
	auto endTime = std::chrono::high_resolution_clock::now();
	std::cout << "Total de vezes curadas durante a sessao atual: " << std::dec << totalOfHealing << std::endl;
	std::cout << "Tempo de total: " << std::chrono::duration_cast<std::chrono::minutes>(endTime - startTime) << std::endl;
	system("pause");
}

void Cheat::EditMoneyPermition() noexcept
{

	AllowToChangeMoney = AllowToChangeMoney == 6 ? 0 : AllowToChangeMoney + 1;

	ShowInfo(true);
	Sleep(250);
	return;

}

void Cheat::SaveOrignalValues() noexcept
{
	system("clr");
	
	std::cout << "Coletando valores..." << std::endl;

	originals = OriginalValues();
	originals.CloudAtk = *cloudAtk;
	originals.CloudMagAtk = *cloudMagAtk;
	originals.TiffaAtk = *tiffaAtk;
	originals.TiffaMagAtk = *tiffaMagAtk;
	originals.BarretAtk = *barretAtk;
	originals.BarretMagAtk = *barretMagAtk;
	originals.AerithAtk = *aerithAtk;
	originals.AerithMagAtk = *aerithMagAtk;
}

void Cheat::LoadOriginalValues(Char character) noexcept
{
	Sleep(500);
	if (!character)
	{
		Write(CLOUD, ATK, originals.CloudAtk);
		Write(CLOUD, MAGIC_ATK, originals.CloudMagAtk);
		Write(TIFFA, ATK, originals.TiffaAtk);
		Write(TIFFA, MAGIC_ATK, originals.TiffaMagAtk);
		Write(BARRET, ATK, originals.BarretAtk);
		Write(BARRET, MAGIC_ATK, originals.BarretMagAtk);
		Write(AERITH, ATK, originals.AerithAtk);
		Write(AERITH, MAGIC_ATK, originals.AerithMagAtk);
	}
}


