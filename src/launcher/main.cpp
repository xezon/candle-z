
#include <zorro.h>
#include <common/util_win.h>

typedef void (ZORRO_CALL* MainFunc)();

int main()
{
	HMODULE module = LoadLibraryA("candle-z.dll");
	MainFunc mainFunc = util::GetProcAddressT<MainFunc>(module, "main");
	mainFunc();

	return 0;
}
