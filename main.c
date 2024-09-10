
#include "Application/Flashing_manger.h"

int main()
{

	Flashing_manger_init();
	Move_interrupts();
	while (1)
	{
		Flash_manger_unit();
	}
}
